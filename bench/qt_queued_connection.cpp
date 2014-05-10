#include <string>
#include <iostream>

#include "cppa/cppa.hpp"

#include <QObject>
#include <QThread>
#include <QCoreApplication>
#include <QTimer>

#include "nsec_clock.h"

using namespace std;
using namespace cppa;


template<int I>
behavior receiver(event_based_actor* self)
{
    return {
        [=](int i) -> void {
            if (i == I)
                self->quit();
        }
    };
}

template<int I>
void sender(event_based_actor* self, const actor& rec)
{
    for (int i = 0; i <= I; i++) {
        self->send(rec, i);
    }
    self->quit();
}


template<int I>
void bench_cppa()
{
    auto rec = spawn<detached>(receiver<I>);
    nsec_t t0 = nsec_clock();
    spawn(sender<I>, rec);
    await_all_actors_done();
    nsec_t dt = nsec_clock() - t0;

    shutdown();
    printf("cppa: %i nsec\n", (int)dt/I);
}



class Receiver : public QObject
{
    Q_OBJECT

public:
    Receiver(int i) : I(i) {}

signals:
    void done();

public slots:
    void rec(int i)
    {
        if (i == I)
            emit done();
    }

private:
    int I;
};


class Sender : public QObject
{
    Q_OBJECT

public:
    Sender(int i) : I(i){}

    int I;

public slots:
    void sendMessages()
    {
        for (int i = 0; i <= I; i++) {
            message(i);
        }
    }

signals:
    void message(int);
};


template<int I>
void bench_qt(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    QThread thread;
    Receiver rec(I);
    rec.moveToThread(&thread);
    thread.start();

    Sender sender(I);

    QObject::connect(&sender, SIGNAL(message(int)), &rec, SLOT(rec(int)), Qt::QueuedConnection);
    QObject::connect(&rec, SIGNAL(done()), &app, SLOT(quit()));

    QTimer::singleShot(1, &sender, SLOT(sendMessages()));
    nsec_t t0 = nsec_clock();
    app.exec();

    nsec_t dt = nsec_clock() - t0;
    printf("qt  : %i nsec\n", (int)dt/I);

    thread.quit();
    QCoreApplication::processEvents();
}


int main(int argc, char** argv)
{
    const int I = (int)1e6;

    bench_qt<I>(argc, argv);
    bench_cppa<I>();
}

#include "qt_queued_connection.moc"


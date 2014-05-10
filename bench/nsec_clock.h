#pragma once 


#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
typedef long nsec_t;


inline nsec_t nsec_clock()
{
    nsec_t nsecs = 0;

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    nsecs = 1000*1000*1000 * ts.tv_sec;
    nsecs += ts.tv_nsec;

    return nsecs;
}

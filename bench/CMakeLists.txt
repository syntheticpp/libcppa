
project(cppa_bench CXX)

add_custom_target(all_benchmarks)

include(${QT_USE_FILE})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

macro(addqt name)
  set(moced ${CMAKE_CURRENT_BINARY_DIR}/${name}.moc)
  qt4_generate_moc(${name}.cpp ${moced})
  set_source_files_properties(${name}.cpp PROPERTIES OBJECT_DEPENDS ${moced})
  add_executable(${name} ${name}.cpp ${ARGN})
  target_link_libraries(${name} ${CMAKE_DL_LIBS} ${CPPA_LIBRARY} ${PTHREAD_LIBRARIES} ${QT_LIBRARIES})
  add_dependencies(${name} all_benchmarks)
endmacro()

addqt(qt_queued_connection)

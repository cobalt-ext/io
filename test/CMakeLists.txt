add_executable(cobalt_io_test EXCLUDE_FROM_ALL test_main.cpp io/sleep.cpp io/endpoint.cpp io/resolver.cpp)
target_link_libraries(cobalt_io_test  Boost::cobalt Boost::unit_test_framework cobalt::io)
add_test(NAME cobalt_io_test COMMAND cobalt_io_test)


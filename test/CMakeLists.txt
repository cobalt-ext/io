add_executable(boost_cobalt_experimental_io EXCLUDE_FROM_ALL test_main.cpp sleep.cpp endpoint.cpp resolver.cpp)
target_link_libraries(boost_cobalt_experimental_io  Boost::cobalt Boost::unit_test_framework cobalt::io)
add_test(NAME boost_cobalt_experimental_io COMMAND boost_cobalt_experimental_io)


cmake_minimum_required(VERSION 3.29)

project(cobalt.io)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED cobalt process filesystem)

add_library(cobalt_io
            src/acceptor.cpp
            src/datagram_socket.cpp
            src/endpoint.cpp
            src/file.cpp
            src/pipe.cpp
            src/popen.cpp
            src/process.cpp
            src/random_access_file.cpp
            src/read.cpp
            src/resolver.cpp
            src/seq_packet_socket.cpp
            src/serial_port.cpp
            src/signal_set.cpp
            src/socket.cpp
            src/ssl.cpp
            src/steady_timer.cpp
            src/stream_file.cpp
            src/stream_socket.cpp
            src/system_timer.cpp
            src/write.cpp
            src/stdio.cpp)

target_link_libraries(cobalt_io PUBLIC Boost::cobalt Boost::process Boost::filesystem)
target_include_directories(cobalt_io PUBLIC ./include)
target_compile_features(cobalt_io PUBLIC cxx_std_20)
target_compile_definitions(cobalt_io PRIVATE COBALT_IO_SOURCE=1 BOOST_PROCESS_USE_STD_FS=1)
add_library(cobalt::io ALIAS cobalt_io)

if (BUILD_TESTING)
    find_package(Boost REQUIRED unit_test_framework)
    add_subdirectory(test)
endif()
cmake_minimum_required(VERSION 3.3)
project(Poll_Event)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDEDIR})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUG -g -O0 -std=c++14")
set(NEW_SOURCE
        address.cpp
        address.h
        debug.h
        io_service.h
        io_service.cpp
        acceptor.cpp
        acceptor.h
        epoll_error.cpp
        outstring.cpp
        outstring.h
        epoll_error.h
        posix_sockets.cpp posix_sockets.h
        connection.h connection.cpp
        rshd_main.cpp
         utils.h utils.cpp handle.cpp handle.h sshd.cpp sshd.h pty_fd.cpp pty_fd.h)
add_executable(NEW ${NEW_SOURCE})
target_link_libraries(NEW ${Boost_LIBRARIES})

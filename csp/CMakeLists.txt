cmake_minimum_required(VERSION 3.7)
project(csp)

set(CMAKE_CXX_STANDARD 11)
link_libraries(pthread dl)

set(SOURCE_FILES main.cpp WebServer.cpp DebugPrint.cpp Session.cpp GetCPPSP.cpp HttpCode.cpp Request.cpp
        Response.cpp Listener.h Epoll.h InetAddress.h TaskQueue.h ReadEpoll.h WriteEpoll.h Buffer.h)
add_executable(csp ${SOURCE_FILES})
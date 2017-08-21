cmake_minimum_required(VERSION 3.7)
project(csp)

set(CMAKE_CXX_STANDARD 11)
link_libraries(pthread dl)

set(SOURCE_FILES main.cpp Web.cpp DebugPrint.cpp Session.cpp GetCPPSP.cpp HttpCode.cpp Request.cpp Response.cpp)
add_executable(csp ${SOURCE_FILES})
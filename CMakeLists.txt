cmake_minimum_required(VERSION 3.10)

project(simple-linux-listen-socket-with-client CXX)

add_definitions(-DAPI_EXPORTS)
set(CMAKE_BUILD_TYPE Release)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(test-client test-client.cpp src/plain-socket.cpp)
add_executable(test-server test-server.cpp src/listen-socket.cpp src/plain-socket.cpp)

target_compile_options(test-client PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -O3>)
target_compile_options(test-server PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -O3>)

target_link_libraries(test-client Threads::Threads)
target_link_libraries(test-server Threads::Threads)

cmake_minimum_required(VERSION 3.16)

project(simple-linux-listen-socket-with-client)

add_definitions(-DAPI_EXPORTS)
set(CMAKE_BUILD_TYPE Release)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(test-client src/applications/test_client.cpp src/plain_socket.cpp)
target_compile_options(test-client PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -O3>)
target_link_libraries(test-client Threads::Threads)

add_executable(test-server src/applications/test_server.cpp src/listen_socket.cpp src/plain_socket.cpp)
target_compile_options(test-server PRIVATE $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -O3>)
target_link_libraries(test-server Threads::Threads)

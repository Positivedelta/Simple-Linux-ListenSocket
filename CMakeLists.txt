cmake_minimum_required(VERSION 3.10)

project(simple-linux-listen-socket CXX)

add_definitions(-DAPI_EXPORTS)
set(CMAKE_BUILD_TYPE Release)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)

file(GLOB_RECURSE SRCS ${PROJECT_SOURCE_DIR}/src/*.cpp)
add_executable(test-server ${PROJECT_SOURCE_DIR}/test-server.cpp ${SRCS})
target_compile_options(test-server
    PRIVATE
    $<$<COMPILE_LANGUAGE:CXX>:-std=c++17 -O3>
)

target_link_libraries(test-server Threads::Threads)

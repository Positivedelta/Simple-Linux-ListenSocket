cmake_minimum_required(VERSION 3.16)

project(simple-linux-simple-socket-library)

add_definitions(-DAPI_EXPORTS)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_FLAGS_RELEASE "-pthread -O3")

# notes 1, setup the install base path for the simple socket shared library, include files and the test applications
#       2, setup the RPATH for the test applications in bin/ directory
#       3, choose the /usr/local/bit-parallel path once ready to deploy in production (or edit to suit your needs)
#
#set(BIT_PARALLEL_INSTALL_ROOT "/usr/local/bit-parallel")
set(BIT_PARALLEL_INSTALL_ROOT "${PROJECT_SOURCE_DIR}/bit-parallel")
set(CMAKE_INSTALL_RPATH "${BIT_PARALLEL_INSTALL_ROOT}/lib")

include_directories(${PROJECT_SOURCE_DIR}/include)
add_library(socket-bp-v1.0 SHARED ${PROJECT_SOURCE_DIR}/src/copyable_atomic.cpp ${PROJECT_SOURCE_DIR}/src/plain_socket.cpp ${PROJECT_SOURCE_DIR}/src/listen_socket.cpp)
install(TARGETS socket-bp-v1.0 DESTINATION ${BIT_PARALLEL_INSTALL_ROOT}/lib)

add_executable(test-client-socket ${PROJECT_SOURCE_DIR}/src/applications/test_client.cpp)
target_link_libraries(test-client-socket socket-bp-v1.0)

add_executable(test-server-socket ${PROJECT_SOURCE_DIR}/src/applications/test_server.cpp)
target_link_libraries(test-server-socket socket-bp-v1.0)

install(TARGETS test-client-socket test-server-socket DESTINATION ${BIT_PARALLEL_INSTALL_ROOT}/bin/socket)
file(GLOB SOCKET_HPP_FILES "${PROJECT_SOURCE_DIR}/include/*.hpp")
install(FILES ${SOCKET_HPP_FILES} DESTINATION ${BIT_PARALLEL_INSTALL_ROOT}/include/socket)

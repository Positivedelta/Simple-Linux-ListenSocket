# A Simple C++ ListenSocket Class with client for Linux

#### Prerequisites
- Linux
- CMake 3.16 (can be tweaked to use older versions)
- C++ 2017 and greater

#### Build Instructions
To build and run the test application, use:

```
mkdir build
cd build
cmake ..
make -j4
./test-server-socket
./test-client-socket
```

#### Install Instructions
To install the core shared library, the include files and test applications, use:

```
make install
```

The following default paths are used, edit `CMakeLists.txt` if you want to update these
- The shared library is installed to `bit-parallel/lib`
- The common include files are installed to `bit-parallel/include/communications`
- The include files are installed to `bit-parallel/include/communications/socket`
- The test appliactions are installed to `bit-parallel/bin/socket`

#### Notes
- Listen sockets are implemented by the `ListenSocket` class defined in `listen-socket.hpp`
- Regular sockets are implemented the the `PlainSocket` class defined in `plain-socket.hpp`
- Currently the `ListenSocket` class only accepts a single client connection
- Please refer to `test-server.cpp` and `test-client.cpp` for the details of an example client / server
- Tested on a Raspberry Pi 4 running the official 32- or 64-bit OS and the Nvidia Orin / Xavier
- Please contact me at max.vandaalen@bitparallel.com if you have any questions

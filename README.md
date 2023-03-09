# A Simple C++ ListenSocket Class with client for Linux

#### Prerequisites
- Linux
- C++ 2017 and greater

#### Build Instructions
To build and run the test application

```
mkdir build
cd build
cmake ..
make -j4
./test-server
./test-client
```

#### Notes
- Listen sockets are implemented by the `ListenSocket` class defined in `listen-socket.hpp`
- Regular sockets are implemented the the `PlainSocket` class defined in `plain-socket.hpp`
- Currently the `ListenSocket` class only accepts a single client connection
- Please refer to `test-server.cpp` and `test-client.cpp` for the details of an example client / server
- Tested on a Raspberry Pi 4 running the official 32- or 64-bit OS and the Nvidia Orin / Xavier
- Please contact me at max.vandaalen@bitparallel.com if you have any questions

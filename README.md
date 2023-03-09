# A Simple C++ ListenSocket Class for Linux

#### Prerequisites
- Linux
- C++ 2017 (should support c++ 2011 as a minimum, but untested)

#### Build Instructions
To build and run the test application

```
mkdir build
cd build
cmake ..
make -j4
./test-server
```

#### Notes
- Only accepts a single client connection, but if closed will accept another until its own close method is called
- Please refer to test-server.cpp for an example of how to use this class
- Tested on a Raspberry Pi 4 running the official 32- or 64-bit OS and the Nvidia Orin / Xavier
- Please contact me at max.vandaalen@bitparallel.com if you have any questions

# A Simple C++ ListenSocket Class for Linux

#### Prerequisites
- Linux
- C++ 2017 (older versions are fine)

#### Build Instructions
To build and run the test application

```
make clean
make -j4
./test
```

#### Notes
- Only accepts a single client connection, but will loop and accept a new one if closed
- Tested on a Raspberry Pi 4 running the official 32- or 64-bit OS
- Please contact me at max.vandaalen@bitparallel.com if you have any questions

#
# (c) Bit Parallel Ltd (Max van Daalen), September 2022
#

CC=g++

CC_COMPILE_FLAGS=-std=c++17 -O3 -I .
CC_LINK_FLAGS=-pthread

all: listen-socket.o test.o
	$(CC) $(CC_LINK_FLAGS) listen-socket.o test.o -o test

listen-socket.o: listen-socket.cpp
	$(CC) $(CC_COMPILE_FLAGS) -c listen-socket.cpp

test.o: test.cpp
	$(CC) $(CC_COMPILE_FLAGS) -c test.cpp

.PHONY clean:
clean:
	rm -f *.o test

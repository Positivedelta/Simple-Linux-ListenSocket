//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#ifndef H_PLAIN_SOCKET
#define H_PLAIN_SOCKET

//#include <atomic>
#include <cstdint>
#include <string>
#include <thread>
#include <arpa/inet.h>

#include "read-listener.hpp"

class PlainSocket
{
    public:
        static inline const uint8_t NEW_LINE[] = {0x0d, 0x0a};

    private:
        const static inline int32_t RX_BUFFER_SIZE = 4096;
        const static inline int32_t RX_SELECT_TIMEOUT_US = 100000;

        const int32_t socketFd;
        const sockaddr_in socketEndpoint;
        bool doReceive;
//      std::atomic<bool> atomicDoReceive;
        std::thread rxTask;

    public:
        PlainSocket(const int32_t socketFd, const sockaddr_in socketEndpoint);
        void setRxHandler(const ReadListener& listener);
        void setTcpNoDelay(const bool tcpNoDelay) const;
        const std::string getIpAddress() const;
        const uint32_t getTcpPort() const;
        void close();

        void write(const uint8_t bytes[], int32_t length) const;
        void write(const uint8_t singleByte) const;
        void print(const std::string& text) const;
        void printLine() const;
        void printLine(const std::string& text) const;
};

#endif

//
// (c) Bit Parallel Ltd, March 2023
//

#ifndef BIT_PARALLEL_PLAIN_SOCKET_HPP
#define BIT_PARALLEL_PLAIN_SOCKET_HPP

#include <cstdint>
#include <string>
#include <thread>
#include <arpa/inet.h>

#include "copyable_atomic.hpp"
#include "read_listener.hpp"

namespace bpl
{
    class PlainSocket
    {
        public:
            static inline const uint8_t NEW_LINE[] = {0x0d, 0x0a};

        private:
            const static inline int32_t RX_BUFFER_SIZE = 4096;
            const static inline int32_t RX_SELECT_TIMEOUT_US = 100000;

            const int32_t socketFd;
            const sockaddr_in socketEndpoint;
            CopyableAtomic<bool> doReceive;
            std::thread rxTask;

        public:
            PlainSocket(const int32_t remoteTcpPort, const std::string& remoteIpAddress);
            PlainSocket(const int32_t socketFd, const sockaddr_in socketEndpoint);
            PlainSocket(const PlainSocket& plainSocket);
            PlainSocket& operator=(const PlainSocket& plainSocket);

            void connect() const;
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

            ~PlainSocket();

        private:
            int32_t makeSocket() const;
            sockaddr_in makeEndpoint(const int32_t remoteTcpPort, const std::string remoteIpAddress) const;
    };
}

#endif

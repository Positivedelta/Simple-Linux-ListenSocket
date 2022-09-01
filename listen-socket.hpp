//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#ifndef H_LISTEN_SOCKET
#define H_LISTEN_SOCKET

#include <cstdint>
#include <thread>
#include <string>

#include <arpa/inet.h>

#include "read-listener.hpp"

class ListenSocket
{
    public:
        static inline const uint8_t NEW_LINE[] = {0x0d, 0x0a};

    private:
        const static inline std::string DEFAULT_BIND_ADDRESS = "127.0.0.1";
//      const static inline int32_t RX_BUFFER_SIZE = 4096;
//      const static inline int32_t RX_SELECT_TIMEOUT_US = 100000;
//      const static inline ReadListener DEFAULT_RX_LISTENER = [](const uint8_t rxedBytes[], const int32_t length) {};

//      const bool enableReceiver;
//      const std::string& deviceName;
        int32_t listenSocketFd, clientSocketFd;
        sockaddr_in clientSocketAddress;
//      bool doReceive;
//      std::thread rxTask;

    public:
        ListenSocket(const int32_t tcpPort, const std::string bindAddress = DEFAULT_BIND_ADDRESS);
        void setRxHandler(const ReadListener& rxListener);
        sockaddr_in accept();
        void close();
        void shutdown();

        void write(const uint8_t bytes[], int32_t length) const;
        void write(const uint8_t singleByte) const;
        void print(const std::string& text) const;
        void printLine() const;
        void printLine(const std::string& text) const;
        ~ListenSocket();

        static const std::string toIpString(const sockaddr_in rawIpEndpoint);
        static const uint32_t toTcpPort(const sockaddr_in rawIpEndpoint);
};

#endif

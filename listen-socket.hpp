//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#ifndef H_LISTEN_SOCKET
#define H_LISTEN_SOCKET

#include <cstdint>
#include <optional>
#include <string>

#include "read-listener.hpp"
#include "plain-socket.hpp"

class ListenSocket
{
//public:
//    static inline const uint8_t NEW_LINE[] = {0x0d, 0x0a};

    private:
        const static inline std::string DEFAULT_BIND_ADDRESS = "127.0.0.1";
//      const static inline int32_t RX_BUFFER_SIZE = 4096;
//      const static inline int32_t RX_SELECT_TIMEOUT_US = 100000;
//      const static inline ReadListener DEFAULT_RX_LISTENER = [](const uint8_t rxedBytes[], const int32_t length) {};

//      const bool enableReceiver;
//      const std::string& deviceName;
        int32_t socketFd;
        sockaddr_in socketEndpoint;
//      bool doReceive;
//      std::thread rxTask;

    public:
        ListenSocket(const int32_t tcpPort, const std::string bindAddress = DEFAULT_BIND_ADDRESS);
//void setRxHandler(const ReadListener& rxListener);
        std::optional<PlainSocket> accept(const uint32_t msTimeout) const;
        void close() const;
        void shutdown() const;
        ~ListenSocket();
};

#endif

//
// (c) Bit Parallel Ltd, March 2023
//

#ifndef BIT_PARALLEL_LISTEN_SOCKET_HPP
#define BIT_PARALLEL_LISTEN_SOCKET_HPP

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>

#include "plain_socket.hpp"

namespace bpl
{
    class ListenSocket
    {
        private:
            const static inline std::string DEFAULT_BIND_ADDRESS = "127.0.0.1";

            int32_t socketFd;
            sockaddr_in socketEndpoint;

        public:
            ListenSocket(const int32_t tcpPort, const std::string& bindAddress = DEFAULT_BIND_ADDRESS);
            std::optional<PlainSocket> accept(const std::chrono::milliseconds connectTimeout) const;
            void close() const;
    };
}

#endif

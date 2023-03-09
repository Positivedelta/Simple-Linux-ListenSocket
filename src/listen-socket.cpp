//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "listen-socket.hpp"

ListenSocket::ListenSocket(const int32_t tcpPort, const std::string bindAddress)
{
    // notes 1, currently only using IPv4 addressing, include the use of AF_INET6 to support IPv6
    //       2, could use htonl(INADDR_ANY) to bind the socket to all available addresses
    //
    sockaddr_in socketEndpoint;
    std::memset((uint8_t*)&socketEndpoint, 0, sizeof(socketEndpoint));
    socketEndpoint.sin_family = AF_INET;
    socketEndpoint.sin_addr.s_addr = inet_addr(bindAddress.c_str());
    socketEndpoint.sin_port = htons(tcpPort);

    socketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (socketFd < 0) throw std::string("Unable to create the underlying listen socket, reason: " + std::to_string(errno));

    // allow the socket to rebind whilst in the TCP_WAIT state
    //
    const int32_t enableReuseAddr = 1;
    const int32_t reuseAddrStatus = setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &enableReuseAddr, sizeof(int32_t));
    if (reuseAddrStatus < 0) throw std::string("Unable to set SO_REUSEADDR on the underlying listen socket, reason: " + std::to_string(errno));

    const int32_t bindStatus = bind(socketFd, (struct sockaddr*)&socketEndpoint, sizeof(socketEndpoint));
    if (bindStatus < 0) throw std::string("Unable to bind the listen socket to address: " + bindAddress);

    // set the backlog queue to 1 as this class only supports a single connection
    //
    const int32_t listenStatus = listen(socketFd, 1);
    if (listenStatus < 0) throw std::string("Unable to configure the underlying to socket listen for incoming connections, reason: " + std::to_string(errno));
}

std::optional<PlainSocket> ListenSocket::accept(const std::chrono::milliseconds connectTimeout) const
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = std::chrono::duration_cast<std::chrono::microseconds>(connectTimeout).count();

    const int32_t maxFd = 1 + socketFd;
    fd_set listenFdSet;
    FD_ZERO(&listenFdSet);
    FD_SET(socketFd, &listenFdSet);
    auto fdCount = select(maxFd, &listenFdSet, nullptr, nullptr, &timeout);
    if (fdCount > 0 && FD_ISSET(socketFd, &listenFdSet))
    {
        // note, ::accept() updates the clientSocketAddressSize, with the actual size, not used here but explains the reference
        //
        sockaddr_in clientSocketEndpoint;
        socklen_t clientSocketEndpointSize = sizeof(clientSocketEndpoint);
        const int32_t clientSocketFd = ::accept(socketFd, (sockaddr*)&clientSocketEndpoint, &clientSocketEndpointSize);
        if (clientSocketFd < 0) throw std::string("Error accepting client request, reason: " + std::to_string(errno));

        return PlainSocket(clientSocketFd, clientSocketEndpoint);
    }
    else
    {
        // the timeout condition
        //
        return std::nullopt;
    }
}

void ListenSocket::close() const
{
    ::close(socketFd);
}

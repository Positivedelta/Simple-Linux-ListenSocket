//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <cstring>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include "listen-socket.hpp"

//
// https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example/blob/master/tcp-Server.cpp
//

ListenSocket::ListenSocket(const int32_t tcpPort, const std::string bindAddress)
{
    // notes 1, currently only using IPv4 addressing, include the use of AF_INET6 to support IPv6
    //       2, could use htonl(INADDR_ANY) to bind the socket to all available addresses
    //
    sockaddr_in listenSocketAddress;
    std::memset((uint8_t*)&listenSocketAddress, 0, sizeof(listenSocketAddress));
    listenSocketAddress.sin_family = AF_INET;
    listenSocketAddress.sin_addr.s_addr = inet_addr(bindAddress.c_str());
    listenSocketAddress.sin_port = htons(tcpPort);

    listenSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listenSocketFd < 0) throw std::string("Unable to create the underlying listen socket, reason: " + std::to_string(errno));

    //
    // FIXME! set socket options, turn of Nagel's algorithm, add timeout on accept() to allow shutdown() if blocking
    //

    const int32_t bindStatus = bind(listenSocketFd, (struct sockaddr*)&listenSocketAddress, sizeof(listenSocketAddress));
    if(bindStatus < 0) throw std::string("Unable to bind the listen socket to address: " + bindAddress);

    // set the backlog queue to 1 as this class only supports a single connection
    //
    const int32_t listenStatus = listen(listenSocketFd, 1);
    if (listenStatus < 0) throw std::string("Unable to configure the underlying to socket listen for incoming connections, reason: " + std::to_string(errno));

/*      // if enabled, start up the receiver task
        //
        doReceive = true;
        if (enableReceiver)
        {
            rxTask = std::thread([this, rxListener]() {
                struct timeval timeout;
                timeout.tv_sec = 0;
                timeout.tv_usec = RX_SELECT_TIMEOUT_US;

                fd_set readFdSet;
                uint8_t rxBuffer[RX_BUFFER_SIZE];
                const int32_t maxFd = 1 + deviceFd;
                while (doReceive)
                {
                    FD_ZERO(&readFdSet);
                    FD_SET(deviceFd, &readFdSet);

                    auto fdCount = select(maxFd, &readFdSet, nullptr, nullptr, &timeout);
                    if (fdCount > 0 && FD_ISSET(deviceFd, &readFdSet))
                    {
                        // FIXME! currently ignoring read() errors, i.e. a -ve return value
                        //        perhaps add an error callback...
                        //
                        const int32_t bytesRead = ::read(deviceFd, rxBuffer, RX_BUFFER_SIZE);
                        if (bytesRead > 0) rxListener(rxBuffer, bytesRead);
                    }
                }
            });
        } */
}

void ListenSocket::setRxHandler(const ReadListener& rxListener)
{
}

sockaddr_in ListenSocket::accept()
{
    // note, ::accept() updates the clientSocketAddressSize, with the actual size, not used here but explains the reference
    //
    socklen_t clientSocketAddressSize = sizeof(clientSocketAddress);
    clientSocketFd = ::accept(listenSocketFd, (sockaddr*)&clientSocketAddress, &clientSocketAddressSize);
    if (clientSocketFd < 0) throw std::string("Error accepting client request, reason: " + std::to_string(errno));

    return clientSocketAddress;
}

void ListenSocket::close()
{
    ::close(clientSocketFd);
}

void ListenSocket::shutdown()
{
    ::close(clientSocketFd);
    ::close(listenSocketFd);
}

void ListenSocket::write(const uint8_t bytes[], int32_t length) const
{
    int32_t i = 0;
    int32_t writeStatus; 
    while ((length > 0) && (writeStatus = ::write(clientSocketFd, &bytes[i], length)) != length)
    {
        if (writeStatus < 0)
        {
            // FIXME! review these errors, are they relevant when writing to sockets?
            //
            if ((errno == EINTR) || (errno == EAGAIN) || (errno == EWOULDBLOCK)) continue;

            throw std::string("Unable to write to the client socket, reason: " + std::to_string(writeStatus));
        }

        length -= writeStatus;
        i += writeStatus;
    }
}

void ListenSocket::write(const uint8_t singleByte) const
{
    const uint8_t bytes[1] = {singleByte};
    write(bytes, 1);
}

void ListenSocket::print(const std::string& text) const
{
    write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

void ListenSocket::printLine() const
{
    write(NEW_LINE, sizeof(NEW_LINE));
}

void ListenSocket::printLine(const std::string& text) const
{
    print(text);
    write(NEW_LINE, sizeof(NEW_LINE));
}

ListenSocket::~ListenSocket()
{
/*  if (enableReceiver)
    {
        doReceive = false;
        rxTask.join();
    } */

    ::close(clientSocketFd);
    ::close(listenSocketFd);
}

//
// static helper methods
//

const std::string ListenSocket::toIpString(const sockaddr_in rawIpEndpoint)
{
//inet_ntop(AF_INET, &(clientAddress.sin_addr), clientIpString, INET_ADDRSTRLEN);
    char dottedString[INET_ADDRSTRLEN + 1];
    auto result = inet_ntop(AF_INET, &(rawIpEndpoint.sin_addr), dottedString, sizeof(dottedString));
    if (result == nullptr) throw std::string("Unable to convert " + std::to_string(rawIpEndpoint.sin_addr.s_addr) + " to a dotted IP4 address string");

    return std::string(dottedString);
}

const uint32_t ListenSocket::toTcpPort(const sockaddr_in rawIpEndpoint)
{
    return rawIpEndpoint.sin_port;
}

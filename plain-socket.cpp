//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <unistd.h>

#include "plain-socket.hpp"

PlainSocket::PlainSocket(const int32_t socketFd, const sockaddr_in socketEndpoint):
    socketFd(socketFd), socketEndpoint(socketEndpoint), doReceive(false) {
}

// FIXME! doReceive needs to be atomic
//
void PlainSocket::setRxHandler(const ReadListener& rxHandler)
{
    doReceive = true;
    rxTask = std::thread([&rxHandler, this]() {
        struct timeval timeout;
        timeout.tv_sec = 0;
        timeout.tv_usec = RX_SELECT_TIMEOUT_US;

        fd_set socketReadFdSet;
        uint8_t rxBuffer[RX_BUFFER_SIZE];
        const int32_t maxFd = 1 + socketFd;
        while (doReceive)
        {
            FD_ZERO(&socketReadFdSet);
            FD_SET(socketFd, &socketReadFdSet);

            auto fdCount = select(maxFd, &socketReadFdSet, nullptr, nullptr, &timeout);
            if (fdCount > 0 && FD_ISSET(socketFd, &socketReadFdSet))
            {
                // FIXME! currently ignoring read() errors, i.e. a -ve return value
                //        perhaps add an error callback...
                //
                const int32_t bytesRead = recv(socketFd, rxBuffer, RX_BUFFER_SIZE, 0);
                if (bytesRead > 0) rxHandler(rxBuffer, bytesRead);
            }
        }
    });
}

const std::string PlainSocket::getIpAddress() const
{
    char dottedString[INET_ADDRSTRLEN];
    auto result = inet_ntop(AF_INET, &(socketEndpoint.sin_addr), dottedString, INET_ADDRSTRLEN);
    if (result == nullptr) throw std::string("Unable to convert " + std::to_string(socketEndpoint.sin_addr.s_addr) + " to a dotted IP4 address string");

    return std::string(dottedString);
}

const uint32_t PlainSocket::getTcpPort() const
{
    return socketEndpoint.sin_port;
}

void PlainSocket::close()
{
    doReceive = false;
    rxTask.join();

    ::close(socketFd);
}

void PlainSocket::write(const uint8_t bytes[], int32_t length) const
{
    int32_t i = 0;
    int32_t writeStatus;
    while ((length > 0) && (writeStatus = send(socketFd, &bytes[i], length, 0)) != length)
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

void PlainSocket::write(const uint8_t singleByte) const
{
    const uint8_t bytes[1] = {singleByte};
    write(bytes, 1);
}

void PlainSocket::print(const std::string& text) const
{
    write(reinterpret_cast<const uint8_t*>(text.data()), text.size());
}

void PlainSocket::printLine() const
{
    write(NEW_LINE, sizeof(NEW_LINE));
}

void PlainSocket::printLine(const std::string& text) const
{
    print(text);
    write(NEW_LINE, sizeof(NEW_LINE));
}
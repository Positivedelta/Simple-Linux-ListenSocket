//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <unistd.h>

#include "plain-socket.hpp"

PlainSocket::PlainSocket(const int32_t socketFd, const sockaddr_in socketEndpoint):
    socketFd(socketFd), socketEndpoint(socketEndpoint) {
}

void PlainSocket::setRxHandler(const ReadListener& rxListener)
{
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

void PlainSocket::close() const
{
    ::close(socketFd);
}

void PlainSocket::write(const uint8_t bytes[], int32_t length) const
{
    int32_t i = 0;
    int32_t writeStatus; 
    while ((length > 0) && (writeStatus = ::write(socketFd, &bytes[i], length)) != length)
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

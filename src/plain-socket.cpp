//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <cstring>
#include <netinet/tcp.h>
#include <unistd.h>

#include "plain-socket.hpp"

PlainSocket::PlainSocket(const int32_t remoteTcpPort, const std::string remoteIpAddress):
    socketFd(makeSocket()), socketEndpoint(makeEndpoint(remoteTcpPort, remoteIpAddress)), doReceive(false) {
}

PlainSocket::PlainSocket(const int32_t socketFd, const sockaddr_in socketEndpoint):
    socketFd(socketFd), socketEndpoint(socketEndpoint), doReceive(false) {
}

PlainSocket::PlainSocket(const PlainSocket& plainSocket):
    socketFd(plainSocket.socketFd), socketEndpoint(plainSocket.socketEndpoint), doReceive(plainSocket.doReceive) {
}

// note, this can't close the socket, instances need to be copyable in order to work with std::optional, as used by ListenSocket
//
PlainSocket::~PlainSocket()
{
    doReceive = false;
    if (rxTask.joinable()) rxTask.join();
}

void PlainSocket::connect() const
{
    const int32_t connectStatus = ::connect(socketFd, (struct sockaddr*)&socketEndpoint, sizeof(socketEndpoint));
    if (connectStatus < 0) throw std::string("Unable to connect with endpoint"); // " + remoteIpAddress + ":" + std::to_string(remoteTcpPort));
}

void PlainSocket::setRxHandler(const ReadListener& rxHandler)
{
    // FIXME! this is a little heavy handed, but as it's unlikely to occur it'll do for the moment
    //        investigate making rxHandler an atomic instance propery and then do a swap
    //
    if (doReceive)
    {
        doReceive = false;
        rxTask.join();
    }

    doReceive = true;
    rxTask = std::thread([&rxHandler, this]() {
        fd_set socketReadFdSet;
        struct timeval timeout;
        uint8_t rxBuffer[RX_BUFFER_SIZE];
        const int32_t maxFd = 1 + socketFd;
        while (doReceive)
        {
            FD_ZERO(&socketReadFdSet);
            FD_SET(socketFd, &socketReadFdSet);

            timeout.tv_sec = 0;
            timeout.tv_usec = RX_SELECT_TIMEOUT_US;

            // note, the timeout allows the thread to exit when doReceive is false
            //
            const auto fdCount = select(maxFd, &socketReadFdSet, nullptr, nullptr, &timeout);
            if (fdCount > 0 && FD_ISSET(socketFd, &socketReadFdSet))
            {
                // notes 1, errors are returned as a 0 or a -ve length
                //       2, a 0 return indicates that the underlying socket has been closed
                //       3, this read thread will exit on error
                //
                const int32_t bytesRead = recv(socketFd, rxBuffer, RX_BUFFER_SIZE, 0);
                rxHandler(rxBuffer, bytesRead);
                if (bytesRead <= 0) doReceive = false;
            }
        }
    });
}

// turn on / off Nagel's algorithm (on by default)
// FIXME! this could be applied to the listen socket and then inherited, something to consider
//
void PlainSocket::setTcpNoDelay(const bool tcpNoDelay) const
{
    const int32_t noDelayFlag = (tcpNoDelay) ? 1 : 0;
    const int32_t noDelayStatus = setsockopt(socketFd, IPPROTO_TCP, TCP_NODELAY, &noDelayFlag, sizeof(int32_t));
    if (noDelayStatus < 0) throw std::string("Unable to disable Nagel's algorithm on the underlying listen socket, reason: " + std::to_string(errno));
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
    if (rxTask.joinable()) rxTask.join();

    ::close(socketFd);
}

void PlainSocket::write(const uint8_t bytes[], int32_t length) const
{
    int32_t i = 0;
    int32_t writeStatus;
    while ((length > 0) && (writeStatus = send(socketFd, &bytes[i], length, MSG_NOSIGNAL)) != length)
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

//
// private methods used by the port/ip address constructor
//

int32_t PlainSocket::makeSocket() const
{
    int32_t plainSocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (plainSocketFd < 0) throw std::string("Unable to create the client socket, reason: " + std::to_string(errno));

    return plainSocketFd;
}

sockaddr_in PlainSocket::makeEndpoint(const int32_t remoteTcpPort, std::string remoteIpAddress) const
{
    sockaddr_in plainSocketEndpoint;
    std::memset((uint8_t*)&plainSocketEndpoint, 0, sizeof(plainSocketEndpoint));
    plainSocketEndpoint.sin_family = AF_INET;
    plainSocketEndpoint.sin_addr.s_addr = inet_addr(remoteIpAddress.c_str());
    plainSocketEndpoint.sin_port = htons(remoteTcpPort);

    return plainSocketEndpoint;
}

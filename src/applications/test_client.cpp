//
// (c) Bit Parallel Ltd, March 2023
//

#include <iostream>

#include "read_listener.hpp"
#include "plain_socket.hpp"

int32_t main()
{
    static int32_t remotePort = 11000;
    static std::string remoteIpAddress = "192.168.1.233";

    const bpl::ReadListener rxHandler = [](const uint8_t bytes[], const int32_t length) {
        if (length < 1) return;

        const auto text = std::string(reinterpret_cast<const char*>(bytes), length);
        std::cout << text << std::flush;
    };

    auto socket = bpl::PlainSocket(remotePort, remoteIpAddress);
    try
    {
        socket.connect();
        socket.setRxHandler(rxHandler);
        socket.printLine("Hello server, this is the client!");

        auto userInput = std::string();
        while (true)
        {
            std::getline(std::cin, userInput);
            if (userInput.compare("quit") == 0) break;

            socket.printLine(userInput);
        }
    }
    catch (const std::string& message)
    {
        std::cout << "Unexpected exception, details: " << message << std::endl;
    }

    socket.close();

    return 0;
}

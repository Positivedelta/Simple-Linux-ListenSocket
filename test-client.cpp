//
// (c) Bit Parallel Ltd (Max van Daalen), March 2023
//

#include <iostream>

#include "read-listener.hpp"
#include "plain-socket.hpp"

int32_t main()
{
    static int32_t remotePort = 11000;
    static std::string remoteIpAddress = "192.168.1.233";

    const ReadListener rxHandler = [](const uint8_t bytes[], const int32_t length) {
        if (length < 1) return;

        const auto text = std::string(reinterpret_cast<const char*>(bytes), length);
        std::cout << text << std::flush;
    };

    try
    {
        auto socket = PlainSocket(remotePort, remoteIpAddress);
        socket.setRxHandler(rxHandler);
        socket.printLine("Hello server, this is the client!");

        auto userInput = std::string();
        while (true)
        {
            std::getline(std::cin, userInput);
            if (userInput.compare("quit") == 0) break;

            socket.printLine(userInput);
        }

        socket.close();
    }
    catch (const std::string& message)
    {
        std::cout << "Unexpected exception, details: " << message << std::endl;
    }

    return 0;
}

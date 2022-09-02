//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <thread>

#include "listen-socket.hpp"

int32_t main()
{
    static int32_t bindPort = 11000;
    static std::string bindAddress = "192.168.1.192";
//  static std::string bindAddress = "192.168.43.153";
    static uint32_t acceptTimeoutMs = 100;

    try
    {
        // creates and binds the underlying listen socket using the provided address and port number
        //
        auto listenSocket = ListenSocket(bindPort, bindAddress);
        std::cout << "Listen socket created, endpoint: (" << bindAddress << ":" << bindPort << ")\n";
        std::cout << "Waiting for client connections...\n";

        auto running = true;
        while (running)
        {
            // wait for a client connection...
            //
            auto socket = listenSocket.accept(acceptTimeoutMs);
            if (!socket) continue;
            std::cout << "Client connected, endpoint: (" << socket->getIpAddress() << ":" << socket->getTcpPort() << ")\n";

            // the receive handlerecho, echo text to the console and detect the "quit" option
            //
            socket->setRxHandler([&running](const uint8_t bytes[], const int32_t length) {
                const auto text = std::string(reinterpret_cast<const char*>(bytes), length);
                std::cout << text << std::flush;

                running = text.compare("quit") == 0;
            });

            // talk to the client
            //
            socket->printLine("Hello World!");
            socket->print("Please type some text: ");

            int32_t i = 0;
            while (running && (i++ < 20)) std::this_thread::sleep_for(std::chrono::seconds(1));

            // time is up, tidy up the output and close the socket
            // loop for the next connection... unles "quit" was typed by the user
            //
            socket->printLine();
            socket->close();
            std::cout << "Client disconnected\n";
        }

        listenSocket.close();
    }
    catch (const std::string& message)
    {
        std::cout << "Unexpected exception, details: " + message << std::endl;
    }

    return 0;
}

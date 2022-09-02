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
    static std::string bindAddress = "192.168.43.153";
//  static std::string bindAddress = "192.168.1.192";
    static uint32_t acceptTimeoutMs = 100;

    try
    {
        // creates and binds the underlying listen socket using the provided address and port number
        // the destructor or the shutdown() method will close the underlying listen socket
        //
        auto listenSocket = ListenSocket(bindPort, bindAddress);
        std::cout << "Listen socket created, endpoint: (" << bindAddress << ":" << bindPort << ")\n";
        std::cout << "Waiting for client connections...\n";

        auto running = true;
        while (running)
        {
//          // if required, the receive handler must be set before invoking accept()
//          // when set, an associated receive thread is created and started
//          //
//          listenSocket.setRxHandler([&running](const uint8_t bytes[], const int32_t length) {
//              const auto text = std::string(reinterpret_cast<const char*>(bytes), length);
//              std::cout << text << std::flush;
//
//              running = text.compare("quit") == 0;
//          });

            // wait for a client connection...
            // then say hello and sleep for 20 seconds to allow any RXed to be received and displayed
            //
            const auto socket = listenSocket.accept(acceptTimeoutMs);
            if (!socket) continue;

            std::cout << "Client connected, endpoint: (" << socket->getIpAddress() << ":" << socket->getTcpPort() << ")\n";
            socket->printLine("Hello World!");
            socket->print("Please type some text: ");

            int32_t i = 0;
            while (running && (i++ < 20)) std::this_thread::sleep_for(std::chrono::seconds(1));

            socket->printLine("Test...");

            // time is up, tidy up the output and close the socket
            // loop for the next connection... unles "quit" was typed by the user
            //
            socket->printLine();
            socket->close();
            std::cout << "Client disconnected\n";
        }

        listenSocket.shutdown();
    }
    catch (const std::string& message)
    {
        std::cout << "Unexpected exception, details: " + message << std::endl;
    }

    return 0;
}

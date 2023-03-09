//
// (c) Bit Parallel Ltd (Max van Daalen), September 2022
//

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <cstring>
#include <thread>

#include "read-listener.hpp"
#include "listen-socket.hpp"

int32_t main()
{
    static int32_t bindPort = 11000;
    static std::string bindAddress = "192.168.1.233";

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
            auto socket = listenSocket.accept(std::chrono::milliseconds(100));
            if (!socket) continue;

            auto connected = true;
            std::cout << "Client connected, endpoint: (" << socket->getIpAddress() << ":" << socket->getTcpPort() << ")\n";

            // communicate with the client...
            // notes 1, designed to be used with PuTTy in raw mode, each line of text is only sent when RET is pressed (default behaviour) 
            //       2, the receive handler echos text to the console and detects the "quit" option
            //
            const ReadListener rxHandler = [&running, &connected](const uint8_t bytes[], const int32_t length) {
                if (running)
                {
                    // notes 1, a zero or -ve length indicates an error
                    //       2, typically a zero represents a disconnect
                    //       3, the PlainSocket receiver thread will exit as the underlying socket has failed or been closed
                    //       4, in this example code an error forced a re-connect
                    //
                    if (!(connected = (length > 0))) return;

                    // ignore the CRLF, PuTTy sends the text and CRLF separately when in raw mode
                    //
                    if ((length == 2) && (std::memcmp(bytes, PlainSocket::NEW_LINE, 2) == 0)) return;

                    const auto text = std::string(reinterpret_cast<const char*>(bytes), length);
                    std::cout << "[" << text.length() << "] " << text << "\n";

                    running = text.compare("quit") != 0;
                }
            };

            socket->setRxHandler(rxHandler);
            socket->printLine("Hello World!");
            socket->print("Please type some text: ");

            int32_t i = 0;
            while (running && connected && (i++ < 20)) std::this_thread::sleep_for(std::chrono::seconds(1));

            // time is up, tidy up the output and close the socket
            // loop for the next connection... unless "quit" was typed by the user
            //
            socket->printLine();
            socket->close();
            std::cout << "Client disconnected\n";
        }

        listenSocket.close();
        std::cout << "ListenSocket closed\n";
    }
    catch (const std::string& message)
    {
        std::cout << "Unexpected exception, details: " << message << std::endl;
    }

    return 0;
}

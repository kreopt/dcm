// TCP: http://think-async.com/Asio/asio-1.10.2/doc/
// IPC: http://www.boost.org/doc/libs/1_56_0/doc/html/interprocess.html

#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <asio.hpp>

#include "TcpServer.h"
#include "TcpClient.h"

using asio::ip::tcp;

// Usage example
int main(int argc, char *argv[]) {
    try {
        // I. Start server
        // Check command line parameters
        if (argc < 2) {
            std::cerr << "Usage: Server <port> [| <port>]" << std::endl;
            return 1;
        }

        // Create a server object
        asio::io_service io_service;
        TcpServer::TcpServerPtrs servers;
        for (int i = 1; i < argc; i++) {
            tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[i]));
            TcpServer::TcpServerPtr server(new TcpServer(io_service, endpoint));
            servers.push_back(server);
        }

        // Run the io_service
        io_service.run();

        // II. Start client
        // Check command line parameters
        if (argc != 3) {
            std::cerr << "Usage: Client <host> <port>" << std::endl;
            return 1;
        }

        // Create client object
        //asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], argv[2]);
        tcp::resolver::iterator iterator = resolver.resolve(query);
        TcpClient client(io_service, iterator);
        asio::thread client_thread(std::bind(&asio::io_service::run, &io_service));

        // Send message
        char line[NetMessage::MAX_BODY_LEN + 1] = "Sample message";
        NetMessage message;
        message.body_length(strlen(line));
        memcpy(message.body(), line, message.body_length());
        message.encode_header();
        client.write(message);
        client.close();
        client_thread.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

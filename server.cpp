// TCP: http://think-async.com/Asio/asio-1.10.2/doc/
// IPC: http://www.boost.org/doc/libs/1_56_0/doc/html/interprocess.html

#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <asio.hpp>

#include "socket/stream_socket_server.hpp"

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
//        dcm::tcp_server::ServerPtrs servers;
//        for (int i = 1; i < argc; i++) {
//            asio::ip::tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[i]));
//            dcm::tcp_server::ServerPtr server(new dcm::tcp_server(io_service, endpoint));
//            servers.push_back(server);
//        }

        std::shared_ptr<dcm::server> server = dcm::make_server(dcm::connection_type::unix, "unix_server.sock");
        server->start();
        // Run the io_service
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

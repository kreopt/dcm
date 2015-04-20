#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <asio.hpp>
#include <thread>

#include "socket/dcm_socket_server.hpp"

using asio::ip::tcp;

// Usage example
int main(int argc, char *argv[]) {
    try {
        // I. Start receiver
        // Check command line parameters
        if (argc < 2) {
            std::cerr << "Usage: Server <port> [| <port>]" << std::endl;
            return 1;
        }

        // Create a receiver object
//        dcm::tcp_server::ServerPtrs servers;
//        for (int i = 1; i < argc; i++) {
//            asio::ip::tcp::endpoint endpoint(tcp::v4(), std::stoi(argv[i]));
//            dcm::tcp_server::ServerPtr receiver(new dcm::tcp_server(io_service, endpoint));
//            servers.push_back(receiver);
//        }

        auto server = dcm::streamsocket::make_receiver(interproc::conn_type::unix, "unix_server.sock");
        server->on_message = [](const dcm::signal &_message) {
            std::cout << "caught " << _message.name() << " signal with data: "<< _message.get_data("data") << std::endl;
            // TODO: run subscription resolver
            // TODO: IPC: MP send header
            // TODO: IPC: ShMem send body
        };
        server->start();
        // Run the io_service
        std::this_thread::sleep_for(std::chrono::seconds(5));
        server->wait_until_stopped();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}

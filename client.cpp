// TCP: http://think-async.com/Asio/asio-1.10.2/doc/
// IPC: http://www.boost.org/doc/libs/1_56_0/doc/html/interprocess.html

#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <thread>
#include <asio.hpp>
#include <fstream>

#include "socket/stream_socket_client.hpp"

using asio::ip::tcp;

// Usage example
int main(int argc, char *argv[]) {
    try {
        // II. Start client
        // Check command line parameters
        if (argc != 3) {
            std::cerr << "Usage: Client <host> <port>" << std::endl;
            return 1;
        }

        // Create client object
//        auto io_service = std::make_shared<asio::io_service>();
//        tcp::resolver resolver(*io_service);
//        tcp::resolver::query query(argv[1], argv[2]);
//        tcp::resolver::iterator iterator = resolver.resolve(query);
//        tcp_client client(*io_service, iterator);
//        std::thread client_thread([&io_service](){io_service->run();});

        auto client = dcm::make_client(dcm::connection_type::unix, "/home/wf34/h2w.sock");
        client->connect();

        int i = 0;
        while (true) {
            dcm::message message;
            message.header["signal"] = "dcm.sample.signal";
            message.body["data"] = "sample data " + std::to_string(i);

            client->send(message);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i++;
            //if (i>10) break;
        }

        client->close();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
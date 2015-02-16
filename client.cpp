// TCP: http://think-async.com/Asio/asio-1.10.2/doc/
// IPC: http://www.boost.org/doc/libs/1_56_0/doc/html/interprocess.html

#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <thread>
#include <asio.hpp>
#include <fstream>

#include "TcpServer.h"
#include "TcpClient.h"

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
//        TcpClient client(*io_service, iterator);
//        std::thread client_thread([&io_service](){io_service->run();});

        //TcpClient client(argv[1], argv[2]);
        //client.run();

        // Send message
        NetMessage message;

        message.set_header("test", "123");
        message.set_header("test1", "1232");
        message.set_data("testb", "1233312");

        auto encoded = message.encode();
        std::cout << encoded << std::endl;

        NetMessage message1;
        dcm::ibufstream bs(encoded);
        int32_t len = 0;
        dcm::read_size(bs, len);
        message1.decode_header(dcm::buffer(encoded, sizeof(int32_t), len));

        dcm::ibufstream bbs(dcm::buffer(encoded, sizeof(int32_t)+len));
        int32_t len1 = 0;
        dcm::read_size(bbs, len1);
        message1.decode_body(dcm::buffer(encoded.begin()+2*sizeof(int32_t)+len, encoded.end()));

        encoded = message1.encode();
        std::cout << encoded << std::endl;

        // TODO: promise
        //client.write(message);
        //client.close();
        //client_thread.join();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
// TCP: http://think-async.com/Asio/asio-1.10.2/doc/
// IPC: http://www.boost.org/doc/libs/1_56_0/doc/html/interprocess.html

#define ASIO_STANDALONE 1

#include <iostream>
#include <functional>
#include <thread>
#include <asio.hpp>
#include <fstream>
#include <core/message.hpp>

#include "json11.hpp"
#include "socket/dcm_socket_client.hpp"

using asio::ip::tcp;

// Usage example
int main(int argc, char *argv[]) {
    try {
        // II. Start sender
        // Check command line parameters
        if (argc < 1) {
            std::cerr << "Usage: sender <unix_socket>" << std::endl;
            return 1;
        }

        // Create sender object
//        auto io_service = std::make_shared<asio::io_service>();
//        tcp::resolver resolver(*io_service);
//        tcp::resolver::query query(argv[1], argv[2]);
//        tcp::resolver::iterator iterator = resolver.resolve(query);
//        tcp_client sender(*io_service, iterator);
//        std::thread client_thread([&io_service](){io_service->run();});

        auto sender = dcm::streamsocket::make_sender(interproc::streamsocket_type::unix, argv[1]);
        sender->connect();

        int i = 0;
        while (true) {
            std::string sig_name, json_data;
            std::cout << "signal: ";
            std::getline(std::cin, sig_name);
            if (sig_name.empty()) {
                break;
            }
            std::cout << std::endl << "data in json: ";
            std::getline(std::cin, json_data);

            dcm::message message;
            message.header["signal"] = sig_name;
            try {
                std::string err;
                json11::Json json = json11::Json::parse(json_data, err);
                if (!err.empty()) {
                    std::cerr << err << std::endl;
                    continue;
                }

                for (auto &e: json.object_items()) {
                    message.body[e.first] = e.second.string_value();
                }
            } catch(...){
                std::cerr << "bad input" << std::endl;
                continue;
            }

            sender->send(message);
            std::this_thread::sleep_for(std::chrono::seconds(1));
            i++;
            //if (i>10) break;
        }

        sender->close();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
#ifndef TCP_CLIENT
#define TCP_CLIENT

#include <asio.hpp>
#include <thread>

#include "NetMessage.h"
#include "messagereader.hpp"
using asio::ip::tcp;

// TcpClient class
class TcpClient : public dcm::message_reader {
private:
    std::shared_ptr<asio::io_service> io_service_;
    std::shared_ptr<tcp::socket>      socket_;
    tcp::resolver::iterator iterator_;

    std::thread client_thread_;
    NetMessage::MessageQueue output_messages_;

    // Event handlers
    void handle_connect(const asio::error_code &error);

    void handle_write(const asio::error_code &error);

    // Client operations
    void do_write(NetMessage message);

    void do_close();

public:
    // Constructor
    TcpClient(const std::string &_host, const std::string &_port);

    // Client operation interfaces
    void send(const NetMessage &message);

    void run();
    void close();
};

#endif

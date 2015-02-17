#include <error.h>
#include "TcpClient.h"


// Write operation interface
void TcpClient::send(const NetMessage &message) {
    // TODO: make new promise & pass to do_write
    io_service_->post(std::bind(&TcpClient::do_write, this, message));
    // TODO: return thenable
}

// Close operation interface
void TcpClient::close() {
    io_service_->post(std::bind(&TcpClient::do_close, this, asio::error_code()));
}

//
// Client operations
//

// Write operation
void TcpClient::do_write(NetMessage message) {
    bool write_in_progress = !output_messages_.empty();

    // Add message to queue
    output_messages_.push_back(message);
    // TODO: add promise to queue

    // Start async write for new queue
    if (!write_in_progress) {
        // TODO: pass promise from queue to handle_write
        auto msg = output_messages_.front().encode();
        asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()), std::bind(&TcpClient::handle_write, this, std::placeholders::_1));
    }
}

// Close operation
void TcpClient::do_close(const asio::error_code &error) {
    std::cout << error.message() << std::endl;
    // Close connection
    if (socket_->is_open()) {
        socket_->close();
    }
    io_service_->stop();
}

//
// Event handlers
//

// Connect handler
void TcpClient::handle_connect(const asio::error_code &error) {
    if (!error) {
        // Start async read when connection succeeded
        read_size();
    } else {
        do_close(error);
    }
}


// Write message handler
void TcpClient::handle_write(const asio::error_code &error) {
    // TODO: resolve or reject promise
    if (!error) {
        // Check if queue non empty
        output_messages_.pop_front();
        if (!output_messages_.empty()) {
            // Async write data to buffer
            auto msg = output_messages_.front().encode();
            asio::async_write(*socket_, asio::buffer(msg, msg.size()), std::bind(&TcpClient::handle_write, this, std::placeholders::_1));
        }
    }
    else {
        // Close connection else
        std::cout << error.message() << std::endl;
        do_close();
    }
}

TcpClient::TcpClient(const std::string &_host, const std::string &_port) {
    io_service_ = std::make_shared<asio::io_service>();
    tcp::resolver resolver(*io_service_);
    tcp::resolver::query query(_host, _port);
    iterator_ = resolver.resolve(query);
    socket_ = std::make_shared<tcp::socket>(*io_service_);

    set_socket(socket_);
    on_fail = std::bind(&TcpClient::do_close, this, std::placeholders::_1);
}

void TcpClient::run() {
    asio::async_connect(*socket_, iterator_, std::bind(&TcpClient::handle_connect, this, std::placeholders::_1));
    io_service_->run();
}

TcpClient::~TcpClient() {
    do_close();
}

void TcpClient::wait_unfinished() {
}

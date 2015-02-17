#include <error.h>
#include "NetSession.h"

// Constructor
NetSession::NetSession(asio::io_service &io_service, NetRoom &room) : socket_(std::make_shared<tcp::socket>(io_service)), room_(room) {
    set_socket(socket_);
    on_fail = [this](const asio::error_code &error){room_.leave(shared_from_this());};
    on_read = [this](){
        room_.deliver(input_message_);
    };
}

std::shared_ptr<tcp::socket> &NetSession::socket() {
    return socket_;
}

// Start session
void NetSession::start() {
    // Join room
    room_.join(shared_from_this());

    // Check input buffer
    read_size();
}

// Deliver message to client
void NetSession::deliver(const NetMessage &message) {
    bool write_in_progress = !output_messages_.empty();
    output_messages_.push_back(message);

    // Start async write for new queue
    if (!write_in_progress) {

    }
}

// Write message handler
void NetSession::handle_write(const asio::error_code &error) {
    if (!error) {
        // Check if queue non empty
        output_messages_.pop_front();
        if (!output_messages_.empty()) {
            // Async write data to buffer
            write_messages();
        }
    }
    else {
        // Leave room else
        room_.leave(shared_from_this());
    }
}

void NetSession::write_messages() {
    auto msg = output_messages_.front().encode();
    asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()), std::bind(&NetSession::handle_write, shared_from_this(), std::placeholders::_1));
}

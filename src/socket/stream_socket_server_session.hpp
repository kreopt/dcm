#ifndef NET_SESSION
#define NET_SESSION

#include <asio.hpp>

#include "NetRoom.h"
#include "socket/stream_socket_message_reader.hpp"

namespace dcm {
    template <typename socket_type>
    class stream_socket_server_session : public NetParticipant,
                                         public dcm::stream_socket_message_reader<socket_type>,
                                         public std::enable_shared_from_this<stream_socket_server_session<socket_type>> {
    private:
        std::shared_ptr<socket_type> socket_;
        NetRoom &room_;
        dcm::message::MessageQueue output_messages_;

        void write_messages() {
            auto msg = output_messages_.front().encode();
            asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()),
                    std::bind(&stream_socket_server_session<socket_type>::handle_write, this->shared_from_this(), std::placeholders::_1));
        }

        // Event handlers
        void handle_write(const asio::error_code &error) {
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
                room_.leave(this->shared_from_this());
            }
        }

    public:

        // Constructor
        stream_socket_server_session(asio::io_service &io_service, NetRoom &room)
                : socket_(std::make_shared<socket_type>(io_service)), room_(room) {
            this->set_socket(socket_);
            this->on_fail = [this](const asio::error_code &error) {
                room_.leave(this->shared_from_this());
            };
            this->on_read = [this]() {
                room_.deliver(this->input_message_);
            };
        }

        // Session operations
        void start() {
            // Join room
            room_.join(this->shared_from_this());

            // Check input buffer
            this->read_size();
        }

        void deliver(const dcm::message &message) {
            bool write_in_progress = !output_messages_.empty();
            output_messages_.push_back(message);

            // Start async write for new queue
            if (!write_in_progress) {

            }
        }

        // Overloads
        std::shared_ptr<socket_type> &socket() {
            return socket_;
        }
    };
}

#endif

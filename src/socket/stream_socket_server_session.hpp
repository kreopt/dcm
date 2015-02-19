#ifndef NET_SESSION
#define NET_SESSION

#include <asio.hpp>

#include "NetRoom.h"
#include "socket/stream_socket_message_reader.hpp"
#include "socket/stream_socket_message_writer.hpp"
namespace dcm {
    template <typename socket_type>
    class stream_socket_server_session : public NetParticipant,
                                         public dcm::stream_socket_message_reader<socket_type>,
                                         public dcm::stream_socket_message_writer<socket_type>,
                                         public std::enable_shared_from_this<stream_socket_server_session<socket_type>> {
    private:
        std::shared_ptr<socket_type> socket_;
        NetRoom &room_;

    public:

        // Constructor
        stream_socket_server_session(asio::io_service &io_service, NetRoom &room)
                : socket_(std::make_shared<socket_type>(io_service)), room_(room) {
            this->set_reader_socket(socket_);
            this->set_writer_socket(socket_);
            this->on_read_fail = [this](const asio::error_code &error) {
                room_.leave(this->shared_from_this());
            };
            this->on_write_fail = this->on_read_fail;
            this->on_read = [this](const message &_message) {
                room_.deliver(_message);
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
            bool write_in_progress = !this->output_messages_.empty();
            this->output_messages_.push_back(message);

            // Start async write for new queue
            if (!write_in_progress) {
                this->write_messages();
            }
        }

        // Overloads
        std::shared_ptr<socket_type> &socket() {
            return socket_;
        }
    };
}

#endif

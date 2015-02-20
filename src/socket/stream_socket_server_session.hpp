#ifndef NET_SESSION
#define NET_SESSION

#include <asio.hpp>
#include <error.h>

#include "socket/stream_socket_message_reader.hpp"
#include "socket/stream_socket_message_writer.hpp"
namespace dcm {
    template <typename socket_type>
    class stream_socket_server_session : public dcm::stream_socket_message_reader<socket_type>,
                                         public dcm::stream_socket_message_writer<socket_type>,
                                         public std::enable_shared_from_this<stream_socket_server_session<socket_type>> {
    private:
        std::shared_ptr<socket_type> socket_;

    public:

        // Constructor
        stream_socket_server_session(asio::io_service &io_service)
                : socket_(std::make_shared<socket_type>(io_service)) {
            this->set_reader_socket(socket_);
            this->set_writer_socket(socket_);
            this->on_read_fail_ = [this](const asio::error_code &error) {
                std::cerr << error.message() << std::endl;
                if (on_error) on_error(this->shared_from_this());
            };
            this->on_write_fail_ = this->on_read_fail_;

        }

        ~stream_socket_server_session(){
            if (socket_->is_open()) {
                socket_->close();
            }
            std::cout << "destroy session" << std::endl;
        }

        // Session operations
        void start() {
            // Check input buffer
            this->read_size_block();
        }

        // Overloads
        std::shared_ptr<socket_type> socket() {
            return socket_;
        }

        std::function<void(std::shared_ptr<stream_socket_server_session<socket_type>> _session)> on_error;
    };
}

#endif

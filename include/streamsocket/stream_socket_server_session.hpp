#ifndef NET_SESSION
#define NET_SESSION

#include <asio.hpp>
#include <error.h>

#include <iostream>

#include "stream_socket_reader.hpp"
#include "stream_socket_writer.hpp"
namespace netlib {
    template <typename socket_type>
    class stream_socket_server_session : public std::enable_shared_from_this<stream_socket_server_session<socket_type>> {
    private:
        std::shared_ptr<socket_type> socket_;
        std::shared_ptr<stream_socket_reader<socket_type>>                   reader_;
        std::shared_ptr<stream_socket_writer<socket_type>>                   writer_;

    public:

        // Constructor
        explicit stream_socket_server_session(asio::io_service &io_service)
                : socket_(std::make_shared<socket_type>(io_service)) {

            reader_ = std::make_shared<stream_socket_reader<socket_type>>(socket_);
            writer_ = std::make_shared<stream_socket_writer<socket_type>>(socket_);

            this->writer_->on_fail = this->reader_->on_fail = [this](const asio::error_code &error) {
                if (error) {
                    std::cerr << error.message() << std::endl;
                    if (on_error) on_error(this->shared_from_this());
                }
            };

            this->on_message = reader_->on_success;
        }

        ~stream_socket_server_session(){
            if (socket_->is_open()) {
                socket_->close();
            }
            std::cout << "destroy session" << std::endl;
        }

        void send(const netlib::buffer &_buf){
            this->writer_->write(_buf);
        }

        // Session operations
        void start() {
            // Check input buffer
            this->reader_->read();
        }

        // Overloads
        std::shared_ptr<socket_type> socket() {
            return socket_;
        }

        std::function<void(netlib::buffer &&_buf)> on_message;
        std::function<void(std::shared_ptr<stream_socket_server_session<socket_type>> _session)> on_error;
    };
}

#endif

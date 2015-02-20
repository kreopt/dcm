#ifndef __SF_MESSAGE_WRITER_
#define __SF_MESSAGE_WRITER_

#include <functional>
#include <asio/error_code.hpp>
#include "core/buffer.hpp"
#include "core/message.hpp"

namespace dcm {
    // TODO: timeouts
    template <typename socket_type>
    class stream_socket_message_writer {
    protected:
        std::shared_ptr<socket_type> socket_;

        std::function<void(const asio::error_code &)> on_write_fail_;

        inline void set_writer_socket(std::shared_ptr<socket_type> &_socket) {
            socket_ = _socket;
        };
    public:

        void write_message(const dcm::message &_message) {
            auto msg = _message.encode();
            asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()),
                    std::bind(&stream_socket_message_writer<socket_type>::handle_write, this, std::placeholders::_1));
        }

        // Event handlers
        void handle_write(const asio::error_code &error) {
            if (!error) {
                if (on_write) on_write();
            }
            else {
                if (on_write_fail_) on_write_fail_(error);
            }
        }


        std::function<void()> on_write;
    };
}

#endif
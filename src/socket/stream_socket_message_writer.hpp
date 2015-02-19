#ifndef __SF_MESSAGE_WRITER_
#define __SF_MESSAGE_WRITER_

#include <functional>
#include <asio/error_code.hpp>
#include "core/buffer.hpp"
#include "core/message.hpp"

namespace dcm {
    template <typename socket_type>
    class stream_socket_message_writer {
    protected:
        std::shared_ptr<socket_type> socket_;
        dcm::message::MessageQueue output_messages_;

        inline void set_writer_socket(std::shared_ptr<socket_type> &_socket) {
            socket_ = _socket;
        };
    public:

        void write_messages() {
            auto msg = output_messages_.front().encode();
            asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()),
                    std::bind(&stream_socket_message_writer<socket_type>::handle_write, this, std::placeholders::_1));
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
                if (on_write_fail) on_write_fail(error);
            }
        }


        std::function<void()> on_write;
        std::function<void(const asio::error_code &)> on_write_fail;
    };
}

#endif
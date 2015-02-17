#ifndef __SF_MESSAGE_READER_
#define __SF_MESSAGE_READER_

#include <functional>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include "core/buffer.hpp"
#include "core/message.hpp"

namespace dcm {
    template <typename socket_type>
    class stream_socket_message_reader {
    protected:
        bool is_header_;
        dcm::block_size_t block_size_;
        dcm::buffer input_buffer_;
        dcm::message input_message_;
        std::shared_ptr<socket_type> socket_;

        inline void set_socket(std::shared_ptr<socket_type> &_socket) {
            socket_ = _socket;
        };
    public:

        stream_socket_message_reader() : is_header_(true) {};

        void read_size() {
            input_buffer_.resize(dcm::BLOCK_SIZE_SIZE);
            char *bufstart = &input_buffer_[0];
            asio::async_read(*socket_, asio::buffer(bufstart, dcm::BLOCK_SIZE_SIZE),
                    std::bind(&dcm::stream_socket_message_reader<socket_type>::handle_read_size, this, std::placeholders::_1));
        }

        void read_data() {
            input_buffer_.resize(block_size_);
            char *bufstart = &input_buffer_[0];
            asio::async_read(*socket_, asio::buffer(bufstart, block_size_),
                    std::bind(&dcm::stream_socket_message_reader<socket_type>::handle_read_data, this, std::placeholders::_1));
        }

        void handle_read_size(const asio::error_code &_error) {
            if (!_error) {
                dcm::ibufstream bs(input_buffer_);
                dcm::read_size(bs, block_size_);
                read_data();
            } else {
                // Close connection else
                if (on_fail) {
                    on_fail(_error);
                }
            }
        }

        void handle_read_data(const asio::error_code &_error) {
            if (!_error) {
                // Try to read next message
                if (is_header_) {
                    input_message_.decode_header(input_buffer_);
                } else {
                    input_message_.decode_body(input_buffer_);
                    if (on_read) {
                        on_read();
                    }
                }
                is_header_ = !is_header_;
                read_size();
            }
            else {
                // Close connection else
                if (on_fail) {
                    on_fail(_error);
                }
            }
        }

        std::function<void()> on_read;
        std::function<void(const asio::error_code &)> on_fail;
    };
}

#endif
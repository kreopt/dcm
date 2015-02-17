#ifndef __SF_MESSAGE_READER_
#define __SF_MESSAGE_READER_

#include <functional>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include "buffer.hpp"
#include "NetMessage.h"

namespace dcm {
    class message_reader {
    protected:
        bool        is_header_;
        size_t      block_size_;
        dcm::buffer input_buffer_;
        NetMessage  input_message_;
        std::shared_ptr<asio::ip::tcp::socket>      socket_;

        inline void set_socket(std::shared_ptr<asio::ip::tcp::socket> &_socket){
            socket_ = _socket;
        };
    public:

        message_reader() : is_header_(true){};

        void read_size();
        void read_data();
        void handle_read_size(const asio::error_code &error);
        void handle_read_data(const asio::error_code &error);

        std::function<void()> on_read;
        std::function<void(const asio::error_code&)> on_fail;
    };
}

#endif
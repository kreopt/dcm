#ifndef __NETLIB_BUFFER_READER_
#define __NETLIB_BUFFER_READER_

#include <functional>
#include <asio/error_code.hpp>
#include <asio/ip/tcp.hpp>
#include <asio/read.hpp>
#include <asio/streambuf.hpp>
#include <asio/buffers_iterator.hpp>
#include "netlib.hpp"

namespace netlib {
    // TODO: timeouts
    template <typename socket_type>
    class stream_socket_reader {
        using handler_t = std::function<void(const asio::error_code &_error)>;
    protected:
        netlib::buffer buffer_;
        asio::streambuf response_;
        bool streambuf_read_;
        std::shared_ptr<socket_type> socket_;
        handler_t default_handler_;
    public:

        explicit stream_socket_reader(std::shared_ptr<socket_type> _socket) : socket_(_socket) {
            default_handler_ = std::bind(&netlib::stream_socket_reader<socket_type>::handle_read, this, std::placeholders::_1);
        };

        // TODO: protect against concurrent read into the same buffer
        void read(std::size_t _size = 0, handler_t _handler = nullptr) {
            if (_size) {
                streambuf_read_=false;
                buffer_.resize(_size);
                char *bufstart = &buffer_[0];
                asio::async_read(*socket_, asio::buffer(bufstart, _size), (_handler ? _handler : default_handler_));
            } else {
                streambuf_read_=true;
                asio::async_read(*socket_, response_,
                        asio::transfer_at_least(1),
                        (_handler ? _handler : default_handler_));
            }
        }

        void handle_read(const asio::error_code &_error) {
            if (!_error) {
                if (on_success) {
                    if (streambuf_read_){
                        buffer_.clear();
                        asio::streambuf::const_buffers_type bufs = response_.data();
                        buffer_ = netlib::buffer(asio::buffers_begin(bufs), asio::buffers_begin(bufs) + response_.size());
                    }
                    on_success(buffer_);
                }
            } else {
                if (on_fail) on_fail(_error);
            }
        }

        std::function<void(const asio::error_code &)>          on_fail;
        std::function<void(netlib::buffer &&_buffer)>       on_success;
    };
}

#endif
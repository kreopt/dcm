#ifndef __NETLIB_BUFFER_WRITER_
#define __NETLIB_BUFFER_WRITER_

#include <functional>
#include <memory>
#include <asio.hpp>
#include "netlib.hpp"

namespace netlib {
    // TODO: timeouts
    template <typename socket_type>
    class stream_socket_writer {
    protected:
        std::shared_ptr<socket_type> socket_;

    public:
        explicit stream_socket_writer(std::shared_ptr<socket_type> _socket) {
            socket_ = _socket;
        }

        void write(const netlib::buffer &_buf) {
            asio::async_write(*socket_, asio::buffer(_buf.data(), _buf.size()),
                    std::bind(&stream_socket_writer<socket_type>::handle_write, this, std::placeholders::_1));
        }

        // Event handlers
        void handle_write(const asio::error_code &error) {
            if (!error) {
                if (on_write) on_write();
            }
            else {
                if (on_write_fail) on_write_fail(error);
            }
        }

        std::function<void(const asio::error_code &)> on_fail;
        std::function<void()> on_success;
    };
}

#endif
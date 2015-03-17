#ifndef __SF_MESSAGE_WRITER_
#define __SF_MESSAGE_WRITER_

#include <functional>
#include <asio/error_code.hpp>
#include "stream_socket_writer.hpp"
#include "../core/buffer.hpp"
#include "../core/message.hpp"

namespace dcm {
    // TODO: timeouts
    template <typename socket_type>
    class dcm_message_writer : public netlib::stream_socket_writer<socket_type> {
    public:
        explicit dcm_message_writer(std::shared_ptr<socket_type> _socket) :
                stream_socket_writer<socket_type>(_socket) {}
        void write_message(const dcm::message &_message) {
            auto msg = _message.encode();
            this->write(msg);
        }
    };
}

#endif
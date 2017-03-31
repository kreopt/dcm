#ifndef _DCM_DCM_SOCKET_CLIENT_HPP_
#define _DCM_DCM_SOCKET_CLIENT_HPP_

#include <dcm/interprocess/streamsocket/sender.hpp>
#include "../core/sender.hpp"
#include "../core/message.hpp"

namespace dcm {
    namespace streamsocket {

        template<typename protocol_type>
        class stream_socket_sender : public sender {
            std::shared_ptr<interproc::sender<interproc::buffer>> sender_;
        public:
            virtual ~stream_socket_sender() {}

            explicit stream_socket_sender(const std::string &_endpoint) :
                    sender_(std::make_shared<interproc::streamsocket::sender_impl<protocol_type>>(_endpoint)) {
            };

            virtual void connect() override {
                sender_->connect();
            };

            virtual void send(const dcm::signal &_msg) {
                sender_->send(_msg.encode());
            };

            virtual void close() {
                sender_->close();
            };
        };

        using tcp_sender = stream_socket_sender<asio::ip::tcp>;
        using unix_sender = stream_socket_sender<asio::local::stream_protocol>;
    }
}

#endif //_DCM_DCM_SOCKET_CLIENT_HPP_

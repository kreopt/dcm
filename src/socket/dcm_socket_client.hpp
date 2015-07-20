#ifndef _DCM_DCM_SOCKET_CLIENT_HPP_
#define _DCM_DCM_SOCKET_CLIENT_HPP_

#include <dcm/interprocess/streamsocket/sender.hpp>
#include "../core/message.hpp"

namespace dcm {
    namespace streamsocket {

        class dcm_sender_t: public interproc::sender<dcm::signal> {
        public:
            using ptr = std::shared_ptr<dcm_sender_t>;
        };

        template<typename protocol_type>
        class stream_socket_sender : public dcm_sender_t {
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

        inline dcm_sender_t::ptr make_sender(interproc::conn_type _type, const std::string &_ep) {
            switch (_type) {
                case interproc::conn_type::unix:
                    return std::make_shared<unix_sender>(_ep);
                case interproc::conn_type::tcp:
                    return std::make_shared<tcp_sender>(_ep);
            }
        };
    }
}

#endif //_DCM_DCM_SOCKET_CLIENT_HPP_

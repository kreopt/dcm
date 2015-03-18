#ifndef _DCM_DCM_SOCKET_CLIENT_HPP_
#define _DCM_DCM_SOCKET_CLIENT_HPP_

#include <specforge/interprocess/streamsocket/sender.hpp>
namespace dcm {
    namespace streamsocket {

        using dcm_sender_t = interproc::sender<dcm::message>;
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

            virtual void send(const dcm::message &_msg) {
                sender_->send(_msg.encode());
            };

            virtual void close(const asio::error_code &error = asio::error_code()) {
                sender_->close(error);
            };
        };

        using tcp_sender = stream_socket_sender<asio::ip::tcp>;
        using unix_sender = stream_socket_sender<asio::local::stream_protocol>;

        inline std::shared_ptr<interproc::sender<dcm::message>> make_sender(interproc::streamsocket_type _type, const std::string &_ep) {
            switch (_type) {
                case interproc::streamsocket_type::unix:
                    return std::make_shared<unix_sender>(_ep);
                case interproc::streamsocket_type::tcp:
                    return std::make_shared<tcp_sender>(_ep);
            }
        };
    }
}

#endif //_DCM_DCM_SOCKET_CLIENT_HPP_

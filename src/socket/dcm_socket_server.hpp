#ifndef _DCM_DCM_SOCKET_SERVER_HPP_
#define _DCM_DCM_SOCKET_SERVER_HPP_

#include <specforge/interprocess/streamsocket/receiver.hpp>
#include "dcm_socket_session.hpp"
#include "../core/message.hpp"

namespace dcm {
    namespace streamsocket {
        using dcm_receiver_t = interproc::receiver<dcm::message>;
        // tcp_server class
        template<typename protocol_type>
        class stream_socket_receiver : public dcm_receiver_t {
            std::shared_ptr<interproc::receiver<interproc::buffer>> receiver_;
        public:
            // Constructor
            explicit stream_socket_receiver(const std::string &_endpoint) :
                    receiver_(std::make_shared<interproc::streamsocket::receiver_impl<protocol_type, dcm::streamsocket::receiver_session>>(_endpoint)) {
                receiver_->on_message = [this](interproc::buffer &&_buf){
                    this->on_message(dcm::message(_buf));
                };
            }

            virtual void broadcast(const dcm::message &_msg) {
                receiver_->broadcast(_msg.encode());
            }

            virtual void start() override {
                receiver_->start();
            }

            virtual void stop() override {
                receiver_->stop();
            };

            virtual void join() override {
                receiver_->join();
            };
        };

        using tcp_receiver = stream_socket_receiver<asio::ip::tcp>;
        using unix_receiver = stream_socket_receiver<asio::local::stream_protocol>;

        inline std::shared_ptr<dcm_receiver_t> make_receiver(interproc::streamsocket_type _type, const std::string &_ep) {
            switch (_type) {
                case interproc::streamsocket_type::unix: {
                    std::remove(_ep.c_str());
                    return std::make_shared<unix_receiver>(_ep);
                }
                case interproc::streamsocket_type::tcp:
                    return std::make_shared<tcp_receiver>(_ep);
                default:
                    return nullptr;
            }
        };
    }
}
#endif //_DCM_DCM_SOCKET_SERVER_HPP_

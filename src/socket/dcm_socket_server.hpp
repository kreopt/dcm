#ifndef _DCM_DCM_SOCKET_SERVER_HPP_
#define _DCM_DCM_SOCKET_SERVER_HPP_

#include <dcm/interprocess/streamsocket/listener.hpp>
#include <dcm/interprocess/streamsocket/listener_session.hpp>
#include <mutex>
#include "../core/message.hpp"

namespace dcm {
    namespace streamsocket {

        class dcm_receiver_t : public interproc::listener<dcm::signal> {
        public:
            virtual std::shared_ptr<dcm_receiver_t> on(const std::string &_signal, std::function<void(const dcm::signal&)> _handler) = 0;
            virtual std::shared_ptr<dcm_receiver_t> off(const std::string &_signal) = 0;
            virtual std::shared_ptr<dcm_receiver_t> fail(std::function<void(std::exception &e)>) = 0;
        };

        template<typename protocol_type, typename buffer_type = interproc::buffer>
        class session : public interproc::streamsocket::listener_session<protocol_type>{
            bool is_size_;
        public:
            explicit session(asio::io_service &io_service) :
                interproc::streamsocket::listener_session<protocol_type>(io_service) {
                is_size_ = true;
            }

            virtual void read() {
                // Check input buffer
                this->reader_->read(interproc::BLOCK_SIZE_SIZE);
            }

            virtual void read_success_handler(interproc::buffer &&_buffer) {
                interproc::block_size_t sz;
                if (is_size_) {
                    interproc::ibufstream bs(_buffer);
                    interproc::read_size(bs, sz);
                } else {
                    sz = interproc::BLOCK_SIZE_SIZE;
                    if (this->on_message) this->on_message(_buffer);
                }
                is_size_=!is_size_;
                if (!this->eof_) {
                    this->reader_->read(sz);
                }
            }
        };

        // tcp_server class
        template<typename protocol_type>
        class stream_socket_receiver : public dcm_receiver_t,
                                       public std::enable_shared_from_this<stream_socket_receiver<protocol_type>> {
            std::shared_ptr<interproc::listener<interproc::buffer>> receiver_;
            std::unordered_map<std::string, std::function<void(const dcm::signal&)>> handlers_;
            std::mutex handler_mtx_;
            // TODO: handle generic exceptions
            std::function<void(std::exception &e)> exc_handler_;
        public:
            // Constructor
            explicit stream_socket_receiver(const std::string &_endpoint) :
                    receiver_(std::make_shared<interproc::streamsocket::listener_impl<protocol_type, interproc::buffer, session>>(_endpoint)) {
                receiver_->on_message = [this](const interproc::buffer &_buf){
                    if (this->on_message) {
                        this->on_message(dcm::signal::decode(_buf));
                    }

                    auto msg = dcm::signal(dcm::signal::decode(_buf));
                    try {
                        auto sig_name = msg.name();
                        std::lock_guard<std::mutex> lck(handler_mtx_);
                        if (handlers_.count(sig_name)) {
                            try {
                                // TODO: async
                                handlers_.at(sig_name)(msg);
                            } catch (std::exception &e){
                                if (exc_handler_){
                                    exc_handler_(e);
                                }
                            }
                        }
                    } catch (std::out_of_range &e) {
                        //
                    }
                };
            }

            virtual void broadcast(const dcm::signal &_msg) {
                receiver_->broadcast(_msg.encode());
            }

            virtual void start() override {
                receiver_->start();
            }

            virtual void stop() override {
                receiver_->stop();
            };

            virtual void wait_until_stopped() override {
                receiver_->wait_until_stopped();
            };


            // TODO: move to subscriber
            virtual std::shared_ptr<dcm_receiver_t> on(const std::string &_signal, std::function<void(const dcm::signal&)> _handler) override {
                std::lock_guard<std::mutex> lck(handler_mtx_);
                handlers_[_signal] = _handler;
                return this->shared_from_this();
            }

            virtual std::shared_ptr<dcm_receiver_t> off(const std::string &_signal) override {
                std::lock_guard<std::mutex> lck(handler_mtx_);
                handlers_.erase(_signal);
                return this->shared_from_this();
            }

            virtual std::shared_ptr<dcm_receiver_t> fail(std::function<void(std::exception &e)> _exc_handler) override {
                exc_handler_ = _exc_handler;
                return this->shared_from_this();
            }

        };

        using tcp_receiver = stream_socket_receiver<asio::ip::tcp>;
        using unix_receiver = stream_socket_receiver<asio::local::stream_protocol>;

        inline std::shared_ptr<dcm_receiver_t> make_receiver(interproc::conn_type _type, const std::string &_ep) {
            switch (_type) {
                case interproc::conn_type::unix: {
                    std::remove(_ep.c_str());
                    return std::make_shared<unix_receiver>(_ep);
                }
                case interproc::conn_type::tcp:
                    return std::make_shared<tcp_receiver>(_ep);
                default:
                    return nullptr;
            }
        };
    }
}
#endif //_DCM_DCM_SOCKET_SERVER_HPP_

#ifndef _DCM_DCM_SOCKET_SESSION_HPP_
#define _DCM_DCM_SOCKET_SESSION_HPP_

#include <dcm/interprocess/streamsocket/receiver_session.hpp>

namespace dcm {
    namespace streamsocket {
        template <typename socket_type, typename buffer_type = interproc::buffer>
        class receiver_session : public interproc::streamsocket::receiver_session<socket_type, buffer_type> {
            bool is_header_;
            bool is_size_;
            interproc::buffer message_;
        public:
            explicit receiver_session(asio::io_service &io_service):
                    interproc::streamsocket::receiver_session<socket_type>(io_service){
                this->reader_->on_success = [this](interproc::buffer &&_buf){
                    message_.append(_buf);
                    if (is_size_) {
                        interproc::block_size_t block_size;
                        interproc::ibufstream bs(_buf);
                        interproc::read_size(bs, block_size);
                        is_size_ = !is_size_;
                        this->reader_->read(block_size);
                    } else {
                        if (!is_header_) {
                            if (this->on_message) {
                                this->on_message(std::move(message_));
                                message_.clear();
                            }
                        }
                        is_header_ = !is_header_;
                        is_size_ = !is_size_;
                        this->reader_->read(interproc::BLOCK_SIZE_SIZE);
                    }
                };
//                this->reader_->on_fail = [this](const asio::error_code &_ec) {
//                    if (on_error) on_error(_ec);
//                };
            }

            virtual void start() override {
                is_size_ = true;
                is_header_ = true;
                this->reader_->read(interproc::BLOCK_SIZE_SIZE);
            }

            std::function<void(std::shared_ptr<receiver_session<socket_type, buffer_type>> _session)> on_connect;
            std::function<void(std::shared_ptr<receiver_session<socket_type, buffer_type>> _session)> on_error;
        };
    }
}
#endif //_DCM_DCM_SOCKET_SESSION_HPP_

#ifndef __DCM_SOCKET_CLIENT_
#define __DCM_SOCKET_CLIENT_

#include <asio.hpp>
#include <thread>
#include <error.h>
#include <mutex>

#include "core/message.hpp"
#include "core/connection.hpp"
#include "stream_socket_message_reader.hpp"
#include "stream_socket_message_writer.hpp"
#include "endpoint.hpp"

using asio::ip::tcp;

namespace dcm {

    class client{
    public:
        virtual ~client(){}
        virtual void connect() = 0;
        virtual void send(const message &_message) = 0;
        virtual void close(const asio::error_code &error = asio::error_code()) = 0;
    };

    template <typename protocol_type>
    class stream_socket_client : public client,
                                 public dcm::stream_socket_message_reader<typename protocol_type::socket>,
                                 public dcm::stream_socket_message_writer<typename protocol_type::socket>,
                                 public std::enable_shared_from_this<stream_socket_client<protocol_type>> {
    private:
        using socket_type = typename protocol_type::socket;
        using endpoint_type = typename protocol_type::endpoint;

        std::shared_ptr<asio::io_service>                   io_service_;
        std::shared_ptr<socket_type>                        socket_;
        typename protocol_type::endpoint                    endpoint_;
        std::thread                                         client_thread_;
        volatile size_t                                     pending_count_;
        std::mutex                                          pending_ops_mtx_;
        std::mutex                                          can_close_mtx_;


        // Event handlers
        void handle_connect(const asio::error_code &error) {
            if (!error) {
                this->read_size_block();
            } else {
                close(error);
            }
        }

    public:
        // Constructor
        stream_socket_client(const std::string &_endpoint){
            io_service_ = std::make_shared<asio::io_service>();
            socket_ = std::make_shared<socket_type >(*io_service_);
            endpoint_ = dcm::make_endpoint<endpoint_type>(_endpoint, *io_service_);
            pending_count_ = 0;
            this->set_reader_socket(socket_);
            this->set_writer_socket(socket_);
            this->on_read_fail_ = [this](const asio::error_code &error){
                std::lock_guard<std::mutex> lck(pending_ops_mtx_);
                can_close_mtx_.unlock();
                close();
            };
            this->on_write_fail_ = this->on_read_fail_;
            this->on_write = [this](){
                std::lock_guard<std::mutex> lck(pending_ops_mtx_);
                pending_count_--;
                if (pending_count_ <= 0) {
                    can_close_mtx_.unlock();
                } else {
                    can_close_mtx_.try_lock();
                }
            };
        }

        ~stream_socket_client() {
            std::cout << "destroy client" << std::endl;
            close();
        }

        virtual void connect() override{
            socket_->async_connect(endpoint_, std::bind(&stream_socket_client<protocol_type>::handle_connect, this->shared_from_this(), std::placeholders::_1));
            client_thread_ = std::thread([this](){
                io_service_->run();
            });
        }

        virtual void send(const message &_message) override {
            {
                std::lock_guard<std::mutex> lck(pending_ops_mtx_);
                pending_count_++;
            }
            this->write_message(_message);
        }

        virtual void close(const asio::error_code &error = asio::error_code()) override {
            can_close_mtx_.lock();
            if (socket_->is_open()) {
                socket_->close();
            }
            if (!io_service_->stopped()) {
                io_service_->stop();
            }
            if (error) {
                std::cerr << error.message() << std::endl;
            }
            if (client_thread_.joinable()){
                client_thread_.join();
            }
        }
    };
    using tcp_client = stream_socket_client<asio::ip::tcp>;
    using unix_client = stream_socket_client<asio::local::stream_protocol>;

    inline std::shared_ptr<client> make_client(connection_type _type, const std::string &_ep){
        switch (_type) {
            case connection_type::unix: return std::make_shared<unix_client >(_ep);
            case connection_type::tcp: return std::make_shared<tcp_client >(_ep);
        }
    };
}
#endif
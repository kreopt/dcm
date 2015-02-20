#ifndef __DCM_SOCKET_CLIENT_
#define __DCM_SOCKET_CLIENT_

#include <asio.hpp>
#include <thread>
#include <error.h>

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
        std::function<void(message &&)> on_message;
    };

    template <typename protocol_type>
    class stream_socket_client : public client,
                                 public dcm::stream_socket_message_reader<typename protocol_type::socket>,
                                 public dcm::stream_socket_message_writer<typename protocol_type::socket> {
    private:
        using socket_type = typename protocol_type::socket;
        using endpoint_type = typename protocol_type::endpoint;

        std::shared_ptr<asio::io_service>                   io_service_;
        std::shared_ptr<socket_type>                        socket_;
        typename protocol_type::endpoint                    endpoint_;
        std::thread                                         client_thread_;


        // Event handlers
        void handle_connect(const asio::error_code &error) {
            if (!error) {
                this->read_size();
            } else {
                close(error);
            }
        }

        void do_write(const message &_message) {
            bool write_in_progress = !this->output_messages_.empty();

            // Add message to queue
            this->output_messages_.push_back(_message);
            // TODO: add promise to queue

            // Start async write for new queue
            if (!write_in_progress) {
                // TODO: pass promise from queue to handle_write
                this->write_messages();
            }
        }
    public:
        // Constructor
        stream_socket_client(const std::string &_endpoint){
            io_service_ = std::make_shared<asio::io_service>();
            socket_ = std::make_shared<socket_type >(*io_service_);
            endpoint_ = dcm::make_endpoint<endpoint_type>(_endpoint, *io_service_);
            this->set_reader_socket(socket_);
            this->set_writer_socket(socket_);
            this->on_message = this->on_message;
            this->on_read_fail = std::bind(&stream_socket_client<protocol_type>::close, this, std::placeholders::_1);
            this->on_write_fail = this->on_read_fail;
        }

        ~stream_socket_client() {
            close();
        }

        virtual void connect() override{
            socket_->async_connect(endpoint_, std::bind(&stream_socket_client<protocol_type>::handle_connect, this, std::placeholders::_1));
            client_thread_ = std::thread([this](){
                io_service_->run();
            });
        }

        virtual void send(const message &_message) override{
            io_service_->post(std::bind(&stream_socket_client<protocol_type>::do_write, this, _message));
        }

        virtual void close(const asio::error_code &error = asio::error_code()) override {
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
#ifndef __DCM_SOCKET_CLIENT_
#define __DCM_SOCKET_CLIENT_

#include <asio.hpp>
#include <thread>

#include "core/message.hpp"
#include "stream_socket_message_reader.hpp"
using asio::ip::tcp;

namespace dcm {

    template <typename protocol_type>
    class stream_socket_client : public dcm::stream_socket_message_reader/*, public dcm::stream_socket_message_writer*/ {
    private:
        using socket_type = asio::basic_stream_socket<protocol_type>;
        using endpoint_type = protocol_type::endpoint;

        std::shared_ptr<asio::io_service>                   io_service_;
        std::shared_ptr<socket_type>                        socket_;
        std::shared_ptr<protocol_type::endpoint>            endpoint_;

        std::thread client_thread_;
        message::MessageQueue output_messages_;

        void make_endpoint(const std::string &_ep);

        // Event handlers
        void handle_connect(const asio::error_code &error);

        void connect(){
            socket_->async_connect(endpoint_, std::bind(&stream_socket_client<protocol_type>::handle_connect, this, std::placeholders::_1));
            io_service_->run();
        }

    public:
        // Constructor
        stream_socket_client(std::string &_endpoint){
            io_service_ = std::make_shared<asio::io_service>();
            socket_ = std::make_shared<socket_type >(*io_service_);
            make_endpoint(_endpoint);
            set_socket(socket_);
        }

        ~socket_stream_client();

        void send(const message &_message);

        void close();

        void wait_unfinished();

        std::function<void(const message &)> on_message;
    };

    template <>
    void stream_socket_client<asio::ip::tcp>::make_endpoint(const std::string &_ep) {
        asio::ip::tcp::resolver resolver(*io_service_);
        asio::ip::tcp::resolver::query query(_host, _port);
        auto iterator = resolver.resolve(query);
        endpoint_ = iterator.endpoint();
    }

    template <>
    void stream_socket_client<asio::local::stream_protocol>::make_endpoint(const std::string &_ep) {
        endpoint_ = std::make_shared<endpoint_type>(_ep);
    }

    // Write operation interface
    void dcm::tcp_client::send(const message &message) {
        // TODO: make new promise & pass to do_write
        io_service_->post(std::bind(&tcp_client::do_write, this, message));
        // TODO: return thenable
    }

// Close operation interface
    void dcm::tcp_client::close() {
        io_service_->post(std::bind(&tcp_client::do_close, this, asio::error_code()));
    }

//
// Client operations
//

// Write operation
    void dcm::tcp_client::do_write(message message) {
        bool write_in_progress = !output_messages_.empty();

        // Add message to queue
        output_messages_.push_back(message);
        // TODO: add promise to queue

        // Start async write for new queue
        if (!write_in_progress) {
            // TODO: pass promise from queue to handle_write
            auto msg = output_messages_.front().encode();
            asio::async_write(*socket_, asio::buffer(msg.data(), msg.size()), std::bind(&tcp_client::handle_write, this, std::placeholders::_1));
        }
    }

// Close operation
    void dcm::tcp_client::do_close(const asio::error_code &error) {
        std::cout << error.message() << std::endl;
        // Close connection
        if (socket_->is_open()) {
            socket_->close();
        }
        io_service_->stop();
    }

//
// Event handlers
//

// Connect handler
    void dcm::tcp_client::handle_connect(const asio::error_code &error) {
        if (!error) {
            // Start async read when connection succeeded
            read_size();
        } else {
            do_close(error);
        }
    }


// Write message handler
    void dcm::tcp_client::handle_write(const asio::error_code &error) {
        // TODO: resolve or reject promise
        if (!error) {
            // Check if queue non empty
            output_messages_.pop_front();
            if (!output_messages_.empty()) {
                // Async write data to buffer
                auto msg = output_messages_.front().encode();
                asio::async_write(*socket_, asio::buffer(msg, msg.size()), std::bind(&tcp_client::handle_write, this, std::placeholders::_1));
            }
        }
        else {
            // Close connection else
            std::cout << error.message() << std::endl;
            do_close();
        }
    }

    dcm::tcp_client::tcp_client(const std::string &_host, const std::string &_port) {
        io_service_ = std::make_shared<asio::io_service>();
        tcp::resolver resolver(*io_service_);
        tcp::resolver::query query(_host, _port);
        iterator_ = resolver.resolve(query);
        socket_ = std::make_shared<tcp::socket>(*io_service_);

        set_socket(socket_);
        on_fail = std::bind(&tcp_client::do_close, this, std::placeholders::_1);
    }

    void dcm::tcp_client::run() {
        asio::async_connect(*socket_, iterator_, std::bind(&tcp_client::handle_connect, this, std::placeholders::_1));
        io_service_->run();
    }

    dcm::tcp_client::~tcp_client() {
        do_close();
    }

    void dcm::tcp_client::wait_unfinished() {
    }
}
#endif
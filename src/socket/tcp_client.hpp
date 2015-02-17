#ifndef TCP_CLIENT
#define TCP_CLIENT

#include <asio.hpp>
#include <thread>

#include "core/message.hpp"
#include "stream_socket_message_reader.hpp"
using asio::ip::tcp;

namespace dcm {
// tcp_client class
    class tcp_client : public dcm::stream_socket_message_reader {
    private:
        std::shared_ptr<asio::io_service> io_service_;
        std::shared_ptr<tcp::socket> socket_;
        tcp::resolver::iterator iterator_;

        std::thread client_thread_;
        message::MessageQueue output_messages_;

        // Event handlers
        void handle_connect(const asio::error_code &error);

        void handle_write(const asio::error_code &error);

        // Client operations
        void do_write(message message);

        void do_close(const asio::error_code &error = asio::error_code());

    public:
        // Constructor
        tcp_client(const std::string &_host, const std::string &_port);

        ~tcp_client();

        // Client operation interfaces
        void send(const message &message);

        void run();

        void close();

        void wait_unfinished();
    };
}
#endif

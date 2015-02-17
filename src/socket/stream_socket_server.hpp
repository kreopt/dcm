#ifndef __DCM_SOCKET_SERVER_
#define __DCM_SOCKET_SERVER_

#include <list>
#include <asio.hpp>

#include "stream_socket_server_session.hpp"

using asio::ip::tcp;

namespace dcm {
// tcp_server class
    template <typename protocol_type>
    class stream_socket_server {
    private:
        using acceptor_type = typename protocol_type::acceptor;
        using socket_type = typename protocol_type::socket;

        asio::io_service    &io_service_;
        acceptor_type       acceptor_;
        NetRoom room_;

        // Event handlers
        void handle_accept(std::shared_ptr<stream_socket_server_session<socket_type>> session, const asio::error_code &error) {
            if (!error) {
                session->start();
            }
            start_accept();
        }

        // Initiates an asynchronous accept operation to wait for a new connection
        void start_accept(){
            std::shared_ptr<stream_socket_server_session<socket_type>> new_session(new stream_socket_server_session<socket_type>(io_service_, room_));
            acceptor_.async_accept(*new_session->socket(),
                    std::bind(&stream_socket_server<protocol_type>::handle_accept,
                            this, new_session, std::placeholders::_1));
        }

    public:
        typedef std::shared_ptr<stream_socket_server<protocol_type>> ServerPtr;
        typedef std::list<ServerPtr> ServerPtrs;
        using endpoint_type = typename protocol_type::endpoint;

        // Constructor
        stream_socket_server(asio::io_service &io_service, const endpoint_type &endpoint):
                io_service_(io_service), acceptor_(io_service, endpoint) {
            start_accept();
        }
    };

    using tcp_server = stream_socket_server<asio::ip::tcp>;
    using unix_server = stream_socket_server<asio::local::stream_protocol>;
}
#endif
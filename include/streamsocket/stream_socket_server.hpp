#ifndef __netlib_SOCKET_SERVER_
#define __netlib_SOCKET_SERVER_

#include <list>
#include <set>
#include <asio.hpp>
#include <thread>

#include "netlib.hpp"
#include "endpoint.hpp"
#include "connection.hpp"
#include "stream_socket_server_session.hpp"

// TODO: catch sigint

using asio::ip::tcp;

namespace netlib {
    class server{
    public:
        virtual ~server(){}
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void join() = 0;
        virtual void broadcast(const netlib::buffer &_buf) = 0;
        // TODO: send to single instance

        std::function<void(netlib::buffer &&_buf)> on_message;
    };

    // tcp_server class
    template <typename protocol_type>
    class stream_socket_server : public server {
    private:
        using acceptor_type = typename protocol_type::acceptor;
        using socket_type = typename protocol_type::socket;

        std::shared_ptr<asio::io_service>    io_service_;
        std::shared_ptr<acceptor_type>       acceptor_;
        std::set<std::shared_ptr<stream_socket_server_session<socket_type>>> sessions_;
        std::thread                          server_thread_;

        // Event handlers
        void handle_accept(std::shared_ptr<stream_socket_server_session<socket_type>> session, const asio::error_code &error) {
            if (!error) {
                session->start();
            }
            start_accept();
        }

        // Initiates an asynchronous accept operation to wait for a new connection
        void start_accept(){
            auto new_session=std::make_shared<stream_socket_server_session<socket_type>>(*io_service_);
            new_session->on_message = on_message;
            new_session->on_error = [this](std::shared_ptr<stream_socket_server_session<socket_type>> _session){
                sessions_.erase(_session);
            };
            sessions_.insert(new_session);
            acceptor_->async_accept(*new_session->socket(),
                    std::bind(&stream_socket_server<protocol_type>::handle_accept,
                            this, new_session, std::placeholders::_1));
        }

    public:
        typedef std::shared_ptr<stream_socket_server<protocol_type>> ServerPtr;
        typedef std::list<ServerPtr> ServerPtrs;
        using endpoint_type = typename protocol_type::endpoint;

        // Constructor
        explicit stream_socket_server(const std::string &_endpoint) {
            io_service_ = std::make_shared<asio::io_service>();
            endpoint_type ep = netlib::make_endpoint<endpoint_type>(_endpoint, *io_service_);
            acceptor_ = std::make_shared<acceptor_type>(*io_service_, ep);
        }

        ~stream_socket_server(){
            std::cout << "destroy server" << std::endl;
            stop();
        }

        virtual void broadcast(const netlib::buffer &_buf) {
            for (auto &session: sessions_) {
                session->send(_buf);
            }
        }

        virtual void start() override {
            server_thread_ = std::thread([this](){
                start_accept();
                io_service_->run();
                std::cout << "io stopped" << std::endl;
            });
        }
        virtual void stop() override {
            if (!io_service_->stopped()) {
                io_service_->stop();
            }
            if (server_thread_.joinable()) {
                server_thread_.join();
            }
        };
        virtual void join() override {
            if (server_thread_.joinable()) {
                server_thread_.join();
            }
        };
    };

    using tcp_server = stream_socket_server<asio::ip::tcp>;
    using unix_server = stream_socket_server<asio::local::stream_protocol>;

    inline std::shared_ptr<server> make_server(streamsocket_type _type, const std::string &_ep){
        switch (_type) {
            case connection_type::unix: {
                std::remove(_ep.c_str());
                return std::make_shared<unix_server>(_ep);
            }
            case connection_type::tcp: return std::make_shared<tcp_server>(_ep);
            default:
                return nullptr;
        }
    };
}
#endif
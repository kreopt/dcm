#ifndef TCP_SERVER
#define TCP_SERVER

#include <list>
#include <asio.hpp>

#include "NetSession.h"

using asio::ip::tcp;

// TcpServer class
class TcpServer {
private:
    asio::io_service &io_service_;
    tcp::acceptor acceptor_;
    NetRoom room_;

    // Event handlers
    void handle_accept(NetSession::NetSessionPtr session, const asio::error_code &error);

    // Initiates an asynchronous accept operation to wait for a new connection
    void start_accept();

public:
    typedef std::shared_ptr<TcpServer> TcpServerPtr;
    typedef std::list<TcpServerPtr> TcpServerPtrs;

    // Constructor
    TcpServer(asio::io_service &io_service, const tcp::endpoint &endpoint);
};

#endif

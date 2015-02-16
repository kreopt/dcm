#include <boost/bind.hpp>

#include "TcpServer.h"
#include "NetSession.h"

// Constructor
TcpServer::TcpServer(asio::io_service &io_service, const tcp::endpoint &endpoint)
        : io_service_(io_service), acceptor_(io_service, endpoint) {
    start_accept();
}

// Initiates an asynchronous accept operation to wait for a new connection
void TcpServer::start_accept() {
    NetSession::NetSessionPtr new_session(new NetSession(io_service_, room_));
    acceptor_.async_accept(*new_session->socket(), std::bind(&TcpServer::handle_accept, this, new_session, std::placeholders::_1));
}

// Called when start_accept finishes
void TcpServer::handle_accept(NetSession::NetSessionPtr session, const asio::error_code &error) {
    if (!error) {
        session->start();
    }

    start_accept();
}

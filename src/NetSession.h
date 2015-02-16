#ifndef NET_SESSION
#define NET_SESSION

#include <asio.hpp>

#include "NetRoom.h"
#include "messagereader.hpp"

using asio::ip::tcp;

class NetSession : public NetParticipant, public std::enable_shared_from_this<NetSession>, public dcm::message_reader {
private:
    std::shared_ptr<tcp::socket> socket_;
    NetRoom &room_;
    NetMessage input_message_;
    NetMessage::MessageQueue output_messages_;

    void write_messages();
    // Event handlers
    void handle_write(const asio::error_code &error);

public:
    typedef std::shared_ptr<NetSession> NetSessionPtr;

    // Constructor
    NetSession(asio::io_service &io_service, NetRoom &room);

    // Session operations
    void start();

    void deliver(const NetMessage &message);

    // Overloads
    std::shared_ptr<tcp::socket> &socket();
};

#endif

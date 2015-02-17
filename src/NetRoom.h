#ifndef NET_ROOM
#define NET_ROOM

#include <set>

#include "core/message.hpp"

// NetParticipant abstract class
class NetParticipant {
public:
    typedef std::shared_ptr<NetParticipant> NetParticipantPtr;

    virtual ~NetParticipant() {}

    virtual void deliver(const dcm::message &message) = 0;
};

// NetRoom class
class NetRoom {
private:
    std::set<NetParticipant::NetParticipantPtr> participants_;
    dcm::message::MessageQueue recent_messages_;

public:
    static const int MAX_RECENT_MESSAGES = 100;

    void join(NetParticipant::NetParticipantPtr participant);

    void leave(NetParticipant::NetParticipantPtr participant);

    void deliver(const dcm::message &message);
};

#endif

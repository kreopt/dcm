#ifndef NET_ROOM
#define NET_ROOM

#include <set>

#include "NetMessage.h"

// NetParticipant abstract class
class NetParticipant {
public:
    typedef std::shared_ptr<NetParticipant> NetParticipantPtr;

    virtual ~NetParticipant() {}

    virtual void deliver(const NetMessage &message) = 0;
};

// NetRoom class
class NetRoom {
private:
    std::set<NetParticipant::NetParticipantPtr> participants_;
    NetMessage::MessageQueue recent_messages_;

public:
    static const int MAX_RECENT_MESSAGES = 100;

    void join(NetParticipant::NetParticipantPtr participant);

    void leave(NetParticipant::NetParticipantPtr participant);

    void deliver(const NetMessage &message);
};

#endif

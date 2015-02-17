#include <algorithm>
#include <functional>
#include <asio.hpp>

#include "NetRoom.h"

// Participant joins communication
void NetRoom::join(NetParticipant::NetParticipantPtr participant) {
    participants_.insert(participant);

    // Deliver existing messages to participant
    std::for_each(recent_messages_.begin(), recent_messages_.end(), std::bind(&NetParticipant::deliver, participant, std::placeholders::_1));
}

// Participant leaves communication
void NetRoom::leave(NetParticipant::NetParticipantPtr participant) {
    participants_.erase(participant);
}

// Deliver message to all participants
void NetRoom::deliver(const dcm::message &message) {
    recent_messages_.push_back(message);

    std::cout << "caught " << message.header.at("signal") << " signal with data: "<< message.body.at("data") << std::endl;

    // Cleanup room message history
    while (recent_messages_.size() > NetRoom::MAX_RECENT_MESSAGES) {
        recent_messages_.pop_front();
    }

    // Deliver message to all participants
    for (auto &participant: participants_){
        participant->deliver(message);
    }
};

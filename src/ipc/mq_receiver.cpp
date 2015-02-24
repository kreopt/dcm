#include "mq_receiver.hpp"

#include <boost/interprocess/ipc/message_queue.hpp>
#include <thread>
#include "core/buffer.hpp"
#include "core/message.hpp"

namespace bip = boost::interprocess;

dcm::ipc::mq::receiver::receiver(const std::string &_mbox) : mbox_(_mbox) {
    bip::message_queue::remove(_mbox.c_str());
    mq_ = std::make_shared<bip::message_queue>(bip::create_only, mbox_.c_str(), MAX_MESSAGES, MAX_MESSAGE_SIZE);
}

dcm::ipc::mq::receiver::~receiver() {
    bip::message_queue::remove(mbox_.c_str());
}

void dcm::ipc::mq::receiver::listen() {
    size_t recvd_size;
    uint priority;
    while (!stop_) {
        auto raw_msg = std::shared_ptr<char>(new char[MAX_MESSAGE_SIZE]);
        mq_->receive(raw_msg.get(), MAX_MESSAGE_SIZE, recvd_size, priority);
        if (on_message){
            dcm::message msg;
            dcm::buffer encoded(raw_msg.get(), recvd_size);
            msg.decode_header(encoded);
            on_message(std::move(msg));
        }
        std::this_thread::yield();
    }

}

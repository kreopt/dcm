#ifndef __DCM_MQ_CLIENT_H_
#define __DCM_MQ_CLIENT_H_

#include <string>
#include <boost/interprocess/ipc/message_queue.hpp>
#include "core/message.hpp"
namespace dcm {
    namespace ipc {
        namespace mq {
            class receiver {
                // TODO: get these values from config
                const size_t MAX_MESSAGES = 100;
                const size_t MAX_MESSAGE_SIZE = 4096;
                volatile bool stop_;
                std::string mbox_;
                std::shared_ptr<boost::interprocess::message_queue> mq_;
            public:
                receiver(const std::string &_mbox);
                ~receiver();

                void listen();

                std::function<void(::dcm::message &&_message)> on_message;
            };
        }
    }
}

#endif
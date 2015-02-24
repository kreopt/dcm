#ifndef __DCM_MQ_SERVER_H_
#define __DCM_MQ_SERVER_H_

#include <string>
#include "core/buffer.hpp"

namespace dcm {
    namespace ipc {
        namespace mq {
            class sender {
            public:
                void open(const std::string &_name);
                void close(const std::string &_name);
                void send(const dcm::buffer &_data);
            };
        }
    }
}

#endif
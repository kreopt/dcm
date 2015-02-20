#ifndef __DCM_IPC_SERVER_H_
#define __DCM_IPC_SERVER_H_

#include <string>
#include <vector>
#include "core/message.hpp"

namespace dcm {
    namespace ipc {
        class server {
        public:
            void send(std::vector<std::string> &_mboxes, const std::string &_msg, bool _only_header=false);
        };
    }
}
#endif
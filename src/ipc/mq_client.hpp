#ifndef __DCM_MQ_CLIENT_H_
#define __DCM_MQ_CLIENT_H_

#include <string>

namespace dcm {
    namespace ipc {
        class client {
        public:
            void listen(const std::string &_mbox);
            void close();
        };
    }
}

#endif
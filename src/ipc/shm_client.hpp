#ifndef __DCM_SHM_CLIENT_H_
#define __DCM_SHM_CLIENT_H_

#include <string>
#include "core/buffer.hpp"
namespace dcm {
    namespace ipc {
        namespace shm {
            class client {
            public:
                dcm::buffer get(const std::string &_name);
            };
        }
    }
}

#endif
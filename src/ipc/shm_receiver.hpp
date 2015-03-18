#ifndef __DCM_SHM_CLIENT_H_
#define __DCM_SHM_CLIENT_H_

#include <string>
#include <interprocess/interproc.hpp>

namespace dcm {
    namespace ipc {
        namespace shm {
            class receiver {
            public:
                interproc::buffer get(const std::string &_name);
            };
        }
    }
}

#endif
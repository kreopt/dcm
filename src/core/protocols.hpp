#ifndef DCM_PROTOCOLS_HPP
#define DCM_PROTOCOLS_HPP

#include "util.hpp"

namespace dcm {
    enum class protocol: symbol_t {
        ipc = symbol("ipc"),
        tcp = symbol("tcp"),
        unix = symbol("unix")
    };
}

#endif //DCM_PROTOCOLS_HPP

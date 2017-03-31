#ifndef DCM_SENDER_HPP
#define DCM_SENDER_HPP

#include <memory>
#include "../socket/dcm_socket_client.hpp"
#include "../ipc/ipc_client.hpp"
#include "message.hpp"

namespace dcm {
    class sender : public interproc::sender<dcm::signal> {
    public:
        using ptr = std::shared_ptr<sender>;
    };

    using make_sender = interproc::make_sender<dcm::signal>;
}
#endif //DCM_SENDER_HPP

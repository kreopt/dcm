#ifndef DCM_DCM_HPP
#define DCM_DCM_HPP

#include "socket/dcm_socket_client.hpp"
#include "socket/dcm_socket_server.hpp"
#include "ipc/ipc_client.hpp"
#include "ipc/ipc_server.hpp"


namespace dcm {
    inline dcm::sender::ptr make_sender(interproc::conn_type _type, const std::string &_ep) {
        switch (_type) {
            case interproc::conn_type::unix:
                return std::make_shared<unix_sender>(_ep);
            case interproc::conn_type::tcp:
                return std::make_shared<tcp_sender>(_ep);
        }
    };
}

#endif //DCM_DCM_HPP

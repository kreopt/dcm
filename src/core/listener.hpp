#ifndef DCM_LISTENER_HPP
#define DCM_LISTENER_HPP

#include <string>
#include <memory>
#include "message.hpp"
#include "util.hpp"
#include "protocols.hpp"

namespace dcm {
    class listener : public interproc::listener<dcm::signal> {
    public:
        virtual std::shared_ptr<listener> on(const std::string &_signal, std::function<void(const dcm::signal&)> _handler) = 0;
        virtual std::shared_ptr<listener> off(const std::string &_signal) = 0;
        virtual std::shared_ptr<listener> fail(std::function<void(std::exception &e)>) = 0;

        using ptr = std::shared_ptr<listener>;
    };

    using make_listener = interproc::make_listener<dcm::signal>;

    inline listener::ptr make_sender(const std::string &_ep) {
        auto pos = _ep.find("://");
        if (pos==std::string::npos) {
            throw std::runtime_error("invalid protocol. supported types are ipc, tcp and unix");
        }
        auto proto = _ep.substr(0, pos);
        auto ep = _ep.substr(pos+3);

        switch (symbol(proto)) {
            case dcm::protocol::unix:
                return std::make_shared<streamsocket::unix_listener>(ep);
            case dcm::protocol::tcp:
                return std::make_shared<streamsocket::tcp_listener>(ep);
            case dcm::protocol::ipc:
                return std::make_shared<ipc::ipc_listener>(ep);
        }
    };
}
#endif //DCM_LISTENER_HPP

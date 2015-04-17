#ifndef DCM_OBJECT_HPP
#define DCM_OBJECT_HPP

#include <memory>

namespace dcm {

    class sender;
    class local_receiver;

    class net_participant {
        std::string name_;
    public:
        explicit net_participant(const std::string &_name);
        inline std::string name() { return name_;};
    };

    class publisher : public net_participant {
        std::shared_ptr<dcm::sender> sender_;
    public:
        explicit publisher(const std::string &_name);

        void emit();
    };

    class subscriber : public net_participant {
        std::shared_ptr<dcm::local_receiver> receiver_;
    public:

        explicit subscriber(const std::string &_name);

        void on(const std::string &_signal, callback_t _handler);

        void off(const std::string &_signal);

        //TODO: use boost::any for exception handlers container
        /*

         class exception_handler_t_base {
            ~exception_handler_t_base() = default;
         }
         class exception_handler_t<typename ExceptionType>{
            std::function<void(ExceptionType &_e)> handler;
         }

         */
        template <typename ExceptionType>
        void fail(exception_handler_t<ExceptionType> _handler);
    };

    class pubsub : public publisher, public subscriber {
    public:
        explicit pubsub(const std::string &_name) : publisher(_name), subscriber(_name){};
    };

}

#endif //DCM_OBJECT_HPP

#ifndef _DCM_DCM_SOCKET_CLIENT_HPP_
#define _DCM_DCM_SOCKET_CLIENT_HPP_

#include "stream_socket_client.hpp"
namespace dcm {

    class client{
    public:
        virtual ~client(){}
        virtual void connect() = 0;
        virtual void send(const message &_message) = 0;
        virtual void close(const asio::error_code &error = asio::error_code()) = 0;
    };

}

#endif //_DCM_DCM_SOCKET_CLIENT_HPP_

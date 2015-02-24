#ifndef __DCM_ENDPOINT_H_
#define __DCM_ENDPOINT_H_

#include <type_traits>
#include <regex>
#include <asio/local/stream_protocol.hpp>
#include <asio/ip/tcp.hpp>

namespace dcm {
    template <  typename endpoint_type,
                typename = typename std::enable_if<
                                std::is_same<endpoint_type, asio::ip::tcp::endpoint>::value ||
                                std::is_same<endpoint_type, asio::local::stream_protocol::endpoint>::value
                           >::type>
    endpoint_type make_endpoint(const std::string &_ep, asio::io_service &_io_service);
}
#endif
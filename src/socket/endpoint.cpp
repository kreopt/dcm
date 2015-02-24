#include "endpoint.hpp"

namespace dcm {
    template<>
    asio::ip::tcp::endpoint make_endpoint(const std::string &_ep, asio::io_service &_io_service) {
        asio::ip::tcp::resolver resolver(_io_service);

        std::string ip_re = "((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3})";
        std::string host_re = "((?:[a-zA-Z0-9]+|[a-zA-Z0-9][-a-zA-Z0-9]+[a-zA-Z0-9])(?:\\.[a-zA-Z0-9]+|[a-zA-Z0-9][-a-zA-Z0-9]+[a-zA-Z0-9])?)";
        std::smatch mr;
        std::regex_match(_ep, mr, std::regex("(" + ip_re + "|" + host_re + "):(\\d+)"));

        if (mr.size() < 2) {
            throw std::runtime_error("bad host address");
        }

        std::string host = *(mr.begin() + 1);
        std::string port = *(mr.end() - 1);

        asio::ip::tcp::resolver::query query(host, port);
        auto iterator = resolver.resolve(query);
        return iterator->endpoint();
    }

    template<>
    asio::local::stream_protocol::endpoint make_endpoint(const std::string &_ep, asio::io_service &_io_service) {
        return asio::local::stream_protocol::endpoint(_ep);
    }
}
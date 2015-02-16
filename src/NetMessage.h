#ifndef NET_MESSAGE
#define NET_MESSAGE

#include <deque>
#include <string>
#include <unordered_map>
#include "buffer.hpp"
// NetMessage class
class NetMessage{
private:
    std::unordered_map<std::string, dcm::buffer> header_;
    std::unordered_map<std::string, dcm::buffer> body_;

    void decode(const dcm::buffer& _buf, std::unordered_map<std::string, dcm::buffer>& _container);

public:
    typedef std::deque<NetMessage> MessageQueue;

    // Constructor
    NetMessage();
    NetMessage(const dcm::buffer &_encoded);
    NetMessage(dcm::buffer &&_encoded);

    inline const dcm::buffer& get_header(const std::string &_name) const {return header_.at(_name);};
    void set_header(const std::string &_name, const dcm::buffer &_data);
    void remove_header(const std::string &_name);

    void set_data(const std::string &_name, const dcm::buffer &_data);
    void remove_data(const std::string &_name);

    inline const dcm::buffer& operator [](const std::string &_name) const {return body_.at(_name);};
    inline dcm::buffer& operator [](const std::string &_name){return body_[_name];};

    void decode_header(const dcm::buffer &_encoded);
    void decode_body(const dcm::buffer &_encoded);
    dcm::buffer encode();


};

#endif

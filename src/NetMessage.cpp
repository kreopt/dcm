#include <chrono>
#include <string>
#include <cstring>
#include <sstream>
#include "NetMessage.h"


NetMessage::NetMessage(const dcm::buffer &_encoded) {
}

NetMessage::NetMessage(dcm::buffer &&_body) {

}

NetMessage::NetMessage() {

}

void NetMessage::set_header(const std::string &_name, const dcm::buffer &_data) {
    header_.insert(std::make_pair(_name, _data));
}

void NetMessage::remove_header(const std::string &_name) {
    header_.erase(_name);
}

void NetMessage::set_data(const std::string &_name, const dcm::buffer &_data) {
    body_.insert(std::make_pair(_name, _data));
}

void NetMessage::remove_data(const std::string &_name) {
    body_.erase(_name);
}

dcm::buffer NetMessage::encode() {
    dcm::obufstream ba;
    dcm::write_size(ba, 0);
    size_t block_size = 0;
    for (auto &h: header_){
        dcm::write_size(ba, h.first.size());
        ba << h.first;
        dcm::write_size(ba, h.second.size());
        ba << h.second;
        block_size += h.first.size()+h.second.size()+2*sizeof(uint32_t);
    }
    auto pos = ba.tellp();
    ba.seekp(0);
    dcm::write_size(ba, block_size);
    ba.seekp(pos);
    block_size = 0;
    dcm::write_size(ba, 0);
    for (auto &b: body_){
        dcm::write_size(ba, b.first.size());
        ba << b.first;
        dcm::write_size(ba, b.second.size());
        ba << b.second;
        block_size += b.first.size()+b.second.size()+2*sizeof(uint32_t);
    }
    ba.seekp(pos);
    dcm::write_size(ba, block_size);
    return ba.str();
}

void NetMessage::decode_header(const dcm::buffer &_encoded) {
    decode(_encoded, header_);
}

void NetMessage::decode_body(const dcm::buffer &_encoded) {
    decode(_encoded, body_);
}

void NetMessage::decode(const dcm::buffer &_buf, std::unordered_map<std::string, dcm::buffer> &_container) {
    std::string key;
    dcm::buffer val;
    dcm::ibufstream bs(_buf);
    int32_t len;
    int pos = 0;
    while (pos < _buf.size()){
        dcm::read_size(bs, len);
        key.resize(len);
        bs.read(&key[0], len);
        pos+=len;

        dcm::read_size(bs, len);
        val.resize(len);
        bs.read(&val[0], len);
        pos+=len;

        _container.insert(std::make_pair(key, val));
        pos+=2*sizeof(size_t);
    }
}

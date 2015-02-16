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

dcm::buffer &&NetMessage::encode() {
    dcm::obufstream ba;
    dcm::write_size(ba, 0);
    size_t block_size = 0;
    for (auto &h: header_){
        dcm::write_size(ba, h.first.size());
        ba << h.first;
        dcm::write_size(ba, h.second.size());
        ba << h.second;
        block_size += h.first.size()+h.second.size()+2*sizeof(size_t);
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
        block_size += b.first.size()+b.second.size()+2*sizeof(size_t);
    }
    ba.seekp(pos);
    dcm::write_size(ba, block_size);
    return std::move(ba.str());
}

void NetMessage::decode_header(const dcm::buffer &_encoded) {

}

void NetMessage::decode_body(const dcm::buffer &_encoded) {

}

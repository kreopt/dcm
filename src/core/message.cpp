#include <chrono>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include "message.hpp"


/*dcm::message::message(const interproc::buffer &_encoded) {
    decode(_encoded);
}

dcm::message::message(interproc::buffer &&_encoded) {
    decode(_encoded);
}

dcm::message::message() {

}

interproc::buffer dcm::message::encode_block(const dcm::message::block_t &_block) const {
    interproc::obufstream ba;
    interproc::write_size(ba, 0);
    interproc::block_size_t block_size = 0;
    for (auto &b: _block){
        interproc::write_size(ba, b.first.size());
        ba << b.first;
        interproc::write_size(ba, b.second.size());
        ba << b.second;
        block_size += b.first.size()+b.second.size()+2*sizeof(interproc::block_size_t);
    }
    auto pos = ba.tellp();
    ba.seekp(0);
    interproc::write_size(ba, block_size);
    ba.seekp(pos);
    return ba.str();
}

interproc::buffer dcm::message::encode() const {
    return encode_block(header)+encode_block(body);
}

void dcm::message::decode_header(const interproc::buffer &_encoded) {
    decode_block(_encoded, header);
}

void dcm::message::decode_body(const interproc::buffer &_encoded) {
    decode_block(_encoded, body);
}

void dcm::message::decode_block(const interproc::buffer &_buf, std::unordered_map<std::string, interproc::buffer> &_container) {
    std::string key;
    interproc::buffer val;
    interproc::ibufstream bs(_buf);
    interproc::block_size_t len;
    int pos = 0;
    while (pos < _buf.size()){
        interproc::read_size(bs, len);
        key.resize(len);
        bs.read(&key[0], len);
        pos+=len;

        interproc::read_size(bs, len);
        val.resize(len);
        bs.read(&val[0], len);
        pos+=len;

        _container[key] = val;
        pos+=2*interproc::BLOCK_SIZE_SIZE;
    }
}

void dcm::message::decode(const interproc::buffer &_encoded) {
    interproc::ibufstream bs(_encoded);
    interproc::block_size_t len = 0;
    interproc::read_size(bs, len);
    decode_header(interproc::buffer(_encoded, interproc::BLOCK_SIZE_SIZE, len));

    interproc::ibufstream bbs(interproc::buffer(_encoded, interproc::BLOCK_SIZE_SIZE+len));
    interproc::block_size_t len1 = 0;
    interproc::read_size(bbs, len1);
    decode_body(interproc::buffer(_encoded.begin()+2*interproc::BLOCK_SIZE_SIZE+len, _encoded.end()));
}

dcm::message::message(dcm::message &&_message) {
    this->header = std::move(_message.header);
    _message.header.clear();
    this->body = std::move(_message.body);
    _message.body.clear();
}

interproc::buffer dcm::message::encode_header() const {
    return encode_block(header);
}

interproc::buffer dcm::message::encode_body() const {
    return encode_block(body);
}

dcm::message::message(dcm::message::block_t &&_header, dcm::message::block_t &&_body) {
    header = _header;
    body = _body;
}
*/

const interproc::buffer dcm::signal::encode_block(const dcm::signal::message_block_t &_block) {
    interproc::obufstream ba;
    interproc::write_size(ba, 0);
    interproc::block_size_t block_size = 0;
    for (auto &b: _block){
        interproc::write_size(ba, b.first.size());
        ba << b.first;
        interproc::write_size(ba, b.second.size());
        ba << b.second;
        block_size += b.first.size()+b.second.size()+2*sizeof(interproc::block_size_t);
    }
    auto pos = ba.tellp();
    ba.seekp(0);
    interproc::write_size(ba, block_size);
    ba.seekp(pos);

    return interproc::to_buffer(ba.str());
}

dcm::signal::message_block_t dcm::signal::decode_block(const interproc::buffer&_buf) {
    std::string key;
    interproc::buffer val;
    interproc::ibufstream bs(_buf);
    interproc::block_size_t len;
    dcm::signal::message_block_t values;
    int pos = 0;
    while (pos < _buf.size()){
        interproc::read_size(bs, len);
        key.resize(len);
        bs.read(&key[0], len);
        pos+=len;

        interproc::read_size(bs, len);
        val.resize(len);
        bs.read(&val[0], len);
        pos+=len;

        values[key] = val;
        pos+=2*interproc::BLOCK_SIZE_SIZE;
    }
    return values;
}

void dcm::signal::set_header(const std::string &_key, const interproc::buffer &_value) {
    header_[_key] =_value;
}

void dcm::signal::erase_header(const std::string &_key) {
    header_.erase(_key);
}

interproc::buffer dcm::signal::get_header(const std::string &_key) const {
    return header_.at(_key);
}

void dcm::signal::set_data(const std::string &_key, const interproc::buffer &_value) {
    body_[_key] = _value;
}

void dcm::signal::erase_data(const std::string &_key) {
    body_.erase(_key);
}

interproc::buffer dcm::signal::get_data(const std::string &_key) const {
    return body_.at(_key);
}


interproc::buffer dcm::signal::encode() const {
    interproc::obufstream ba;
    auto hb = encode_block(header_);
    auto bb = encode_block(body_);
    interproc::write_size(ba, hb.size()+bb.size());

    ba << hb << bb;

    return interproc::to_buffer(ba.str());
}

dcm::signal dcm::signal::decode(const interproc::buffer &_buf) {
    dcm::signal s;
    interproc::ibufstream bs0(_buf);
    interproc::ibufstream bs(interproc::buffer(_buf, interproc::BLOCK_SIZE_SIZE));
    interproc::block_size_t pkg_len = 0;
    interproc::block_size_t len = 0;
    interproc::read_size(bs0, pkg_len);
    interproc::read_size(bs, len);

    if (len > pkg_len || pkg_len != _buf.length()-interproc::BLOCK_SIZE_SIZE) {
        throw decode_error("bad block length: pkg_len="+std::to_string(pkg_len)+"; head_len="+std::to_string(len)+"; real_len="+std::to_string(_buf.length()));
    }
    s.header_ = decode_block(interproc::buffer(_buf, 2*interproc::BLOCK_SIZE_SIZE, len));

    interproc::ibufstream bbs(interproc::buffer(_buf, 2*interproc::BLOCK_SIZE_SIZE+len));
    interproc::block_size_t len1 = 0;
    interproc::read_size(bbs, len1);

    if (len+len1 > pkg_len) {
        throw decode_error("bad block length: pkg_len="+std::to_string(pkg_len)+"; head_len="+std::to_string(len)+"; body_len="+std::to_string(len1)+"; real_len="+std::to_string(_buf.length()));
    }
    s.body_ = decode_block(interproc::buffer(_buf.begin()+3*interproc::BLOCK_SIZE_SIZE+len, _buf.end()));
    return s;
}

dcm::signal::signal(const std::string &_name) {
    if (_name.length()) {
        header_["signal"] = interproc::to_buffer(_name);
    }
}

bool dcm::signal::has_data(const std::string &_key) const {
    return body_.count(_key) > 0;
}

bool dcm::signal::has_header(const std::string &_key) const {
    return header_.count(_key) > 0;
}

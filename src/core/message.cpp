#include <chrono>
#include <string>
#include <cstring>
#include <sstream>
#include <iostream>
#include "message.hpp"

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
    interproc::ibufstream bs(_buf);
    interproc::block_size_t len = 0;
    interproc::read_size(bs, len);

    if (len > _buf.length()-interproc::BLOCK_SIZE_SIZE) {
        throw decode_error("bad block length: head_len="+std::to_string(len)+"; real_len="+std::to_string(_buf.length()));
    }
    s.header_ = decode_block(interproc::buffer(_buf, interproc::BLOCK_SIZE_SIZE, len));

    interproc::ibufstream bbs(interproc::buffer(_buf, interproc::BLOCK_SIZE_SIZE+len));
    interproc::block_size_t len1 = 0;
    interproc::read_size(bbs, len1);

    if (len+len1 > _buf.length()-2*interproc::BLOCK_SIZE_SIZE) {
        throw decode_error("bad block length: head_len="+std::to_string(len)+"; body_len="+std::to_string(len1)+"; real_len="+std::to_string(_buf.length()));
    }
    s.body_ = decode_block(interproc::buffer(_buf.begin()+2*interproc::BLOCK_SIZE_SIZE+len, _buf.end()));
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

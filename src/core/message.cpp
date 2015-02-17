#include <chrono>
#include <string>
#include <cstring>
#include <sstream>
#include "message.hpp"


dcm::message::message(const dcm::buffer &_encoded) {
    decode(_encoded);
}

dcm::message::message(dcm::buffer &&_encoded) {
    decode(_encoded);
}

dcm::message::message() {

}

dcm::buffer dcm::message::encode_block(const dcm::message::block_t &_block) const {
    dcm::obufstream ba;
    dcm::write_size(ba, 0);
    dcm::block_size_t block_size = 0;
    for (auto &b: _block){
        dcm::write_size(ba, b.first.size());
        ba << b.first;
        dcm::write_size(ba, b.second.size());
        ba << b.second;
        block_size += b.first.size()+b.second.size()+2*sizeof(dcm::block_size_t);
    }
    auto pos = ba.tellp();
    ba.seekp(0);
    dcm::write_size(ba, block_size);
    ba.seekp(pos);
    return ba.str();
}

dcm::buffer dcm::message::encode() const {
    return encode_block(header)+encode_block(body);
}

void dcm::message::decode_header(const dcm::buffer &_encoded) {
    decode_block(_encoded, header);
}

void dcm::message::decode_body(const dcm::buffer &_encoded) {
    decode_block(_encoded, body);
}

void dcm::message::decode_block(const dcm::buffer &_buf, std::unordered_map<std::string, dcm::buffer> &_container) {
    std::string key;
    dcm::buffer val;
    dcm::ibufstream bs(_buf);
    dcm::block_size_t len;
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
        pos+=2*dcm::BLOCK_SIZE_SIZE;
    }
}

void dcm::message::decode(const dcm::buffer &_encoded) {
    dcm::ibufstream bs(_encoded);
    dcm::block_size_t len = 0;
    dcm::read_size(bs, len);
    decode_header(dcm::buffer(_encoded, dcm::BLOCK_SIZE_SIZE, len));

    dcm::ibufstream bbs(dcm::buffer(_encoded, dcm::BLOCK_SIZE_SIZE+len));
    dcm::block_size_t len1 = 0;
    dcm::read_size(bbs, len1);
    decode_body(dcm::buffer(_encoded.begin()+2*dcm::BLOCK_SIZE_SIZE+len, _encoded.end()));
}

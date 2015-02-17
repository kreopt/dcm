#ifndef __SF_BUFFER_HPP_
#define __SF_BUFFER_HPP_
#include <string>
#include <sstream>

namespace dcm {
    using block_size_t = uint32_t;
    using byte_t = char;
    using buffer = std::basic_string<byte_t>;
    using ibufstream = std::basic_istringstream<byte_t>;
    using obufstream = std::basic_ostringstream<byte_t>;

    const uint32_t BLOCK_SIZE_SIZE = sizeof(block_size_t);

    inline void write_size(obufstream& _s, block_size_t _sz){
        _s.write(reinterpret_cast<dcm::byte_t*>(&_sz), dcm::BLOCK_SIZE_SIZE);
    }

    inline void read_size(ibufstream& _s, block_size_t &_sz){
        char * lb = reinterpret_cast<char*>(&_sz);
        _s.read(lb, dcm::BLOCK_SIZE_SIZE);
    }
}
#endif
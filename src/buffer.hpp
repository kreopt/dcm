#ifndef __SF_BUFFER_HPP_
#define __SF_BUFFER_HPP_
#include <string>
#include <sstream>

namespace dcm {
    using byte_t = char;
    using buffer = std::basic_string<byte_t>;
    using ibufstream = std::basic_istringstream<byte_t>;
    using obufstream = std::basic_ostringstream<byte_t>;

    inline void write_size(obufstream& _s, size_t _sz){
        _s.write(reinterpret_cast<dcm::byte_t*>(&_s), sizeof _sz);
    }
}
#endif
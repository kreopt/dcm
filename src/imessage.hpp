#ifndef __DCM_IMESSAGE_HPP_
#define __DCM_IMESSAGE_HPP_

#include "buffer.hpp"

namespace dcm {

    // TODO: decompose to encoder and structural interfaces

    class imessage {
    public:
        virtual ~imessage(){};
        virtual dcm::buffer encode() const = 0;
        virtual void decode(const dcm::buffer &_encoded) = 0;
    };

    class iblock_message : public imessage {
    public:
        virtual ~iblock_message(){};
        virtual void decode_block(const dcm::buffer &_buf, uint8_t _block_idx) = 0;
        virtual dcm::buffer encode_block(uint8_t _block_idx) = 0;
    };
}

#endif
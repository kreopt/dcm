#ifndef __DCM_IMESSAGE_HPP_
#define __DCM_IMESSAGE_HPP_


namespace dcm {

    // TODO: decompose to encoder and structural interfaces

    class imessage {
    public:
        virtual ~imessage(){};
        virtual interproc::buffer encode() const = 0;
        virtual void decode(const interproc::buffer &_encoded) = 0;
    };

    class iblock_message : public imessage {
    public:
        virtual ~iblock_message(){};
        virtual void decode_block(const interproc::buffer &_buf, uint8_t _block_idx) = 0;
        virtual interproc::buffer encode_block(uint8_t _block_idx) = 0;
    };
}

#endif
#ifndef NET_MESSAGE
#define NET_MESSAGE

#include <deque>
#include <string>
#include <map>
#include <unordered_map>
#include "buffer.hpp"
#include "imessage.hpp"
namespace dcm {
// message class
    class message : dcm::imessage {
    private:
        using block_t = std::unordered_map<std::string, dcm::buffer>;

        void decode_block(const dcm::buffer &_buf, block_t &_block);

        dcm::buffer encode_block(const block_t &_block) const;

    public:
        typedef std::deque<message> MessageQueue;

        // Constructor
        message();

        message(const dcm::buffer &_encoded);

        message(dcm::buffer &&_encoded);

        message(dcm::message &&_message);
        message(const dcm::message &_message) = default;

        void decode_header(const dcm::buffer &_encoded);

        void decode_body(const dcm::buffer &_encoded);

        virtual void decode(const dcm::buffer &_encoded) override;

        virtual dcm::buffer encode() const override;
        dcm::buffer encode_header() const;
        dcm::buffer encode_body() const;

        block_t header;
        block_t body;
    };
}
#endif

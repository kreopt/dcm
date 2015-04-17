#ifndef NET_MESSAGE
#define NET_MESSAGE

#include <deque>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <dcm/interprocess/interproc.hpp>

namespace dcm {

    enum class message_param_type {
        str,
        num,
        raw
    };

    class message_block_t : public interproc::buffer {
        std::unordered_map<std::string, std::pair<interproc::buffer, message_param_type>> values_;
        bool dirty_;
    public:
        using interproc::buffer::buffer;

        // TODO: encode data within these functions
        virtual const interproc::byte_t* data() const override;
        virtual interproc::byte_t* data();
        size_t size() const;

        void set();
        void get();
        void erase();
    };

    class message_t : public interproc::buffer {
        std::vector<message_block_t> blocks_;
        bool dirty_;
    public:
        using interproc::buffer::buffer;

        // TODO: encode data within these functions
        virtual const interproc::byte_t* data() const override;
        virtual interproc::byte_t* data();
        size_t size() const;
    };

    class signal_t : public message_t {
    public:
        using message_t::message_t;

        void set_header();
        void erase_header();
        void set_data();
        void erase_data();
        const std::string name() const;
    };

// message class
    /*class message : dcm::imessage {
    public:
            using block_t = std::unordered_map<std::string, interproc::buffer>;
    private:

        void decode_block(const interproc::buffer &_buf, block_t &_block);

        interproc::buffer encode_block(const block_t &_block) const;

    public:
        typedef std::deque<message> MessageQueue;

        // Constructor
        message();

        message(const interproc::buffer &_encoded);

        message(interproc::buffer &&_encoded);

        message(block_t &&_header, block_t &&_body);

        message(dcm::message &&_message);
        message(const dcm::message &_message) = default;

        void decode_header(const interproc::buffer &_encoded);

        void decode_body(const interproc::buffer &_encoded);

        virtual void decode(const interproc::buffer &_encoded) override;

        virtual interproc::buffer encode() const override;
        interproc::buffer encode_header() const;
        interproc::buffer encode_body() const;

        block_t header;
        block_t body;
    };*/
}
#endif

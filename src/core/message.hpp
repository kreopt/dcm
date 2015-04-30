#ifndef NET_MESSAGE
#define NET_MESSAGE

#include <deque>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <dcm/interprocess/interproc.hpp>
//#include <object.hpp>

namespace dcm {

    enum class message_param_type {
        str,
        num,
        raw
    };

    class signal {
        using message_block_t = std::unordered_map<std::string, interproc::buffer>;

        message_block_t   header_;
        message_block_t   body_;

        static message_block_t decode_block(const interproc::buffer&_buf);
        static const interproc::buffer encode_block(const message_block_t &_block);

    public:
        class decode_error : public std::runtime_error {
        public:
            using std::runtime_error::runtime_error;
        };

        signal(const signal& _sig) = default;
        signal(signal&& _sig) = default;
        explicit signal(const std::string &_name="");

        interproc::buffer encode() const;
        static signal decode(const interproc::buffer &_buf);


        void set_header(const std::string &_key, const interproc::buffer &_value);
        void erase_header(const std::string &_key);
        interproc::buffer get_header(const std::string &_key) const;
        bool has_header(const std::string &_key) const;
        void set_data(const std::string &_key, const interproc::buffer &_value);
        void erase_data(const std::string &_key);
        interproc::buffer get_data(const std::string &_key) const;
        bool has_data(const std::string &_key) const;


        inline const message_block_t& headers() const { return header_; };
        inline const message_block_t& data() const { return header_; };

        inline const std::string name() const { return header_.at("signal");};
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

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
        inline const message_block_t& data() const { return body_; };

        inline const std::string name() const { return header_.at("signal");};
    };
}
#endif

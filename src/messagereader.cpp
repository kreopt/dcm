#include <functional>
#include <asio/buffer.hpp>
#include <asio/read.hpp>
#include <error.h>
#include "messagereader.hpp"

// Read body handler
void dcm::message_reader::handle_read_data(const asio::error_code &error) {
    if (!error) {
        // Try to read next message
        if (is_header_){
            input_message_.decode_header(input_buffer_);
        } else {
            input_message_.decode_body(input_buffer_);
            if (on_read) {
                on_read();
            }
        }
        is_header_ = !is_header_;
        read_size();
    }
    else {
        // Close connection else
        if (on_fail) {
            on_fail(error);
        }
    }
}

void dcm::message_reader::handle_read_size(const asio::error_code &error) {
    if (!error) {
        // TODO: make sure we have read sieof(size_t) bytes
        block_size_ = reinterpret_cast<size_t *>(*input_buffer_.begin())[0];
        read_data();
    } else {
        // Close connection else
        if (on_fail) {
            on_fail(error);
        }
    }
}

void dcm::message_reader::read_size() {
    input_buffer_.reserve(sizeof(size_t));
    char *bufstart = &input_buffer_.front();
    asio::async_read(*socket_, asio::buffer(bufstart, sizeof(size_t)), std::bind(&dcm::message_reader::handle_read_size, this, std::placeholders::_1));
}

void dcm::message_reader::read_data() {
    input_buffer_.reserve(block_size_);
    char *bufstart = &input_buffer_.front();
    asio::async_read(*socket_, asio::buffer(bufstart, block_size_), std::bind(&dcm::message_reader::handle_read_data, this, std::placeholders::_1));
}

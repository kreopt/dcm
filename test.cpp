#include <NetMessage.h>

void test_message_encode(){
    NetMessage message;

    message.set_header("test", "123");
    message.set_header("test1", "1232");
    message.set_data("testb", "1233312");
    auto encoded = message.encode();
    std::cout << encoded << std::endl;

    NetMessage message1;
    message1.decode(encoded);

    encoded = message1.encode();
    std::cout << encoded << std::endl;
}
void test_message_encode(){
    NetMessage message;

    message.set_header("test", "123");
    message.set_header("test1", "1232");
    message.set_data("testb", "1233312");
    auto encoded = message.encode();
    std::cout << encoded << std::endl;

    NetMessage message1;
    dcm::ibufstream bs(encoded);
    dcm::block_size_t len = 0;
    dcm::read_size(bs, len);
    message1.decode_header(dcm::buffer(encoded, sizeof(dcm::block_size_t), len));

    dcm::ibufstream bbs(dcm::buffer(encoded, sizeof(int32_t)+len));
    dcm::block_size_t len1 = 0;
    dcm::read_size(bbs, len1);
    message1.decode_body(dcm::buffer(encoded.begin()+2*sizeof(dcm::block_size_t)+len, encoded.end()));

    encoded = message1.encode();
    std::cout << encoded << std::endl;
}
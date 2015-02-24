#include "shm_sender.hpp"

#include <cstring>
#include <boost/interprocess/mapped_region.hpp>

namespace bip = boost::interprocess;

void dcm::ipc::shm::sender::open(const std::string &_name) {
    maps_.emplace(_name, std::make_pair(
            bip::shared_memory_object(bip::create_only, _name.c_str(), bip::read_write),
            std::make_shared<shmem_remover>(_name)
    ));
}

void dcm::ipc::shm::sender::close(const std::string &_name) {
    maps_.erase(_name);
}

void dcm::ipc::shm::sender::send(const std::string &_name, const dcm::buffer &_data) {
    try {
        auto &shmem_write = maps_.at(_name).first;
        shmem_write.truncate(_data.size());

        bip::mapped_region region_write(shmem_write, bip::read_write);

        std::memcpy(region_write.get_address(), _data.data(), _data.size());
    } catch (std::out_of_range &e){
        // Do nothing
    }
}

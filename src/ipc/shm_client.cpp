#include "shm_client.hpp"

#include <cstring>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace bip = boost::interprocess;

dcm::buffer dcm::ipc::shm::client::get(const std::string& _name){
    bip::shared_memory_object shmem_read(bip::open_only, _name.c_str(), bip::read_only);

    // Map the whole shared memory in this process
    bip::mapped_region region_read(shmem_read, bip::read_only);

    dcm::buffer buf;
    buf.resize(region_read.get_size());
    std::memcpy(const_cast<char*>(buf.data()), region_read.get_address(), region_read.get_size());
    return buf;
}

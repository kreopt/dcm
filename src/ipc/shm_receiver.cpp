#include "shm_receiver.hpp"

#include <cstring>
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>
#include <dcm/interprocess/interproc.hpp>

namespace bip = boost::interprocess;

interproc::buffer dcm::ipc::shm::receiver::get(const std::string& _name){
    bip::shared_memory_object shmem_read(bip::open_only, _name.c_str(), bip::read_only);

    // Map the whole shared memory in this process
    bip::mapped_region region_read(shmem_read, bip::read_only);

    interproc::buffer buf;
    buf.resize(region_read.get_size());
    std::memcpy(const_cast<char*>(buf.data()), region_read.get_address(), region_read.get_size());
    return buf;
}

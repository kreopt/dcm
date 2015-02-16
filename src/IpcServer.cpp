#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/mapped_region.hpp>

#include "IpcServer.h"

using namespace boost::interprocess;

// Constructor
IpcServer::IpcServer() {
    // Not implemented
}

// Test shared memory
int IpcServer::TestSharedMemory() {
    // I. Created shared memory object in parent process:
    // Remove shared memory on construction and destruction
    struct shmem_remove {
        shmem_remove() {
            shared_memory_object::remove("MySharedMemory");
        }

        ~shmem_remove() {
            shared_memory_object::remove("MySharedMemory");
        }
    } remover;

    // Create shared memory object
    shared_memory_object shmem_write(create_only, "MySharedMemory", read_write);

    // Set size
    shmem_write.truncate(1000);

    // Map the whole shared memory in this process
    mapped_region region_write(shmem_write, read_write);

    // Write all the memory to 1
    std::memset(region_write.get_address(), 1, region_write.get_size());

    // II. Read shared memory object in child process:
    // Open already created shared memory object
    shared_memory_object shmem_read(open_only, "MySharedMemory", read_only);

    // Map the whole shared memory in this process
    mapped_region region_read(shmem_read, read_only);

    // Check that memory was initialized to 1
    char *mem = static_cast<char *>(region_read.get_address());
    for (std::size_t i = 0; i < region_read.get_size(); i++) {
        if (*mem++ != 1) {
            // Error checking memory
            return 1;
        }
    }

    return 0;
}

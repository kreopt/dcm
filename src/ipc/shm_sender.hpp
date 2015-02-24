#ifndef __DCM_SHM_SERVER_H_
#define __DCM_SHM_SERVER_H_

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/interprocess/shared_memory_object.hpp>
#include "core/buffer.hpp"

namespace dcm {
    namespace ipc {
        namespace shm {
            class sender {
                struct shmem_remover {
                    shmem_remover(const std::string &_name) {
                        name_ = _name;
                        boost::interprocess::shared_memory_object::remove(_name.c_str());
                    }
                    ~shmem_remover() {
                        boost::interprocess::shared_memory_object::remove(name_.c_str());
                    }
                private:
                    std::string name_;
                };

                std::unordered_map<std::string, std::pair<boost::interprocess::shared_memory_object, std::shared_ptr<shmem_remover>>> maps_;
            public:
                void open(const std::string &_name);
                void close(const std::string &_name);
                void send(const std::string &_name, const dcm::buffer &_data);
            };
        }
    }
}
#endif
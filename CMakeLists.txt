cmake_minimum_required(VERSION 2.8.4)
project(dcm)

add_definitions(-std=c++14)

include_directories(src/)
include_directories(include/)

file(GLOB_RECURSE SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/**/*.hpp)

set(SOURCE_FILES ${SOURCES} ${HEADERS})
add_executable(dcm_server ${SOURCE_FILES} server.cpp)
add_executable(dcm_client ${SOURCE_FILES} client.cpp)

set(BOOST_LIBS boost_system rt pthread)

target_link_libraries(dcm_server
    ${BOOST_LIBS}
)

target_link_libraries(dcm_client
    ${BOOST_LIBS}
)
cmake_minimum_required(VERSION 3.20)
project(nvp)

set(CMAKE_CXX_STANDARD 17)

find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)

add_executable(nvp main.cpp
        commands.h)

target_link_libraries(nvp CURL::libcurl nlohmann_json::nlohmann_json)
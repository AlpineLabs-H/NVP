cmake_minimum_required(VERSION 3.20)
project(nvp VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

add_executable(nvp main.cpp)

target_link_libraries(nvp PRIVATE CURL::libcurl nlohmann_json::nlohmann_json)

target_compile_options(nvp PRIVATE
    $<$<C_COMPILER_ID:MSVC>:/W4 /permissive->
    $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic>
)

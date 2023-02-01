cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

project(jpng)
set(CMAKE_BUILD_TYPE "Debug")

add_subdirectory(lib/libpng-1.6.39)
include_directories(lib/libpng-1.6.39)

add_subdirectory(lib/libjpeg-turbo)
include_directories(lib/libjpeg-turbo)

set(src yizhantest.cpp)
add_executable(jpng ${src})
target_link_libraries(jpng png_static jpeg-static)


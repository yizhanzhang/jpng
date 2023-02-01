cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

project(jpng)
set(CMAKE_BUILD_TYPE "Debug")

include_directories(lib/libpng-1.6.39)
AUX_SOURCE_DIRECTORY(lib/libpng-1.6.39 LIB_PNG_INCLUDE)
AUX_SOURCE_DIRECTORY(lib/libpng-1.6.39/arm LIB_PNG_ARM)
add_library(libpng STATIC ${LIB_PNG_INCLUDE} ${LIB_PNG_ARM})
find_package(ZLIB REQUIRED)
target_link_libraries(libpng ${ZLIB_LIBRARIES})

set(src yizhantest.cpp)
add_executable(jpng ${src})
target_link_libraries(jpng libpng)


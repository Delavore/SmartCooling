cmake_minimum_required(VERSION 3.22)

project (SmartCooling)
enable_language(C CXX ASM)

# set (CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_SOURCE_DIR}/cmake_toolchain/toolchain_config.cmake)

add_subdirectory(hardware)
add_subdirectory(src)
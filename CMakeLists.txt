
cmake_minimum_required(VERSION 3.20)

project(AeroArch VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aeroarch_core STATIC
    src/system/system_info.cpp
)

target_include_directories(aeroarch_core
    PUBLIC
        ${CMAKE_SOURCE_DIR}/include
)

add_executable(aeroarch
    src/core/main.cpp
)

target_link_libraries(aeroarch
    PRIVATE aeroarch_core
)


cmake_minimum_required(VERSION 3.20)

project(AeroArch VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(aeroarch_core STATIC
    src/core/config.cpp
    src/core/logger.cpp
    src/system/system_info.cpp
    src/system/system_info_format.cpp
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

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(aeroarch_gui
    src/gui/main_window.cpp
)

target_link_libraries(aeroarch_gui
    PRIVATE
        aeroarch_core
        Qt6::Widgets
)

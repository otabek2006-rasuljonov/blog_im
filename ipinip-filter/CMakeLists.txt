cmake_minimum_required(VERSION 3.16)
project(ipinip_filter CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ogohlantirishlarni yoqish
add_compile_options(-Wall -Wextra -Wpedantic)

# Umumiy manba fayllari (sniffer va filtrator ikkalasi ham ishlatadi)
set(COMMON_SOURCES
    src/tunnel.cpp
    src/sniffer.cpp
    src/filtrator.cpp
)

# ---- Sniffer binary ----
add_executable(sniffer
    src/sniffer_main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(sniffer PRIVATE src)

# ---- Filtrator binary ----
add_executable(filtrator
    src/filtrator_main.cpp
    ${COMMON_SOURCES}
)
target_include_directories(filtrator PRIVATE src)

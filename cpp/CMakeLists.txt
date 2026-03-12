cmake_minimum_required(VERSION 3.14)
project(ipinip_filter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ----- Ogohlantirishlar -----
add_compile_options(-Wall -Wextra -Wpedantic)

# ----- Umumiy kutubxona -----
add_library(ipinip_lib STATIC
    tunnel.cpp
    sniffer.cpp
    filtrator.cpp
)
target_include_directories(ipinip_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# ----- Sniffer binary -----
add_executable(sniffer sniffer_main.cpp)
target_link_libraries(sniffer PRIVATE ipinip_lib)

# ----- Filtrator binary -----
add_executable(filtrator filtrator_main.cpp)
target_link_libraries(filtrator PRIVATE ipinip_lib)

# ----- O'rnatish (ixtiyoriy) -----
install(TARGETS sniffer filtrator DESTINATION bin)

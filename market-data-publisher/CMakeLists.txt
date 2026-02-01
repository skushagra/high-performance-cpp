cmake_minimum_required(VERSION 3.16)

project(market_data_publisher
        VERSION 1.0
        LANGUAGES CXX)

# ============================================================
# C++ configuration
# ============================================================
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================
# Dependencies
# ============================================================
find_package(fmt REQUIRED)

# ============================================================
# Publisher executable
# ============================================================
add_executable(publisher
    publisher/main.cpp
    publisher/publisher.cpp
)

target_include_directories(publisher
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(publisher
    PRIVATE
        fmt::fmt
)

# ============================================================
# Shared Memory Consumer
# ============================================================
add_executable(consumer_shm
    consumer_shm/main.cpp
    consumer_shm/consumer.cpp
)

target_include_directories(consumer_shm
    PRIVATE
        ${PROJECT_SOURCE_DIR}
)

target_link_libraries(consumer_shm
    PRIVATE
        fmt::fmt
)

# ============================================================
# TCP Consumer (Boost.Asio header-only)
# ============================================================
add_executable(consumer_tcp
    consumer_tcp/main.cpp
    consumer_tcp/consumer.cpp
)

target_include_directories(consumer_tcp
    PRIVATE
        ${PROJECT_SOURCE_DIR}
        /opt/homebrew/include   # Boost headers
)

target_link_libraries(consumer_tcp
    PRIVATE
        fmt::fmt
)

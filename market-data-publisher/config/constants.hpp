#pragma once
#include <cstdint>
#include <chrono>

constexpr uint16_t CACHE_LINE = 64;
constexpr const char* SHM_NAME = "/market_data_shm";
constexpr uint32_t RING_SIZE = 1024;
constexpr uint16_t MESSAGE_INTERVAL = 5;
constexpr size_t MAX_INSTRUMENT_NAME_SIZE = 16;
constexpr uint32_t TCP_PORT = 9000;
constexpr const char* TCP_IP = "127.0.0.1";

constexpr uint32_t INITIAL_INSTRUMENT_PRICE = 11550; // this will translate to 115.50 but right now I print it straight away
constexpr uint32_t TICK_SIZE = 25; // this will translate to 0.25
constexpr const char* INSTRUMENT_NAME = "RELIANCE";

static inline uint64_t now_ns() {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::steady_clock::now().time_since_epoch()
    ).count();
}
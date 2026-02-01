#pragma once
#include <atomic>
#include "message.hpp"
#include "../config/constants.hpp"

template<uint32_t N>
struct RingBuffer {
    
    // added this for modulo operation
    static_assert((N & (N - 1)) == 0, "RingBuffer size must be a power of two");

    alignas(CACHE_LINE) std::atomic<uint32_t> head{0};
    alignas(CACHE_LINE) std::atomic<uint32_t> tail{0};
    alignas(CACHE_LINE) Message buffer[N];
};


template<uint32_t N>
inline bool push(RingBuffer<N> &rb, const Message &msg) {
    uint32_t current_head = rb.head.load(std::memory_order_relaxed);

    uint32_t current_tail = rb.tail.load(std::memory_order_acquire);

    if ((current_head-current_tail) >= N) {
        return false;
    }

    rb.buffer[current_head & (N - 1)] = msg;

    rb.head.store(current_head + 1, std::memory_order_release);
    return true;
}

template<uint32_t N>
inline bool pop(RingBuffer<N> &rb, Message &msg) {

    uint32_t current_tail = rb.tail.load(std::memory_order_relaxed);

    uint32_t current_head = rb.head.load(std::memory_order_acquire);

    if (current_tail == current_head) {
        return false;
    }

    msg = rb.buffer[current_tail & (N - 1)];

    rb.tail.store(current_tail + 1, std::memory_order_release);
    return true;
}
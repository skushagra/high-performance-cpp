#pragma once

#include "../types/ring_buffer.hpp"
#include "../types/message.hpp"

class ShmConsumer {
public:
    ShmConsumer(const char* shm_name);
    ~ShmConsumer();

    void run();

private:
    void attach_shm(const char* shm_name);

private:
    int shm_fd_{-1};
    RingBuffer<RING_SIZE>* rb_{nullptr};
};

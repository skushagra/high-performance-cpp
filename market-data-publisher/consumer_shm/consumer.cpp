#include "consumer.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <chrono>
#include <fmt/core.h>
#include <cstdlib>

ShmConsumer::ShmConsumer(const char* shm_name) {
    attach_shm(shm_name);
}

ShmConsumer::~ShmConsumer() {
    if (rb_) {
        munmap(rb_, sizeof(RingBuffer<RING_SIZE>));
    }
    if (shm_fd_ != -1) {
        close(shm_fd_);
    }
}

void ShmConsumer::attach_shm(const char* shm_name) {
    using Ring = RingBuffer<RING_SIZE>;

    shm_fd_ = shm_open(shm_name, O_RDWR, 0666);
    if (shm_fd_ == -1) {
        perror("shm_open");
        std::abort();
    }

    void* ptr = mmap(nullptr,
                     sizeof(Ring),
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     shm_fd_,
                     0);

    if (ptr == MAP_FAILED) {
        perror("mmap");
        std::abort();
    }

    rb_ = static_cast<Ring*>(ptr);

    fmt::print("SHM consumer attached\n");
}

void ShmConsumer::run() {
    Message msg;

    while (true) {
        if (pop(*rb_, msg)) {
            fmt::print("[{}] {} BID={} ASK={}\n",
                       msg.timestamp_ns,
                       msg.instrument,
                       msg.bid,
                       msg.ask);
        }
    }
}

#pragma once

#include <cstdint>
#include <boost/asio.hpp>

#include "../types/ring_buffer.hpp"
#include "../types/message.hpp"

class MarketDataPublisher {
public:
    explicit MarketDataPublisher(const char* shm_name);
    ~MarketDataPublisher();

    void run();

private:
    
    void init_shm(const char* shm_name);
    void publish_shm(const Message& msg);

    
    void init_tcp();
    void publish_tcp(const Message& msg);

    
    Message generate_message();

private:
    // SHM
    int shm_fd_{-1};
    RingBuffer<RING_SIZE>* rb_{nullptr};

    // TCP (Boost.Asio)
    boost::asio::io_context io_;
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::ip::tcp::socket socket_;
    bool tcp_connected_{false};

    uint64_t price_{INITIAL_INSTRUMENT_PRICE};
};

#define BOOST_ASIO_DISABLE_BOOST_SYSTEM
#define BOOST_ASIO_USE_STD_SYSTEM_ERROR

#include "publisher.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <chrono>
#include <fmt/core.h>

using boost::asio::ip::tcp;

MarketDataPublisher::MarketDataPublisher(const char* shm_name)
    : acceptor_(io_), socket_(io_) {
    init_shm(shm_name);
    init_tcp();
}

MarketDataPublisher::~MarketDataPublisher() {
    if (rb_) {
        munmap(rb_, sizeof(RingBuffer<RING_SIZE>));
    }
    if (shm_fd_ != -1) {
        close(shm_fd_);
    }
}

void MarketDataPublisher::init_shm(const char* shm_name) {
    using Ring = RingBuffer<RING_SIZE>;

    shm_unlink(shm_name);

    shm_fd_ = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd_ == -1) {
        perror("shm_open");
        std::abort();
    }

    if (ftruncate(shm_fd_, sizeof(Ring)) == -1) {
        perror("ftruncate");
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

    rb_ = new (ptr) Ring();
    fmt::print("SHM publisher initialized\n");
}

void MarketDataPublisher::publish_shm(const Message& msg) {
    push(*rb_, msg);
}

void MarketDataPublisher::init_tcp() {
    using tcp = boost::asio::ip::tcp;

    tcp::endpoint endpoint(
        boost::asio::ip::make_address(TCP_IP),
        TCP_PORT
    );

    acceptor_.open(endpoint.protocol());
    acceptor_.set_option(tcp::acceptor::reuse_address(true));
    acceptor_.bind(endpoint);
    acceptor_.listen();

    acceptor_.non_blocking(true);

    tcp_connected_ = false;

    fmt::print("TCP publisher listening on {}:{}\n", TCP_IP, TCP_PORT);
}


void MarketDataPublisher::publish_tcp(const Message& msg) {
    using tcp = boost::asio::ip::tcp;

    try {
        if (!tcp_connected_) {
            boost::system::error_code ec;
            acceptor_.accept(socket_, ec);

            if (ec == boost::asio::error::would_block) {
                return;
            }

            if (ec) {
                fmt::print("TCP accept error: {}\n", ec.message());
                return;
            }

            socket_.set_option(tcp::no_delay(true));
            tcp_connected_ = true;
            fmt::print("TCP consumer connected\n");
        }

        auto json = fmt::format(
            "{{\"instrument\":\"{}\",\"bid\":{},\"ask\":{},\"timestamp_ns\":{}}}\n",
            msg.instrument,
            msg.bid,
            msg.ask,
            msg.timestamp_ns
        );

        boost::asio::write(socket_, boost::asio::buffer(json));
    }
    catch (const std::exception& e) {
        fmt::print("TCP error: {}\n", e.what());
        tcp_connected_ = false;
        if (socket_.is_open()) socket_.close();
    }
}




Message MarketDataPublisher::generate_message() {
    Message msg{};
    std::strncpy(msg.instrument, INSTRUMENT_NAME, sizeof(msg.instrument));
    msg.bid = price_;
    msg.ask = price_ + TICK_SIZE;
    msg.timestamp_ns = now_ns();

    price_ += TICK_SIZE;
    return msg;
}

void MarketDataPublisher::run() {
    fmt::print("Market Data Publisher running (1 message/{} seconds)\n", MESSAGE_INTERVAL);
    while (true) {
        Message msg = generate_message();

        publish_shm(msg);
        publish_tcp(msg);

        ::sleep(MESSAGE_INTERVAL);
    }
}

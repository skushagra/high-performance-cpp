
#include "consumer.hpp"

#include <fmt/core.h>
#include <iostream>

using boost::asio::ip::tcp;

TcpConsumer::TcpConsumer(const char* ip, int port)
    : socket_(io_) {

    tcp::endpoint endpoint(
        boost::asio::ip::make_address(ip),
        port
    );

    socket_.connect(endpoint);

    socket_.set_option(tcp::no_delay(true)); // I have disabled the Nagle's algorithm

    fmt::print("TCP consumer connected to {}:{}\n", ip, port);
}

void TcpConsumer::run() {
    std::string pending;
    pending.reserve(4096);

    char buffer[4096];

    for (;;) {
        boost::system::error_code ec;
        const std::size_t bytes =
            socket_.read_some(boost::asio::buffer(buffer), ec);

        if (ec) {
            fmt::print("TCP read error: {}\n", ec.message());
            return;
        }

        pending.append(buffer, bytes);

        std::size_t newline_pos;
        while ((newline_pos = pending.find('\n')) != std::string::npos) {
            std::string_view line(pending.data(), newline_pos);
            if (!line.empty()) {
                fmt::print("TCP: {}\n", line);
            }
            pending.erase(0, newline_pos + 1);
        }
    }
}

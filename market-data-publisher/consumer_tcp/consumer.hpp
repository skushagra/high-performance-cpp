#pragma once

#include <boost/asio.hpp>

class TcpConsumer {
public:
    TcpConsumer(const char* ip, int port);
    void run();

private:
    boost::asio::io_context io_;
    boost::asio::ip::tcp::socket socket_;
};
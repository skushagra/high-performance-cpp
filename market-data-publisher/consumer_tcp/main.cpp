#include "consumer.hpp"
#include "../config/constants.hpp"

int main() {
    TcpConsumer consumer(TCP_IP, TCP_PORT);
    consumer.run();
    return 0;
}

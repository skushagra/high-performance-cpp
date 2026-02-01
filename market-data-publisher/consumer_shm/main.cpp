#include "consumer.hpp"

int main() {
    ShmConsumer consumer(SHM_NAME);
    consumer.run();
    return 0;
}

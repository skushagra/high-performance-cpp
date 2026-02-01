#include "publisher.hpp"

int main() {
    MarketDataPublisher publisher(SHM_NAME);
    publisher.run();
    return 0;
}

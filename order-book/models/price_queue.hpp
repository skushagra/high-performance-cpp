#pragma once

#include <stdint.h>
#include <set>
#include "order.hpp"

// Comparator for maintaining FIFO order within a price level
// Orders are sorted by timestamp (earlier timestamp = higher priority)
struct OrderComparator {
    bool operator()(const Order* a, const Order* b) const {
        // First sort by timestamp (FIFO)
        if (a->timestamp_ns != b->timestamp_ns) {
            return a->timestamp_ns < b->timestamp_ns;
        }
        // If timestamps are equal (unlikely), use order_id as tiebreaker
        return a->order_id < b->order_id;
    }
};

struct PriceQueue {
private:
    double price;
    uint64_t total_quantity = 0;
    
    std::set<Order*, OrderComparator> orders;

public:

    PriceQueue() = default;
    PriceQueue(double p): price(p), total_quantity(0) {}

    // Add order to the queue - O(log n) where n is orders at this price
    void add_order(Order* order) {
        orders.insert(order);
        total_quantity += order->quantity;
    }

    // Remove a specific order from the queue - O(log n)
    bool remove_order(Order* order) {
        auto it = orders.find(order);
        if (it != orders.end()) {
            total_quantity -= order->quantity;
            orders.erase(it);
            return true;
        }
        return false;
    }

    // Update order quantity (for amend operations)
    void update_quantity(Order* order, uint64_t old_quantity, uint64_t new_quantity) {
        total_quantity = total_quantity - old_quantity + new_quantity;
    }

    // Check if queue is empty
    bool is_empty() const {
        return orders.empty();
    }

    // Get total quantity at this price level
    uint64_t get_total_quantity() const {
        return total_quantity;
    }

    // Get price of this level
    double get_price() const {
        return price;
    }

    // Get number of orders at this price
    size_t get_order_count() const {
        return orders.size();
    }
};
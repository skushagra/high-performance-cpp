#pragma once

#include <cstdint>
#include <vector>
#include <string>
#include "models/order.hpp"
#include "models/price_model.hpp"
#include <map>
#include <iostream>
#include <iomanip>
#include "models/price_queue.hpp"

class OrderBook {
public:
    void add_order(const Order& m_order);
    bool cancel_order(uint64_t m_orderId);
    bool amend_order(uint64_t m_orderId, double m_newPrice, uint64_t m_newQuantity);
    void get_snapshot(size_t m_depth, std::vector<PriceLevel>& m_bids, std::vector<PriceLevel>& m_asks) const;
    void print_book(size_t m_depth) const;
private:
    std::map<double, PriceQueue, std::greater<double>> m_bids;
    std::map<double, PriceQueue, std::less<double>> m_asks;
    std::unordered_map<uint64_t, Order*> m_orderMap;
};

void OrderBook::add_order(const Order& m_order) {
    Order* new_order = new Order(m_order);
    
    m_orderMap[new_order->order_id] = new_order;
    
    if (new_order->is_buy) {
        auto it = m_bids.find(new_order->price);
        if (it == m_bids.end()) {
            m_bids.emplace(new_order->price, PriceQueue(new_order->price));
            it = m_bids.find(new_order->price);
        }
        it->second.add_order(new_order);
    } else {
        auto it = m_asks.find(new_order->price);
        if (it == m_asks.end()) {
            m_asks.emplace(new_order->price, PriceQueue(new_order->price));
            it = m_asks.find(new_order->price);
        }
        it->second.add_order(new_order);
    }
}

bool OrderBook::cancel_order(uint64_t m_orderId) {
    auto it = m_orderMap.find(m_orderId);
    if (it == m_orderMap.end()) {
        return false;
    }
    
    Order* order = it->second;
    
    if (order->is_buy) {
        auto price_it = m_bids.find(order->price);
        if (price_it != m_bids.end()) {
            price_it->second.remove_order(order);
            if (price_it->second.is_empty()) {
                m_bids.erase(price_it);
            }
        }
    } else {
        auto price_it = m_asks.find(order->price);
        if (price_it != m_asks.end()) {
            price_it->second.remove_order(order);
            if (price_it->second.is_empty()) {
                m_asks.erase(price_it);
            }
        }
    }
    
    m_orderMap.erase(it);
    
    delete order;
    
    return true;
}

bool OrderBook::amend_order(uint64_t m_orderId, double m_newPrice, uint64_t m_newQuantity) {
    auto it = m_orderMap.find(m_orderId);
    if (it == m_orderMap.end()) {
        return false;
    }
    
    Order* order = it->second;
    double old_price = order->price;
    uint64_t old_quantity = order->quantity;
    
    if (old_price != m_newPrice) {
        Order new_order = *order;
        new_order.price = m_newPrice;
        new_order.quantity = m_newQuantity;
        
        cancel_order(m_orderId);
        
        add_order(new_order);
    } else {
        order->quantity = m_newQuantity;
        
        if (order->is_buy) {
            auto price_it = m_bids.find(order->price);
            if (price_it != m_bids.end()) {
                price_it->second.update_quantity(order, old_quantity, m_newQuantity);
            }
        } else {
            auto price_it = m_asks.find(order->price);
            if (price_it != m_asks.end()) {
                price_it->second.update_quantity(order, old_quantity, m_newQuantity);
            }
        }
    }
    
    return true;
}

void OrderBook::get_snapshot(size_t m_depth, std::vector<PriceLevel>& m_bids, std::vector<PriceLevel>& m_asks) const {
    m_bids.clear();
    m_asks.clear();
    
    size_t count = 0;
    for (const auto& [price, queue] : this->m_bids) {
        if (count >= m_depth) break;
        PriceLevel level;
        level.price = price;
        level.total_quantity = queue.get_total_quantity();
        m_bids.push_back(level);
        count++;
    }
    
    count = 0;
    for (const auto& [price, queue] : this->m_asks) {
        if (count >= m_depth) break;
        PriceLevel level;
        level.price = price;
        level.total_quantity = queue.get_total_quantity();
        m_asks.push_back(level);
        count++;
    }
}

void OrderBook::print_book(size_t m_depth) const {
    std::vector<PriceLevel> bids, asks;
    get_snapshot(m_depth, bids, asks);
    
    std::cout << "\n==================== ORDER BOOK ====================\n";
    std::cout << std::fixed << std::setprecision(2);
    
    // Print header
    std::cout << std::setw(20) << "BIDS" << " | " << std::setw(20) << "ASKS" << "\n";
    std::cout << std::setw(10) << "Quantity" << " " << std::setw(10) << "Price" 
              << " | " << std::setw(10) << "Price" << " " << std::setw(10) << "Quantity" << "\n";
    std::cout << "----------------------------------------------------\n";
    
    size_t max_levels = std::max(bids.size(), asks.size());
    for (size_t i = 0; i < max_levels; ++i) {
        if (i < bids.size()) {
            std::cout << std::setw(10) << bids[i].total_quantity << " " 
                      << std::setw(10) << bids[i].price;
        } else {
            std::cout << std::setw(21) << " ";
        }
        
        std::cout << " | ";
        
        if (i < asks.size()) {
            std::cout << std::setw(10) << asks[i].price << " " 
                      << std::setw(10) << asks[i].total_quantity;
        }
        
        std::cout << "\n";
    }
    
    std::cout << "====================================================\n\n";
}

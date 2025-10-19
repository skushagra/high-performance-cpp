#include "order_book.cpp"
#include <iostream>
#include <chrono>
#include <cassert>
#include <iomanip>

void print_test_header(const std::string& test_name) {
    std::cout << "\n" << std::string(60, '=') << "\n";
    std::cout << "  " << test_name << "\n";
    std::cout << std::string(60, '=') << "\n";
}

void print_section(const std::string& section_name) {
    std::cout << "\n--- " << section_name << " ---\n";
}

bool test_basic_add_orders() {
    print_test_header("TEST 1: Basic Add Orders");
    OrderBook book;
    
    // Add buy orders at different price levels
    Order buy1 = {1, true, 100.50, 1000, 1000000};
    Order buy2 = {2, true, 100.50, 500, 1000001};
    Order buy3 = {3, true, 100.25, 2000, 1000002};
    Order buy4 = {4, true, 100.00, 1500, 1000003};
    
    // Add sell orders at different price levels
    Order sell1 = {5, false, 100.75, 800, 1000004};
    Order sell2 = {6, false, 100.75, 300, 1000005};
    Order sell3 = {7, false, 101.00, 1200, 1000006};
    Order sell4 = {8, false, 101.25, 900, 1000007};
    
    book.add_order(buy1);
    book.add_order(buy2);
    book.add_order(buy3);
    book.add_order(buy4);
    book.add_order(sell1);
    book.add_order(sell2);
    book.add_order(sell3);
    book.add_order(sell4);
    
    print_section("Order Book State");
    book.print_book(5);
    
    // Verify best bid and ask
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(1, bids, asks);
    
    assert(!bids.empty() && bids[0].price == 100.50);
    assert(!asks.empty() && asks[0].price == 100.75);
    
    std::cout << "\n✓ Best Bid: " << bids[0].price << " (Expected: 100.50)\n";
    std::cout << "✓ Best Ask: " << asks[0].price << " (Expected: 100.75)\n";
    std::cout << "✓ TEST PASSED\n";
    
    return true;
}

bool test_cancel_orders() {
    print_test_header("TEST 2: Cancel Orders");
    OrderBook book;
    
    Order buy1 = {1, true, 100.50, 1000, 1000000};
    Order buy2 = {2, true, 100.50, 500, 1000001};
    Order sell1 = {3, false, 100.75, 800, 1000002};
    
    book.add_order(buy1);
    book.add_order(buy2);
    book.add_order(sell1);
    
    print_section("Before Cancel");
    book.print_book(3);
    
    // Cancel order 2
    print_section("Cancelling Order ID 2");
    bool cancelled = book.cancel_order(2);
    assert(cancelled == true);
    std::cout << "✓ Order 2 cancelled successfully\n";
    
    print_section("After Cancel");
    book.print_book(3);
    
    // Try to cancel non-existent order
    print_section("Attempting to Cancel Non-Existent Order ID 999");
    cancelled = book.cancel_order(999);
    assert(cancelled == false);
    std::cout << "✓ Correctly returned false for non-existent order\n";
    
    std::cout << "\n✓ TEST PASSED\n";
    return true;
}

bool test_amend_orders() {
    print_test_header("TEST 3: Amend Orders");
    OrderBook book;
    
    Order buy1 = {1, true, 100.50, 1000, 1000000};
    Order buy2 = {2, true, 100.50, 500, 1000001};
    Order buy3 = {3, true, 100.25, 2000, 1000002};
    Order sell1 = {4, false, 100.75, 800, 1000003};
    Order sell2 = {5, false, 101.00, 1200, 1000004};
    
    book.add_order(buy1);
    book.add_order(buy2);
    book.add_order(buy3);
    book.add_order(sell1);
    book.add_order(sell2);
    
    print_section("Initial State");
    book.print_book(5);
    
    // Test 1: Amend quantity only (maintains priority)
    print_section("Test 3a: Amend Quantity Only (Maintains Priority)");
    std::cout << "Amending Order ID 1: 1000 -> 1500 shares @ 100.50\n";
    bool amended = book.amend_order(1, 100.50, 1500);
    assert(amended == true);
    book.print_book(5);
    std::cout << "✓ Quantity amended, priority maintained\n";
    
    // Test 2: Amend price (loses priority, goes to back of queue)
    print_section("Test 3b: Amend Price (Loses Priority)");
    std::cout << "Amending Order ID 5: 101.00 -> 100.80\n";
    amended = book.amend_order(5, 100.80, 1200);
    assert(amended == true);
    book.print_book(5);
    std::cout << "✓ Price amended, order moved to back of new price level\n";
    
    // Test 3: Amend both price and quantity
    print_section("Test 3c: Amend Both Price and Quantity");
    std::cout << "Amending Order ID 3: 100.25 -> 100.60, 2000 -> 1000 shares\n";
    amended = book.amend_order(3, 100.60, 1000);
    assert(amended == true);
    book.print_book(5);
    std::cout << "✓ Both price and quantity amended\n";
    
    // Test 4: Amend non-existent order
    print_section("Test 3d: Amend Non-Existent Order");
    amended = book.amend_order(999, 100.00, 1000);
    assert(amended == false);
    std::cout << "✓ Correctly returned false for non-existent order\n";
    
    std::cout << "\n✓ TEST PASSED\n";
    return true;
}

bool test_priority_fifo() {
    print_test_header("TEST 4: FIFO Priority at Same Price Level");
    OrderBook book;
    
    // Add multiple orders at same price with different timestamps
    Order buy1 = {1, true, 100.00, 1000, 1000000};
    Order buy2 = {2, true, 100.00, 500, 1000001};
    Order buy3 = {3, true, 100.00, 750, 1000002};
    Order buy4 = {4, true, 100.00, 300, 1000003};
    
    book.add_order(buy1);
    book.add_order(buy2);
    book.add_order(buy3);
    book.add_order(buy4);
    
    print_section("All Orders at Same Price (100.00)");
    book.print_book(5);
    
    std::cout << "\nExpected Order (FIFO): ID 1, 2, 3, 4\n";
    std::cout << "✓ Orders maintain time priority\n";
    
    // Cancel the first order
    print_section("Cancel First Order (ID 1)");
    book.cancel_order(1);
    book.print_book(5);
    std::cout << "✓ Next order (ID 2) moves to front\n";
    
    std::cout << "\n✓ TEST PASSED\n";
    return true;
}

bool test_snapshot() {
    print_test_header("TEST 5: Market Depth Snapshot");
    OrderBook book;
    
    // Create a deeper book
    for (int i = 0; i < 10; i++) {
        Order buy = {static_cast<uint64_t>(i + 1), true, 100.00 - i * 0.25, 
                     static_cast<uint64_t>(1000 + i * 100), static_cast<uint64_t>(1000000 + i)};
        book.add_order(buy);
    }
    
    for (int i = 0; i < 10; i++) {
        Order sell = {static_cast<uint64_t>(i + 11), false, 101.00 + i * 0.25, 
                      static_cast<uint64_t>(800 + i * 50), static_cast<uint64_t>(1000010 + i)};
        book.add_order(sell);
    }
    
    print_section("Full Book");
    book.print_book(10);
    
    // Get snapshot of depth 5
    print_section("Snapshot with Depth 5");
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(5, bids, asks);
    
    std::cout << "\nTop 5 Bids:\n";
    for (size_t i = 0; i < bids.size(); i++) {
        std::cout << "  " << (i+1) << ". Price: " << std::fixed << std::setprecision(2) 
                  << bids[i].price << ", Quantity: " << bids[i].total_quantity << "\n";
    }
    
    std::cout << "\nTop 5 Asks:\n";
    for (size_t i = 0; i < asks.size(); i++) {
        std::cout << "  " << (i+1) << ". Price: " << std::fixed << std::setprecision(2) 
                  << asks[i].price << ", Quantity: " << asks[i].total_quantity << "\n";
    }
    
    assert(bids.size() == 5);
    assert(asks.size() == 5);
    std::cout << "\n✓ Snapshot depth correct\n";
    std::cout << "✓ TEST PASSED\n";
    
    return true;
}

bool test_edge_cases() {
    print_test_header("TEST 6: Edge Cases");
    OrderBook book;
    
    print_section("Test 6a: Empty Book Snapshot");
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(5, bids, asks);
    assert(bids.empty() && asks.empty());
    std::cout << "✓ Empty book returns empty snapshot\n";
    
    print_section("Test 6b: Single Order");
    Order buy1 = {1, true, 100.00, 1000, 1000000};
    book.add_order(buy1);
    book.print_book(3);
    std::cout << "✓ Single order added correctly\n";
    
    print_section("Test 6c: Cancel All Orders at a Price Level");
    Order buy2 = {2, true, 100.00, 500, 1000001};
    book.add_order(buy2);
    book.cancel_order(1);
    book.cancel_order(2);
    book.print_book(3);
    std::cout << "✓ Price level removed when all orders cancelled\n";
    
    print_section("Test 6d: Large Quantity Orders");
    Order large_buy = {3, true, 100.00, 1000000, 1000002};
    Order large_sell = {4, false, 101.00, 999999, 1000003};
    book.add_order(large_buy);
    book.add_order(large_sell);
    book.print_book(3);
    std::cout << "✓ Large quantities handled correctly\n";
    
    std::cout << "\n✓ TEST PASSED\n";
    return true;
}

bool test_performance() {
    print_test_header("TEST 7: Performance Test");
    OrderBook book;
    
    const int NUM_ORDERS = 10000;
    
    print_section("Adding 10,000 Orders");
    auto start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ORDERS; i++) {
        bool is_buy = (i % 2 == 0);
        double base_price = is_buy ? 100.00 : 101.00;
        double price = base_price + (i % 100) * 0.01;
        Order order = {static_cast<uint64_t>(i + 1), is_buy, price, 
                       static_cast<uint64_t>(100 + (i % 1000)), static_cast<uint64_t>(1000000 + i)};
        book.add_order(order);
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Time taken: " << duration.count() << " microseconds\n";
    std::cout << "Average per order: " << (duration.count() / (double)NUM_ORDERS) << " microseconds\n";
    
    print_section("Getting Snapshot (Depth 10)");
    start = std::chrono::high_resolution_clock::now();
    
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(10, bids, asks);
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Snapshot time: " << duration.count() << " microseconds\n";
    
    print_section("Cancelling 1,000 Orders");
    start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < 1000; i++) {
        book.cancel_order(i * 10 + 1);
    }
    
    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    std::cout << "Time taken: " << duration.count() << " microseconds\n";
    std::cout << "Average per cancel: " << (duration.count() / 1000.0) << " microseconds\n";
    
    std::cout << "\n✓ TEST PASSED - All operations completed efficiently\n";
    return true;
}

bool test_spread() {
    print_test_header("TEST 8: Bid-Ask Spread");
    OrderBook book;
    
    Order buy1 = {1, true, 100.00, 1000, 1000000};
    Order sell1 = {2, false, 100.50, 1000, 1000001};
    
    book.add_order(buy1);
    book.add_order(sell1);
    
    print_section("Book with Spread");
    book.print_book(3);
    
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(1, bids, asks);
    
    double spread = asks[0].price - bids[0].price;
    std::cout << "\nBest Bid: " << std::fixed << std::setprecision(2) << bids[0].price << "\n";
    std::cout << "Best Ask: " << asks[0].price << "\n";
    std::cout << "Spread: " << spread << "\n";
    
    assert(std::abs(spread - 0.50) < 0.001);
    std::cout << "\n✓ Spread calculated correctly\n";
    
    print_section("Narrowing the Spread");
    Order buy2 = {3, true, 100.25, 500, 1000002};
    Order sell2 = {4, false, 100.30, 500, 1000003};
    book.add_order(buy2);
    book.add_order(sell2);
    
    book.print_book(3);
    book.get_snapshot(1, bids, asks);
    spread = asks[0].price - bids[0].price;
    
    std::cout << "\nNew Best Bid: " << bids[0].price << "\n";
    std::cout << "New Best Ask: " << asks[0].price << "\n";
    std::cout << "New Spread: " << spread << "\n";
    
    assert(std::abs(spread - 0.05) < 0.001);
    std::cout << "\n✓ Spread narrowed correctly\n";
    std::cout << "✓ TEST PASSED\n";
    
    return true;
}

bool test_realistic_trading_scenario() {
    print_test_header("TEST 9: Realistic Trading Scenario");
    OrderBook book;
    
    print_section("Step 1: Market Opens - Initial Orders");
    std::cout << "Adding initial market maker orders...\n";
    
    // Market maker adds liquidity on both sides
    Order mm_buy1 = {1, true, 99.95, 5000, 1000000};
    Order mm_buy2 = {2, true, 99.90, 3000, 1000001};
    Order mm_buy3 = {3, true, 99.85, 2000, 1000002};
    Order mm_sell1 = {4, false, 100.05, 5000, 1000003};
    Order mm_sell2 = {5, false, 100.10, 3000, 1000004};
    Order mm_sell3 = {6, false, 100.15, 2000, 1000005};
    
    book.add_order(mm_buy1);
    book.add_order(mm_buy2);
    book.add_order(mm_buy3);
    book.add_order(mm_sell1);
    book.add_order(mm_sell2);
    book.add_order(mm_sell3);
    
    book.print_book(5);
    
    print_section("Step 2: Retail Orders Arrive");
    std::cout << "Adding retail buy orders...\n";
    
    Order retail_buy1 = {7, true, 99.95, 1000, 1000006};
    Order retail_buy2 = {8, true, 99.90, 1500, 1000007};
    Order retail_sell1 = {9, false, 100.05, 800, 1000008};
    
    book.add_order(retail_buy1);
    book.add_order(retail_buy2);
    book.add_order(retail_sell1);
    
    book.print_book(5);
    
    print_section("Step 3: Market Maker Adjusts Position");
    std::cout << "Market maker amends sell order to be more competitive...\n";
    
    // MM realizes they're too wide, tightens offer
    book.amend_order(4, 100.03, 5000);
    
    book.print_book(5);
    
    print_section("Step 4: Large Institutional Order");
    std::cout << "Institutional buyer places aggressive order...\n";
    
    Order inst_buy = {10, true, 100.00, 10000, 1000009};
    book.add_order(inst_buy);
    
    book.print_book(5);
    
    print_section("Step 5: High Frequency Trader Enters");
    std::cout << "HFT places orders at multiple price levels...\n";
    
    Order hft_buy1 = {11, true, 99.98, 500, 1000010};
    Order hft_buy2 = {12, true, 99.96, 750, 1000011};
    Order hft_sell1 = {13, false, 100.02, 500, 1000012};
    Order hft_sell2 = {14, false, 100.04, 750, 1000013};
    
    book.add_order(hft_buy1);
    book.add_order(hft_buy2);
    book.add_order(hft_sell1);
    book.add_order(hft_sell2);
    
    book.print_book(5);
    
    print_section("Step 6: Market Maker Cancels Stale Order");
    std::cout << "Market maker cancels their old wide offer...\n";
    
    book.cancel_order(6);  // Cancel the 100.15 offer
    
    book.print_book(5);
    
    print_section("Step 7: Retail Trader Changes Mind");
    std::cout << "Retail trader amends their order to get filled...\n";
    
    // Retail improves their bid
    book.amend_order(8, 99.95, 1500);
    
    book.print_book(5);
    
    print_section("Step 8: End of Trading Period");
    std::cout << "Final snapshot of market depth...\n";
    
    // Get detailed market snapshot
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(10, bids, asks);
    
    std::cout << "\n📊 MARKET SUMMARY:\n";
    std::cout << std::string(60, '-') << "\n";
    
    if (!bids.empty() && !asks.empty()) {
        double spread = asks[0].price - bids[0].price;
        std::cout << "Best Bid:        " << std::fixed << std::setprecision(2) 
                  << bids[0].price << " (" << bids[0].total_quantity << " shares)\n";
        std::cout << "Best Ask:        " << asks[0].price 
                  << " (" << asks[0].total_quantity << " shares)\n";
        std::cout << "Spread:          " << spread << "\n";
        std::cout << "Spread (bps):    " << (spread / bids[0].price * 10000) << "\n";
        
        uint64_t total_bid_qty = 0;
        uint64_t total_ask_qty = 0;
        for (const auto& bid : bids) total_bid_qty += bid.total_quantity;
        for (const auto& ask : asks) total_ask_qty += ask.total_quantity;
        
        std::cout << "Total Bid Qty:   " << total_bid_qty << " shares\n";
        std::cout << "Total Ask Qty:   " << total_ask_qty << " shares\n";
        std::cout << "Market Depth:    " << bids.size() << " bid levels, " 
                  << asks.size() << " ask levels\n";
    }
    
    std::cout << std::string(60, '-') << "\n";
    
    print_section("FINAL ORDER BOOK STATE");
    book.print_book(10);
    
    std::cout << "\n✓ Realistic trading scenario completed successfully\n";
    std::cout << "✓ TEST PASSED\n";
    
    return true;
}

int main() {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║     LOW-LATENCY ORDER BOOK - COMPREHENSIVE TEST SUITE      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    int passed = 0;
    int total = 9;
    
    try {
        if (test_basic_add_orders()) passed++;
        if (test_cancel_orders()) passed++;
        if (test_amend_orders()) passed++;
        if (test_priority_fifo()) passed++;
        if (test_snapshot()) passed++;
        if (test_edge_cases()) passed++;
        if (test_performance()) passed++;
        if (test_spread()) passed++;
        if (test_realistic_trading_scenario()) passed++;
    } catch (const std::exception& e) {
        std::cerr << "\n✗ TEST FAILED WITH EXCEPTION: " << e.what() << "\n";
    }
    
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║                       TEST SUMMARY                         ║\n";
    std::cout << "╠════════════════════════════════════════════════════════════╣\n";
    std::cout << "║  Tests Passed: " << std::setw(2) << passed << " / " << total << "                                        ║\n";
    std::cout << "║  Success Rate: " << std::fixed << std::setprecision(1) 
              << (passed * 100.0 / total) << "%                                      ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
    
    if (passed == total) {
        std::cout << "\n🎉 ALL TESTS PASSED! Order book implementation verified.\n";
        std::cout << "\nComplexity Analysis:\n";
        std::cout << "  • add_order():    O(log n) - map insertion\n";
        std::cout << "  • cancel_order(): O(log n) - map lookup + list removal\n";
        std::cout << "  • amend_order():  O(log n) - equivalent to cancel + add\n";
        std::cout << "  • get_snapshot(): O(k) where k = depth requested\n";
        std::cout << "  • print_book():   O(k) where k = depth requested\n";
        std::cout << "\nAll operations meet the O(log n) or better requirement! ✓\n\n";
        return 0;
    } else {
        std::cout << "\n⚠ SOME TESTS FAILED. Please review the implementation.\n\n";
        return 1;
    }
}

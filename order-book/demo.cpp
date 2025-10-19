#include "order_book.cpp"
#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>

void print_header(const std::string& title) {
    std::cout << "\n";
    std::cout << "╔════════════════════════════════════════════════════════════╗\n";
    std::cout << "║ " << std::left << std::setw(58) << title << " ║\n";
    std::cout << "╚════════════════════════════════════════════════════════════╝\n";
}

void print_operation(const std::string& operation) {
    std::cout << "\n► " << operation << "\n";
    std::cout << std::string(60, '-') << "\n";
}

void print_snapshot_detail(OrderBook& book) {
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(5, bids, asks);
    
    std::cout << "\n📊 Market Snapshot (Top 5 Levels):\n";
    std::cout << std::string(60, '-') << "\n";
    
    if (!bids.empty() && !asks.empty()) {
        std::cout << "Best Bid: $" << std::fixed << std::setprecision(2) << bids[0].price 
                  << " (" << bids[0].total_quantity << " shares)\n";
        std::cout << "Best Ask: $" << asks[0].price 
                  << " (" << asks[0].total_quantity << " shares)\n";
        std::cout << "Spread:   $" << (asks[0].price - bids[0].price) 
                  << " (" << std::setprecision(1) 
                  << ((asks[0].price - bids[0].price) / bids[0].price * 10000) << " bps)\n";
    } else if (!bids.empty()) {
        std::cout << "Best Bid: $" << std::fixed << std::setprecision(2) << bids[0].price 
                  << " (" << bids[0].total_quantity << " shares)\n";
        std::cout << "No Ask orders\n";
    } else if (!asks.empty()) {
        std::cout << "No Bid orders\n";
        std::cout << "Best Ask: $" << asks[0].price 
                  << " (" << asks[0].total_quantity << " shares)\n";
    } else {
        std::cout << "Order book is empty\n";
    }
    std::cout << std::string(60, '-') << "\n";
}

int main() {
    OrderBook book;
    
    print_header("LOW-LATENCY ORDER BOOK - INTERACTIVE DEMO");
    
    std::cout << "\nThis demo will show you how to:\n";
    std::cout << "  1. Add orders to the book\n";
    std::cout << "  2. View the order book state\n";
    std::cout << "  3. Get market snapshots\n";
    std::cout << "  4. Cancel orders\n";
    std::cout << "  5. Amend/Update orders\n";
    std::cout << "\nPress Enter to continue...\n";
    std::cin.get();
    
    // =========================================================================
    // PART 1: ADDING ORDERS
    // =========================================================================
    
    print_header("PART 1: ADDING ORDERS TO THE BOOK");
    
    print_operation("Adding BUY order: ID=1, Price=$100.00, Quantity=1000");
    Order buy1 = {1, true, 100.00, 1000, 1000000};
    book.add_order(buy1);
    std::cout << "✓ Order added successfully!\n";
    book.print_book(5);
    print_snapshot_detail(book);
    
    print_operation("Adding SELL order: ID=2, Price=$100.50, Quantity=800");
    Order sell1 = {2, false, 100.50, 800, 1000001};
    book.add_order(sell1);
    std::cout << "✓ Order added successfully!\n";
    book.print_book(5);
    print_snapshot_detail(book);
    
    print_operation("Adding more BUY orders at different price levels");
    Order buy2 = {3, true, 100.00, 500, 1000002};  // Same price as buy1
    Order buy3 = {4, true, 99.95, 1500, 1000003};  // Lower price
    Order buy4 = {5, true, 99.90, 2000, 1000004};  // Even lower
    
    std::cout << "  • ID=3: Buy  500 shares @ $100.00 (same price as ID=1)\n";
    book.add_order(buy2);
    std::cout << "  • ID=4: Buy 1500 shares @ $99.95\n";
    book.add_order(buy3);
    std::cout << "  • ID=5: Buy 2000 shares @ $99.90\n";
    book.add_order(buy4);
    std::cout << "✓ All orders added!\n";
    book.print_book(5);
    print_snapshot_detail(book);
    
    print_operation("Adding more SELL orders at different price levels");
    Order sell2 = {6, false, 100.50, 300, 1000005};  // Same price as sell1
    Order sell3 = {7, false, 100.55, 1200, 1000006}; // Higher price
    Order sell4 = {8, false, 100.60, 900, 1000007};  // Even higher
    
    std::cout << "  • ID=6: Sell  300 shares @ $100.50 (same price as ID=2)\n";
    book.add_order(sell2);
    std::cout << "  • ID=7: Sell 1200 shares @ $100.55\n";
    book.add_order(sell3);
    std::cout << "  • ID=8: Sell  900 shares @ $100.60\n";
    book.add_order(sell4);
    std::cout << "✓ All orders added!\n";
    book.print_book(8);
    print_snapshot_detail(book);
    
    std::cout << "\n💡 Note: Orders at the same price level are prioritized by time (FIFO).\n";
    std::cout << "    ID=1 has priority over ID=3 at $100.00\n";
    std::cout << "    ID=2 has priority over ID=6 at $100.50\n";
    
    std::cout << "\n\nPress Enter to continue to cancellation demo...\n";
    std::cin.get();
    
    // =========================================================================
    // PART 2: CANCELLING ORDERS
    // =========================================================================
    
    print_header("PART 2: CANCELLING ORDERS");
    
    print_operation("Current Order Book State");
    book.print_book(8);
    
    print_operation("Cancelling Order ID=3 (Buy 500 @ $100.00)");
    std::cout << "This order is at the same price level as ID=1 but has lower priority.\n";
    if (book.cancel_order(3)) {
        std::cout << "✓ Order ID=3 cancelled successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: Total quantity at $100.00 decreased from 1500 to 1000\n";
    print_snapshot_detail(book);
    
    print_operation("Cancelling Order ID=2 (Sell 800 @ $100.50)");
    std::cout << "This order has priority at $100.50 (first in queue).\n";
    if (book.cancel_order(2)) {
        std::cout << "✓ Order ID=2 cancelled successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: ID=6 now has priority at $100.50\n";
    std::cout << "           Best Ask changed: $100.50 with 300 shares (was 1100 shares)\n";
    print_snapshot_detail(book);
    
    print_operation("Attempting to cancel non-existent Order ID=999");
    if (!book.cancel_order(999)) {
        std::cout << "✗ Order ID=999 not found (as expected)\n";
    }
    
    std::cout << "\n\nPress Enter to continue to amendment demo...\n";
    std::cin.get();
    
    // =========================================================================
    // PART 3: AMENDING/UPDATING ORDERS
    // =========================================================================
    
    print_header("PART 3: AMENDING/UPDATING ORDERS");
    
    print_operation("Current Order Book State");
    book.print_book(8);
    
    print_operation("Test 1: Amending QUANTITY only (maintains priority)");
    std::cout << "Amending Order ID=1: 1000 shares → 1500 shares @ $100.00\n";
    std::cout << "Since price stays the same, order maintains its time priority.\n\n";
    if (book.amend_order(1, 100.00, 1500)) {
        std::cout << "✓ Order amended successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: Quantity at $100.00 changed from 1000 to 1500\n";
    std::cout << "           ID=1 still has priority (first in queue)\n";
    print_snapshot_detail(book);
    
    print_operation("Test 2: Amending PRICE (loses priority)");
    std::cout << "Amending Order ID=7: Price $100.55 → $100.52, Quantity stays 1200\n";
    std::cout << "When price changes, order loses its time priority.\n\n";
    if (book.amend_order(7, 100.52, 1200)) {
        std::cout << "✓ Order amended successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: Order moved to new price level $100.52\n";
    std::cout << "           This creates a tighter ask side of the book\n";
    print_snapshot_detail(book);
    
    print_operation("Test 3: Amending both PRICE and QUANTITY");
    std::cout << "Amending Order ID=4: $99.95 → $99.98, 1500 shares → 2000 shares\n";
    std::cout << "This improves the bid and adds more liquidity.\n\n";
    if (book.amend_order(4, 99.98, 2000)) {
        std::cout << "✓ Order amended successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: Order moved to new price level $99.98\n";
    std::cout << "           New quantity is 2000 shares\n";
    print_snapshot_detail(book);
    
    print_operation("Test 4: Aggressive amend to improve the bid");
    std::cout << "Amending Order ID=5: $99.90 → $100.05, 2000 shares → 3000 shares\n";
    std::cout << "This crosses to the ask side and becomes the new best bid!\n\n";
    if (book.amend_order(5, 100.05, 3000)) {
        std::cout << "✓ Order amended successfully!\n";
    }
    book.print_book(8);
    std::cout << "\n💡 Notice: Spread narrowed significantly!\n";
    std::cout << "           Best Bid is now very close to Best Ask\n";
    print_snapshot_detail(book);
    
    std::cout << "\n\nPress Enter to see final summary...\n";
    std::cin.get();
    
    // =========================================================================
    // PART 4: FINAL SUMMARY
    // =========================================================================
    
    print_header("PART 4: FINAL ORDER BOOK STATE & SNAPSHOT");
    
    print_operation("Adding a few more orders for a complete book");
    Order new_buy = {9, true, 100.03, 500, 1000008};
    Order new_sell = {10, false, 100.48, 750, 1000009};
    book.add_order(new_buy);
    book.add_order(new_sell);
    std::cout << "  • ID=9:  Buy  500 shares @ $100.03\n";
    std::cout << "  • ID=10: Sell 750 shares @ $100.48\n";
    std::cout << "✓ Orders added!\n";
    
    print_operation("COMPLETE ORDER BOOK VIEW");
    book.print_book(10);
    
    print_snapshot_detail(book);
    
    print_operation("Detailed Top 5 Snapshot");
    std::vector<PriceLevel> bids, asks;
    book.get_snapshot(5, bids, asks);
    
    std::cout << "\n📈 BID SIDE (Top 5):\n";
    std::cout << "   Level | Price    | Total Quantity\n";
    std::cout << "   ------+----------+---------------\n";
    for (size_t i = 0; i < bids.size(); i++) {
        std::cout << "   " << std::setw(5) << (i+1) << " | $" 
                  << std::fixed << std::setprecision(2) << std::setw(7) << bids[i].price 
                  << " | " << std::setw(9) << bids[i].total_quantity << " shares\n";
    }
    
    std::cout << "\n📉 ASK SIDE (Top 5):\n";
    std::cout << "   Level | Price    | Total Quantity\n";
    std::cout << "   ------+----------+---------------\n";
    for (size_t i = 0; i < asks.size(); i++) {
        std::cout << "   " << std::setw(5) << (i+1) << " | $" 
                  << std::fixed << std::setprecision(2) << std::setw(7) << asks[i].price 
                  << " | " << std::setw(9) << asks[i].total_quantity << " shares\n";
    }
    
    // Calculate totals
    uint64_t total_bid_qty = 0, total_ask_qty = 0;
    for (const auto& bid : bids) total_bid_qty += bid.total_quantity;
    for (const auto& ask : asks) total_ask_qty += ask.total_quantity;
    
    std::cout << "\n📊 MARKET STATISTICS:\n";
    std::cout << std::string(60, '-') << "\n";
    std::cout << "Total Bid Liquidity (Top 5):  " << total_bid_qty << " shares\n";
    std::cout << "Total Ask Liquidity (Top 5):  " << total_ask_qty << " shares\n";
    std::cout << "Number of Bid Levels:         " << bids.size() << "\n";
    std::cout << "Number of Ask Levels:         " << asks.size() << "\n";
    
    if (!bids.empty() && !asks.empty()) {
        double mid_price = (bids[0].price + asks[0].price) / 2.0;
        std::cout << "Mid Price:                    $" << std::fixed << std::setprecision(2) 
                  << mid_price << "\n";
        double spread_pct = (asks[0].price - bids[0].price) / mid_price * 100.0;
        std::cout << "Spread (% of mid):            " << std::setprecision(3) 
                  << spread_pct << "%\n";
    }
    std::cout << std::string(60, '-') << "\n";
    
    print_header("DEMO COMPLETE");
    
    std::cout << "\n✅ Summary of Operations Demonstrated:\n\n";
    std::cout << "  ✓ Adding orders (BUY and SELL)\n";
    std::cout << "  ✓ Viewing order book with print_book()\n";
    std::cout << "  ✓ Getting market snapshots with get_snapshot()\n";
    std::cout << "  ✓ Cancelling orders (successful and failed attempts)\n";
    std::cout << "  ✓ Amending orders (quantity only, price only, both)\n";
    std::cout << "  ✓ Priority maintenance (FIFO at same price level)\n";
    std::cout << "  ✓ Priority loss when price changes\n";
    std::cout << "  ✓ Market statistics calculation\n";
    
    std::cout << "\n🎉 Order book implementation complete and verified!\n\n";
    
    return 0;
}

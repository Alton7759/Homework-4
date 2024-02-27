#pragma once

#include <map>
#include <set>
#include <string>
#include <iostream>

#include "GroceryItem.hpp"



class GroceryStore
{
  public:
    // Type Definition Aliases
    //    |Alias Name    |            |  Key             |  | Value                  |
    //    +--------------+            +------------------+  +------------------------+
    using GroceryItemsSold = std::set<std::string /*UPC */   /* N/A */                >;  // A collection of unique UPCs representing grocery items that have been sold

    using Inventory_DB     = std::map<std::string /*UPC */,  unsigned int /*quantity*/>;  // A collection of quantities indexed by UPC:                  Maintains of the quantity of grocery items in stock identified by UPC
    using ShoppingCart     = std::map<std::string /*UPC */,  GroceryItem              >;  // A collection of groceries indexed by UPC:                   An individual shopping cart filled with groceries
    using ShoppingCarts    = std::map<std::string /*name*/,  ShoppingCart             >;  // A collection of shopping carts indexed by customer's name:  A collection of shoppers, identified by name, each pushing a shopping
                                                                                          //                                                             cart.  Notice that this structure is a tree, and each element in the
                                                                                          //                                                             tree is also a tree. That is, this is a tree of trees.

    // Constructors, assignments, destructor
    GroceryStore( const std::string & persistentInventoryDB = "GroceryStoreInventory.dat" );

    // Returns a reference to the store's one and only inventory database
    Inventory_DB & inventory();


    // Each customer, in turn, places the groceries in their shopping cart on the checkout counter where they are scanned, paid for, and issued a receipt.
    // Returns a collection of unique UPCs for grocery items that have been sold
    GroceryItemsSold ringUpCustomers( const ShoppingCarts & shoppingCarts, std::ostream & receipt = std::cout );


    // Re-orders grocery items sold that have fallen below the re-order threshold, then clears the reorder list
    void reorderItems( GroceryItemsSold & todaysSales, std::ostream & reorderReport = std::cout );


    // Initializes a bunch of customers pushing shopping carts filled with groceries
    ShoppingCarts  makeShoppingCarts();


  private:
    // Instance attributes
    Inventory_DB _inventoryDB;                                                            // This store's inventory of grocery items indexed by UPC.


    // Class attributes
    inline static constexpr unsigned int REORDER_THRESHOLD = 15;                          // When the quantity on hand dips below this threshold, it's time to order more inventory
    inline static constexpr unsigned int LOT_COUNT         = 20;                          // Number of items that can be ordered at one time


    // Helper functions
    GroceryItemsSold ringUpCustomer( const ShoppingCart & shoppingCart, std::ostream & receipt );
};

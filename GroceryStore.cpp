///////////////////////// TO-DO (1) //////////////////////////////
  /// Include necessary header files
  /// Hint:  Include what you use, use what you include
  #include <fstream>
  #include <string>
  #include <iostream>
  #include <ostream>
  #include <iomanip>
  #include <utility>

  #include <GroceryStore.hpp>
  #include <GroceryItemDatabase.hpp>
/////////////////////// END-TO-DO (1) ////////////////////////////






GroceryStore::GroceryStore( const std::string & persistentInventoryDB )
{
  std::ifstream fin( persistentInventoryDB );                     // Creates the stream object, and then opens the file if it can
                                                                  // The file is closed as fin goes out of scope
  if( !fin.is_open() ) std::cerr << "Warning:  Could not open persistent inventory database file \"" << persistentInventoryDB << "\".  Proceeding with empty inventory\n\n";

  // The file's inventory contents consists of a quoted UPC string followed by a quantity on hand unsigned integer separated by
  // whitespace, like this:
  //     "00044100117428"     8
  //     "00041780001566"    46
  //     "00021000043309"    35
  //     "00021000043309"     9

  ///////////////////////// TO-DO (2) //////////////////////////////
    /// While no errors have been detected and not end-of-file, read the inventory record from the input stream and then add that
    /// information to the memory resident inventory database.
    ///
    /// Hint: Since we didn't define an InventoryRecord class that defines the extraction operator (best practices says we should
    ///       have), extract the quoted string and the quantity attributes directly
    ///
    std::string sholder;
    unsigned int intholder;
    while(fin >> std::quoted(sholder) >> std::ws >> intholder) _inventoryDB.insert({sholder,intholder});
    
    
    /// Hint: Just as you did in class GroceryItem, use std::quoted to read quoted strings.  Don't try to parse the quotes yourself.
    ///       See
    ///        1) https://en.cppreference.com/w/cpp/io/manip/quoted
    ///        2) https://www.youtube.com/watch?v=Mu-GUZuU31A
  
  /////////////////////// END-TO-DO (2) ////////////////////////////
}                                                                 // File is closed as fin goes out of scope (RAII)







GroceryStore::Inventory_DB & GroceryStore::inventory()
{ return _inventoryDB; }







GroceryStore::GroceryItemsSold GroceryStore::ringUpCustomers( const ShoppingCarts & shoppingCarts, std::ostream & receipt )
{
  GroceryItemsSold todaysSales;                                   // a collection of unique UPCs of grocery items sold

  ///////////////////////// TO-DO (3) //////////////////////////////
    ///  Ring up each customer accumulating the groceries purchased
    ///  Hint:  merge each customer's purchased groceries into today's sales.  (https://en.cppreference.com/w/cpp/container/set/merge)
    
    for(const auto & p : shoppingCarts)
    {
      receipt << p.first << "'s shopping cart contains:\n";
      todaysSales.merge(ringUpCustomer(p.second,receipt));
    }
    
    
  /////////////////////// END-TO-DO (3) ////////////////////////////

  return todaysSales;
} // ringUpCustomers







GroceryStore::GroceryItemsSold GroceryStore::ringUpCustomer( const ShoppingCart & shoppingCart, std::ostream & receipt )
{
  auto & worldWideGroceryDatabase = GroceryItemDatabase::instance();        // Get a reference to the world wide database of all
                                                                            // groceries in the world. The database will contains a
                                                                            // full description of the item and the item's price.

  GroceryItemsSold purchasedGroceries;                                      // a collection of UPCs of the groceries purchased by this customer


  ///////////////////////// TO-DO (4) //////////////////////////////
    /// Print out a receipt containing a full description of the grocery items (obtained from the database of all groceries in the world) in
    /// the customer's shopping cart along with the total amount due. As items are being scanned, decrement the quantity on hand for
    /// that grocery item in the store's inventory.
    ///
    ///
    /// Hint:  Here's some pseudocode to get you started.
    ///       1        Initialize the amount due to zero x
    ///       2        For each grocery item in the customer's shopping cart x
    ///       2.1          If the item is not found in the world wide grocery item database, indicate on the receipt it's free of charge x
    ///       2.2          Otherwise 
    ///       2.2.1            Print the grocery item's full description on the receipt x
    ///       2.2.2            Add the grocery item's price to the amount due x
    ///       2.2.3            If the grocery item is something the store sells (the item is in the store's inventory) x
    ///       2.2.3.1              Decrease the number of items on hand for the item sold  x
    ///       2.2.3.2              Add the items's UPC to the list of groceries purchased x
    ///       3         Print the total amount due on the receipt
  double amount{0}; // step 1

  for(const auto & p : shoppingCart){ // step 2
  receipt << "  ";
   auto checker = worldWideGroceryDatabase.find(p.first);
   if(checker != nullptr){
    //add it
    receipt << *checker << '\n';
    amount += checker->price();
    --_inventoryDB.find(checker->upcCode())->second;
    purchasedGroceries.insert(checker->upcCode());
   }else{
    receipt << std::quoted(p.first) << " (" << p.second.productName() << ") not found, the item is free!\n"; 
   }
  }
  receipt << "-------------------------\nTotal $" << amount << "\n\n";
  
  /////////////////////// END-TO-DO (4) ////////////////////////////

  return purchasedGroceries;
} // ringUpCustomer







void GroceryStore::reorderItems( GroceryItemsSold & todaysSales, std::ostream & reorderReport )
{
  auto & worldWideGroceryDatabase = GroceryItemDatabase::instance();    // Get a reference to the world wide database of all
                                                                        // groceries in the world. The database will contains a
                                                                        // full description of the item and the item's price.

  ///////////////////////// TO-DO (5) //////////////////////////////
    /// For each grocery item that has fallen below the reorder threshold, assume an order has been placed and now the shipment has
    /// arrived. Update the store's inventory to reflect the additional items on hand.
    ///
    /// Hint:  Here's some pseudocode to get you started.
    ///        1       For each grocery item sold todayx
    ///        1.1         If the grocery item is not in the store's inventory or if the number of grocery items on hand has fallen below the re-order threshold (REORDER_THRESHOLD) x
    ///        1.1.1           If the grocery item is not in the world wide grocery item database, x
    ///        1.1.1.1             display just the UPC x
    ///        1.1.2           Otherwise, x
    ///        1.1.2.1             display the grocery item's full description x
    ///        1.1.3           If the grocery item is not in the store's inventory x
    ///        1.1.3.1             display a notice indicating the grocery item is no longer sold in this store and will not be re-ordered x
    ///        1.1.4           Otherwise, x
    ///        1.1.4.1             Display the current quantity on hand and the quantity re-ordered x
    ///        1.1.4.2             Increase the quantity on hand by the number of items ordered and received (LOT_COUNT) x
    ///        2       Reset the list of grocery item sold today so the list can be reused again later x
    ///
    /// Take special care to avoid excessive searches in your solution
    reorderReport << "Re-Ordering grocery items the store is running low on\n\n";
    unsigned tracker{1};
    for (const auto& p : todaysSales){
      auto checker = worldWideGroceryDatabase.find(p);
      auto checker2 = _inventoryDB.find(p);

      if(checker2 == _inventoryDB.end()) {
        reorderReport << tracker++ << ": {" << *checker << "}\n *** no longer sold in this store and will not be re-ordered\n\n"; 
        }

      if(checker2->second < REORDER_THRESHOLD ){
        reorderReport << tracker++ << ": {" << *checker << "}\n only " << checker2->second
        << " remain in stock which is " << REORDER_THRESHOLD - checker2->second  << " unit(s) below reorder threshold (" << REORDER_THRESHOLD << "), re-ordering " 
        << LOT_COUNT << " more\n\n";
        checker2->second += LOT_COUNT;
        }
      }

    todaysSales.clear();
  /////////////////////// END-TO-DO (5) ////////////////////////////
}







GroceryStore::ShoppingCarts GroceryStore::makeShoppingCarts()
  {
    // Our store has many customers, and each (identified by name) is pushing a shopping cart. Shopping carts are structured as
    // trees of trees of grocery items.
    ShoppingCarts carts =
    {
      // first shopping cart
      { "Red Baron",        { {"00075457129000", {"milk"     }}, {"00038000291210", {"rice krispies"}}, {"00025317533003", {"hotdogs"}},
                              {"09073649000493", {"apple pie"}}, {"00835841005255", {"bread"        }}, {"00688267039317", {"eggs"   }},
                              {"00037466065908", {"Truffles" }}
                            }
      },

      // second shopping cart
      { "Peppermint Patty", { {"00813608012401", {"Cheese"      }}, {"00037466065908", {"Truffles"      }}, {"00631723300704", {"Pepperoncini"}},
                              {"00051500280645", {"Pancake Mix" }}, {"00792851355155", {"Soup"          }}, {"00898425001333", {"Sandwich"    }},
                              {"00021908501871", {"Potato Spuds"}}, {"00856414001092", {"Plantain Chips"}}, {"00036192122930", {"Applesauce"  }},
                              {"00763795931415", {"Paper Plates"}}, {"00070596000647", {"Soap"          }}, {"00079400266200", {"Deodorant"   }}
                            }
      },

      // third shopping cart
      { "Woodstock",        { {"00859013004327", {"Mint Tea"         }}, {"00041331092609", {"Spanish Omelet"}}, {"00217039300005", {"Fillet Steak"   }},
                              {"00636874220338", {"Face Exfoliate"   }}, {"00688267138973", {"Green Tea"     }}, {"00033674100066", {"Forskohlii"     }},
                              {"00891475001421", {"Candy Fruit Chews"}}, {"00688267133442", {"Coffee"        }}, {"00021000778690", {"Cheesy Potatoes"}},
                              {"00071758100823", {"Pepper"           }}, {"00041520010667", {"Mouth Wash"    }}, {"00216079600007", {"Lamb Chops"     }},
                              {"00790555051526", {"Black Beans"      }}, {"00070177862640", {"Tea Sampler"   }}, {"00018000001958", {"Cinnamon Rolls" }}
                            }
      },

      // forth shopping cart
      { "Schroeder",        { {"00072591002251", {"Peanut Butter"}}, {"00219019100001", {"Ground Beef"           }}, {"00890692002020", {"Stretch Cream" }},
                              {"00637876981630", {"Mozzarella"   }}, {"00716237183507", {"Facial Towelettes"     }}, {"00886911000052", {"Pastry Popover"}},
                              {"00084783499008", {"Massage Oil"  }}, {"00016055456747", {"Children's Fruit Drink"}}, {"00021908743325", {"Granola Cereal"}},
                              {"00688267138973", {"Green Tea"    }}
                            }
      },

      // fifth shopping cart
      { "Lucy van Pelt",    { {"00070327012116", {"Pineapple"     }}, {"00073377801839", {"Sleep Mask Blue"}}, {"00030768009670", {"Calcium Softgels"}},
                              {"00852697001422", {"Baby Food"     }}, {"00079471102414", {"Manzanillo"     }}, {"00072736013753", {"Dressing"        }},
                              {"00216210900003", {"Cheddar Cheese"}}, {"00780872310185", {"Dog Food"       }}, {"00710069086036", {"Crackers"        }},
                              {"00092657016985", {"Tea",          }}, {"00086449903065", {"Deodorant"      }}, {"00885229000198", {"Drinks"          }}
                            }
      },

      // sixth shopping cart
      { "Charlie Brown",    { {"00611508524006", {"Lump Of Coal"}}
                            }
      }
    };

    return carts;
  }  // makeShoppingCarts

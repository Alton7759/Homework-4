#include <exception>
#include <iomanip>                                                          // setprecision()
#include <iostream>                                                         // boolalpha(), showpoint(), fixed(), endl()
#include <sstream>

#include "CheckResults.hpp"
#include "GroceryStore.hpp"





namespace  // anonymous
{
  class Redirect
  {
    public:
      Redirect( std::ostream & from, std::ostream & to ) : originalStream( from )
      {
        from.flush();
        from.rdbuf( to.rdbuf() );
      }

      ~Redirect()
      {
        // restore the original buffer
        originalStream.flush();
        originalStream.rdbuf( originalBuffer );
      }

    private:
      std::ostream &         originalStream;
      std::streambuf * const originalBuffer = originalStream.rdbuf();
  };






  class GroceryStoreRegressionTest
  {
    public:
      GroceryStoreRegressionTest();

    private:
      void test_1( const GroceryStore::Inventory_DB & inventory );
      void test_2( const GroceryStore::Inventory_DB & inventory );
      void test_3( const GroceryStore::Inventory_DB & inventory );
      void test_4( const GroceryStore::GroceryItemsSold & soldGroceryItems, const GroceryStore::Inventory_DB & inventory );

      void validate( const GroceryStore::Inventory_DB & inventory, const GroceryStore::Inventory_DB & pairs );

      Regression::CheckResults affirm;

      GroceryStore::Inventory_DB expectedValues =
      {
        // "Red Baron"
        {"00075457129000", 10}, {"00038000291210", 23}, {"00025317533003",  9}, //{"09073649000493",  5},
        {"00835841005255", 27}, {"00688267039317", 33}, {"00037466065908", 23},

        // Peppermint Patty
        {"00813608012401", 36}, {"00037466065908", 23}, {"00631723300704", 31}, {"00051500280645", 19},
        {"00792851355155", 31}, {"00898425001333", 52}, {"00021908501871", 21}, {"00856414001092", 40},
        {"00036192122930", 39}, {"00763795931415", 12}, {"00070596000647", 13}, {"00079400266200", 16},

        // Woodstock
        {"00859013004327", 49}, {"00041331092609",  9}, {"00217039300005", 27}, {"00636874220338", 27},
        {"00688267138973", 11}, {"00033674100066", 20}, {"00891475001421", 21}, {"00688267133442",  9},
        {"00021000778690", 30}, {"00071758100823", 26}, {"00041520010667", 24}, {"00216079600007", 49},
        {"00790555051526", 38}, {"00070177862640", 35}, {"00018000001958", 21},

        // Schroeder
        {"00072591002251", 12}, {"00219019100001", 14}, {"00890692002020", 42}, {"00637876981630", 47},
        {"00716237183507", 50}, {"00886911000052", 53}, {"00084783499008", 49}, {"00016055456747", 44},
        {"00021908743325", 39}, {"00688267138973", 11},

        // Lucy van Pelt
        {"00070327012116", 45}, {"00073377801839", 13}, {"00030768009670", 17}, {"00852697001422", 51},
        {"00079471102414", 31}, {"00072736013753", 38}, {"00216210900003", 50}, {"00780872310185", 27},
        {"00710069086036", 46}, {"00092657016985", 46}, {"00086449903065", 45}, {"00885229000198", 17},

        // Charlie Brown
        {"00611508524006", 28}
      };
  } run_GroceryStore_tests;






  GroceryStoreRegressionTest::GroceryStoreRegressionTest()
  {
    std::clog << std::boolalpha << std::showpoint << std::fixed << std::setprecision( 2 );

    try
    {
      // It is anticipated the implementation of the functions under test will write data to standard output. Capture and (for now)
      // ignore standard output from called functions only for the duration of these tests.  We may wish to inspect the contents of
      // this output at a future date.  These tests look and validate the function's returned parameters and side affects, but not
      // what they write to standard output.
      std::ostringstream buffer;
      Redirect           null( std::cout, buffer );    // null is an RAII object and the redirection gets reset when object is destroyed


      std::clog << "\n\n\nGroceryStore Regression Test:\n";

      // Create a store and make some carts
      GroceryStore theStore;
      auto         shoppingCarts = theStore.makeShoppingCarts();

      // Get a handle to this store's inventory and verify its content
      auto & inventory = theStore.inventory();
      test_1( inventory );

      // Service all the customers and verify new inventory and items sold that dipped below re-order threshold
      auto groceryItemsSold = theStore.ringUpCustomers( shoppingCarts );

      test_2( inventory );
      test_4( groceryItemsSold, inventory );

      // Change what the store carries before recording, reorder, then validate inventory content again
      inventory     .erase( "00041331092609" );
      expectedValues.erase( "00041331092609" );

      theStore.reorderItems( groceryItemsSold );
      test_3( inventory );

      std::clog << "\n\nGroceryStore Regression Test " << affirm << "\n\n";
    }

    catch( const std::exception & ex )
    {
      std::clog << "FAILURE:  Regression test for \"class GroceryStore\" failed with an unhandled exception. \n\n\n"
                << ex.what() << std::endl;

      // Reminder:  Objects with automatic storage are not destroyed by calling std::exit().  In this case, this is okay.
      // std::exit( -__LINE__ ); // uncomment to terminate program with test failures
    }
  }






  void GroceryStoreRegressionTest::validate( const GroceryStore::Inventory_DB & actualInventory, const GroceryStore::Inventory_DB & expectedInventory )
  {
    bool allPassed = true;
    for( const auto & [expectedUpc, expectedQuantity] : expectedInventory )  if( actualInventory.at( expectedUpc ) != expectedQuantity )
    {
      affirm.is_equal( "Inventory item \"" + expectedUpc + "\" quantity", expectedQuantity, actualInventory.at( expectedUpc ) );
      allPassed = false;
    }

    if( allPassed ) affirm.is_true( "Inventory item verification", true );
  }






  void GroceryStoreRegressionTest::test_1( const GroceryStore::Inventory_DB & inventory )
  {
    affirm.is_equal( "Inventory Item Check - Opening inventory database size", 104'361ULL, inventory.size() );

    validate( inventory, expectedValues);
  }






  void GroceryStoreRegressionTest::test_2( const GroceryStore::Inventory_DB & inventory )
  {
    affirm.is_equal( "Inventory Item Check - Closing inventory database size", 104'361ULL, inventory.size() );

    for( auto && [upc, quantity] : expectedValues ) --quantity;

    // Multiple customers bought the same grocery item, so we need to deduct a bit more
    for( auto && upc : { "00037466065908", "00688267138973" } )
    {
      --expectedValues.at( upc );
    }

    validate( inventory, expectedValues );
  }






  void GroceryStoreRegressionTest::test_3( const GroceryStore::Inventory_DB & inventory )
  {
    affirm.is_equal( "Inventory Item Check - Reorder inventory database size", 104'360ULL, inventory.size() );

    for( auto & upc : { "00025317533003", /* "00041331092609",*/ "00070596000647", "00072591002251", "00073377801839",
                        "00075457129000",    "00219019100001",   "00688267133442", "00688267138973", "00763795931415" } )
    {
      expectedValues.at( upc ) += 20;
    }
    validate( inventory, expectedValues );
  }






  void GroceryStoreRegressionTest::test_4( const GroceryStore::GroceryItemsSold & soldGroceryItems, const GroceryStore::Inventory_DB & inventory )
  {

    GroceryStore theStore;
    auto         shoppingCarts = theStore.makeShoppingCarts();

    GroceryStore::GroceryItemsSold expectedList;
    for( auto & [name, cart] : shoppingCarts ) for( auto & [upc, groceryItem] : cart)
    {
      if( inventory.find(upc) != inventory.end() )  expectedList.insert( upc );
    }

    affirm.is_equal( "Items sold today - size",    expectedList.size(), soldGroceryItems.size() );
    affirm.is_true( "Items sold today - content", expectedList == soldGroceryItems );


    GroceryStore::GroceryItemsSold expectedGroceryItemsToReorder =
                {"00025317533003", "00041331092609", "00070596000647", "00072591002251",
                 "00073377801839", "00075457129000", "00219019100001", "00688267133442",
                 "00688267138973", "00763795931415"};

    GroceryStore::GroceryItemsSold groceryItemsToReorder;
    for( auto & upc : expectedList )  if( inventory.at(upc) < 15) groceryItemsToReorder.insert(upc);
    affirm.is_equal( "Items to reorder - count", expectedGroceryItemsToReorder.size(), groceryItemsToReorder.size() );

    affirm.is_true( "Items to reorder - content", expectedGroceryItemsToReorder == groceryItemsToReorder );
  }

} // namespace

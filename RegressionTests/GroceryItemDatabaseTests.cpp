#include <cstddef>                                                                        // size_t
#include <exception>
#include <filesystem>                                                                     // exists()
#include <iomanip>                                                                        // setprecision()
#include <iostream>                                                                       // boolalpha(), showpoint(), fixed(), clog
#include <map>
#include <string>

#include "CheckResults.hpp"
#include "GroceryItemDatabase.hpp"





namespace  // anonymous
{
  class GroceryItemDatabaseRegressionTest
  {
    public:
      GroceryItemDatabaseRegressionTest();

    private:
      void tests();

      Regression::CheckResults affirm;
  } run_groceryItemDatabase_tests;




  void GroceryItemDatabaseRegressionTest::tests()
  {
    GroceryItemDatabase & db           = GroceryItemDatabase::instance();
    std::size_t           expectedSize = 0;
    if     ( std::filesystem::exists( "Grocery_UPC_Database-Full.dat"   ) ) expectedSize = 10'837'828;
    else if( std::filesystem::exists( "Grocery_UPC_Database-Large.dat"  ) ) expectedSize = 104'361;
    else if( std::filesystem::exists( "Grocery_UPC_Database-Medium.dat" ) ) expectedSize = 10'003;
    else if( std::filesystem::exists( "Grocery_UPC_Database-Small.dat"  ) ) expectedSize = 230;
    else if( std::filesystem::exists( "Sample_GroceryItem_Database.dat" ) ) expectedSize = 15;

    affirm.is_equal( "Database construction - Expected size", expectedSize, db.size() );

    if( auto p = db.find( "00014100072331" ); p == nullptr )
    {
      affirm.is_not_equal( "Database query - search for an existing grocery item", nullptr, p );
    }
    else
    {
      GroceryItem control( "Pepperidge Farm \n          Classic Cookie Favorites",
                           "Pepperidge Farm",
                           "00014100072331",
                           14.43 );

      affirm.is_equal( "Database query - existing grocery item located", control, *p );

      // Modify the contents and verify the DB sees those changes
      p->brandName( "Modified \"Brand Name\"" );
      auto q = db.find( p->upcCode() );

      affirm.is_equal( "Database query - return pointer to grocery item in database 1",  p,  q );
      affirm.is_equal( "Database query - return pointer to grocery item in database 2", *p, *q );

      // Now, put it back how you found it
      *p = control;
    }

    {
      auto groceryItem = db.find( "--------------" );
      affirm.is_equal( "Database query - search for a non-existent grocery item", nullptr, groceryItem );
    }

    {
      // Grocery Item Database over Map:
      //
      //
      // The attributes of the Grocery Item Database are private, so I can't get to them in the usual way.  If you're reading this,
      // don't do what I'm about to do - this is strictly for testing, and then only in a very controlled environment. If the order
      // of attributes in GroceryItemDatabase, or if other attributes are added - I'm screwed.  Not to mention I'm depending on
      // GroceryItemDatabase.hpp including what I need here.  By creating a struct that mirrors the attribute layout of the
      // GroceryItemDatabase I ensure proper attribute alignment and offset while gaining visibility.
      struct Attributes                                                                         // must exactly match the type and order of GroceryItemDatabase's instance attributes
      {
        std::map<std::string, GroceryItem> testData;
      };

      // Let's do a little sanity checking to verify the GroceryItemDatabase and the Attribute classes at lest have the same size.
      // The types and quantities can't possibly be identical if the sizes don't match. It's pretty week, but better than nothing.
      if constexpr (sizeof(GroceryItemDatabase) != sizeof(Attributes))
      {
        affirm.is_equal( "Database Deep Interrogation:  Tests aborted, incorrect GroceryItemDatabase attributes", sizeof( Attributes ), sizeof( GroceryItemDatabase ) );
      }
      else
      {
        // Create a grocery item DB and access its private instance attributes, something we should never do outside of a controlled
        // white-box test like this
        auto & DB_attributes = reinterpret_cast<Attributes &>( db );                            // direct access to db's private parts

        std::map<std::string, GroceryItem> originalData;
        originalData.swap( DB_attributes.testData );                                            // save the original database so it can be restored later

        // Attempt to find something from an empty database
        DB_attributes.testData.clear();
        auto groceryItem = db.find( "00014100072331" );
        affirm.is_equal( "Empty Database query - searching an empty database", nullptr, groceryItem );


        DB_attributes.testData = { { "001", { "", "", "001" } }, { "002", { "", "", "002" } }, { "003", { "", "", "003" } } };
        groceryItem    = db.find( "003" );
        affirm.is_equal( "Database query - Searching for the last item", GroceryItem{ "", "", "003" }, *groceryItem );

        groceryItem = db.find( "001" );
        affirm.is_equal( "Database query - Searching for the first item", GroceryItem{ "", "", "001" }, *groceryItem );


        originalData.swap( DB_attributes.testData );                                            // restore the original database
      }
    }
  }



  GroceryItemDatabaseRegressionTest::GroceryItemDatabaseRegressionTest()
  {
    std::clog << std::boolalpha << std::showpoint << std::fixed << std::setprecision( 2 );


    try
    {
      std::clog << "\n\n\nGroceryItem Database Regression Test:\n";
      tests();

      std::clog << "\n\nGroceryItem Database Regression Test " << affirm << "\n\n";
    }
    catch( const std::exception & ex )
    {
      std::clog << "FAILURE:  Regression test for \"class GroceryItemDatabase\" failed with an unhandled exception. \n\n\n"
                << ex.what() << std::endl;
    }
  }
} // namespace

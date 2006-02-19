// This code is provided just for an example. You should delete it from
// the project once you are ready to compile your test suite.  More information
// about using CppUnit can be found on the CppUnit homepage.
// http://cppunit.sourceforge.net/doc/lastest/index.html

// List of all modules can be found here:
// http://cppunit.sourceforge.net/doc/lastest/modules.html

// More information on the test fixture macros can be found at the following:
// http://cppunit.sourceforge.net/doc/lastest/group___writing_test_fixture.html

/*
CPPUNIT_TEST_SUITE(ATestFixtureType)
  Begin test suite.

CPPUNIT_TEST_SUB_SUITE(ATestFixtureType, ASuperClass)
  Begin test suite (includes parent suite).

CPPUNIT_TEST_SUITE_END()
  End declaration of the test suite.

CPPUNIT_TEST_SUITE_END_ABSTRACT()
  End declaration of an abstract test suite.

CPPUNIT_TEST_SUITE_ADD_TEST(test)   context.addTest( test )
  Add a test to the suite (for custom test macro).

CPPUNIT_TEST(testMethod)
  Add a method to the suite.

CPPUNIT_TEST_EXCEPTION(testMethod, ExceptionType)
  Add a test which fail if the specified exception is not caught.

CPPUNIT_TEST_FAIL(testMethod)   CPPUNIT_TEST_EXCEPTION( testMethod, CPPUNIT_NS::Exception )
  Adds a test case which is excepted to fail.

CPPUNIT_TEST_SUITE_ADD_CUSTOM_TESTS(testAdderMethod)   testAdderMethod( context )
  Adds some custom test cases.

CPPUNIT_TEST_SUITE_PROPERTY(APropertyKey, APropertyValue)
  Adds a property to the test suite builder context.
*/

#ifndef SQLITE_UNIT_TEST_H
#define SQLITE_UNIT_TEST_H

#include <cppunit/Portability.h>
#include <cppunit/Exception.h>
#include <cppunit/Asserter.h>
#include <cppunit/extensions/HelperMacros.h>

#include "SqliteWrap.h"

//Add the project to the sqlite_source solution to perform the testing
class SQLiteUnitTest : public   CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE( SQLiteUnitTest );

    //Tests for SqliteDB class
    CPPUNIT_TEST( TestDBConstructor );
    CPPUNIT_TEST( TestOpen );
    CPPUNIT_TEST( TestOpenDb );
    CPPUNIT_TEST( TestCloseDB );
    CPPUNIT_TEST( TestGenerateDatabase );
    CPPUNIT_TEST( TestDumpDatabase );

    //Tests for SqliteVM class
    CPPUNIT_TEST( TestVMConstructor );
    CPPUNIT_TEST( TestExecVM );
    CPPUNIT_TEST( TestQueriesVM );


    CPPUNIT_TEST_SUITE_END();

private:
    SqliteVM* m_vm1;
    SqliteVM* m_vm2;
    SqliteVM* m_vm3;

    SqliteDB* m_db1;
    SqliteDB* m_db2;
    SqliteDB* m_db3;

public:
    //Sets the initial settings. 3 objects from each class are created
    void setUp();
    //Deletes the memory allocated at initialization
    void tearDown();

    //Test cases for sqliteDB

    //Tests the constructor by ensuring that data members are set to NULL
    void TestDBConstructor();
    //Ensures that we cannot invoke ConnectDB() when there exists active connection
    void TestOpen();
    //Tests the Init(...) method. Ensures that no connection is created if the database name is invalid
    void TestOpenDb();
    //Test CloseDB(...) method. Ensures that the connection is reset after invoking the method
    void TestCloseDB();

    //TESTS CASES FOR SQLITEVM

    //Test the SqliteVM constructor
    void TestVMConstructor();

    //Test Execute. Ensure that the previous virtual machine is deleted and a new one is created
    void TestExecVM();

    //Tests various SQL queries. Checks if correct results are returned
    void TestQueriesVM();

    //Tests Generate database functionality
    void TestGenerateDatabase();

    //Test dump database to text file functionality
    void TestDumpDatabase();

};

#endif  // SQLITE_UNIT_TEST_H

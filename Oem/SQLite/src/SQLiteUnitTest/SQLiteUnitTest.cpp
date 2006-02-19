// This code is provided just for an example. You should delete it from
// the project once you are ready to compile your test suite.  More information
// about using CppUnit can be found on the CppUnit homepage.
// http://cppunit.sourceforge.net/doc/lastest/index.html

// List of all modules can be found here:
// http://cppunit.sourceforge.net/doc/lastest/modules.html

// Below are the macros for all Assertions available with CppUnit. If you want to create your own assertion
// go to: http://cppunit.sourceforge.net/doc/lastest/group___creating_new_assertions.html

/*
CPPUNIT_ASSERT(condition)
  Assertions that a condition is true.

CPPUNIT_ASSERT_MESSAGE(message, condition)
  Assertion with a user specified message.

CPPUNIT_FAIL(message)
  Fails with the specified message.

CPPUNIT_ASSERT_EQUAL(expected, actual)
  Asserts that two values are equals.

CPPUNIT_ASSERT_EQUAL_MESSAGE(message, expected, actual)
  Asserts that two values are equals, provides additional messafe on failure.

CPPUNIT_ASSERT_DOUBLES_EQUAL(expected, actual, delta)
  Macro for primitive value comparisons.

CPPUNIT_ASSERT_THROW(expression, ExceptionType)
  Asserts that the given expression throws an exception of the specified type.

CPPUNIT_ASSERT_NO_THROW(expression)
  Asserts that the given expression does not throw any exceptions.

CPPUNIT_ASSERT_ASSERTION_FAIL(assertion)   CPPUNIT_ASSERT_THROW( assertion, CPPUNIT_NS::Exception )
  Asserts that an assertion fail.

CPPUNIT_ASSERT_ASSERTION_PASS(assertion)   CPPUNIT_ASSERT_NO_THROW( assertion )
  Asserts that an assertion pass.
*/

#include "SQLiteUnitTest.h"
#include <iostream>
using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION(SQLiteUnitTest);

//Sets the initial settings. 3 objects from each class are created
void SQLiteUnitTest::setUp()
{
    m_db1 = new SqliteDB;
    m_db2 = new SqliteDB;
    m_db3 = new SqliteDB;

    //Connect to two databases. m_db3 remains NULL
    m_db1->Open(L"new_db1.db");
    m_db2->Open(L"new_db2.db");

    //Create two virtual machines
    m_vm1 = new SqliteVM(m_db1);
    m_vm2 = new SqliteVM(m_db2);
}

// This method is executed at the end of each test case.  It's
// purpose is to deallocate anything that was allocated in the
// setUp() method.
void SQLiteUnitTest::tearDown()
{
    delete m_vm1;
    delete m_vm2;

    delete m_db1;
    delete m_db2;
    delete m_db3;
}


// Put in your own test cases here
void SQLiteUnitTest::TestDBConstructor()
{
    //Error message in case assertion fails
    std::string message("SqliteDB constructor failed to initilize data members");

    CPPUNIT_ASSERT_MESSAGE( message, L""==(m_db3->GetName()));
    CPPUNIT_ASSERT_MESSAGE( message, NULL==(m_db3->IsConnected()));
    CPPUNIT_ASSERT_MESSAGE( message, L""==(m_db3->GetErrMsg()));
}

//Ensures that we cannot invoke Open()  when there exists active connection
void SQLiteUnitTest::TestOpen()
{

    //Error messages
    string wrongInitialSetting ("Connection should be established at the beginning");
    string duplicateConnect("Established connection although the connection already existed");
    string failedConnect("Unable to establish connection");

    //Check initial setting
    CPPUNIT_ASSERT_MESSAGE(wrongInitialSetting, NULL != (m_db2->IsConnected()));

    //Try to establish connection again. The method should throw an exception
    CPPUNIT_ASSERT_THROW((m_db2->Open(L"new_db2.db")), CantOpenDbException* );

    //Disconnect and then connect. This has to succeed
    m_db2->CloseDB();
    CPPUNIT_ASSERT_MESSAGE(failedConnect, ( SQLITE_OK == m_db2->Open(L"new_db2.db")) );

}

//Tests the Init(...) method. Ensures that no connection is created if the database name is invalid
void SQLiteUnitTest::TestOpenDb()
{
    string acceptInvalidInput("Init(...) method for a DB object accepted invalid parameter");
    string failToInitialize("Init(...) method failed to initialize a DB object");
    //Ensure the method accepts valid string only. The following three should throw an exception
    CPPUNIT_ASSERT_THROW((m_db3->Open(L"")), CantOpenDbException* );
    CPPUNIT_ASSERT_THROW((m_db3->Open(L"#^%$&^%*&^(**&(^")), CantOpenDbException* );
    CPPUNIT_ASSERT_THROW((m_db3->Open(L".??")), CantOpenDbException* );

    //Check for proper initialization if parameter is valid
    CPPUNIT_ASSERT_MESSAGE( failToInitialize, SQLITE_OK == (m_db3->Open(L"new_db3.db")) );
}

//Test CloseDB(...) method. Ensures that the connection is reset after invoking the method
void SQLiteUnitTest::TestCloseDB()
{
    string wrongInitialSetting("No connection detected in the begining of TestCloseDB()");
    string testFail("CloseDB(...) method did not close existing connection");

    //Check for existing connection
    CPPUNIT_ASSERT_MESSAGE(wrongInitialSetting, NULL != (m_db2->IsConnected()));
    //Close connection
    m_db2->CloseDB();
    CPPUNIT_ASSERT_MESSAGE(testFail, NULL == (m_db2->IsConnected()));
}

//TESTS CASES FOR SQLITEVM

//Test the SqliteVM constructor
void SQLiteUnitTest::TestVMConstructor()
{
    string message("SqliteVM object created without valid database connection ");
    SqliteVM* m_vm3 = new SqliteVM(m_db3);
    m_vm1 = new SqliteVM(m_db1);

    CPPUNIT_ASSERT_MESSAGE( message, m_vm1->IsConnected() );
    CPPUNIT_ASSERT_MESSAGE( message, !(m_vm3->IsConnected()) );

    //Pass a NULL pointer to the constructor
    CPPUNIT_ASSERT_THROW((new SqliteVM(NULL)), SqliteException*);

    delete m_vm3;
}

//Test Execute. Ensure that the previous virtual machine is deleted and a new one is created
void SQLiteUnitTest::TestExecVM()
{
    string invalidQuery("Execute(...) method accepted NULL pointer as a parameter");
    string failToExecute("Unable to execute a query");

    //Create and drop a table
    CPPUNIT_ASSERT_MESSAGE( failToExecute, SQLITE_DONE == (m_vm1->Execute(L"CREATE TABLE test1(a INTEGER PRIMARY KEY, b text);")));
    CPPUNIT_ASSERT_MESSAGE( failToExecute, SQLITE_DONE == (m_vm1->Execute(L"DROP TABLE test1")));

    //Try to execute invalid queries
    //Q1: Table does not exist
    CPPUNIT_ASSERT_THROW(m_vm1->Execute(L"SELECT * FROM test1"), SQLitePrepareException*);
    //Q2: Invalid SQLite syntax
    CPPUNIT_ASSERT_THROW(m_vm1->Execute(L"blah"), SQLitePrepareException*);
    //Q3: The query is an empty string
    CPPUNIT_ASSERT_THROW(m_vm1->Execute(L""), ExecuteErrorException*);
}

//Tests various SQL queries. Checks if correct results are returned
void SQLiteUnitTest::TestQueriesVM()
{
   INT8 testIntVar = 2;
    STRING expected =  L"INTEGER";
    string generalFailure("Error occur during the set up phase");
    string errorResult("The query returned invalid result");

    //Create a table
    CPPUNIT_ASSERT_MESSAGE(errorResult, (m_vm1->Execute(L"CREATE TABLE test1(a INTEGER PRIMARY KEY, b TEXT);")));
    CPPUNIT_ASSERT_THROW( m_vm1->ColumnType(L"a"), ColumnNotFoundException*);

    //Do a select on an empty table
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_DONE == (m_vm1->Execute(L"SELECT * FROM test1;")));
    //Check the number of columns received
    CPPUNIT_ASSERT_EQUAL_MESSAGE( errorResult, testIntVar, (m_vm1->NumCols()));

    //Test the column types of the created table
    //First column
    STRING actual = m_vm1->ColumnType(L"a");
    CPPUNIT_ASSERT_MESSAGE( errorResult, expected == actual);

    //Second column
    actual = m_vm1->ColumnType(L"b") ;
    expected = L"TEXT";
    CPPUNIT_ASSERT_MESSAGE( errorResult, expected == actual);

    //Test the column names of the created table
    actual = m_vm1->ColumnName(0);
    expected = L"a";
    CPPUNIT_ASSERT_MESSAGE( errorResult, expected == actual);

    actual = m_vm1->ColumnName(1);
    expected = L"b";
    CPPUNIT_ASSERT_MESSAGE( errorResult, expected == actual);

    //Non existing column
    CPPUNIT_ASSERT_THROW( m_vm1->ColumnType(L"v"), ColumnNotFoundException*);
    CPPUNIT_ASSERT_THROW( m_vm1->ColumnName(5), ColumnNotFoundException*);

    //Insert a rows in the table
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_DONE == (m_vm1->Execute(L"INSERT INTO test1 VALUES(NULL, \"test1\");")));
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_DONE == (m_vm1->Execute(L"INSERT INTO test1 VALUES(NULL, \"test2\");")));
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_DONE == (m_vm1->Execute(L"INSERT INTO test1 VALUES(NULL, \"test3\");")));

    //Try to get data. No result should be returned after an INSERT statement
    CPPUNIT_ASSERT_THROW(m_vm1->GetString(L"a"), SqliteException*);

    //Do a select and try to get the data inserted
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_ROW == (m_vm1->Execute(L"SELECT * FROM test1;")));
    CPPUNIT_ASSERT_MESSAGE(errorResult, 1 == m_vm1->GetInt(L"a"));

    expected = L"test1";
    actual = m_vm1->GetString(L"b");
    CPPUNIT_ASSERT_MESSAGE(errorResult, expected == actual );

    //Value for non existing column
    //Non existing column
    CPPUNIT_ASSERT_THROW( m_vm1->GetString(L"u"), ColumnNotFoundException*);

    //Get the next row of the result
    //This time get the data using the column name
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_ROW == (m_vm1->NextRow()));

    CPPUNIT_ASSERT_MESSAGE(errorResult, 2 == m_vm1->GetInt(L"a") );

    CPPUNIT_ASSERT_MESSAGE(errorResult, L"test2" == m_vm1->GetString(L"b") );

    //Non existing column
    CPPUNIT_ASSERT_THROW( m_vm1->GetString(L"nonExisting"), ColumnNotFoundException*);

    //Finished, so drop the table
    CPPUNIT_ASSERT_MESSAGE( generalFailure, SQLITE_DONE == (m_vm1->Execute(L"DROP TABLE test1;")));

}

void SQLiteUnitTest::TestGenerateDatabase()
{
    SqliteDB* testDb = new SqliteDB;
    testDb->GenerateDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump",
        L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    testDb->GenerateDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump",
        L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    testDb->GenerateDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump",
        L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    int result = _unlink("c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    CPPUNIT_ASSERT_MESSAGE("Unable to unlink database file", 0 == result);
}

void SQLiteUnitTest::TestDumpDatabase()
{
    FILE* iniFile = fopen("c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.ini","w");
    fprintf(iniFile, ".output c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump2");
    fclose(iniFile);

    SqliteDB* testDb = new SqliteDB;
    testDb->GenerateDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump",
        L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    testDb->Open(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.db");
    testDb->DumpDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.ini");
    testDb->DumpDatabase(L"c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.ini");
    testDb->CloseDB();

    _unlink("c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.ini");

    int result = _unlink("c:/mgdev/unittest/testdata/resourceservice/resourceservicetest.dump2");
    CPPUNIT_ASSERT_MESSAGE("Unable to unlink dump file", 0 == result);
}

// This is the file that runs all of the testsuites.  There should be no need
// to do anything to this file.  This will output an xml file with a name that
// has the following format: SQLiteUnitTest-yymmdd-hhmm-username.xml

#include <time.h>

#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include "tester.h"

using namespace std;

int main(int argc, char* argv[])
{
    wstring testResultsPath(TESTRESULTSPATH);
    wstring fileName = BuildFileName(wstring(TESTNAME));

    // Build full file name including the directory path
    wstring fullFileName = testResultsPath + fileName;
    char* tmpString = ConvertToCharArray(fullFileName);
    ofstream* outfile = new ofstream(tmpString, ios::out);
    delete [] tmpString;

    // Failed to open file in TestResults directory
    if(!outfile->is_open())
    {
        delete outfile;
        printf("Failed to open %ls for writing, attempting to create results file in current directory.\n", fullFileName.c_str());
        printf("Please remember to move the file to the test results directory.\n");

        fullFileName = fileName;
        tmpString = ConvertToCharArray(fullFileName);
        outfile = new ofstream(tmpString, ios::out);
        delete [] tmpString;

        // Failed to open file in current directory
        if(!outfile->is_open())
        {
            delete outfile;
            printf("Failed to open %ls for writing, exiting...\n", fullFileName.c_str());
            exit(1);
        }
    }

    WriteResultsToFile(*outfile);
    outfile->close();
    delete outfile;

    // Inform the user of the location of the file
    printf("Wrote results to: %ls\n", fullFileName.c_str());

    return 0;
}

/*
Converts a wstring to a char*
*/
char* ConvertToCharArray(wstring &theStr)
{
    int length = static_cast<int>(theStr.length())+1;
    char* theStr2 = new char[length];
    wcstombs(theStr2, theStr.c_str(), length);
    return theStr2;
}

/*
Converts a char* to a wstring
*/
wstring ConvertToWstring(char* theStr)
{
    int length = static_cast<int>(strlen(theStr))+1;
    wchar_t* theStr2 = new wchar_t[length];
    mbstowcs(theStr2, theStr, length);
    wstring* theStr3 = new wstring(theStr2);
    delete [] theStr2;
    return *theStr3;
}

/*
This builds the filename by getting the date and time of the system and
gettig the username of the person running the tests.
*/
wstring BuildFileName(wstring rootFileName)
{
    // Get current system time
    time_t rawtime;
    struct tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Put current system time into format similar to: 041015-1640
    char dateTime[12];
    strftime(dateTime, 12, "%y%m%d-%H%M", timeinfo);

    wchar_t dateTime2[100];
    mbstowcs(dateTime2,dateTime,12);

    // Get username of person running the test
    wstring userName = GetName();

    wstring fileName = rootFileName + wstring(L"-") + wstring(dateTime2) + wstring(L"-") + userName + wstring(L".xml");

    return fileName;
}

wstring GetName()
{
    GETUSER;
    return ConvertToWstring(userName);
}

// Run the registered test suites and output the results in xml to a file
int WriteResultsToFile(ofstream &outfile)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry().makeTest());
    runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), outfile, "ISO-8859-1"));
    bool wasSuccessful = runner.run();

    return (wasSuccessful ? 0 : 1);
}

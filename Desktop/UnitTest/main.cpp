#include "MgDesktop.h"
#include <fstream>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

/*
// for memory leak detection
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#ifdef _DEBUG
   #define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
   #define DEBUG_CLIENTBLOCK
#endif

#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif
#endif
*/

int main(int argc, char** argv)
{
/*
#ifdef _WIN32
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    #ifdef _DEBUG
    // to debug memory leaks, set a breakpoint here and set iBlock
    // to the block allocation you want to break on
    long iBlock = -1;
    _CrtSetBreakAlloc(iBlock);
    #endif
#endif
*/

    ACE_DEBUG((LM_INFO, ACE_TEXT("Initialize Platform.ini\n")));
    //Benchmark this
#ifdef WIN32
    long lStart = GetTickCount();
#endif
    MgPlatform::Initialize(L"Platform.ini");
#ifdef WIN32
    ACE_DEBUG((LM_INFO, ACE_TEXT("Platform Initialization time in %dms\n"), (GetTickCount()-lStart)));
#endif
    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
    Ptr<MgCoordinateSystemCatalog> csCatalog = csFactory->GetCatalog();
    STRING dictDir = csCatalog->GetDictionaryDir();

    ACE_DEBUG((LM_INFO, ACE_TEXT("MENTOR_DICTIONARY_PATH is: %s\n"), dictDir.c_str()));

    CppUnit::TextUi::TestRunner runner;

    // Add all of the tests
    //NOTE: Leave trace log off, otherwise one of the tests here will fail
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
#ifdef _DEBUG
    ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance and CoordinateSystem. <<<<<\n\n")));
#else
    ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance. <<<<<\n\n")));
    //runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
#endif
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestProfilingService").makeTest());
    
    //This causes access violations in Visual Leak Detector when run in debug mode. Only uncommment
    //to verify functionality, but don't use VLD for memory leak detection. Seek an alternate tool/library
    //in this case.
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());

    STRING fileName = L"UnitTestResults.xml";
    if (fileName.size() > 0)
    {
        ofstream outfile(MG_WCHAR_TO_CHAR(fileName.c_str()));

        if (outfile.is_open())
        {
            runner.setOutputter(new CppUnit::XmlOutputter(&runner.result(), outfile, "ISO-8859-1"));
            runner.run();
            outfile.close();
        }
    }
    else
    {
        runner.setOutputter(new CppUnit::TextOutputter(&runner.result(), std::cout));
        runner.run();
    }

    int nResult = runner.result().testFailuresTotal();

    MgPlatform::Terminate();

    return nResult;
}
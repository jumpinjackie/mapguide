#include "MgDesktop.h"
#include <fstream>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

// for memory leak detection
#if defined(_WIN32) && defined(_DEBUG)
#if !defined(USING_VLD)
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
#endif

#define TEST_LOG_MANAGER        1
#define TEST_RESOURCE_SERVICE   1
#define TEST_FEATURE_SERVICE    1
#define TEST_MAPPING_SERVICE    1
#define TEST_PROFILING_SERVICE  1
#define TEST_RENDERING_SERVICE  1
#define TEST_TILE_SERVICE       1

int main(int argc, char** argv)
{
#if defined(_WIN32) && defined(_DEBUG)
#if !defined(USING_VLD)
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

    #ifdef _DEBUG
    // to debug memory leaks, set a breakpoint here and set iBlock
    // to the block allocation you want to break on
    long iBlock = -1;
    _CrtSetBreakAlloc(iBlock);
    #endif
#endif
#endif

    ACE_DEBUG((LM_INFO, ACE_TEXT("Initialize Platform.ini\n")));
    //Benchmark this
#ifdef WIN32
    //Muffle errors due to broken FDO RDBMS provider dlls, they aren't the ones exercised
    //under test anyway
    SetErrorMode(SEM_FAILCRITICALERRORS);
    long lStart = GetTickCount();
#endif
    MgdPlatform::Initialize(L"Platform.ini");
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
#if TEST_LOG_MANAGER == 1
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
#endif
#if TEST_RESOURCE_SERVICE == 1
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
#endif
#if TEST_RENDERING_SERVICE == 1
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
#endif
#if TEST_FEATURE_SERVICE == 1
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
#endif
#if TEST_MAPPING_SERVICE == 1
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
#endif
#if TEST_PROFILING_SERVICE == 1
	runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestProfilingService").makeTest());
#endif
#if TEST_TILE_SERVICE == 1
    //This causes access violations in Visual Leak Detector when run in debug mode. Only uncommment
    //to verify functionality, but don't use VLD for memory leak detection. Seek an alternate tool/library
    //in this case.
    runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
#endif

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

    MgdPlatform::Terminate();

    return nResult;
}
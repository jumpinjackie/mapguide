//
//  Copyright (C) 2004-2006  Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "AceCommon.h"
#include "UnitTesting.h"
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int Execute(CREFSTRING fileName, CREFSTRING test)
{
    int nResult = 0;
    bool bRunTests = true;

    // Set the default user information for this test run.
    // Any specific test case may overwrite these information.
    Ptr<MgUserInformation> userInfo = new MgUserInformation(
        MgUser::Administrator, L"");
    MgUserInformation::SetCurrentUserInfo(userInfo);

    CppUnit::TextUi::TestRunner runner;

    // Setup which tests to run
    if(test.size() > 0)
    {
        if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestModeDefaultTests)) == 0)
        {
            // Add all of the tests
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestModeListTests)) == 0)
        {
            // Title
            ACE_OS::printf("The following unit tests are available:\n\n");

            // Available tests
            ACE_OS::printf("  TestCoordinateSystem\n");
            ACE_OS::printf("  TestFeatureService\n");
            ACE_OS::printf("  TestGeometry\n");
            ACE_OS::printf("  TestLogManager\n");
            ACE_OS::printf("  TestMappingService\n");
            ACE_OS::printf("  TestRenderingService\n");
            ACE_OS::printf("  TestResourceService\n");
            ACE_OS::printf("  TestTileService\n");
            ACE_OS::printf("  TestServerAdminService\n");
            ACE_OS::printf("  TestServerManager\n");
            ACE_OS::printf("  TestServiceManager\n");
            ACE_OS::printf("  TestSiteService\n");
            ACE_OS::printf("\n");

            bRunTests = false;
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestCoordinateSystem")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Coordinate System tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestFeatureService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Feature Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestGeometry")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Geometry tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestLogManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Log Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestMappingService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Mapping Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestRenderingService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Rendering Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestResourceService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Resource Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestTileService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Tile Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestServerAdminService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only ServerAdmin Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestServerManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Server Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestServiceManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Service Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TestSiteService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Site Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
        }
        else
        {
            // Test suite not found
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Unrecognized unit test: %s <<<<<\n\n"), MG_WCHAR_TO_TCHAR(test)));
            bRunTests = false;
        }
    }
    else
    {
        // Add all of the tests
        ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests. <<<<<\n\n")));
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
    }

    if(bRunTests)
    {
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

        nResult = runner.result().testFailuresTotal();
    }

    // Reset the current user information for this test run.
    MgUserInformation::SetCurrentUserInfo(NULL);

    return nResult;
}

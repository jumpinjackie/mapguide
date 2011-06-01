//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "UnitTesting.h"
#include <cppunit/TestResultCollector.h>
#include <cppunit/TextOutputter.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include "FoundationDefs.h"
int Execute(CREFSTRING fileName, CREFSTRING test)
{
    int nResult = 0;
    bool bRunTests = true;

    // Set the default user information for this test run.
    // Any specific test case may overwrite these information.
    Ptr<MgUserInformation> userInfo = new MgUserInformation(MgUser::Administrator, L"");
    MgUserInformation::SetCurrentUserInfo(userInfo);

    CppUnit::TextUi::TestRunner runner;

    // Setup which tests to run
    if(test.size() > 0)
    {
        if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestDefaultTests)) == 0)
        {
            // Add all of the tests
#ifdef _DEBUG
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance and CoordinateSystem. <<<<<\n\n")));
#else
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
#endif
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMdfModel").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestKmlService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteManager").makeTest());
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTransformMesh").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), MG_WCHAR_TO_TCHAR(MgResources::ServerCmdTestListTests)) == 0)
        {
            // Title
            ACE_OS::printf("The following unit tests are available:\n\n");

            // Available tests
#ifdef _DEBUG
            ACE_OS::printf("  All - Excluding Performance and CoordinateSystem\n");
#else
            ACE_OS::printf("  All - Excluding Performance\n");
#endif
            ACE_OS::printf("  CoordinateSystem\n");
            ACE_OS::printf("  FeatureService\n");
            ACE_OS::printf("  Geometry\n");
            ACE_OS::printf("  KmlService\n");
            ACE_OS::printf("  LogManager\n");
            ACE_OS::printf("  MappingService\n");
            ACE_OS::printf("  MdfModel\n");
            ACE_OS::printf("  Performance\n");
            ACE_OS::printf("  RenderingService\n");
            ACE_OS::printf("  ResourceService\n");
            ACE_OS::printf("  ServerAdminService\n");
            ACE_OS::printf("  ServerManager\n");
            ACE_OS::printf("  ServiceManager\n");
            ACE_OS::printf("  SiteManager\n");
            ACE_OS::printf("  SiteService\n");
            ACE_OS::printf("  TileService\n");
            ACE_OS::printf("  TransformMesh\n");
            ACE_OS::printf("\n");

            bRunTests = false;
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("CoordinateSystem")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Coordinate System tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("FeatureService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Feature Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("Geometry")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Geometry tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("KmlService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Kml Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestKmlService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("LogManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Log Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("MappingService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Mapping Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("MdfModel")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only MdfModel tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMdfModel").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("Performance")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Performance tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestPerformance").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("RenderingService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Rendering Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("ResourceService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Resource Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("ServerAdminService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only ServerAdmin Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("ServerManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Server Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("ServiceManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Service Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("SiteManager")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Site Manager tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteManager").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("SiteService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Site Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TileService")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Tile Service tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
        }
        else if(ACE_OS::strcasecmp(MG_WCHAR_TO_TCHAR(test), ACE_LIB_TEXT("TransformMesh")) == 0)
        {
            ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running only Transform Mesh tests. <<<<<\n\n")));
            runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTransformMesh").makeTest());
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
#ifdef _DEBUG
        ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance and CoordinateSystem. <<<<<\n\n")));
#else
        ACE_DEBUG((LM_INFO, ACE_TEXT(">>>>> Running all unit tests - Excluding Performance. <<<<<\n\n")));
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestCoordinateSystem").makeTest());
#endif
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestFeatureService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestGeometry").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestLogManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMdfModel").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestKmlService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestMappingService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestRenderingService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestResourceService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTileService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerAdminService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServerManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestServiceManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteService").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestSiteManager").makeTest());
        runner.addTest(CppUnit::TestFactoryRegistry::getRegistry("TestTransformMesh").makeTest());
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

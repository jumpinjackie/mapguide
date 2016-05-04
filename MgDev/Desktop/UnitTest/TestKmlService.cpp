//
//  Copyright (C) 2007-2010 by Autodesk, Inc.
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

#include "MgDesktop.h"
#include "TestKmlService.h"
#include "ServiceManager.h"
#include "ServerResourceService.h"
#include "ServerSiteService.h"
#include "../Common/Manager/FdoConnectionManager.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestKmlService, "TestKmlService");


TestKmlService::TestKmlService()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcKml = dynamic_cast<MgKmlService*>(
        serviceManager->RequestService(MgServiceType::KmlService));
    assert(m_svcKml != NULL);

    // Initialize a site connection.
    Ptr<MgServerSiteService> svcSite = dynamic_cast<MgServerSiteService*>(
        serviceManager->RequestService(MgServiceType::SiteService));
    assert(svcSite != NULL);

    Ptr<MgUserInformation> userInfo = new MgUserInformation(
        L"Administrator", L"admin");
    userInfo->SetLocale(TEST_LOCALE);

    // Set the current MgUserInformation
    // This must be done before calling CreateSession()
    MgUserInformation::SetCurrentUserInfo(userInfo);

    STRING session = svcSite->CreateSession();
    assert(!session.empty());
    userInfo->SetMgSessionId(session);

    // Set the current MgUserInformation
    MgUserInformation::SetCurrentUserInfo(userInfo);

    m_siteConnection = new MgSiteConnection();
    m_siteConnection->Open(userInfo);
}


TestKmlService::~TestKmlService()
{
}


void TestKmlService::setUp()
{
}


void TestKmlService::tearDown()
{
}


void TestKmlService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Kml Service tests.\n")));

    try
    {
        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif

        //set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        //publish the map definition
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgByteSource> mdfsrc = new MgByteSource(L"../UnitTestFiles/UT_Sheboygan.mdf", false);
        Ptr<MgByteReader> mdfrdr = mdfsrc->GetReader();
        m_svcResource->SetResource(mapres, mdfrdr, NULL);

        //publish the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        Ptr<MgByteSource> ldfsrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.ldf", false);
        Ptr<MgByteReader> ldfrdr1 = ldfsrc1->GetReader();
        m_svcResource->SetResource(ldfres1, ldfrdr1, NULL);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgByteSource> ldfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.ldf", false);
        Ptr<MgByteReader> ldfrdr2 = ldfsrc2->GetReader();
        m_svcResource->SetResource(ldfres2, ldfrdr2, NULL);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        Ptr<MgByteSource> ldfsrc3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.ldf", false);
        Ptr<MgByteReader> ldfrdr3 = ldfsrc3->GetReader();
        m_svcResource->SetResource(ldfres3, ldfrdr3, NULL);

        //publish the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        Ptr<MgByteSource> fssrc1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.fs", false);
        Ptr<MgByteReader> fsrdr1 = fssrc1->GetReader();
        m_svcResource->SetResource(fsres1, fsrdr1, NULL);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        Ptr<MgByteSource> fssrc2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.fs", false);
        Ptr<MgByteReader> fsrdr2 = fssrc2->GetReader();
        m_svcResource->SetResource(fsres2, fsrdr2, NULL);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        Ptr<MgByteSource> fssrc3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.fs", false);
        Ptr<MgByteReader> fsrdr3 = fssrc3->GetReader();
        m_svcResource->SetResource(fsres3, fsrdr3, NULL);

        // publish the resource data
        Ptr<MgByteSource> dataSource1 = new MgByteSource(L"../UnitTestFiles/UT_HydrographicPolygons.sdf", false);
        Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
        m_svcResource->SetResourceData(fsres1, L"UT_HydrographicPolygons.sdf", L"File", dataReader1);

        Ptr<MgByteSource> dataSource2 = new MgByteSource(L"../UnitTestFiles/UT_Parcels.sdf", false);
        Ptr<MgByteReader> dataReader2 = dataSource2->GetReader();
        m_svcResource->SetResourceData(fsres2, L"UT_Parcels.sdf", L"File", dataReader2);

        Ptr<MgByteSource> dataSource3 = new MgByteSource(L"../UnitTestFiles/UT_Rail.sdf", false);
        Ptr<MgByteReader> dataReader3 = dataSource3->GetReader();
        m_svcResource->SetResourceData(fsres3, L"UT_Rail.sdf", L"File", dataReader3);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}


void TestKmlService::TestEnd()
{
    try
    {
        //set user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        //delete the map definition
        Ptr<MgResourceIdentifier> mapres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        m_svcResource->DeleteResource(mapres);

        //delete the layer definitions
        Ptr<MgResourceIdentifier> ldfres1 = new MgResourceIdentifier(L"Library://UnitTests/Layers/HydrographicPolygons.LayerDefinition");
        m_svcResource->DeleteResource(ldfres1);

        Ptr<MgResourceIdentifier> ldfres2 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        m_svcResource->DeleteResource(ldfres2);

        Ptr<MgResourceIdentifier> ldfres3 = new MgResourceIdentifier(L"Library://UnitTests/Layers/Rail.LayerDefinition");
        m_svcResource->DeleteResource(ldfres3);

        //delete the feature sources
        Ptr<MgResourceIdentifier> fsres1 = new MgResourceIdentifier(L"Library://UnitTests/Data/HydrographicPolygons.FeatureSource");
        m_svcResource->DeleteResource(fsres1);

        Ptr<MgResourceIdentifier> fsres2 = new MgResourceIdentifier(L"Library://UnitTests/Data/Parcels.FeatureSource");
        m_svcResource->DeleteResource(fsres2);

        Ptr<MgResourceIdentifier> fsres3 = new MgResourceIdentifier(L"Library://UnitTests/Data/Rail.FeatureSource");
        m_svcResource->DeleteResource(fsres3);

        #ifdef _DEBUG
        MgFdoConnectionManager* pFdoConnectionManager = MgFdoConnectionManager::GetInstance();
        if(pFdoConnectionManager)
        {
            pFdoConnectionManager->ShowCache();
        }
        #endif
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nKml Service tests completed.\n\n")));
}


void TestKmlService::TestCase_GetMapKml()
{
    try
    {
        //create map object
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSheboygan");

        //set other params
        double dpi = 96;
        STRING agentUri = L"http://myserver/mapguide/mapagent/mapagent.fcgi";
        STRING format = L"KML";

        try
        {
            //call GetMapKml
            Ptr<MgByteReader> reader = m_svcKml->GetMapKml(map, dpi, agentUri, format);

            STRING mimeType = reader->GetMimeType();
            CPPUNIT_ASSERT(mimeType.compare(MgMimeType::Kml) == 0);
        }
        catch(MgConnectionFailedException* e)
        {
            SAFE_RELEASE(e);
            ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_GetMapKml skipped because unable to connect to test server.\n")));

            // We need to update the mapguide server status so that it can be used again because we have used the
            // same exception class for servers outside the one actually executing this code.
            MgSiteManager* siteManager = MgSiteManager::GetInstance();
            Ptr<MgSiteInfo> siteInfo = siteManager->GetSiteInfo(0);
            siteInfo->SetStatus(MgSiteInfo::Ok);
        }

        //compare results against referenced content
        //TODO: Find a way to make the comparison work on Windows AND Linux
        //CPPUNIT_ASSERT(CompareContent(reader, L"../UnitTestFiles/UT_GetMapKMLResult.txt"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


void TestKmlService::TestCase_GetMapKmz()
{
    try
    {
        //create map object
        Ptr<MgResourceIdentifier> mdfres = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map = new MgMap(m_siteConnection);
        map->Create(mdfres, L"UnitTestSheboygan");

        //set other params
        double dpi = 96;
        STRING agentUri = L"http://myserver/mapguide/mapagent/mapagent.fcgi";
        STRING format = L"KMZ";

        try
        {
            //call GetMapKml
            Ptr<MgByteReader> reader = m_svcKml->GetMapKml(map, dpi, agentUri, format);

            STRING mimeType = reader->GetMimeType();
            CPPUNIT_ASSERT(mimeType.compare(MgMimeType::Kmz) == 0);
        }
        catch(MgConnectionFailedException* e)
        {
            SAFE_RELEASE(e);
            ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_GetMapKmz skipped because unable to connect to test server.\n")));

            // We need to update the mapguide server status so that it can be used again because we have used the
            // same exception class for servers outside the one actually executing this code.
            MgSiteManager* siteManager = MgSiteManager::GetInstance();
            Ptr<MgSiteInfo> siteInfo = siteManager->GetSiteInfo(0);
            siteInfo->SetStatus(MgSiteInfo::Ok);
        }
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


void TestKmlService::TestCase_GetLayerKml()
{
    try
    {
        //create the layer
        Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldfres, m_svcResource);

        //set up other params
        Ptr<MgEnvelope> extents = new MgEnvelope(-87.7398334220901,43.73822699224595,-87.72068284176505,43.75068777906662);
        INT32 width = 796;
        INT32 height = 717;
        double dpi = 96;
        INT32 drawOrder = 0;
        STRING agentUri = L"http://myserver/mapguide/mapagent/mapagent.fcgi";
        STRING format = L"KML";

        try
        {
            //call GetLayerKml
            Ptr<MgByteReader> reader = m_svcKml->GetLayerKml(layer, extents, width, height, dpi, drawOrder, agentUri, format);

            STRING mimeType = reader->GetMimeType();
            CPPUNIT_ASSERT(mimeType.compare(MgMimeType::Kml) == 0);
        }
        catch(MgConnectionFailedException* e)
        {
            SAFE_RELEASE(e);
            ACE_DEBUG((LM_INFO, ACE_TEXT("\nTestCase_GetLayerKml skipped because unable to connect to test server.\n")));

            // We need to update the mapguide server status so that it can be used again because we have used the
            // same exception class for servers outside the one actually executing this code.
            MgSiteManager* siteManager = MgSiteManager::GetInstance();
            Ptr<MgSiteInfo> siteInfo = siteManager->GetSiteInfo(0);
            siteInfo->SetStatus(MgSiteInfo::Ok);
        }

        //compare results against referenced content
        //TODO: Find a way to make the comparison work on Windows AND Linux
        //CPPUNIT_ASSERT(CompareContent(reader, L"../UnitTestFiles/UT_GetLayerKMLResult.txt"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


void TestKmlService::TestCase_GetFeaturesKml()
{
    try
    {
        //create the layer
        Ptr<MgResourceIdentifier> ldfres = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldfres, m_svcResource);

        //set up other params
        Ptr<MgEnvelope> extents = new MgEnvelope(-87.7398334220901,43.73822699224595,-87.72068284176505,43.75068777906662);
        INT32 width = 796;
        INT32 height = 717;
        double dpi = 96;
        INT32 drawOrder = 0;
        STRING format = L"KML";

        //call GetLayerKml
        Ptr<MgByteReader> reader = m_svcKml->GetFeaturesKml(layer, extents, width, height, dpi, drawOrder, format);

        STRING mimeType = reader->GetMimeType();
        CPPUNIT_ASSERT(mimeType.compare(MgMimeType::Kml) == 0);

        //compare results against referenced content
        //TODO: Find a way to make the comparison work on Windows AND Linux
        //TODO: Find a way to allow tolerance in floating point value comparisons
        //CPPUNIT_ASSERT(CompareContent(reader, L"../UnitTestFiles/UT_GetFeaturesKMLResult.txt"));
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


STRING TestKmlService::RemoveSessionIds(CREFSTRING content)
{
    STRING newContent = content;

    STRING sessionId;
    Ptr<MgUserInformation> userInfo = MgUserInformation::GetCurrentUserInfo();
    if (userInfo != NULL)
    {
        sessionId = userInfo->GetMgSessionId();
    }

    if(!sessionId.empty())
    {
        STRING dummyId = L"SessionIDRemoved";
        STRING::size_type sessionIdLocation;
        do
        {
            sessionIdLocation = newContent.find(sessionId);
            if(sessionIdLocation != STRING::npos)
            {
                newContent.replace(sessionIdLocation, sessionId.length(), dummyId);
            }
        }
        while(sessionIdLocation != STRING::npos);
    }
    return newContent;
}


void TestKmlService::SaveToFile(CREFSTRING content, CREFSTRING filename)
{
    FILE* file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("wt"));
    if (file != NULL)
    {
        fwprintf (file, content.c_str());
        fclose (file);
    }
}


STRING TestKmlService::LoadFromFile(CREFSTRING filename)
{
    STRING content;
    FILE* file = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("rt"));
    if (file != NULL)
    {
        wchar_t buffer[1024];
        while(fgetws(buffer, 1024, file) != NULL)
        {
            content.append(buffer);
        }
        fclose (file);
    }
    return content;
}


bool TestKmlService::CompareContent(MgByteReader* reader, CREFSTRING referenceFile)
{
    bool match = false;
    reader->GetByteSource()->SetMimeType(L"text/xml");
    STRING content = reader->ToString();
    STRING thisContent = RemoveSessionIds(content);

    try
    {
        STRING referenceContent = LoadFromFile(referenceFile);
        match = (thisContent.compare(0, thisContent.length(), referenceContent) == 0);
    }
    catch(...)
    {
        match = false;
    }

    //if we didn't get a match, dump out the content we did get
    if(!match)
    {
        STRING errorFile = referenceFile + L".err";
        SaveToFile(thisContent, errorFile);
    }

    return match;
}

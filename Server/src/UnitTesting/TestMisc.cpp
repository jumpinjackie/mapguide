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
#include "TestMisc.h"
#include "ServiceManager.h"
#include "ServerResourceService.h"
#include "ServerSiteService.h"
#include "../UnitTesting/CppUnitExtensions.h"
#include "FoundationDefs.h"
#include "FdoConnectionManager.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMisc, "TestMisc");

TestMisc::TestMisc()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcResource = dynamic_cast<MgResourceService*>(
        serviceManager->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

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


TestMisc::~TestMisc()
{
}


void TestMisc::setUp()
{
}


void TestMisc::tearDown()
{
}


void TestMisc::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Miscellaneous tests.\n")));

    //Reusing Mapping Service test data
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

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan_833.MapDefinition");
        Ptr<MgByteSource> mdfsrc2 = new MgByteSource(L"../UnitTestFiles/UT_Sheboygan_833.mdf", false);
        Ptr<MgByteReader> mdfrdr2 = mdfsrc2->GetReader();
        m_svcResource->SetResource(mapres2, mdfrdr2, NULL);

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

        // publish the print layouts
        Ptr<MgResourceIdentifier> plres1 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        Ptr<MgByteSource> plsrc1 = new MgByteSource(L"../UnitTestFiles/UT_AllElements.pl", false);
        Ptr<MgByteReader> plrdr1 = plsrc1->GetReader();
        m_svcResource->SetResource(plres1, plrdr1, NULL);

        Ptr<MgResourceIdentifier> plres2 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/NoLegend.PrintLayout");
        Ptr<MgByteSource> plsrc2 = new MgByteSource(L"../UnitTestFiles/UT_NoLegend.pl", false);
        Ptr<MgByteReader> plrdr2 = plsrc2->GetReader();
        m_svcResource->SetResource(plres2, plrdr2, NULL);

        // publish the symbol library
        Ptr<MgResourceIdentifier> slres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/SymbolMart.SymbolLibrary");
        Ptr<MgByteSource> slsrc1 = new MgByteSource(L"../UnitTestFiles/UT_SymbolMart.sl", false);
        Ptr<MgByteReader> slrdr1 = slsrc1->GetReader();
        m_svcResource->SetResource(slres1, slrdr1, NULL);
        Ptr<MgByteSource> datasrc = new MgByteSource(L"../UnitTestFiles/UT_Symbols.dwf", false);
        Ptr<MgByteReader> datardr = datasrc->GetReader();
        m_svcResource->SetResourceData(slres1, L"symbols.dwf", L"File", datardr);
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


void TestMisc::TestEnd()
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

        Ptr<MgResourceIdentifier> mapres2 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan_833.MapDefinition");
        m_svcResource->DeleteResource(mapres2);

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

        // delete the print layouts
        Ptr<MgResourceIdentifier> plres1 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/AllElements.PrintLayout");
        m_svcResource->DeleteResource(plres1);

        Ptr<MgResourceIdentifier> plres2 = new MgResourceIdentifier(L"Library://UnitTests/PrintLayouts/NoLegend.PrintLayout");
        m_svcResource->DeleteResource(plres2);

        // delete the symbol library
        Ptr<MgResourceIdentifier> slres1 = new MgResourceIdentifier(L"Library://UnitTests/Symbols/SymbolMart.SymbolLibrary");
        m_svcResource->DeleteResource(slres1);

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

    ACE_DEBUG((LM_INFO, ACE_TEXT("\nMiscellaneous tests completed.\n\n")));
}

void TestMisc::TestCase_CommonExceptionMessages()
{
    try
    {
        MgObject* ptr = NULL;
        CHECKNULL(ptr, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr2 = NULL;
        CHECKNULL((void*)ptr2, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr2' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr2") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr3 = NULL;
        CHECKARGUMENTNULL(ptr3, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr3' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr3") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr4 = NULL;
        CHECKARGUMENTNULL((void*)ptr4, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr4' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr4") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT32 val = 1;
        MG_CHECK_RANGE(val, 2, 3, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'val' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"val") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT32 val = 1;
        INT32 min = 2;
        INT32 max = 3;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT64 val = 1L;
        MG_CHECK_RANGE(val, 2L, 3L, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT64 val = 1L;
        INT64 min = 2L;
        INT64 max = 3L;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        float val = 1.0f;
        MG_CHECK_RANGE(val, 2.0f, 3.0f, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        float val = 1.0f;
        float min = 2.0f;
        float max = 3.0f;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        double val = 1.0;
        MG_CHECK_RANGE(val, 2.0, 3.0, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        double val = 1.0;
        double min = 2.0;
        double max = 3.0;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        Ptr<MgNamedCollection> coll = new MgNamedCollection(false);
        coll->GetItem(L"Foo");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected string 'Foo' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"Foo") != STRING::npos);
        ex->Release();
    }
}

void TestMisc::TestCase_611()
{
    try
    {
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");

        Ptr<MgLayerGroup> detachedGroup = new MgLayerGroup(L"Detached");
        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgLayer> layer = new MgLayer(ldf, m_svcResource);
        layer->SetName(L"BelongsToDetachedGroup");
        layer->SetLegendLabel(L"BelongsToDetachedGroup");
        layer->SetGroup(detachedGroup);
        Ptr<MgLayerCollection> mapLayers = map1->GetLayers();
        mapLayers->Insert(0, layer);

        CPPUNIT_ASSERT_THROW_MG(map1->Save(), MgGroupNotFoundException*);
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

void TestMisc::TestCase_833()
{
    try
    {
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan_833.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");

        Ptr<MgLayerGroupCollection> groups = map1->GetLayerGroups();
        Ptr<MgLayerCollection> layers = map1->GetLayers();

        //These are the ones initially hidden from the legend
        Ptr<MgLayerGroup> group = groups->GetItem(L"Municipal");
        Ptr<MgLayerBase> layer = layers->GetItem(L"HydrographicPolygons");

        CPPUNIT_ASSERT(!group->GetLegendLabel().empty());
        CPPUNIT_ASSERT(!layer->GetLegendLabel().empty());
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

void TestMisc::TestCase_1304()
{
    try
    {
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");

        Ptr<MgSelection> sel = new MgSelection(map1, L"");
        Ptr<MgReadOnlyLayerCollection> selLayers = sel->GetLayers();
        CPPUNIT_ASSERT_MESSAGE("Expected null MgReadOnlyLayerCollection", NULL == selLayers.p); 
        sel->FromXml(L""); //Should be same result
        selLayers = sel->GetLayers();
        CPPUNIT_ASSERT_MESSAGE("Expected null MgReadOnlyLayerCollection", NULL == selLayers.p); 
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

void TestMisc::TestCase_MapLayerCollections()
{
    try
    {
        Ptr<MgResourceIdentifier> mapRes1 = new MgResourceIdentifier(L"Library://UnitTests/Maps/Sheboygan.MapDefinition");
        Ptr<MgMap> map1 = new MgMap(m_siteConnection);
        map1->Create(mapRes1, L"UnitTestSheboygan1");

        Ptr<MgLayerGroup> detachedGroup = new MgLayerGroup(L"DetachedGroup");
        Ptr<MgResourceIdentifier> ldf = new MgResourceIdentifier(L"Library://UnitTests/Layers/Parcels.LayerDefinition");
        Ptr<MgLayer> detachedLayer = new MgLayer(ldf, m_svcResource);
        detachedLayer->SetName(L"DetachedLayer");

        Ptr<MgLayerCollection> mapLayers = map1->GetLayers();
        Ptr<MgLayerGroupCollection> mapGroups = map1->GetLayerGroups();

        //Remove() should be returning false when passing in layers/groups that don't belong
        CPPUNIT_ASSERT(!mapLayers->Remove(detachedLayer));
        CPPUNIT_ASSERT(!mapGroups->Remove(detachedGroup));
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
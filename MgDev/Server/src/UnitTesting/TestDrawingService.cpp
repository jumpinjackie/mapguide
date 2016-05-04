//
//  Copyright (C) 2004-2014 by Autodesk, Inc.
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
#include "TestDrawingService.h"
#include "CppUnitExtensions.h"
#include "ServiceManager.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestDrawingService, "TestDrawingService");

TestDrawingService::TestDrawingService()
{
    // Initialize service objects.
    MgServiceManager* serviceManager = MgServiceManager::GetInstance();

    m_svcDrawing = dynamic_cast<MgDrawingService*>(serviceManager->RequestService(MgServiceType::DrawingService));
    assert(m_svcDrawing != NULL);
}

TestDrawingService::~TestDrawingService() { }

void TestDrawingService::setUp() { }

void TestDrawingService::tearDown() { }

void TestDrawingService::TestStart()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(resSvc != NULL);

        //Set the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        MgResourceIdentifier resourceIdentifier1(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");

        Ptr<MgByteSource> contentSource1 = new MgByteSource(L"../UnitTestFiles/SpaceShipDrawingSource.xml");
        Ptr<MgByteReader> contentReader1 = contentSource1->GetReader();
        resSvc->SetResource(&resourceIdentifier1, contentReader1, NULL);

        Ptr<MgByteSource> dataSource1 = new MgByteSource(L"../UnitTestFiles/SpaceShip.dwf");
        Ptr<MgByteReader> dataReader1 = dataSource1->GetReader();
        resSvc->SetResourceData(&resourceIdentifier1, L"SpaceShip.dwf", L"File", dataReader1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestDrawingService::TestEnd()
{
    try
    {
        MgServiceManager* serviceManager = MgServiceManager::GetInstance();
        Ptr<MgResourceService> resSvc = dynamic_cast<MgResourceService*>(serviceManager->RequestService(MgServiceType::ResourceService));
        assert(resSvc != NULL);

        //Set the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetLocale(TEST_LOCALE);
        MgUserInformation::SetCurrentUserInfo(userInfo);

        MgResourceIdentifier resourceIdentifier1(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");

        resSvc->DeleteResource(&resourceIdentifier1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
}

void TestDrawingService::TestCase_DescribeDrawing()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        Ptr<MgByteReader> rdr1 = m_svcDrawing->DescribeDrawing(resId1);
        CPPUNIT_ASSERT(MgMimeType::Xml == rdr1->GetMimeType());
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

void TestDrawingService::TestCase_EnumerateSections()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        Ptr<MgByteReader> rdr1 = m_svcDrawing->EnumerateSections(resId1);
        CPPUNIT_ASSERT(MgMimeType::Xml == rdr1->GetMimeType());
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

void TestDrawingService::TestCase_EnumerateSectionResources()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        CPPUNIT_ASSERT_THROW_MG(m_svcDrawing->EnumerateSectionResources(resId1, L"foo"), MgDwfSectionNotFoundException*);
        
        STRING sect1 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764";
        Ptr<MgByteReader> rdr1 = m_svcDrawing->EnumerateSectionResources(resId1, sect1);
        CPPUNIT_ASSERT(MgMimeType::Xml == rdr1->GetMimeType());
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

void TestDrawingService::TestCase_GetCoordinateSpace()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        STRING cs1 = m_svcDrawing->GetCoordinateSpace(resId1);
        CPPUNIT_ASSERT(!cs1.empty());
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

void TestDrawingService::TestCase_EnumerateLayers()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        STRING sect1 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764";
        Ptr<MgStringCollection> str1 = m_svcDrawing->EnumerateLayers(resId1, sect1);
        CPPUNIT_ASSERT(str1->GetCount() > 0);
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

void TestDrawingService::TestCase_GetDrawing()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        Ptr<MgByteReader> rdr1 = m_svcDrawing->GetDrawing(resId1);
        CPPUNIT_ASSERT(NULL != rdr1.p);
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

void TestDrawingService::TestCase_GetLayer()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        STRING sect1 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764";
        Ptr<MgStringCollection> dwfLayers = m_svcDrawing->EnumerateLayers(resId1, sect1);
        for (INT32 i = 0; i < dwfLayers->GetCount(); i++)
        {
            Ptr<MgByteReader> rdr = m_svcDrawing->GetLayer(resId1, sect1, dwfLayers->GetItem(i));
        }
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

void TestDrawingService::TestCase_GetSection()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        STRING sect1 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764";
        Ptr<MgByteReader> rdr = m_svcDrawing->GetSection(resId1, sect1);
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

void TestDrawingService::TestCase_GetSectionResource()
{
    try
    {
        Ptr<MgResourceIdentifier> resId1 = new MgResourceIdentifier(L"Library://UnitTests/Drawings/SpaceShip.DrawingSource");
        STRING res1 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764\\4AA701C2E18DD99948F3C0D4FDFAC165.png";
        Ptr<MgByteReader> rdr1 = m_svcDrawing->GetSectionResource(resId1, res1);
        CPPUNIT_ASSERT(rdr1->GetMimeType() == MgMimeType::Png);
        STRING res2 = L"com.autodesk.dwf.ePlot_9E2723744244DB8C44482263E654F764\\descriptor.xml";
        Ptr<MgByteReader> rdr2 = m_svcDrawing->GetSectionResource(resId1, res2);
        CPPUNIT_ASSERT(rdr2->GetMimeType() == MgMimeType::Xml);
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
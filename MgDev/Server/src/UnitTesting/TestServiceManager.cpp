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
#include "ServiceManager.h"
#include "TestServiceManager.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestServiceManager, "TestServiceManager");

void TestServiceManager::setUp()
{
}

void TestServiceManager::tearDown()
{
}

void TestServiceManager::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Service Manager tests.\n")));
}

void TestServiceManager::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nService Manager tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks to see if there is a valid MgServiceManager and that
/// there is only 1 MgServiceManager.
///----------------------------------------------------------------------------
void TestServiceManager::TestCase_ValidServiceManager()
{
    MgServiceManager* pMgServiceManager = MgServiceManager::GetInstance();
    CPPUNIT_ASSERT(pMgServiceManager != NULL);

    MgServiceManager* pMgServiceManager2 = MgServiceManager::GetInstance();
    CPPUNIT_ASSERT(pMgServiceManager == pMgServiceManager2);
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to enable/disable services.
///----------------------------------------------------------------------------
void TestServiceManager::TestCase_EnableServices()
{
    MgServiceManager* pMgServiceManager = MgServiceManager::GetInstance();
    Ptr<MgPropertyCollection> pProperties = new MgPropertyCollection();
    Ptr<MgStringProperty> pProperty = new MgStringProperty(
        MgConfigProperties::HostPropertyDrawingService, L"0");

    pProperties->Add(pProperty);
    pMgServiceManager->EnableServices(pProperties);
    pProperty->SetValue(L"1");
    pMgServiceManager->EnableServices(pProperties);

    try
    {
        Ptr<MgService> pService = pMgServiceManager->RequestService(MgServiceType::DrawingService);
        CPPUNIT_ASSERT(pService != NULL);
    }
    catch(...)
    {
        CPPUNIT_ASSERT(false);
    }
}

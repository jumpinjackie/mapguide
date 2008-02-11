//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

// FoundationTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestResourceService.h"
#include "SiteConnection.h"


int _tmain(int argc, _TCHAR* argv[])
{
    MgSiteConnection conn;

    Ptr<MgResourceService> svc = (MgResourceService*) conn.CreateService(MgServiceType::ResourceService);

    Ptr<MgByteReader> rdr = svc->EnumerateRepositories(L"Session");

    Ptr<MgStringProperty> prop = new MgStringProperty();
    prop->SetName(L"TempPath");
    prop->SetValue(L"?*BadPathTextShouldFailAndThrowMgConfigurationException");

    MgMemoryStreamHelper* helper = new MgMemoryStreamHelper();
    Ptr<MgStream> stream = new MgStream(helper);

    stream->WriteObject(prop);

    MgObject* obj = stream->GetObject();

    MgConfiguration* config = MgConfiguration::GetInstance();

    config->LoadConfiguration(L"webconfig.ini");

    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    propCol->Add(prop);

    // If you get an MgConfigurationException here then MgConfiguration is working properly.
    config->SetProperties(L"GeneralProperties", propCol);

    return 0;
}


bool InitializeStatic();

bool bStatic = InitializeStatic();


bool InitializeStatic()
{
    MgServiceRegistry* fact = MgServiceRegistry::GetInstance();

    fact->RegisterService(MgServiceType::ResourceService, MgTestResourceService::CreateService);

    return true;
}


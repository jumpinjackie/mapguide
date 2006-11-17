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


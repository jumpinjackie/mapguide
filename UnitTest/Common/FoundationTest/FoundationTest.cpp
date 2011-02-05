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

// FoundationTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "TestResourceService.h"
#include "SiteConnection.h"
#include "SAX2Parser.h"
#include "PrintLayout/MapViewportDefinition.h"

//////////////////////////////////////////////////////////////////
// !!! TODO: Refactor into CPPUnit tests !!!
//////////////////////////////////////////////////////////////////
int _tmain(int argc, _TCHAR* argv[])
{
    //////////////////////////////////////////////////////////////////
    // Test MgConfiguration Exception
    //////////////////////////////////////////////////////////////////

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
    bool failed = false;
    try
    {
        config->SetProperties(L"GeneralProperties", propCol);
    }
    catch (MgConfigurationException* e)
    {
        failed = true;
        e->Release();
    }

    if (!failed)
        return 1;

    //////////////////////////////////////////////////////////////////
    // Test MapViewportDefinition Parsing. Just want to prove it runs
    // right now. Data comparison will happen in a subsequent submission
    //////////////////////////////////////////////////////////////////

    // Test Parsing MapViewport Definition
    Ptr<MgByteSource> contentSource = new MgByteSource(L"MapViewportSample1.xml");
    Ptr<MgByteReader> reader = contentSource->GetReader();
    Ptr<MgByteSink> sink = new MgByteSink(reader);
    Ptr<MgByte> bytes = sink->ToBuffer();

    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        return 1;
    }

    auto_ptr<MdfModel::MapViewportDefinition> viewportDefinition(parser.DetachMapViewportDefinition());
    assert(viewportDefinition.get() != NULL);

    //////////////////////////////////////////////////////////////////
    // Test MapViewportDefinition WriteToFile
    //////////////////////////////////////////////////////////////////
    parser.WriteToFile("MapViewportSample1_OUT.xml", NULL, NULL, NULL, NULL, NULL, viewportDefinition.get(), &MdfModel::Version(2, 0, 0));

    //////////////////////////////////////////////////////////////////
    // Test Populate the MgMapViewportBase from the definition
    //////////////////////////////////////////////////////////////////
    Ptr<MgMapViewportBase> vpBase = new MgMapViewportBase();
    vpBase->PopulateFromResource(viewportDefinition.get());

    //////////////////////////////////////////////////////////////////
    // Test Serializing the MgMapViewportBase
    //////////////////////////////////////////////////////////////////
    Ptr<MgMemoryStreamHelper> streamHelper = new MgMemoryStreamHelper();
    Ptr<MgStream> vpStream = new MgStream(streamHelper);
    vpBase->Serialize(vpStream);
    Ptr<MgByteSource> bsource = new MgByteSource((BYTE_ARRAY_IN)streamHelper->GetBuffer(), streamHelper->GetLength());
    Ptr<MgByteReader> resourceData = bsource->GetReader();

    //////////////////////////////////////////////////////////////////
    // Test Deserializing the MgMapViewportBase
    //////////////////////////////////////////////////////////////////
    Ptr<MgMapViewportBase> vpBase2 = new MgMapViewportBase();
    vpBase2->Deserialize(vpStream);

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


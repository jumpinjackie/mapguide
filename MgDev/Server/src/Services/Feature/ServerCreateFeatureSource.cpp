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

#include "ServerFeatureServiceDefs.h"
#include "ServerCreateFeatureSource.h"
#include "ServerFeatureConnection.h"
#include "ServerGetFeatures.h"
#include "ServerDescribeSchema.h"

#include "Fdo/Commands/DataStore/ICreateDataStore.h"

MgServerCreateFeatureSource::MgServerCreateFeatureSource()
{
}

MgServerCreateFeatureSource::~MgServerCreateFeatureSource()
{
}

void MgServerCreateFeatureSource::CreateFeatureSource(MgResourceIdentifier* resource, MgFeatureSourceParams* sourceParams)
{
    MG_FEATURE_SERVICE_TRY()

    Ptr<MgCreateSdfParams> params = SAFE_ADDREF(dynamic_cast<MgCreateSdfParams*>(sourceParams));

    //Some basic schema validation:
    //  A schema must be supplied
    //  The schema must define at least one class
    //  Each class must have an identity property
    //  A coordinate system must be defined
    //
    Ptr<MgFeatureSchema> schema = params->GetFeatureSchema();
    if(schema == NULL)
        throw new MgInvalidArgumentException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSchema", NULL);

    Ptr<MgClassDefinitionCollection> classes = schema->GetClasses();
    if(classes == NULL || classes->GetCount() == 0)
        throw new MgInvalidArgumentException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingClassDef", NULL);

    for(INT32 ci = 0; ci < classes->GetCount(); ci++)
    {
        Ptr<MgClassDefinition> classDef = classes->GetItem(ci);
        Ptr<MgPropertyDefinitionCollection> idProps = classDef->GetIdentityProperties();
        if(idProps == NULL || idProps->GetCount() == 0)
            throw new MgInvalidArgumentException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgClassWOIdentity", NULL);
    }

    // A coordinate system must be defined
    if(params->GetCoordinateSystemWkt().empty())
        throw new MgInvalidArgumentException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"MgMissingSrs", NULL);

    // Connect to provider
    STRING sdfProvider = L"OSGeo.SDF"; // NOXLATE
    STRING sdfConnString = L""; // NOXLATE
    MgServerFeatureConnection msfc(sdfProvider, sdfConnString);

    FdoPtr<FdoIConnection> conn =  msfc.GetConnection();

    if (conn == NULL)
    {
        throw new MgConnectionFailedException(L"MgServerCreateFeatureSource.CreateFeatureSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }


    //TSW TODO: Create temporary file name here...
    STRING tempFileName = MgFileUtil::GenerateTempFileName(true, STRING(L"tmp"), STRING(L"sdf"));

    // Use the custom command to initially create the datastore
    FdoPtr<FdoICreateDataStore> createDsCmd = static_cast<FdoICreateDataStore*>(conn->CreateCommand(FdoCommandType_CreateDataStore));
    FdoPtr<FdoIDataStorePropertyDictionary> dsProp = createDsCmd->GetDataStoreProperties();
    dsProp->SetProperty (L"File", tempFileName.c_str());
    createDsCmd->Execute();

    //now open the connection to the newly created file
    STRING connstr = std::wstring(L"File=") + tempFileName; // NOXLATE
    conn->SetConnectionString(connstr.c_str());

    conn->Open();

    // Create the spatialcontext
    FdoPtr<FdoICreateSpatialContext> spatialContext = (FdoICreateSpatialContext*)conn->CreateCommand(FdoCommandType_CreateSpatialContext);
    spatialContext->SetCoordinateSystemWkt(params->GetCoordinateSystemWkt().c_str());
    spatialContext->SetDescription(params->GetSpatialContextDescription().c_str());
    spatialContext->SetName(params->GetSpatialContextName().c_str());
    spatialContext->SetXYTolerance(params->GetXYTolerance());
    spatialContext->SetZTolerance(params->GetZTolerance());
    spatialContext->Execute();

    // Create and set the schema
    MgServerDescribeSchema descSchema;
    Ptr<MgFeatureSchema> featureSchema = params->GetFeatureSchema();
    FdoPtr<FdoFeatureSchema> fdoSchema = descSchema.GetFdoFeatureSchema(featureSchema);
    FdoPtr<FdoIApplySchema> applyschema = (FdoIApplySchema*)conn->CreateCommand(FdoCommandType_ApplySchema);
    applyschema->SetFeatureSchema(fdoSchema);
    applyschema->Execute();

    // In general, the Close() should not be called directly on the FdoConnection
    // because connections should be pooled by the FdoConnectionManager.
    // In this case, the MgServerFeatureConnection object was created with an
    // empty connection string, which leads to the connection to be unmanaged by the pool.
    // Thus, it is ok to call Close() here.
    conn->Close();

    // Create a FeatureSource for the new sdf
    // This is hardcoded and nasty.  Is there a better way to implement this?
    // NOXLATE

    STRING resourceFile = resource->GetName() + L".sdf";

    STRING featureSource = L"<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">\n\
<Provider>OSGeo.SDF</Provider>\n\
  <Parameter>\n\
    <Name>File</Name>\n";

    featureSource += L"     <Value>%MG_DATA_FILE_PATH%" + resourceFile + L"</Value>\n";
    featureSource += L"     </Parameter>\n\
  <Parameter>\n\
    <Name>ReadOnly</Name>\n\
    <Value>FALSE</Value>\n\
  </Parameter>\n\
</FeatureSource>";

    // Use the Resource Service to create the new feature source.

    string utf8Text = MgUtil::WideCharToMultiByte(featureSource);

    Ptr<MgByteSource> xmlSource = new MgByteSource((BYTE_ARRAY_IN) utf8Text.c_str(), (INT32)utf8Text.length());
    Ptr<MgByteReader> xmlReader = xmlSource->GetReader();

    Ptr<MgByteSource> sdfSource = new MgByteSource(tempFileName, true);
    Ptr<MgByteReader> sdfReader = sdfSource->GetReader();

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    Ptr<MgResourceService> resourceService;
    if (NULL != serviceMan)
    {
        resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    }

    if (NULL != (MgResourceService*)resourceService)
    {
        resourceService->SetResource(resource, xmlReader, NULL);
        resourceService->SetResourceData(resource, resourceFile, MgResourceDataType::File, sdfReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerCreateFeatureSource.CreateFeatureSource")
}

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

#include "Services/Feature/FeatureDefs.h"
#include "GetSchemaMapping.h"
#include "Services/Resource/UnmanagedDataManager.h"

MgGetSchemaMapping::MgGetSchemaMapping() :
    m_bytes(NULL)
{
}

MgGetSchemaMapping::~MgGetSchemaMapping()
{
    delete [] m_bytes;
    m_bytes = NULL;
}


MgByteReader* MgGetSchemaMapping::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    STRING data = partialConnString;
    MgUnmanagedDataManager::SubstituteDataPathAliases(data);

    // Connect to the provider
    Ptr<MgFeatureConnection> msfc = new MgFeatureConnection(providerName, data);
    if ((NULL != msfc.p) && (( msfc->IsConnectionOpen() ) || ( msfc->IsConnectionPending() )))
    {
        // The reference to the FDO connection from the MgFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConnection;
        fdoConnection = msfc->GetConnection();

        // Create the memory stream
        FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
        CHECKNULL((FdoIoMemoryStream*)fmis, L"MgGetSchemaMapping.GetSchemaMapping");

        FdoXmlWriterP writer = FdoXmlWriter::Create(fmis);

        FdoPtr<FdoXmlSpatialContextFlags> flags = FdoXmlSpatialContextFlags::Create();
        flags->SetIncludeDefault(true);

        // Serialize the spatial contexts
        FdoXmlSpatialContextSerializer::XmlSerialize(
            fdoConnection,
            FdoXmlSpatialContextWriterP(
                FdoXmlSpatialContextWriter::Create(writer)
            ),
            flags
        );

        // Get the schema
        FdoPtr<FdoIDescribeSchema> fdoDescribeSchemaCommand = (FdoIDescribeSchema*)fdoConnection->CreateCommand(FdoCommandType_DescribeSchema);
        CHECKNULL((FdoIDescribeSchema*)fdoDescribeSchemaCommand, L"MgGetSchemaMapping.GetSchemaMapping");

        // Execute the command
        FdoPtr<FdoFeatureSchemaCollection> fdoFeatureSchemaCollection;
        fdoFeatureSchemaCollection = fdoDescribeSchemaCommand->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)fdoFeatureSchemaCollection, L"MgGetSchemaMapping.GetSchemaMapping");

        // Write to memory stream
        fdoFeatureSchemaCollection->WriteXml(writer);

        // Get the schema mapping
        FdoPtr<FdoIDescribeSchemaMapping> fdoDescribeSchemaMappingCommand = (FdoIDescribeSchemaMapping*)fdoConnection->CreateCommand(FdoCommandType_DescribeSchemaMapping);
        CHECKNULL((FdoIDescribeSchemaMapping*)fdoDescribeSchemaMappingCommand, L"MgGetSchemaMapping.GetSchemaMapping");

        fdoDescribeSchemaMappingCommand->SetIncludeDefaults(true);

        // Execute the command
        FdoPtr<FdoPhysicalSchemaMappingCollection> fdoPhysicalSchemaMappingCollection;
        fdoPhysicalSchemaMappingCollection = fdoDescribeSchemaMappingCommand->Execute();
        CHECKNULL((FdoPhysicalSchemaMappingCollection*)fdoPhysicalSchemaMappingCollection, L"MgGetSchemaMapping.GetSchemaMapping");

        // Write to memory stream
        fdoPhysicalSchemaMappingCollection->WriteXml(writer);

        // Close the XML writer
        writer->Close();

        fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

        FdoInt64 len = fmis->GetLength();
        m_bytes = new FdoByte[(size_t)len];
        CHECKNULL(m_bytes, L"MgGetSchemaMapping.GetSchemaMapping");

        fmis->Read(m_bytes, (FdoSize)len);

        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)m_bytes, (INT32)len);
        byteSource->SetMimeType(MgMimeType::Xml);
        byteReader = byteSource->GetReader();
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgGetSchemaMapping::GetSchemaMapping()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgGetSchemaMapping.GetSchemaMapping")

    return byteReader.Detach();
}

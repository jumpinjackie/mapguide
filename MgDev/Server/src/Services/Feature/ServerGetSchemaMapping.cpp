//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "ServerGetSchemaMapping.h"

MgServerGetSchemaMapping::MgServerGetSchemaMapping() :
    m_bytes(NULL)
{
}

MgServerGetSchemaMapping::~MgServerGetSchemaMapping()
{
    delete [] m_bytes;
    m_bytes = NULL;
}


MgByteReader* MgServerGetSchemaMapping::GetSchemaMapping(CREFSTRING providerName, CREFSTRING partialConnString)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Connect to the provider
    GisPtr<FdoIConnection> fdoConnection;
    GisPtr<FdoPhysicalSchemaMappingCollection> fdoPhysicalSchemaMappingCollection;

    MgServerFeatureConnection msfc(providerName, partialConnString);
    if (( msfc.IsConnectionOpen() ) || ( msfc.IsConnectionPending() ))
    {
        fdoConnection = msfc.GetConnection();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerGetSchemaMapping::GetSchemaMapping()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    GisPtr<FdoIDescribeSchemaMapping> fdoCommand = (FdoIDescribeSchemaMapping*)fdoConnection->CreateCommand(FdoCommandType_DescribeSchemaMapping);
    CHECKNULL((FdoIDescribeSchemaMapping*)fdoCommand, L"MgServerGetSchemaMapping.GetSchemaMapping");

    fdoCommand->SetIncludeDefaults(true);

    // Execute the command
    fdoPhysicalSchemaMappingCollection = fdoCommand->Execute();
    CHECKNULL((FdoPhysicalSchemaMappingCollection*)fdoPhysicalSchemaMappingCollection, L"MgServerGetSchemaMapping.GetSchemaMapping");

    GisIoMemoryStreamP fmis = GisIoMemoryStream::Create();
    CHECKNULL((GisIoMemoryStream*)fmis, L"MgServerGetSchemaMapping.GetSchemaMapping");

    // Write to memory stream
    fdoPhysicalSchemaMappingCollection->WriteXml(fmis);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    GisInt64 len = fmis->GetLength();
    m_bytes = new GisByte[(size_t)len];
    CHECKNULL(m_bytes, L"MgServerGetSchemaMapping.GetSchemaMapping");

    fmis->Read(m_bytes, (GisSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)m_bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    byteReader = byteSource->GetReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSchemaMapping.GetSchemaMapping")

    return byteReader.Detach();
}

//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "ServerApplySchema.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"
#include "CacheManager.h"

//////////////////////////////////////////////////////////////////
MgServerApplySchema::MgServerApplySchema()
{
}

//////////////////////////////////////////////////////////////////
MgServerApplySchema::~MgServerApplySchema()
{
}

///////////////////////////////////////////////////////////////////////////////
void MgServerApplySchema::ApplySchema(MgResourceIdentifier* resource,
    MgFeatureSchema* schema)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource || NULL == schema)
    {
        throw new MgNullArgumentException(L"MgServerApplySchema.ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Connect to provider
    Ptr<MgServerFeatureConnection> msfc = new MgServerFeatureConnection(resource);

    // connection must be open
    bool bRefresh = false;
    if ((NULL != msfc.p) && ( msfc->IsConnectionOpen() ))
    {
        FdoPtr<FdoIConnection> fdoConn = msfc->GetConnection();

        // Check whether command is supported by provider
        if (!msfc->SupportsCommand((INT32)FdoCommandType_ApplySchema) || 
            !msfc->SupportsCommand((INT32)FdoCommandType_DescribeSchema))
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgServerApplySchema.ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIDescribeSchema> fdoDecribeSchemaCmd = (FdoIDescribeSchema*) fdoConn->CreateCommand(FdoCommandType_DescribeSchema);
        CHECKNULL((FdoIDescribeSchema*)fdoDecribeSchemaCmd, L"MgServerApplySchema.ApplySchema");

        FdoPtr<FdoFeatureSchemaCollection> schemas = fdoDecribeSchemaCmd->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)schemas, L"MgServerApplySchema.ApplySchema");

        FdoPtr<FdoIApplySchema> fdoApplySchemaCmd = (FdoIApplySchema*)fdoConn->CreateCommand(FdoCommandType_ApplySchema);
        CHECKNULL((FdoIApplySchema*)fdoApplySchemaCmd, L"MgServerApplySchema.ApplySchema");

        STRING schemaName = schema->GetName();
        FdoPtr<FdoFeatureSchema> fdoOldSchema = schemas->FindItem(schemaName.c_str());
        if (NULL == fdoOldSchema)
        {
            if (!schema->IsDeleted())
            {
                FdoPtr<FdoFeatureSchema> fdoNewSchema = MgServerFeatureUtil::GetFdoFeatureSchema(schema);
                fdoApplySchemaCmd->SetFeatureSchema(fdoNewSchema);
                fdoApplySchemaCmd->Execute();
                bRefresh = true;
            }
        }
        else
        {
            if (!schema->IsDeleted())
                MgServerFeatureUtil::UpdateFdoFeatureSchema(schema, fdoOldSchema);
            else
                fdoOldSchema->Delete();

            fdoApplySchemaCmd->SetFeatureSchema(fdoOldSchema);
            fdoApplySchemaCmd->Execute();
            bRefresh = true;
        }
    }

    // Clear the cached schema so that MgFeatureService::DescribeSchema 
    // can return the correct schema
    if (bRefresh)
    {
        MgCacheManager* m_cacheManager = MgCacheManager::GetInstance();
        MgFeatureServiceCache* m_featureServiceCache = m_cacheManager->GetFeatureServiceCache();
        m_featureServiceCache->RemoveEntry(resource);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.ApplySchema")
}



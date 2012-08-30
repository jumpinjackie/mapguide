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
#include "ApplySchema.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include "Services/Feature/FeatureServiceCache.h"

//////////////////////////////////////////////////////////////////
MgdApplySchema::MgdApplySchema()
{
}

//////////////////////////////////////////////////////////////////
MgdApplySchema::~MgdApplySchema()
{
}

///////////////////////////////////////////////////////////////////////////////
void MgdApplySchema::ApplySchema(MgResourceIdentifier* resource,
    MgFeatureSchema* schema)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource || NULL == schema)
    {
        throw new MgNullArgumentException(L"MgdApplySchema.ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Connect to provider
    Ptr<MgdFeatureConnection> msfc = new MgdFeatureConnection(resource);

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
            STRING message = MgdFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgdApplySchema.ApplySchema", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIDescribeSchema> fdoDecribeSchemaCmd = (FdoIDescribeSchema*) fdoConn->CreateCommand(FdoCommandType_DescribeSchema);
        CHECKNULL((FdoIDescribeSchema*)fdoDecribeSchemaCmd, L"MgdApplySchema.ApplySchema");

        FdoPtr<FdoFeatureSchemaCollection> schemas = fdoDecribeSchemaCmd->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)schemas, L"MgdApplySchema.ApplySchema");

        FdoPtr<FdoIApplySchema> fdoApplySchemaCmd = (FdoIApplySchema*)fdoConn->CreateCommand(FdoCommandType_ApplySchema);
        CHECKNULL((FdoIApplySchema*)fdoApplySchemaCmd, L"MgdApplySchema.ApplySchema");

        STRING schemaName = schema->GetName();
        FdoPtr<FdoFeatureSchema> fdoOldSchema = schemas->FindItem(schemaName.c_str());
        if (NULL == fdoOldSchema)
        {
            if (!schema->IsDeleted())
            {
                FdoPtr<FdoFeatureSchema> fdoNewSchema = MgdFeatureUtil::GetFdoFeatureSchema(schema);
                fdoApplySchemaCmd->SetFeatureSchema(fdoNewSchema);
                fdoApplySchemaCmd->Execute();
                bRefresh = true;
            }
        }
        else
        {
            if (!schema->IsDeleted())
                MgdFeatureUtil::UpdateFdoFeatureSchema(schema, fdoOldSchema);
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
        MgdFeatureServiceCache* m_cacheManager = MgdFeatureServiceCache::GetInstance();
        m_cacheManager->RemoveEntry(resource);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.ApplySchema")
}

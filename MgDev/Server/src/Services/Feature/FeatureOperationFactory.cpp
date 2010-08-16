//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "FeatureOperationFactory.h"
#include "FeatureOperation.h"

#include "OpApplySchema.h"
#include "OpDescribeSchema.h"
#include "OpDescribeSchemaAsXml.h"
#include "OpExecuteSqlNonQuery.h"
#include "OpExecuteSqlQuery.h"
#include "OpGetCapabilities.h"
#include "OpGetConnectionPropertyValues.h"
#include "OpGetFeatureProviders.h"
#include "OpGetSpatialContexts.h"
#include "OpSelectFeatures.h"
#include "OpSelectFeaturesSpatial.h"
#include "OpTestConnection.h"
#include "OpTestFeatureSourceConnection.h"
#include "OpUpdateFeatures.h"
#include "OpGetFeatures.h"
#include "OpCloseFeatureReader.h"
#include "OpGetLongTransactions.h"
#include "OpSetLongTransaction.h"
#include "OpGetSchemas.h"
#include "OpGetClasses.h"
#include "OpGetClassDefinition.h"
#include "OpGetRaster.h"
#include "OpGetSqlRows.h"
#include "OpCloseSqlReader.h"
#include "OpGetDataRows.h"
#include "OpCloseDataReader.h"
#include "OpSchemaToXml.h"
#include "OpXmlToSchema.h"
#include "OpCreateFeatureSource.h"
#include "OpGetIdentityProperties.h"
#include "OpDescribeWfsFeatureType.h"
#include "OpGetWfsFeature.h"
#include "OpEnumerateDataStores.h"
#include "OpGetSchemaMapping.h"
#include "OpGetFdoCacheInfo.h"
#include "OpBeginTransaction.h"
#include "OpCommitTransaction.h"
#include "OpRollbackTransaction.h"
#include "OpUpdateFeaturesWithTransaction.h"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// The default constructor for an MgFeatureOperationFactory object.  However, since
/// this function is protected, this object should never really be
/// constructed.  Rather, it is merely a wrapper class for other static
/// functions.
/// </summary>
MgFeatureOperationFactory::MgFeatureOperationFactory()
{
}


///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// This static method returns the IMgOperationHandler object that corresponds
/// to the given ID and Version parameters.
/// </summary>
/// <param name="operationId">
/// The ID of the requested operation.
/// </param>
/// <param name="operationVersion">
/// The version of the requested operation.
/// </param>
/// <returns>
/// Returns an IMgOperationHandler object corresponding to the given parameters.
/// Returns NULL if one cannot be found.
/// </returns>
/// <exceptions>
/// An MgException is thrown on failure.
/// </exceptions>
/// TODO:   handle different versions
/// TODO:   set up ids and whatnot in a hash or map instead of hardcoding the ids here
IMgOperationHandler* MgFeatureOperationFactory::GetOperation(
    ACE_UINT32 operationId, ACE_UINT32 operationVersion)
{
    auto_ptr<IMgOperationHandler> handler;

    MG_TRY()

    switch (operationId)
    {
    case MgFeatureServiceOpId::GetFeatureProviders_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetFeatureProviders());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetConnectionPropertyValues_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetConnectionPropertyValues());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::TestConnection_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpTestConnection());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::TestConnectionWithResource_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpTestFeatureSourceConnection());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetCapabilities_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
        case VERSION_SUPPORTED(2,0):
            handler.reset(new MgOpGetCapabilities());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::DescribeSchema_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpDescribeSchema());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::DescribeSchemaAsXml_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpDescribeSchemaAsXml());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetSchemas_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetSchemas());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetClasses_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetClasses());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetClassDefinition_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetClassDefinition());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::SelectFeatures_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSelectFeatures());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::SelectAggregate_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSelectFeaturesSpatial());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::ExecuteSqlQuery_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpExecuteSqlQuery());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::ExecuteSqlQueryWithTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpExecuteSqlQuery());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::ExecuteSqlNonQuery_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpExecuteSqlNonQuery());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::ExecuteSqlNonQueryWithTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpExecuteSqlNonQuery());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetSpatialContexts_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetSpatialContexts());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::UpdateFeatures_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpUpdateFeatures());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::UpdateFeaturesWithTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpUpdateFeaturesWithTransaction());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetFeatures_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetFeatures());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::CloseFeatureReader_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCloseFeatureReader());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetSqlRows_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetSqlRows());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::CloseSqlReader_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCloseSqlReader());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetDataRows_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetDataRows());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::CloseDataReader_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCloseDataReader());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetLongTransactions_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetLongTransactions());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::SetLongTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSetLongTransaction());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetRaster_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetRaster());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::SchemaToXml_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpSchemaToXml());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::XmlToSchema_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpXmlToSchema());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::CreateFeatureSource_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCreateFeatureSource());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetIdentityProperties_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(2,1):
            handler.reset(new MgOpGetIdentityProperties());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::DescribeWfsFeatureType_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
        case VERSION_SUPPORTED(2,3):
            handler.reset(new MgOpDescribeWfsFeatureType());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetWfsFeature_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
        case VERSION_SUPPORTED(2,3):
            handler.reset(new MgOpGetWfsFeature());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::EnumerateDataStores_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpEnumerateDataStores());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetSchemaMapping_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetSchemaMapping());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetFdoCacheInfo_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetFdoCacheInfo());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::GetClassDefinition2_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpGetClassDefinition());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::ApplySchema_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpApplySchema());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::BeginFeatureTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpBeginTransaction());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::CommitFeatureTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpCommitTransaction());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    case MgFeatureServiceOpId::RollbackFeatureTransaction_Id:
        switch (VERSION_NO_PHASE(operationVersion))
        {
        case VERSION_SUPPORTED(1,0):
            handler.reset(new MgOpRollbackTransaction());
            break;
        default:
            throw new MgInvalidOperationVersionException(
                L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        break;

    default:
        throw new MgInvalidOperationException(
            L"MgFeatureOperationFactory.GetOperation", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgFeatureOperationFactory.GetOperation")

    return handler.release();
}

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
#include "Services/FeatureService.h"
#include "Services/Feature/FeatureConnection.h"
#include "UpdateFeatures.h"
#include "Services/Feature/FeatureUtil.h"
#include "FeatureManipulationCommand.h"
#include "Services/Feature/FeatureServiceCache.h"
#include "Services/Transaction.h"

MgdUpdateFeaturesCommand::MgdUpdateFeaturesCommand()
{
    m_SrvrFeatConn = NULL;
}

MgdUpdateFeaturesCommand::~MgdUpdateFeaturesCommand()
{
}

void MgdUpdateFeaturesCommand::Connect(MgResourceIdentifier* resource, MgTransaction* transaction)
{
    if (NULL == transaction)
    {
        m_SrvrFeatConn = new MgdFeatureConnection(resource);
    }
    else
    {
        MgdTransaction* featTransaction = static_cast<MgdTransaction*>(transaction);
        m_SrvrFeatConn = featTransaction->GetConnection();
    }

    if ((NULL != m_SrvrFeatConn.p) && ( !m_SrvrFeatConn->IsConnectionOpen() ))
    {

        throw new MgdConnectionFailedException(L"MgdUpdateFeaturesCommand::MgdUpdateFeaturesCommand()",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

// Executes the commands
MgPropertyCollection* MgdUpdateFeaturesCommand::Execute(MgResourceIdentifier* resource,
                                                      MgFeatureCommandCollection* commands,
                                                      bool useTransaction)
{
    Ptr<MgPropertyCollection> propCol;
    FdoITransaction* transaction = NULL;
    bool commited = false;

    MG_FEATURE_SERVICE_TRY()

    if (resource == NULL || commands == NULL)
    {
        throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand.UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 cnt = commands->GetCount();
    if (cnt == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand.UpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);
    }

    // Connect to provider
    Connect(resource, NULL);

    propCol = new MgPropertyCollection();

    FdoPtr<FdoIConnection> fdoConn = m_SrvrFeatConn->GetConnection();
    if (useTransaction)
    {
        transaction = fdoConn->BeginTransaction();
    }

    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> result;
        Ptr<MgFeatureCommand> command = commands->GetItem(i);
        Ptr<MgdFeatureManipulationCommand> fmServerCommand = MgdFeatureManipulationCommand::CreateCommand(command, m_SrvrFeatConn, i);

        MG_FEATURE_SERVICE_TRY()
        // Execute the manipulation command
        result = fmServerCommand->Execute();

        MG_FEATURE_SERVICE_CATCH(L"MgdUpdateFeaturesCommand.UpdateFeatures")

        if (transaction != NULL)
        {
            MG_FEATURE_SERVICE_THROW() // rethrow if updates are done in transaction
        }
        else
        {
            if (mgException != NULL)
            {
                // When an exception is thrown, we need to communicate this back to user in non-transactional case.
                // We can do this either by setting warnings or a string property. Making it as StringProperty would
                // let users know exactly which ones failed and why.
                STRING str;
                MgUtil::Int32ToString(i, str);
                STRING errMsg = mgException->GetDetails();

                result = new MgStringProperty(str, errMsg);  // If there is an exception which means result would null from execute
                mgException = NULL; // Release the pointer
            }
        }
        // NULL property should not be added.
        if (result != NULL)
        {
            propCol->Add(result);
        }
    }

    if (transaction != NULL)
    {
        transaction->Commit();
        commited = true;
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH(resource, L"MgdUpdateFeaturesCommand.UpdateFeatures")

    if (transaction != NULL && !commited)
    {
        transaction->Rollback();
    }

    MG_FEATURE_SERVICE_THROW()

    return propCol.Detach();
}

// Executes the commands
MgPropertyCollection* MgdUpdateFeaturesCommand::Execute(MgResourceIdentifier* resource,
                                                      MgFeatureCommandCollection* commands,
                                                      MgTransaction* transaction)
{
    Ptr<MgPropertyCollection> propCol;

    MG_FEATURE_SERVICE_TRY()

    if (resource == NULL || commands == NULL)
    {
        throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand.UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 cnt = commands->GetCount();
    if (cnt == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand.UpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);
    }

    // Connect to provider
    Connect(resource, transaction);

    propCol = new MgPropertyCollection();

    for (INT32 i = 0; i < cnt; i++)
    {
        Ptr<MgProperty> result;
        Ptr<MgFeatureCommand> command = commands->GetItem(i);
        Ptr<MgdFeatureManipulationCommand> fmServerCommand = MgdFeatureManipulationCommand::CreateCommand(command, m_SrvrFeatConn, i);

        MG_FEATURE_SERVICE_TRY()
        // Execute the manipulation command
        result = fmServerCommand->Execute();

        MG_FEATURE_SERVICE_CATCH(L"MgdUpdateFeaturesCommand.UpdateFeatures")

        if (transaction != NULL)
        {
            MG_FEATURE_SERVICE_THROW() // rethrow if updates are done in transaction
        }
        else
        {
            if (mgException != NULL)
            {
                // When an exception is thrown, we need to communicate this back to user in non-transactional case.
                // We can do this either by setting warnings or a string property. Making it as StringProperty would
                // let users know exactly which ones failed and why.
                STRING str;
                MgUtil::Int32ToString(i, str);
                STRING errMsg = mgException->GetDetails();

                result = new MgStringProperty(str, errMsg);  // If there is an exception which means result would null from execute
                mgException = NULL; // Release the pointer
            }
        }
        // NULL property should not be added.
        if (result != NULL)
        {
            propCol->Add(result);
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdUpdateFeaturesCommand.UpdateFeatures")

    return propCol.Detach();
}

MgFeatureReader* MgdUpdateFeaturesCommand::ExecuteInsert(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, MgTransaction* trans)
{
    Ptr<MgFeatureReader> reader;

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdUpdateFeaturesCommand::ExecuteInsert");
	CHECKARGUMENTNULL(propertyValues, L"MgdUpdateFeaturesCommand::ExecuteInsert");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand::ExecuteInsert", __LINE__, __WFILE__, NULL, L"", NULL);
	
    Ptr<MgdFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand::ExecuteInsert", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgdFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIInsert> insert = (FdoIInsert*)conn->CreateCommand(FdoCommandType_Insert);
	
	insert->SetFeatureClassName(className.c_str());

	FdoPtr<FdoPropertyValueCollection> propVals = insert->GetPropertyValues();
	for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	{
		Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		FdoPtr<FdoPropertyValue> pv = MgdFeatureUtil::MgPropertyToFdoProperty(mgp);

		propVals->Add(pv);
	}

    if (NULL != fdoTrans.p)
        insert->SetTransaction(fdoTrans);

	FdoPtr<FdoIFeatureReader> insertRes = insert->Execute();

	reader = new MgdFeatureReader(connWrap, insertRes);

    MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdUpdateFeaturesCommand::ExecuteInsert", resource)

    return reader.Detach();
}


MgPropertyCollection* MgdUpdateFeaturesCommand::ExecuteInsert(MgResourceIdentifier* resource, CREFSTRING className, MgBatchPropertyCollection* batchPropertyValues, MgTransaction* trans)
{
    Ptr<MgPropertyCollection> ret;

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdUpdateFeaturesCommand::ExecuteInsert");
	CHECKARGUMENTNULL(batchPropertyValues, L"MgdUpdateFeaturesCommand::ExecuteInsert");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand::ExecuteInsert", __LINE__, __WFILE__, NULL, L"", NULL);
	
    ret = new MgPropertyCollection();

    Ptr<MgdFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand::ExecuteInsert", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {    
        connWrap = new MgdFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIInsert> insert = (FdoIInsert*)conn->CreateCommand(FdoCommandType_Insert);
	
	insert->SetFeatureClassName(className.c_str());

	FdoPtr<FdoPropertyValueCollection> propVals = insert->GetPropertyValues();
	
    if (NULL != fdoTrans.p)
        insert->SetTransaction(fdoTrans);

    //TODO: Support batch parameters, the main beneficiary of this API. Even then,
    //the value flipping approach employed here has performance benefits for certain
    //providers, like SQLite

    for (INT32 i = 0; i < batchPropertyValues->GetCount(); i++)
    {
        Ptr<MgPropertyCollection> propertyValues = batchPropertyValues->GetItem(i);

        //First feature, set up the FDO property value collection
        if (i == 0)
        {
            for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	        {
		        Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		        FdoPtr<FdoPropertyValue> pv = MgdFeatureUtil::MgPropertyToFdoProperty(mgp);

		        propVals->Add(pv);
	        }
        }
        else //Feature after the first
        {
            //Set all to null
            for (INT32 i = 0; i < propVals->GetCount(); i++)
            {
                FdoPtr<FdoPropertyValue> fp = propVals->GetItem(i);
                FdoPtr<FdoValueExpression> expr = fp->GetValue();
                FdoDataValue* fdv = dynamic_cast<FdoDataValue*>(expr.p);
                FdoGeometryValue* fgv = dynamic_cast<FdoGeometryValue*>(expr.p);
                if (fdv)
                {
                    fdv->SetNull();
                }
                else if (fgv)
                {
                    fgv->SetNullValue();
                }
            }

            //Now set the appropriate values. MgdFeatureUtil does the work
            for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	        {
                Ptr<MgNullableProperty> mgp = (MgNullableProperty*)propertyValues->GetItem(i);
                if (!mgp->IsNull())
                {
                    FdoPtr<FdoPropertyValue> fp = propVals->GetItem(mgp->GetName().c_str());
                    MgdFeatureUtil::UpdateFdoPropertyValue(fp, mgp);
                }
            }
        }

        STRING sIndex;
        MgUtil::Int32ToString(i, sIndex);

        //Insert and stash the result in the property collection
	    FdoPtr<FdoIFeatureReader> insertRes = insert->Execute();

	    Ptr<MgFeatureReader> fr = new MgdFeatureReader(connWrap, insertRes);
        Ptr<MgFeatureProperty> fp = new MgFeatureProperty(sIndex, fr);
        ret->Add(fp);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdUpdateFeaturesCommand::ExecuteInsert", resource)

    return ret.Detach();
}

int MgdUpdateFeaturesCommand::ExecuteUpdate(MgResourceIdentifier* resource, CREFSTRING className, MgPropertyCollection* propertyValues, CREFSTRING filter, MgTransaction* trans)
{
    int updated = 0;

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdUpdateFeaturesCommand::ExecuteUpdate");
	CHECKARGUMENTNULL(propertyValues, L"MgdUpdateFeaturesCommand::ExecuteUpdate");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand::ExecuteUpdate", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgdFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;
    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand::ExecuteUpdate", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {
        connWrap = new MgdFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIUpdate> update = (FdoIUpdate*)conn->CreateCommand(FdoCommandType_Update);
	update->SetFeatureClassName(className.c_str());
	
	if (!filter.empty())
		update->SetFilter(filter.c_str());

    if (NULL != fdoTrans.p)
        update->SetTransaction(fdoTrans);

	FdoPtr<FdoPropertyValueCollection> propVals = update->GetPropertyValues();
	for (INT32 i = 0; i < propertyValues->GetCount(); i++)
	{
		Ptr<MgProperty> mgp = propertyValues->GetItem(i);
		FdoPtr<FdoPropertyValue> pv = MgdFeatureUtil::MgPropertyToFdoProperty(mgp);

		propVals->Add(pv);
	}

	updated = update->Execute();

    MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdUpdateFeaturesCommand::ExecuteUpdate", resource)

    return updated;
}

int MgdUpdateFeaturesCommand::ExecuteDelete(MgResourceIdentifier* resource, CREFSTRING className, CREFSTRING filter, MgTransaction* trans)
{
    int deleted = 0;

    MG_FEATURE_SERVICE_TRY()

    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdUpdateFeaturesCommand::ExecuteDelete");
	if (className.empty())
		throw new MgNullArgumentException(L"MgdUpdateFeaturesCommand::ExecuteDelete", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgdFeatureConnection> connWrap;
	FdoPtr<FdoIConnection> conn;
    FdoPtr<FdoITransaction> fdoTrans;

    Ptr<MgdTransaction> mgTrans = dynamic_cast<MgdTransaction*>(trans);
    if (NULL != mgTrans)
    {
        SAFE_ADDREF(mgTrans.p);
        Ptr<MgResourceIdentifier> origFeatureSource = mgTrans->GetFeatureSource();
        //Check that the transaction originates from the same feature source
        if (origFeatureSource->ToString() != resource->ToString())
            throw new MgInvalidArgumentException(L"MgdUpdateFeaturesCommand::ExecuteDelete", __LINE__, __WFILE__, NULL, L"", NULL);

        connWrap = mgTrans->GetConnection(); //Connection is already open
        fdoTrans = mgTrans->GetFdoTransaction();
    }
    else
    {
        connWrap = new MgdFeatureConnection(resource);
    }

    conn = connWrap->GetConnection();
	FdoPtr<FdoIDelete> fdoDelete = (FdoIDelete*)conn->CreateCommand(FdoCommandType_Delete);
	fdoDelete->SetFeatureClassName(className.c_str());
    if (!filter.empty())
	    fdoDelete->SetFilter(filter.c_str());
    
    if (NULL != fdoTrans.p)
        fdoDelete->SetTransaction(fdoTrans);

	deleted = fdoDelete->Execute();

    MG_FEATURE_SERVICE_CATCH_AND_THROW_WITH_FEATURE_SOURCE(L"MgdUpdateFeaturesCommand::ExecuteDelete", resource)

    return deleted;
}

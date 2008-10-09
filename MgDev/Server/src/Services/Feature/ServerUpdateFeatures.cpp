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
#include "Services/FeatureService.h"
#include "ServerFeatureConnection.h"
#include "ServerUpdateFeatures.h"
#include "ServerFeatureUtil.h"
#include "FeatureManipulationCommand.h"
#include "CacheManager.h"

MgServerUpdateFeatures::MgServerUpdateFeatures()
{
    m_SrvrFeatConn = NULL;
}

MgServerUpdateFeatures::~MgServerUpdateFeatures()
{
}

void MgServerUpdateFeatures::Connect(MgResourceIdentifier* resource)
{
    m_SrvrFeatConn = new MgServerFeatureConnection(resource);
    if ( !m_SrvrFeatConn->IsConnectionOpen() )
    {

        throw new MgConnectionFailedException(L"MgServerUpdateFeatures::MgServerUpdateFeatures()",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

// Executes the commands
MgPropertyCollection* MgServerUpdateFeatures::Execute(MgResourceIdentifier* resource,
                                                      MgFeatureCommandCollection* commands,
                                                      bool useTransaction)
{
    Ptr<MgPropertyCollection> propCol;
    FdoITransaction* transaction = NULL;
    bool commited = false;

    MG_FEATURE_SERVICE_TRY()

    if (resource == NULL || commands == NULL)
    {
        throw new MgNullArgumentException(L"MgServerUpdateFeatures.UpdateFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    INT32 cnt = commands->GetCount();
    if (cnt == 0)
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(L"0");

        throw new MgInvalidArgumentException(L"MgServerUpdateFeatures.UpdateFeatures",
            __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);
    }

    // Connect to provider
    Connect(resource);

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
        Ptr<MgFeatureManipulationCommand> fmServerCommand = MgFeatureManipulationCommand::CreateCommand(command, m_SrvrFeatConn, i);

        MG_FEATURE_SERVICE_TRY()
        // Execute the manipulation command
        result = fmServerCommand->Execute();

        MG_FEATURE_SERVICE_CATCH(L"MgServerUpdateFeatures.UpdateFeatures")

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

    MG_FEATURE_SERVICE_CATCH(L"MgServerUpdateFeatures.UpdateFeatures")

    if (transaction != NULL && !commited)
    {
        transaction->Rollback();
    }

    MG_FEATURE_SERVICE_THROW()

    return propCol.Detach();
}

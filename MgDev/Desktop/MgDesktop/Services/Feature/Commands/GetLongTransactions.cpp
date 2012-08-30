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
#include "GetLongTransactions.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/Feature/FeatureUtil.h"
#include <math.h>

MgdGetLongTransactions::MgdGetLongTransactions()
{
}

MgdGetLongTransactions::~MgdGetLongTransactions()
{
}

// Executes the get long transactions command and serializes the schema to XML
MgLongTransactionReader* MgdGetLongTransactions::GetLongTransactions(MgResourceIdentifier* resId, bool bActiveOnly)
{
    Ptr<MgLongTransactionReader> mgLongTransactionReader;
    mgLongTransactionReader = NULL;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resId)
    {
        throw new MgNullArgumentException(L"MgdGetLongTransactions.GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Connect to provider
    Ptr<MgdFeatureConnection> msfc = new MgdFeatureConnection(resId);

    // connection must be open to retrieve list of active contexts
    if ((NULL != msfc.p) && ( msfc->IsConnectionOpen() ))
    {
        // The reference to the FDO connection from the MgdFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConn = msfc->GetConnection();
        m_providerName = msfc->GetProviderName();

        // Check whether command is supported by provider
        if (!msfc->SupportsCommand((INT32)FdoCommandType_GetLongTransactions))
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgdFeatureUtil::GetMessage(L"MgCommandNotSupported");
            throw new MgInvalidOperationException(L"MgdGetLongTransactions.GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIGetLongTransactions> fdoCommand = (FdoIGetLongTransactions*)fdoConn->CreateCommand(FdoCommandType_GetLongTransactions);
        CHECKNULL((FdoIGetLongTransactions*)fdoCommand, L"MgdGetLongTransactions.GetLongTransactions");

        // Execute the command
        FdoPtr<FdoILongTransactionReader> longTransactionReader = fdoCommand->Execute();
        CHECKNULL((FdoILongTransactionReader*)longTransactionReader, L"MgdGetLongTransactions.GetLongTransactions");

        mgLongTransactionReader = new MgLongTransactionReader();
        while (longTransactionReader->ReadNext())
        {
            // If only active long transaction is required skip all others
            if (bActiveOnly)
            {
                if (!longTransactionReader->IsActive())
                    continue;
            }

            // Set providername for which long transaction reader is executed
            mgLongTransactionReader->SetProviderName(m_providerName);

            // Add transaction data to the long transaction reader
            Ptr<MgLongTransactionData> longTransactionData = GetLongTransactionData(longTransactionReader);
            CHECKNULL((MgLongTransactionData*)longTransactionData, L"MgdGetLongTransactions.GetLongTransactions");
            mgLongTransactionReader->AddLongTransactionData(longTransactionData);

            // If only active long transaction is required skip all others
            if (bActiveOnly)
            {
                if (longTransactionReader->IsActive())
                    break;
            }
        }
    }
    else
    {
        throw new MgdConnectionFailedException(L"MgdGetLongTransactions::GetLongTransactions()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resId, L"MgdGetLongTransactions.GetLongTransactions")

    return mgLongTransactionReader.Detach();
}

MgLongTransactionData* MgdGetLongTransactions::GetLongTransactionData(FdoILongTransactionReader* longTransactionReader)
{
    Ptr<MgLongTransactionData> longTransactionData = new MgLongTransactionData();

    // Name must exist
    FdoString* name = longTransactionReader->GetName();
    CHECKNULL((FdoString*)name, L"MgdGetLongTransactions.GetLongTransactions");
    longTransactionData->SetName(STRING(name));

    // Desc for long transaction
    FdoString* desc = longTransactionReader->GetDescription();
    if (desc != NULL)
    {
        longTransactionData->SetDescription(STRING(desc));
    }

    // Owner for long transaction
    FdoString* owner = longTransactionReader->GetOwner();
    if (owner != NULL)
    {
        longTransactionData->SetOwner(STRING(owner));
    }

    // Creation date
    Ptr<MgDateTime> mgDateTime;

    FdoDateTime dateTime = longTransactionReader->GetCreationDate();
    if(dateTime.IsDateTime())
    {
        // Valid datetime
        double seconds;
        double microsecs = modf(dateTime.seconds, &seconds);

        mgDateTime = new MgDateTime(dateTime.year, dateTime.month, dateTime.day, dateTime.hour, dateTime.minute, (INT8)(int)seconds, (INT32)microsecs);
    }
    else
    {
        // Invalid datetime, so use current date
        mgDateTime = new MgDateTime();
    }

    // Whether it is active or not
    bool isActive = longTransactionReader->IsActive();
    longTransactionData->SetActiveStatus(isActive);

    // Whether it is frozen or not
    bool isFrozen = longTransactionReader->IsFrozen();
    longTransactionData->SetFrozenStatus(isFrozen);

    return longTransactionData.Detach();
}

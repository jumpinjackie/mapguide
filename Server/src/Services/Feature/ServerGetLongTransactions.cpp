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
#include "ServerGetLongTransactions.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"
#include <math.h>

#define CHECKOPEN(methodName) \
    if (!IsConnectionOpen())  \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \

MgServerGetLongTransactions::MgServerGetLongTransactions()
{
}

MgServerGetLongTransactions::~MgServerGetLongTransactions()
{
}

// Executes the describe schema command and serializes the schema to XML

MgLongTransactionReader* MgServerGetLongTransactions::GetLongTransactions(MgResourceIdentifier* resId, bool bActiveOnly)
{
    Ptr<MgLongTransactionReader> mgLongTransactionReader;
    mgLongTransactionReader = NULL;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resId)
    {
        throw new MgNullArgumentException(L"MgServerGetLongTransactions.GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Connect to provider
    MgServerFeatureConnection msfc(resId);

    // connection must be open to retrieve list of active contexts
    if ( !msfc.IsConnectionOpen() )
    {
        throw new MgConnectionFailedException(L"MgServerGetLongTransactions::GetLongTransactions()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoPtr<FdoIConnection> fdoConn = msfc.GetConnection();
    m_providerName = msfc.GetProviderName();

    // Check whether command is supported by provider
    if (!msfc.SupportsCommand((INT32)FdoCommandType_GetLongTransactions))
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgCommandNotSupported");
        throw new MgInvalidOperationException(L"MgServerGetLongTransactions.GetLongTransactions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoPtr<FdoIGetLongTransactions> fdoCommand = (FdoIGetLongTransactions*)fdoConn->CreateCommand(FdoCommandType_GetLongTransactions);
    CHECKNULL((FdoIGetLongTransactions*)fdoCommand, L"MgServerGetLongTransactions.GetLongTransactions");

    // Execute the command
    FdoPtr<FdoILongTransactionReader> longTransactionReader = fdoCommand->Execute();
    CHECKNULL((FdoILongTransactionReader*)longTransactionReader, L"MgServerGetLongTransactions.GetLongTransactions");

    mgLongTransactionReader = new MgLongTransactionReader();
    while (longTransactionReader->ReadNext())
    {
        // If only active spatial context is required skip all others
        if (bActiveOnly)
        {
            if (!longTransactionReader->IsActive())
                continue;
        }

        // Set providername for which spatial reader is executed
        mgLongTransactionReader->SetProviderName(m_providerName);

        Ptr<MgLongTransactionData> longTransactionData = GetLongTransactionData(longTransactionReader);
        CHECKNULL((MgLongTransactionData*)longTransactionData, L"MgServerGetLongTransactions.GetLongTransactions");
        // Add spatial data to the spatialcontext reader
        mgLongTransactionReader->AddLongTransactionData(longTransactionData);

        // If only active spatial context is required skip all others
        if (bActiveOnly)
        {
            if (longTransactionReader->IsActive())
                break;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetLongTransactions.GetLongTransactions")

    return mgLongTransactionReader.Detach();
}

MgLongTransactionData* MgServerGetLongTransactions::GetLongTransactionData(FdoILongTransactionReader* longTransactionReader)
{
    Ptr<MgLongTransactionData> longTransactionData = new MgLongTransactionData();

    // Name must exist
    FdoString* name = longTransactionReader->GetName();
    CHECKNULL((FdoString*)name, L"MgServerGetLongTransactions.GetLongTransactions");
    longTransactionData->SetName(STRING(name));

    // Desc for spatial context
    FdoString* desc = longTransactionReader->GetDescription();
    if (desc != NULL)
    {
        longTransactionData->SetDescription(STRING(desc));
    }

    // Desc for spatial context
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

    // Whether it is active or not
    bool isFrozen = longTransactionReader->IsFrozen();
    longTransactionData->SetFrozenStatus(isFrozen);

    return longTransactionData.Detach();
}

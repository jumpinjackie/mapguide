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
#include "MapGuideCommon.h"
#include "Services/Transaction.h"

MgServerFeatureTransaction::MgServerFeatureTransaction(MgResourceIdentifier* resource)
{
    MG_FEATURE_SERVICE_TRY()

    Initialize(resource);

    m_fdoTransaction = m_fdoConnection->BeginTransaction();
    m_bIsClosed      = false;
    m_lastUsed       = ACE_OS::gettimeofday();

    // The transaction takes ownership of the FDO connection
    m_connection->HasReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureTransaction.MgServerFeatureTransaction")
}

MgServerFeatureTransaction::~MgServerFeatureTransaction()
{
    Close();
}

void MgServerFeatureTransaction::Commit()
{
    if (m_bIsClosed) // Has already been closed through Commit, Rollback or SetTimeout.
    {
        return;
    }

    CHECKNULL(m_fdoTransaction, L"MgServerFeatureTransaction.Commit")

    m_fdoTransaction->Commit();

    // close it as this transaction object is obsolete now. 
    Close();
}

void MgServerFeatureTransaction::Rollback()
{
    if (m_bIsClosed) // Has already been closed through Commit, Rollback or SetTimeout.
    {
        return;
    }

    CHECKNULL(m_fdoTransaction, L"MgServerFeatureTransaction.Rollback")

    m_fdoTransaction->Rollback();

    // close it as this transaction object is obsolete now. 
    Close();
}

void MgServerFeatureTransaction::Serialize(MgStream* stream)
{
    bool operationCompleted = false;
    STRING transcationId = L"";

    MG_FEATURE_SERVICE_TRY()

    MgServerFeatureTransactionPool* featTransactionPool = MgServerFeatureTransactionPool::GetInstance();
    CHECKNULL(featTransactionPool, L"MgServerFeatureTransaction.Serialize");

    transcationId = featTransactionPool->GetTransactionId(this);
    // Add transaction instance to pool
    if (L"" == transcationId)
    {
        transcationId = featTransactionPool->AddTransaction(this); // Add the reference
    }

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureTransaction.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteObject((MgResourceIdentifier*)m_resourceId);
        stream->WriteString(transcationId); // Write the transaction ID so we can retrieve it for later use
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }

    MG_FEATURE_SERVICE_THROW();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerFeatureTransaction::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerFeatureTransaction.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

MgServerFeatureConnection* MgServerFeatureTransaction::GetServerFeatureConnection()
{
    return SAFE_ADDREF(m_connection.p);
}

void MgServerFeatureTransaction::SetTimeout()
{
    CHECKNULL(m_fdoTransaction, L"MgServerFeatureTransaction.SetTimeout")

    // Rollback the transaction in the case of timeout.
    m_fdoTransaction->Rollback();

    Close();
}

void MgServerFeatureTransaction::Initialize(MgResourceIdentifier* resource)
{
    if (resource == NULL)
    {
        throw new MgNullArgumentException(L"MgServerFeatureTransaction.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_resourceId     = SAFE_ADDREF(resource);
    m_connection     = new MgServerFeatureConnection(resource);

    if (m_connection->IsConnectionOpen())
    {
        m_fdoConnection  = m_connection->GetConnection();

        FdoPtr<FdoIConnectionCapabilities> connCapability = m_fdoConnection->GetConnectionCapabilities();
        if (NULL == connCapability.p || !connCapability->SupportsTransactions())
        {
            // TODO: specify which argument and message, once we have the mechanism
            STRING message = MgServerFeatureUtil::GetMessage(L"MgTransactionNotSupported");
            throw new MgInvalidOperationException(L"MgServerFeatureTransaction.Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerFeatureTransaction::Initialize", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

void MgServerFeatureTransaction::Close()
{
    if (m_bIsClosed) // Has already been closed.
    {
        return;
    }

    MG_FEATURE_SERVICE_TRY()

    m_fdoTransaction = NULL;

    // Release the connection.
    m_connection = NULL;

    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        fdoConnectionManager->Close(m_fdoConnection);
    }

    m_fdoConnection = NULL;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureTransaction.Close")

    m_bIsClosed = true;
}

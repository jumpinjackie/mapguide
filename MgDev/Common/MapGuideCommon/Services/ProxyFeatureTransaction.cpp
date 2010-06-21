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

#include "MapGuideCommon.h"

MG_IMPL_DYNCREATE(MgProxyFeatureTransaction);

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgProxyFeatureTransaction object
///</summary>
///
MgProxyFeatureTransaction::MgProxyFeatureTransaction(MgResourceIdentifier* resource)
{
    m_resourceId    = SAFE_ADDREF(resource);
    m_transactionId = L"";
    m_service       = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgProxyFeatureTransaction object
///</summary>
///
MgProxyFeatureTransaction::MgProxyFeatureTransaction()
{
    m_resourceId    = NULL;
    m_transactionId = L"";
    m_service       = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct an MgProxyFeatureTransaction object
///</summary>
///
MgProxyFeatureTransaction::~MgProxyFeatureTransaction()
{
    SAFE_RELEASE(m_service);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Commit the transaction
///</summary>

void MgProxyFeatureTransaction::Commit()
{
    CHECKNULL(m_service, L"MgProxyFeatureTransaction.Commit");
    m_service->CommitTransaction(m_transactionId);
}

void MgProxyFeatureTransaction::Rollback()
{
    CHECKNULL(m_service, L"MgProxyFeatureTransaction.Rollback");
    m_service->RollbackTransaction(m_transactionId);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgProxyFeatureTransaction::Serialize(MgStream* stream)
{
    bool operationCompleted = true;
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->WriteObject((MgResourceIdentifier*)m_resourceId);
        stream->WriteString(m_transactionId); // Get the transaction ID so we can use it
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgProxyFeatureTransaction::Deserialize(MgStream* stream)
{
    bool operationCompleted = false;

    stream->GetBoolean(operationCompleted);

    if (operationCompleted)
    {
        m_resourceId = (MgResourceIdentifier*)stream->GetObject();
        stream->GetString(m_transactionId); // Get the transaction ID so we can commit or rollback it
    }
    else
    {
        MgException* exp = (MgException*)stream->GetObject();
        exp->Raise();
    }
}

void MgProxyFeatureTransaction::SetService(MgFeatureService* service)
{
    CHECKNULL(service, L"MgProxyFeatureTransaction.SetService");

    if (m_service == NULL)
    {
        m_service = SAFE_ADDREF(service);
    }
}

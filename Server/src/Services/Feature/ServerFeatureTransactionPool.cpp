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

#include "MapGuideCommon.h"
#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureTransactionPool.h"
#include "ServerFeatureTransaction.h"

// Process-wide MgServerFeatureTransactionPool
Ptr<MgServerFeatureTransactionPool> MgServerFeatureTransactionPool::sm_featureTransactionPool =
    (MgServerFeatureTransactionPool*)NULL;

ACE_Recursive_Thread_Mutex MgServerFeatureTransactionPool::sm_mutex;

/// <summary>
/// Constructor
/// </summary>
MgServerFeatureTransactionPool::MgServerFeatureTransactionPool():m_transactionTimeout(0)
{
}

/// <summary>
/// Destructor
/// </summary>
MgServerFeatureTransactionPool::~MgServerFeatureTransactionPool()
{
    for (FeatureTransactionCollection::iterator iterator = m_featureTransactions.begin(); iterator != m_featureTransactions.end(); iterator++)
    {
        MgServerFeatureTransaction* featTransaction = iterator->second;
        if (NULL != featTransaction)
        {
            SAFE_RELEASE(featTransaction);
        }
    }

    m_featureTransactions.clear();
}

void MgServerFeatureTransactionPool::Dispose()
{
    delete this;
}

void MgServerFeatureTransactionPool::Initialize(INT32 transactionTimeout)
{
    m_transactionTimeout = transactionTimeout;
}


void MgServerFeatureTransactionPool::RemoveExpiredTransaction()
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex));

    MG_CONFIGURATION_TRY()

    ACE_Time_Value now = ACE_OS::gettimeofday();
    FeatureTransactionCollection::iterator iterator = m_featureTransactions.begin();
    while (iterator != m_featureTransactions.end())
    {
        MgServerFeatureTransaction* featTransaction = iterator->second;
        if (NULL != featTransaction)
        {
            INT64 time = now.sec() - featTransaction->LastUsed().sec();
            if (time > m_transactionTimeout)
            {
                try
                {
                    // Set timeout for this transaction object. It will release the internal FdoITransaction object.
                    featTransaction->SetTimeout();
                }
                catch (FdoException* e)
                {
                    FDO_SAFE_RELEASE(e);
                }
                catch (...)
                {
                }

                featTransaction->Release();

                // Add the id to timeout list
                m_transactionTimeoutIds.push_back(iterator->first);

                m_featureTransactions.erase(iterator++);
            }
            else
            {
                ++iterator;
            }
        }
        else
        {
            assert(false);
            ++iterator;
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerFeatureTransactionPool.RemoveExpiredTransaction")
}

/// <summary>
/// Get pointer to a process-wide MgServerFeatureTransactionPool.
/// </summary>
MgServerFeatureTransactionPool* MgServerFeatureTransactionPool::GetInstance()
{
    MG_CONFIGURATION_TRY()

    ACE_TRACE ("MgServerFeatureTransactionPool::GetInstance");

    if (MgServerFeatureTransactionPool::sm_featureTransactionPool == NULL)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (MgServerFeatureTransactionPool::sm_featureTransactionPool == NULL)
        {
            MgServerFeatureTransactionPool::sm_featureTransactionPool = new MgServerFeatureTransactionPool();
        }
    }

    MG_CONFIGURATION_CATCH_AND_THROW(L"MgServerFeatureTransactionPool.GetInstance")

    // To avoid overheads and maintain thread safety,
    // do not assign this returned static singleton to a Ptr object.
    return MgServerFeatureTransactionPool::sm_featureTransactionPool;
}

MgServerFeatureTransaction* MgServerFeatureTransactionPool::CreateTransaction(MgResourceIdentifier* resource)
{
    Ptr<MgServerFeatureTransaction> tran = new MgServerFeatureTransaction(resource);

    // Add it to the pool.
    AddTransaction(tran);

    return tran.Detach();
}

bool MgServerFeatureTransactionPool::CommitTransaction(CREFSTRING transactionId)
{
    ValidateTimeout(transactionId);

    Ptr<MgServerFeatureTransaction> tran = GetTransaction(transactionId);
    if (NULL != tran.p)
    {
        tran->Commit();
    }

    return RemoveTransaction(transactionId);
}

bool MgServerFeatureTransactionPool::RollbackTransaction(CREFSTRING transactionId)
{
    Ptr<MgServerFeatureTransaction> tran = GetTransaction(transactionId);
    if (NULL != tran.p)
    {
        tran->Rollback();
    }

    return RemoveTransaction(transactionId);
}

STRING MgServerFeatureTransactionPool::AddTransaction(MgServerFeatureTransaction* featTransaction)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    if(NULL == featTransaction)
    {
        throw new MgNullArgumentException(L"MgServerFeatureTransactionPool.AddTransaction",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Get a unique ID
    STRING uid = L"";
    MgUtil::GenerateUuid(uid);

    // Increment the reference count
    featTransaction->AddRef();

    // Add it to the collection
    m_featureTransactions.insert(FeatureTransactionCacheEntry_Pair(uid, featTransaction));

    return uid;
}

bool MgServerFeatureTransactionPool::RemoveTransaction(CREFSTRING transactionId)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    bool bResult = false;

    FeatureTransactionCollection::iterator iterator = m_featureTransactions.find(transactionId);
    if(m_featureTransactions.end() != iterator)
    {
        // Release resources
        MgServerFeatureTransaction* featTransaction = iterator->second;
        if(NULL != featTransaction)
        {
            featTransaction->Release();
        }

        // Remove the transaction object
        m_featureTransactions.erase(iterator);
        bResult = true;
    }

    return bResult;
}

bool MgServerFeatureTransactionPool::RemoveTransaction(MgServerFeatureTransaction* featTransaction)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, false));

    if(NULL == featTransaction)
    {
        throw new MgNullArgumentException(L"MgServerFeatureTransactionPool.RemoveTransaction",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool bResult = false;

    for (FeatureTransactionCollection::iterator iterator = m_featureTransactions.begin(); iterator != m_featureTransactions.end(); iterator++)
    {
        if (iterator->second == featTransaction)
        {
            SAFE_RELEASE(iterator->second);

            m_featureTransactions.erase(iterator);
            bResult = true;
            break;
        }
    }

    return bResult;
}

MgServerFeatureTransaction* MgServerFeatureTransactionPool::GetTransaction(CREFSTRING transactionId)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, NULL));

    MgServerFeatureTransaction* featTransaction = NULL;

    FeatureTransactionCollection::iterator iterator = m_featureTransactions.find(transactionId);
    if (m_featureTransactions.end() != iterator)
    {
        featTransaction = iterator->second;
        SAFE_ADDREF(featTransaction);
    }

    return featTransaction;
}

STRING MgServerFeatureTransactionPool::GetTransactionId(MgServerFeatureTransaction* featTransaction)
{
    ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, sm_mutex, L""));

    STRING transactionId = L"";

    for (FeatureTransactionCollection::iterator iterator = m_featureTransactions.begin();iterator != m_featureTransactions.end(); iterator++)
    {
        if(iterator->second == featTransaction)
        {
            // Found it
            transactionId = iterator->first;
            break;
        }
    }

    return transactionId;
}

void MgServerFeatureTransactionPool::ValidateTimeout(CREFSTRING transcationId)
{
    if (transcationId.empty())
    {
        return;
    }

    bool bTimeout = false;
    for (TransactionIdCollection::iterator iter = m_transactionTimeoutIds.begin(); iter != m_transactionTimeoutIds.end(); ++iter)
    {
        if (iter->compare(transcationId) == 0)
        {
            bTimeout = true;
            break;
        }
    }

    if (bTimeout)
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgTransactionTimeout");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerFeatureTransactionPool.ValidateTimeout", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

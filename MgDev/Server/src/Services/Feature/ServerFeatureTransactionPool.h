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

#ifndef _MG_SERVER_FEATURE_TRANSACTION_POOL_H_
#define _MG_SERVER_FEATURE_TRANSACTION_POOL_H_

#include "ServerFeatureDllExport.h"

class MgServerFeatureTransaction;

typedef std::map<STRING, MgServerFeatureTransaction*> FeatureTransactionCollection;
typedef std::pair<STRING, MgServerFeatureTransaction*> FeatureTransactionCacheEntry_Pair;
typedef std::vector<STRING> TransactionIdCollection;

class MG_SERVER_FEATURE_API MgServerFeatureTransactionPool : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgServerFeatureTransactionPool)

public:
    virtual ~MgServerFeatureTransactionPool();

    /// <summary>
    /// Self Destructor
    /// </summary>

    virtual void Dispose();

    /// <summary>
    /// Get pointer to a process-wide MgServerFeatureTransactionPool.
    /// </summary>

    static MgServerFeatureTransactionPool* GetInstance();

    void Initialize(INT32 transactionTimeout);

    MgServerFeatureTransaction* CreateTransaction(MgResourceIdentifier* resource);

    bool CommitTransaction(CREFSTRING transactionId);

    bool RollbackTransaction(CREFSTRING transactionId);

    void RemoveExpiredTransaction();

    STRING AddTransaction(MgServerFeatureTransaction* featTransaction);

    bool RemoveTransaction(CREFSTRING transactionId);

    bool RemoveTransaction(MgServerFeatureTransaction* featTransaction);

    MgServerFeatureTransaction* GetTransaction(CREFSTRING transactionId);

    STRING GetTransactionId(MgServerFeatureTransaction* featTransaction);

    void ValidateTimeout(CREFSTRING transactionId);

private:

    MgServerFeatureTransactionPool();

    /// Pointer to a process-wide singleton.
    static Ptr<MgServerFeatureTransactionPool> sm_featureTransactionPool;
    static ACE_Recursive_Thread_Mutex  sm_mutex;

    FeatureTransactionCollection m_featureTransactions;

    TransactionIdCollection      m_transactionTimeoutIds;
    INT32                        m_transactionTimeout;

};

#endif //_MG_SERVER_FEATURE_TRANSACTION_POOL_H_

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

#ifndef _MG_SERVER_FEATURE_TRANSACTION_H_
#define _MG_SERVER_FEATURE_TRANSACTION_H_

#include "ServerFeatureDllExport.h"

/////////////////////////////////////////////////////////////////
/// <summary>
/// MgServerFeatureTransaction represents a transaction to be performed in
/// a DataStore. If the transaction is time out, commit or rollback a
/// transaction will result in one exception MgFeatureServiceException thrown.
/// </summary>
class MG_SERVER_FEATURE_API MgServerFeatureTransaction : public MgTransaction
{
    DECLARE_CLASSNAME(MgServerFeatureTransaction)

EXTERNAL_API:

    /// \brief
    /// Commit the transaction.
    ///
    virtual void Commit();

    /// \brief
    /// Rollback the transaction.
    ///
    virtual void Rollback();

    /// \brief
    /// Get the identifier of feature source associated with the transaction.
    ///
    /// \return
    /// Returns the identifier of feature source associated with the transaction.
    ///
    virtual MgResourceIdentifier* GetFeatureSource()
    {
        return SAFE_ADDREF(m_resourceId.p);
    }

protected:

    virtual void Dispose()
    {
        delete this;
    }

INTERNAL_API:

    ACE_Time_Value& LastUsed()
    {
        return m_lastUsed;
    }

    void Deserialize(MgStream* stream);
    void Serialize(MgStream* stream);

    MgServerFeatureConnection* GetServerFeatureConnection();

    void SetTimeout();

    MgServerFeatureTransaction(MgResourceIdentifier* resource);
    ~MgServerFeatureTransaction();

private:

    void Initialize(MgResourceIdentifier* resource);
    void Close();

    Ptr<MgResourceIdentifier>      m_resourceId;
    Ptr<MgServerFeatureConnection> m_connection;
    FdoPtr<FdoIConnection>         m_fdoConnection;
    FdoPtr<FdoITransaction>        m_fdoTransaction;

    bool                           m_bIsClosed;
    ACE_Time_Value                 m_lastUsed;
};

#endif

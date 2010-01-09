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

#ifndef _MG_PROXY_FEATURE_TRANSACTION_H
#define _MG_PROXY_FEATURE_TRANSACTION_H

class MgTransaction;
class MgResourceIdentifier;
class MgFeatureService;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// <summary>
/// MgProxyFeatureTransaction represents a transaction to be performed in 
/// a DataStore. If the transaction is time out, commit or rollback a 
/// transaction will result in one exception MgFeatureServiceException thrown.
/// </summary>
class MG_MAPGUIDE_API  MgProxyFeatureTransaction : public MgTransaction
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgProxyFeatureTransaction)

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

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    virtual void SetService(MgFeatureService* service);

    virtual void Dispose()
    {
        delete this;
    }

    STRING GetTransactionId()
    {
        return m_transactionId;
    }

    MgProxyFeatureTransaction(MgResourceIdentifier* resource);
    MgProxyFeatureTransaction();
    ~MgProxyFeatureTransaction();

private:

    Ptr<MgResourceIdentifier> m_resourceId;
    STRING                    m_transactionId;
    MgFeatureService*         m_service; // Do not want to implement a default constructor
};
/// \endcond

#endif

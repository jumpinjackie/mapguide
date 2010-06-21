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

#ifndef TRANSACTION_H
#define TRANSACTION_H

/////////////////////////////////////////////////////////////////
/// \brief
/// MgTransaction represents a transaction to be performed in a DataStore.
/// If the transaction is time out, commit or rollback a transaction will
/// result in one exception MgFeatureServiceException thrown.
class MG_PLATFORMBASE_API MgTransaction : public MgSerializable
{
PUBLISHED_API:
    /// \brief
    /// Commit the transaction.
    ///
    virtual void Commit() = 0;

    /// \brief
    /// Rollback the transaction.
    ///
    virtual void Rollback() = 0;

    /// \brief
    /// Get the identifier of feature source associated with the transaction.
    ///
    /// \return
    /// Returns the identifier of feature source associated with the transaction.
    ///
    virtual MgResourceIdentifier* GetFeatureSource() = 0;

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor
    ///
    MgTransaction();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor. This will clean up the transaction.
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgTransaction();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureTransaction;
};

#endif

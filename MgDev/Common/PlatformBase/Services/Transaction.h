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

#ifndef TRANSACTION_H
#define TRANSACTION_H

/////////////////////////////////////////////////////////////////
/// \brief
/// MgTransaction represents a transaction to be performed in a DataStore.
/// If the transaction is time out, commit or rollback a transaction will
/// result in one exception MgFeatureServiceException thrown.
///
/// \since 2.2
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

    /// \brief
    ///  Create a save point in this transaction.
    /// 
    /// \param suggestName
    /// Suggested save point name.
    /// 
    /// \return
    /// The actual name used
    ///
    /// \since 2.4
    virtual STRING AddSavePoint(CREFSTRING suggestName) = 0;

    /// \brief
    ///  Release a specific save point.
    /// 
    /// \param savePointName 
    /// Save point name.
    /// 
    /// \return
    /// Returns nothing
    ///
    /// \since 2.4
    virtual void ReleaseSavePoint(CREFSTRING savePointName) = 0;
    
    /// \brief
    ///  Rollback the transaction to a specified save point.
    /// 
    /// \param savePointName 
    /// Save point name.
    /// 
    /// \return
    /// Returns nothing 
    ///
    /// \since 2.4
    virtual void Rollback(CREFSTRING savePointName) = 0;

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

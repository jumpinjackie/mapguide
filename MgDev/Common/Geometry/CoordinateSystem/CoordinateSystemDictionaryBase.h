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

#ifndef _MGCOORDINATESYSTEMDICTIONARYBASE_H_
#define _MGCOORDINATESYSTEMDICTIONARYBASE_H_

/// \defgroup MgCoordinateSystemDictionaryBase MgCoordinateSystemDictionaryBase
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Basic functionality for all kinds of dictionaries
///

class MgCoordinateSystemDictionaryBase : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemDictionaryBase)

PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the associated coordinate system catalog
    ///
    /// \return
    /// The associated coordinate system catalog
    ///
    virtual MgCoordinateSystemCatalog* GetCatalog()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the default file name
    ///
    /// \return
    /// The default file name
    ///
    virtual STRING GetDefaultFileName()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the file name of this dictionary 
    ///
    /// \return
    /// The file name of this dictionary
    ///
    virtual STRING GetFileName()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the file name of this dictionary
    ///
    /// \param sFileName (String/string)
    /// The file name
    ///
    virtual void SetFileName(CREFSTRING sFileName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the path of this dictionary
    ///
    /// \return
    /// The path of this dictionary
    ///
    virtual STRING GetPath()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the size of this dictionary
    ///
    /// \return
    /// The size of this dictionary
    ///
    virtual UINT32 GetSize()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Adds the specified definition
    ///
    /// \param pDefinition (MgGuardDisposable)
    /// The definition to add.
    ///
    /// \remarks
    /// Refer to the derived class to determine the type of definition that can be added. For example, for a
    /// MgCoordinateSystemDatumDictionary, the allowed type of definition is MgCoordinateSystemDatum
    ///
    virtual void Add(MgGuardDisposable *pDefinition)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Removes a definition by its specified name
    ///
    /// \param sName (String/string)
    /// The name of the definition to remove
    ///
    virtual void Remove(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Modifies the specified definition
    ///
    /// \param pDefinition (MgGuardDisposable)
    /// The definition to modify.
    ///
    /// \remarks
    /// Refer to the derived class to determine the type of definition that can be modified. For example, for a
    /// MgCoordinateSystemDatumDictionary, the allowed type of definition is MgCoordinateSystemDatum
    ///
    virtual void Modify(MgGuardDisposable *pDefinition)=0;

    virtual MgGuardDisposable* Get(CREFSTRING sName)=0; // TODO - this needs to be removed!

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the definition of the specified name exists in this dictionary
    ///
    /// \return
    /// true if the definition exists. false otherwise
    ///
    virtual bool Has(CREFSTRING sName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the dictionary enumerator
    ///
    /// \return
    /// The dictionary enumerator
    ///
    virtual MgCoordinateSystemEnum* GetEnum()=0;
};
/// \}

#endif //_MGCOORDINATESYSTEMDICTIONARYBASE_H_

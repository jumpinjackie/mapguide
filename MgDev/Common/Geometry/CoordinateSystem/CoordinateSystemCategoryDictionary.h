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

#ifndef _MGCOORDINATESYSTEMCATEGORYDICTIONARY_H_
#define _MGCOORDINATESYSTEMCATEGORYDICTIONARY_H_

/// \defgroup MgCoordinateSystemCategoryDictionary MgCoordinateSystemCategoryDictionary
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Basic functionality for the coordinate system category dictionary
///

class MgCoordinateSystemCategoryDictionary : public MgCoordinateSystemDictionaryBase
{
    DECLARE_CLASSNAME(MgCoordinateSystemCategoryDictionary)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new category
    ///
    /// \return
    /// The coordinate system category
    ///
    virtual MgCoordinateSystemCategory* NewCategory()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Renames the given category name to new specified category name
    ///
    /// \param sOldName (String/string)
    /// The old category
    /// \param sNewName (String/string)
    /// The new category
    /// 
    virtual void Rename(CREFSTRING sOldName, CREFSTRING sNewName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the category of the specified name
    ///
    /// \param sName (String/string)
    /// The category
    ///
    /// \return
    /// The coordinate system category or NULL if not found
    /// 
    virtual MgCoordinateSystemCategory* GetCategory(CREFSTRING sName)=0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemCategoryDictionary;
};
/// \}

#endif //_MGCOORDINATESYSTEMCATEGORYDICTIONARY_H_

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

#ifndef _MGCOORDINATESYSTEMDICTIONARYUTILITY_H_
#define _MGCOORDINATESYSTEMDICTIONARYUTILITY_H_

/// \defgroup MgCoordinateSystemDictionaryUtility MgCoordinateSystemDictionaryUtility
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Basic functionality for all kinds of dictionaries
///

class MgCoordinateSystemDictionaryUtility : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemDictionaryUtility)

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
    /// Creates the ellipsoid dictionary in the specified path
    ///
    /// \param sDictPathName (String/string)
    /// The path to create the dictionary in
    ///
    virtual void CreateEllipsoidDictionary(CREFSTRING sDictPathName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates the datum dictionary in the specified path
    ///
    /// \param sDictPathName (String/string)
    /// The path to create the dictionary in
    ///
    virtual void CreateDatumDictionary(CREFSTRING sDictPathName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates the coordinate system dictionary in the specified path
    ///
    /// \param sDictPathName (String/string)
    /// The path to create the dictionary in
    ///
    virtual void CreateCoordsysDictionary(CREFSTRING sDictPathName)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates the category dictionary in the specified path
    ///
    /// \param sDictPathName (String/string)
    /// The path to create the dictionary in
    ///
    virtual void CreateCategoryDictionary(CREFSTRING sDictPathName)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemDictionaryUtility;
};
/// \}

#endif //_MGCOORDINATESYSTEMDICTIONARYUTILITY_H_

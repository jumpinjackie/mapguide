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

#ifndef _MGCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_
#define _MGCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_

/// \defgroup MgCoordinateSystemGeodeticPathDictionary MgCoordinateSystemGeodeticPathDictionary
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// The MgCoordinateSystemGeodeticPathDictionary provides access to all geodetic transformation path
/// definitions as defined in the dictionary file this instance reads from / writes to.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticPathDictionary : public MgCoordinateSystemDictionaryBase
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticPathDictionary)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new, empty geodetic path object that has to be filled by an API client. This
    /// method does not create a new entry in the dictionary. The caller is responsible
    /// for disposing the object being returned.
    ///
    /// \return
    /// Return a new, in-memory geodetic path object. No content is written to disk
    /// when this method executes.
    ///
    virtual MgCoordinateSystemGeodeticPath* NewGeodeticPath()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the geodetic path object where the name is equal to the parameter
    /// pathName. The caller is responsible for disposing the object.
    ///
    /// \param pathName
    /// The name of the geodetic path to look up in the dictionary. Must not be an empty string.
    ///
    /// \return
    /// Return the geodetic path if found. Otherwise NULL.
    ///
    virtual MgCoordinateSystemGeodeticPath* GetGeodeticPath(CREFSTRING pathName)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticPathDictionary;
};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICPATHDICTIONARY_H_

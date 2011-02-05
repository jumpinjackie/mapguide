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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_


/// \defgroup MgCoordinateSystemGeodeticTransformDefDictionary MgCoordinateSystemGeodeticTransformDefDictionary
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// The MgCoordinateSystemGeodeticTransformDefDictionary provides access to all geodetic transformation
/// definitions as defined in the dictionary file this instance reads from / writes to.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformDefDictionary : public MgCoordinateSystemDictionaryBase
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticTransformDefDictionary)

PUBLISHED_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a new, empty geodetic transformation definition object that has to be filled by an API client. This
    /// method does not create a new entry in the dictionary. The caller is responsible
    /// for disposing the object being returned.
    ///
    /// \param transformationDefType
    /// The type of the geodetic transformation to create. This must be a constant value as defined
    /// in MgCoordinateSystemGeodeticTransformDefType (except for None)
    ///
    /// \return
    /// Return a new, in-memory geodetic transformation definition object. No content is written to disk
    /// when this method executes.
    ///
    virtual MgCoordinateSystemGeodeticTransformDef* NewGeodeticTransformationDef(INT32 transformationDefType) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the geodetic transformation definition object where the name is equal to the parameter
    /// pathName. The caller is responsible for disposing the object.
    ///
    /// \param transformationName
    /// The name of the geodetic transformation to look up in the dictionary. Must not be an empty string.
    ///
    /// \return
    /// Return the geodetic transformation if found. Otherwise NULL.
    ///
    virtual MgCoordinateSystemGeodeticTransformDef* GetGeodeticTransformationDef(CREFSTRING transformationName)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticTransformDefDictionary;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMDEFDICTIONARY_H_

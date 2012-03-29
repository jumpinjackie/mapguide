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

#ifndef _MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMDEFPARAMS_H_
#define _MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMDEFPARAMS_H_

/// \defgroup MgCoordinateSystemGeodeticStandaloneTransformDefParams MgCoordinateSystemGeodeticStandaloneTransformDefParams
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// This class (indirectly) describes the parameters of a geodetic transformation method that's built into the
/// transformation engine. Hence, it does not allow for setting any parameters. Only the well known code
/// can be set/get via (Set/Get)TransformationMethod.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticStandaloneTransformDefParams : public MgCoordinateSystemGeodeticTransformDefParams
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticStandaloneTransformDefParams)

PUBLISHED_API:

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the code of the well known geodetic transformation method. Can only
    /// be one of the constants defined in MgCoordinateSystemGeodeticStandaloneTransformationMethod (except for None).
    ///
    /// \return
    /// Returns the code of the well known geodetic transformation method.
    virtual INT32 GetTransformationMethod() = 0; /// __get, __set
    
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the code of the well-known geodetic transformation method. Can only
    /// be one of the constants defined in MgCoordinateSystemGeodeticStandaloneTransformationMethod (except for None).
    virtual void SetTransformationMethod(INT32 builtInTransformationMethod) = 0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId() { return m_cls_id; };

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticStandaloneTransformDefParams;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMDEFPARAMS_H_

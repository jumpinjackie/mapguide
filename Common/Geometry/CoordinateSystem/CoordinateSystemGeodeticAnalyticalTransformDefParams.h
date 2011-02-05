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

#ifndef _MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMDEFPARAMS_H_
#define _MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMDEFPARAMS_H_

/// \defgroup MgCoordinateSystemGeodeticAnalyticalTransformDefParams MgCoordinateSystemGeodeticAnalyticalTransformDefParams
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// This class holds all parameters that are specific to analytical geodetic transformation methods, e.g.
/// geocentric transformation, molodensky etc.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticAnalyticalTransformDefParams : public MgCoordinateSystemGeodeticTransformDefParams
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticAnalyticalTransformDefParams)

PUBLISHED_API:

    //BEGIN: struct csGeodeticXformParmsGeocentric
    virtual double GetDeltaX() = 0;
    virtual void SetDeltaX(double deltaX) = 0;

    virtual double GetDeltaY() = 0;
    virtual void SetDeltaY(double deltaY) = 0;

    virtual double GetDeltaZ() = 0;
    virtual void SetDeltaZ(double deltaZ) = 0;

    virtual double GetRotateX() = 0;
    virtual void SetRotateX(double rotateX) = 0;

    virtual double GetRotateY() = 0;
    virtual void SetRotateY(double rotateY) = 0;

    virtual double GetRotateZ() = 0;
    virtual void SetRotateZ(double rotateZ) = 0;

    virtual double GetScale() = 0;
    virtual void SetScale(double scale) = 0;

    virtual double GetTranslateX() = 0;
    virtual void SetTranslateX(double translateX) = 0;

    virtual double GetTranslateY() = 0;
    virtual void SetTranslateY(double translateY) = 0;

    virtual double GetTranslateZ() = 0;
    virtual void SetTranslateZ(double translateZ) = 0;

    //END: struct csGeodeticXformParmsGeocentric

    virtual INT32 GetTransformationMethod() = 0;
    virtual void SetTransformationMethod(INT32 analyticalMethodCode) = 0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticAnalyticalTransformDefParams;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICANALYTICALTRANSFORMDEFPARAMS_H_

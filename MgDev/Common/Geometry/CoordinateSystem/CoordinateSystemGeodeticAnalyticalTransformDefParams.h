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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the X delta of this instance
    ///
    /// \return
    /// The X delta of this instance
    ///
    virtual double GetDeltaX() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the X delta of this instance
    ///
    /// \param deltaX (double)
    /// The X delta
    ///
    virtual void SetDeltaX(double deltaX) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Y delta of this instance
    ///
    /// \return
    /// The Y delta of this instance
    ///
    virtual double GetDeltaY() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Y delta of this instance
    ///
    /// \param deltaY (double)
    /// The Y delta
    ///
    virtual void SetDeltaY(double deltaY) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Z delta of this instance
    ///
    /// \return
    /// The Z delta of this instance
    ///
    virtual double GetDeltaZ() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Z delta of this instance
    ///
    /// \param deltaZ (double)
    /// The Z delta
    ///
    virtual void SetDeltaZ(double deltaZ) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the X rotation of this instance
    ///
    /// \return
    /// The X rotation of this instance
    ///
    virtual double GetRotateX() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the X rotation of this instance
    ///
    /// \param rotateX (double)
    /// The X rotation
    ///
    virtual void SetRotateX(double rotateX) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Y rotation of this instance
    ///
    /// \return
    /// The Y rotation of this instance
    ///
    virtual double GetRotateY() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Y rotation of this instance
    ///
    /// \param rotateY (double)
    /// The Y rotation
    ///
    virtual void SetRotateY(double rotateY) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Z rotation of this instance
    ///
    /// \return
    /// The Z rotation of this instance
    ///
    virtual double GetRotateZ() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Z rotation of this instance
    ///
    /// \param rotateZ (double)
    /// The Z rotation
    ///
    virtual void SetRotateZ(double rotateZ) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the scale of this instance
    ///
    /// \return
    /// The scale of this instance
    ///
    virtual double GetScale() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the scale of this instance
    ///
    /// \param scale (double)
    /// The scale
    ///
    virtual void SetScale(double scale) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the X translation of this instance
    ///
    /// \return
    /// The X translation of this instance
    ///
    virtual double GetTranslateX() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the X translation of this instance
    ///
    /// \param translateX (double)
    /// The X translation
    ///
    virtual void SetTranslateX(double translateX) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Y translation of this instance
    ///
    /// \return
    /// The Y translation of this instance
    ///
    virtual double GetTranslateY() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Y translation of this instance
    ///
    /// \param translateY (double)
    /// The Y translation
    ///
    virtual void SetTranslateY(double translateY) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Z translation of this instance
    ///
    /// \return
    /// The Z translation of this instance
    ///
    virtual double GetTranslateZ() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the Z translation of this instance
    ///
    /// \param translateZ (double)
    /// The Z translation
    ///
    virtual void SetTranslateZ(double translateZ) = 0;

    //END: struct csGeodeticXformParmsGeocentric

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the transformation method of this instance
    ///
    /// \return
    /// The transformation method of this instance
    ///
    virtual INT32 GetTransformationMethod() = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the transformation method of this instance
    ///
    /// \param analyticalMethodCode (int)
    /// The transformation method
    ///
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

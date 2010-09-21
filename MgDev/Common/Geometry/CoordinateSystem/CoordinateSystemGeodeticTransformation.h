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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMATION_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMATION_H_

/// \defgroup MgCoordinateSystemGeodeticTransformation MgCoordinateSystemGeodeticTransformation
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// An instance of this class allow a caller do perform a geodetic transformation (datum shift).
/// That is, this class does not operate on coordinate system level, but rather on 
/// datum level.
/// 
class MgCoordinateSystemGeodeticTransformation : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticTransformation)

PUBLISHED_API:
    virtual void SetSourceAndTarget(MgCoordinateSystemDatum *pSource, MgCoordinateSystemDatum *pTarget)=0;
    virtual MgCoordinateSystemDatum* GetSource()=0;
    virtual MgCoordinateSystemDatum* GetTarget()=0;

    virtual void Shift(MgCoordinate* pLonLat)=0;
    virtual MgCoordinate* Shift(double dLongitude, double dLatitude)=0;
    virtual MgCoordinate* Shift(double dLongitude, double dLatitude, double dZ)=0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetOffsetX()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetOffsetY()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetOffsetZ()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual void SetOffset(double x, double y, double z)=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBursaWolfeTransformRotationX()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBursaWolfeTransformRotationY()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBursaWolfeTransformRotationZ()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBursaWolfeTransformBwScale()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual void SetBursaWolfeTransform(double dRotationX, double dRotationY, double dRotationZ, double dBwScale)=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual INT32 GetGeodeticTransformationMethod()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual void SetGeodeticTransformationMethod(INT32 nGeodeticTransformationMethod)=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetMaxOffset()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual bool IsLegalOffset(double dOffset)=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetMaxRotation()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual bool IsLegalRotation(double dRotation)=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBwScaleMin()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual double GetBwScaleMax()=0;
    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Obsolete. Use MgCoordinateSystemGeodeticTransformDef and
    /// MgCoordinateSystemGeodeticTransformDefParams (plus derived classes) instead.
    virtual bool IsLegalBwScale(double dBwScale)=0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemGeodeticTransformation;
};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMATION_H_

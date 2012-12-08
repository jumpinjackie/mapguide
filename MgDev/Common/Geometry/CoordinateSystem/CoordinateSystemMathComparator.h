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

#ifndef _MGCOORDINATESYSTEMMATHCOMPARATOR_H_
#define _MGCOORDINATESYSTEMMATHCOMPARATOR_H_

class MgCoordinateSystem;
class MgCoordinateSystemDatum;
class MgCoordinateSystemEllipsoid;

/////////////////////////////////////////////////////////////
/// \brief
/// A mathematical comparator used to detemerine equality between
/// two different coordinate systems, datums or ellipsoids
///
class MgCoordinateSystemMathComparator : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemMathComparator)

PUBLISHED_API:
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the two given definitions are the same
    ///
    /// \param pDefinition1 (MgGuardDisposable)
    /// The first definition to compare
    /// \param pDefinition2 (MgGuardDisposable)
    /// The second definiton to compare
    ///
    /// \return
    /// true if both definitions are the same. false otherwise
    ///
    virtual bool Same(MgGuardDisposable *pDefinition1, MgGuardDisposable *pDefinition2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the two given ellipsoids are the same
    ///
    /// \param pDefinition1 (MgCoordinateSystemEllipsoid)
    /// The first definition to compare
    /// \param pDefinition2 (MgCoordinateSystemEllipsoid)
    /// The second definiton to compare
    ///
    /// \return
    /// true if both definitions are the same. false otherwise
    ///
    virtual bool SameEllipsoid(MgCoordinateSystemEllipsoid *pDefinition1, MgCoordinateSystemEllipsoid *pDefinition2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the two given datums are the same
    ///
    /// \param pDefinition1 (MgCoordinateSystemDatum)
    /// The first definition to compare
    /// \param pDefinition2 (MgCoordinateSystemDatum)
    /// The second definiton to compare
    ///
    /// \return
    /// true if both definitions are the same. false otherwise
    ///
    virtual bool SameDatum(MgCoordinateSystemDatum *pDefinition1, MgCoordinateSystemDatum *pDefinition2)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the two given coordinate system are the same
    ///
    /// \param pDefinition1 (MgCoordinateSystem)
    /// The first definition to compare
    /// \param pDefinition2 (MgCoordinateSystem)
    /// The second definiton to compare
    ///
    /// \return
    /// true if both definitions are the same. false otherwise
    ///
    virtual bool SameCoordinateSystem(MgCoordinateSystem *pDefinition1, MgCoordinateSystem *pDefinition2)=0;

    /////////////////////////////////////////////////////////////
    /// \brief
    /// Disables or enables the comparison of any integral geodetic transformation information
    /// when comparing datums. By default, this class compares any geodetic transformation
    /// information it finds. If set to false, the MgCoordinateSystemMathComparator::SameDatum method 
    /// only compares ellipsoid information and any other, non-transformation related, properties.
    /// \since 2.3
    virtual void SetCompareInternalDatumOldParameters(bool) = 0;

    /////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the current comparison behavior for datums of this MgCoordinateSystemMathComparator
    /// object. If true (default), this comparator compares geodetic transformation information
    /// it might find for a datum object. If false, this comparison step is skipped.
    /// \since 2.3
    virtual bool GetCompareInternalDatumOldParameters() = 0;

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
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemMathComparator;
};

#endif //_MGCOORDINATESYSTEMMATHCOMPARATOR_H_

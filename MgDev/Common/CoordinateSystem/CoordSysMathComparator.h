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

#ifndef _CCOORDINATESYSTEMMATHCOMPARATOR_H_
#define _CCOORDINATESYSTEMMATHCOMPARATOR_H_

namespace CSLibrary
{

class CCoordinateSystemMathComparator : public MgCoordinateSystemMathComparator
{
    // tolerances for floating-point comparisons
    static const double kdEpsilonGeographic;      // latitude & longitude
    static const double kdEpsilonAzimuth;         // Azimuths
    static const double kdEpsilonLinear;          // radii, false origins,
                                                  // datum deltas, geoid heights,
                                                  // elevations, etc.
    static const double kdEpsilonArcSec;          // Geocentric rotations
    static const double kdEpsilonDatumScale;      // Datum shift scale (ppm)
    static const double kdEpsilonScale;           // scale reduction, units scale
    static const double kdEpsilonCoefficient;     // complex/affine coefficient
    static const double kdEpsilon;                // everything else
public:
    CCoordinateSystemMathComparator();
    virtual ~CCoordinateSystemMathComparator();

    virtual bool Same(MgGuardDisposable *pDefinition1, MgGuardDisposable *pDefinition2);
    virtual bool SameEllipsoid(MgCoordinateSystemEllipsoid *pDefinition1, MgCoordinateSystemEllipsoid *pDefinition2);
    virtual bool SameDatum(MgCoordinateSystemDatum *pDefinition1, MgCoordinateSystemDatum *pDefinition2);
    virtual bool SameCoordinateSystem(MgCoordinateSystem *pDefinition1, MgCoordinateSystem *pDefinition2);

    virtual void SetCompareInternalDatumOldParameters(bool);
    virtual bool GetCompareInternalDatumOldParameters();

protected:
//    INT32 GetClassId();
    //from MgDisposable
    virtual void Dispose();

private:
    bool m_bCompareDatumOldParameters;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMATHCOMPARATOR_H_

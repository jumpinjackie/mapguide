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

#ifndef _CCOORDINATESYSTEMMGRSZONE_H_
#define _CCOORDINATESYSTEMMGRSZONE_H_

namespace CSLibrary
{

//=============================================================================
// MGRS grids can cross UTM zone boundaries, which means that an MGRS grid may
// consist of multiple generic grids.  Thus, we have an object named
// CCoordinateSystemMgrsZone which represents a single zone of a multiple
// zone MGRS grid.  The higher level CCoordinateSystemMgrs object carries a
// collection of these objects.
//
// In this object:
// * UtmZone is an integer value indicating the UTM zone where positive means
//   northern hemisphere, negative means southern hemisphere, and 61 means
//   a polr region (i.e. UPS).  Zero indicates not set yet or invalid.
// * GridCsCode is the key name of the grid coordinate system which will be
//   used to generate the grid.  This definition will be datum dependent.
// * ZoneBoundary starts off being an envelope of the entire UTM zone, and
//   is eventually reduced to be that portion of the Viewport boundary which
//   is covered by this grid object.
// * If useFrameDatum is true, the Grid coordinate system will be adjusted
//   to use the datum of the provided frame coordinate system.  Otherwise,
//   the Grid datum is hardcoded to WGS84.
// * Is used only to generate projected grids.  Graticules are handled
//   separately.  Thus, the grid coordinate system is always a projected
//   coordinate system.
class CCoordinateSystemMgrsZone : public CCoordinateSystemOneGrid
{
public:
    CCoordinateSystemMgrsZone (MgCoordinateSystemGridBoundary* frameBoundary,
                               INT32 utmZoneNbr,
                               bool useFrameDatum,
                               MgCoordinateSystem* frameCS,
                               INT8 letteringScheme,
                               INT64 gridLineMemoryThreshold,
                               INT64 gridTickMemoryThreshold,
                               INT64 gridRegionMemoryThreshold);
    ~CCoordinateSystemMgrsZone (void);

    CCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridBoundary* frameBoundary,
                                                       MgCoordinateSystemGridSpecification* specification);
    CCoordinateSystemGridLineCollection* GetGraticuleLines (MgCoordinateSystemGridBoundary* frameBoundary,
                                                            MgCoordinateSystemGridSpecification* specification);
    CCoordinateSystemGridRegionCollection* GetGridRegions (MgCoordinateSystemGridBoundary* frameBoundary,
                                                           MgCoordinateSystemGridSpecification* specification);
    INT32 ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification);
    INT32 GetUtmZoneNbr (void);

    void ResetGridRegionMemoryThreshold(INT64 memThreshold);

protected:
    CCoordinateSystemGridRegionCollection* BuildRegionCollection (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                  MgCoordinateSystemGridSpecification* specification);
private:
    void BuildMajorRegions (CCoordinateSystemGridRegionCollection* regionCollection,
                            MgCoordinateSystemGridBoundary* frameBoundary,
                            double boundaryPrecision);
    void BuildMinorRegions (CCoordinateSystemGridRegionCollection* regionCollection,
                            MgCoordinateSystemGridBoundary* frameBoundary,
                            double boundaryPrecision);
    ///////////////////////////////////////////////////////////////////////////
    // Data members
    // m_UtmZoneNbr is positive for northern hemisphere, negative for the
    // southern hemisphere.  Polar regions are assigned the value of 61.
    // Zero is the uninitialized/unknown/error value.
    INT32 m_UtmZone;
    INT8 m_LetteringScheme;
    INT64 m_GridRegionMemoryThreshold;
//    Ptr<CCoordinateSystemGridRegionCollection> m_RegionCollection;

    // Not implemented
    CCoordinateSystemMgrsZone (const CCoordinateSystemMgrsZone& source);
    CCoordinateSystemMgrsZone& operator= (const CCoordinateSystemMgrsZone& rhs);
};

class CCoordinateSystemMgrsZoneCollection : public MgGuardDisposable
{
public:
    CCoordinateSystemMgrsZoneCollection (void);
    ~CCoordinateSystemMgrsZoneCollection (void);

    INT32 GetCount () const;
    CCoordinateSystemMgrsZone* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear();
    void SetItem (INT32 index, CCoordinateSystemMgrsZone* value);
    void Add (CCoordinateSystemMgrsZone* value);

protected:
    void Dispose (void);

private:
    // Data Members
    Ptr<MgDisposableCollection> m_OneGridCollection;

    // Member functions not implemented.
    CCoordinateSystemMgrsZoneCollection  (const CCoordinateSystemMgrsZoneCollection & source);
    CCoordinateSystemMgrsZoneCollection& operator= (const CCoordinateSystemMgrsZoneCollection & rhs);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMGRSZONE_H_

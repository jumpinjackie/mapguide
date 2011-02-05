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

#ifndef _CCOORDINATESYSTEMGRIDBASE_H_
#define _CCOORDINATESYSTEMGRIDBASE_H_

namespace CSLibrary
{

///////////////////////////////////////////////////////////////////////////////
// CCoordinateSystemOneGrid
//
// An object which encapsulates the functionality of a single grid; a single
// grid being a grid based on one grid coordinate system and one viewport
// coordinate system.
//
// While a generic grid will always consist of a single instance of an object
// of this type, an MGRS grid (for example) may require several instances of
// this object, using different grid coordinate systems, to accurately
// represent the actual overall grid.  Thus, we encapsulate most basic grid
// functionality in this object.
//
// The object includes a STRING data member which can carry a label which users
// may use at their convenience.  This object does nothing with the label
// other than enable the user to set it and retrieve it.

class CCoordinateSystemOneGrid : public MgGuardDisposable
{
public:
    static const INT32 MaxCurvePoints;
    CCoordinateSystemOneGrid (INT64 gridLineMemoryThreshold,
                              INT64 gridTickMemoryThreshold);
    CCoordinateSystemOneGrid (MgCoordinateSystemGridBoundary* frameBoundary,
                              MgCoordinateSystem* gridCRS,
                              MgCoordinateSystem* frameCRS,
                              INT64 gridLineMemoryThreshold,
                              INT64 gridTickMemoryThreshold);
    ~CCoordinateSystemOneGrid (void);

    void SetUp (MgCoordinateSystemGridBoundary* frameBoundary,
                MgCoordinateSystem* gridCRS,
                MgCoordinateSystem* frameCRS);

    // Returns true if the grid coordinate system is geographic; use this to
    // determine if this is a graticule (as opposed to a grid).
    bool GridFrameCrsAreTheSame ();
    bool IsGeographic (void);
    INT32 GetUserID (void);
    STRING GetLabel (void);
    void SetUserID (INT32 userID);
    void SetLabel (CREFSTRING label);

    MgCoordinate* ConvertToGrid (MgCoordinate* frameCoordinate);
    MgCoordinate* ConvertToFrame (MgCoordinate* gridCoordinate);
    MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specs);
    CCoordinateSystemGridTickCollection* GetBoundaryTicks (MgCoordinateSystemGridSpecification* specs);

    INT32 ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification);
    INT32 ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification);

    void ResetGridLineMemoryThreshold(INT64 memThreshold);
    void ResetGridTickMemoryThreshold(INT64 memThreshold);

protected:
    MgCoordinateSystemGridBoundary* GetFrameBoundary (void);
    MgCoordinateSystemGridBoundary* GetGridBoundary (void);
    MgCoordinateSystem* GetFrameCRS (void);
    MgCoordinateSystem* GetGridCRS (void);
    MgCoordinateSystemTransform* GetGridToFrameXform (void);
    MgCoordinateSystemTransform* GetFrameToGridXform (void);
    void GenerateGridBoundary (double boundaryPrecision);
    void GetGeographicExtents (double& longMin,double& longMax,double& latMin,double& latMax,double precision = 1.0E-05);
    void GetGridExtents (double& eastMin,double& eastMax,double& northMin,double& northMax,double precision = 0.25);
    void Dispose (void);

    bool m_GridFrameCrsSame;
    INT32 m_UserID;                                      // For user convenience (i.e. UTM zone)
    INT32 m_MaxCurvePoints;
    STRING m_Label;                                      // For user conveinence (i.e. MGRS)
    INT64 m_GridLineMemoryThreshold;
    INT64 m_GridTickMemoryThreshold;
    Ptr<MgCoordinateSystem> m_GridCRS;                   // The grid coordinate system
    Ptr<MgCoordinateSystem> m_FrameCRS;                  // The frame coordinate system
    Ptr<MgCoordinateSystemTransform> m_ToFrameXform;     // Converts grid coordinates to frame coordinates
    Ptr<MgCoordinateSystemTransform> m_ToGridXform;      // Converts frame coordinates to grid coordinates
    double m_BoundaryPrecision;                          // Precision at which the boundary conversion was performed
    Ptr<MgCoordinateSystemGridBoundary> m_FrameBoundary; // Grid boundary in frame coordinates
    Ptr<MgCoordinateSystemGridBoundary> m_GridBoundary;  // Grid boundary in grid coordinates

private:
    // Not implemented
    CCoordinateSystemOneGrid (const CCoordinateSystemOneGrid& source);
    CCoordinateSystemOneGrid& operator= (const CCoordinateSystemOneGrid& rhs);
};

}   /* namespace CSLibrary */

#endif /* _CCOORDINATESYSTEMGRIDBASE_H_ */

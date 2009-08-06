//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
class CCoordinateSystemOneGrid
{
public:
	CCoordinateSystemOneGrid (MgCoordinateSystemGridBoundary* frameBoundary,
	                          MgCoordinateSystem* gridCRS,
	                          MgCoordinateSystem* frameCRS);
	~CCoordinateSystemOneGrid (void);

    STRING GetLabel (void);
    void SetLabel (CREFSTRING label);
    MgCoordinate* ConvertToGrid (MgCoordinate* frameCoordinate);
    MgCoordinate* ConvertToFrame (MgCoordinate* gridCoordinate);
    MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specs);
    MgCoordinateSystemGridTickCollection* GetBoundaryTicks (MgCoordinateSystemGridSpecification* specs);
private:
    STRING m_RegionLabel;                                // For user conveinence
    Ptr<MgCoordinateSystem> m_GridCRS;                   // The grid coordinate system
    Ptr<MgCoordinateSystem> m_FrameCRS;                  // The frame coordinate system
    Ptr<MgCoordinateSystemTransform> m_ToFrameXform;     // Converts grid coordinates to frame coordinates
    Ptr<MgCoordinateSystemTransform> m_ToGridXform;      // Converts frame coordinates to grid coordinates
    double m_BoundaryPrecision;                          // Precision at which the boundary conversion was perfromed
    Ptr<MgCoordinateSystemGridBoundary> m_FrameBoundary; // Grid boundary in frame coordinates
    Ptr<MgCoordinateSystemGridBoundary> m_GridBoundary;  // Grid boundary in grid coordinates

    // Not implemented
    CCoordinateSystemOneGrid (const CCoordinateSystemOneGrid& source);
    CCoordinateSystemOneGrid& operator= (const CCoordinateSystemOneGrid& rhs);
};

}   /* namespace CSLibrary */

#endif /* _CCOORDINATESYSTEMGRIDBASE_H_ */

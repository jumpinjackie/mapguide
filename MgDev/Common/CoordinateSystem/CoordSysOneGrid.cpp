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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CoordSysUtil.h"
#include "CriticalSection.h"

#include "CoordSys.h"
#include "CoordSysTransform.h"              //for CCoordinateSystemTransform
#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"

using namespace CSLibrary;

//=============================================================================
// CCoordinateSystemOneGrid

CCoordinateSystemOneGrid::CCoordinateSystemOneGrid (MgCoordinateSystemGridBoundary* frameBoundary,
                                                    MgCoordinateSystem* gridCRS,
                                                    MgCoordinateSystem* frameCRS)
                                                        :
                                                    m_RegionLabel       (),
                                                    m_GridCRS           (),
                                                    m_FrameCRS          (),
                                                    m_ToFrameXform      (),
                                                    m_ToGridXform       (),
                                                    m_BoundaryPrecision (0.0),
                                                    m_GridBoundary      (),
                                                    m_FrameBoundary     ()
{
    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();

    m_FrameBoundary = SAFE_ADDREF (frameBoundary);
    m_GridCRS = SAFE_ADDREF (gridCRS);
    m_FrameCRS = SAFE_ADDREF (frameCRS);
    m_ToFrameXform = csFactory->GetTransform (m_GridCRS,m_FrameCRS);
    m_ToGridXform = csFactory->GetTransform (m_FrameCRS,m_GridCRS);
}
CCoordinateSystemOneGrid::~CCoordinateSystemOneGrid (void)
{
}
STRING CCoordinateSystemOneGrid::GetLabel (void)
{
    return m_RegionLabel;
}
void CCoordinateSystemOneGrid::SetLabel (CREFSTRING label)
{
    m_RegionLabel = label;
}
MgCoordinate* CCoordinateSystemOneGrid::ConvertToGrid (MgCoordinate* frameCoordinate)
{
    return m_ToGridXform->Transform (frameCoordinate);
}
MgCoordinate* CCoordinateSystemOneGrid::ConvertToFrame (MgCoordinate* gridCoordinate)
{
    return m_ToFrameXform->Transform (gridCoordinate);
}
MgCoordinateSystemGridLineCollection* CCoordinateSystemOneGrid::GetGridLines (MgCoordinateSystemGridSpecification* specs)
{
    bool generateGridBoundary;

    double value;
    double delta;
    double increment;
    double precision;
    double eastMin, eastMax;
    double northMin, northMax;

    Ptr<MgCoordinate> coordinate;
    Ptr<MgCoordinate> fromPnt;
    Ptr<MgCoordinate> toPnt;
    Ptr<MgLineString> lineString;
    Ptr<MgLineStringCollection> lineStringCollection;
    Ptr<CCoordinateSystemGridLine> gridLine;

    Ptr<CCoordinateSystemGridLineCollection> gridLineCollection = new CCoordinateSystemGridLineCollection ();

    MG_TRY()
        coordinate = new MgCoordinateXY ();
        fromPnt    = new MgCoordinateXY ();
        toPnt      = new MgCoordinateXY ();

        // If the boundary has not been converted yet, or if the precision
        // value has been changed, reproduce the m_GridBoundary
        // member from the m_FrameBoundary member.  At this point, we
        // need the curve precision value in Grid system units.
        generateGridBoundary = (m_GridBoundary == NULL);
        if (!generateGridBoundary)
        {
            // This gets the user specifiation.
            precision = specs->GetCurvePrecision ();
            generateGridBoundary = (fabs (precision - m_BoundaryPrecision) < 1.0E-12);
        }
        
        if (generateGridBoundary)
        {
            precision = specs->GetCurvePrecision (m_GridCRS);
            m_GridBoundary = new CCoordinateSystemGridBoundary ();
            m_GridBoundary->SetBoundaryExtents (m_FrameBoundary->GetBoundary(m_ToGridXform.p,precision));
        }

        // Get the extents of the frame boundary, and then convert them to
        // grid coordinates.  Doing this here _should_ elimiate the need to
        // arbitrarily expand the grid boundary.
        m_GridBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);
        
        // Adjust so the the grid limits are exact values with regard to the
        // specified increments.  Do this in an expansive way so the the grid
        // limits always get larger, never smaller.
        increment = specs->GetEastingIncrement();
        delta = fabs(fmod (eastMin,increment));
        eastMin -= (eastMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (eastMax,increment));
        eastMax += (eastMax >= 0.0) ? (increment - delta) : delta;
 
        increment = specs->GetNorthingIncrement();
        delta = fabs(fmod (northMin,increment));
        northMin -= (northMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (northMax,increment));
        northMax += (northMax >= 0.0) ? (increment - delta) : delta;

        // Adjust for the base.  Again, we always enlarge, never shrink.
        if (specs->GetEastingBase () > 0.0)
        {
            increment = specs->GetEastingIncrement ();
            delta = fmod (specs->GetEastingBase(),increment);
            eastMin  += delta - increment;
            eastMax  += delta + increment;
        }
        if (specs->GetNorthingBase () > 0.0)
        {
            increment = specs->GetNorthingIncrement ();
            delta = fmod (specs->GetNorthingBase(),increment);
            northMin  += delta - increment;
            northMax  += delta + increment;
        }

        // Given the specification, we double loop, generating lines.
        increment = specs->GetNorthingIncrement ();
        for (value = northMin;value <= northMax;value += increment)
        {
            fromPnt->SetX (eastMin);
            fromPnt->SetY (value);
            toPnt->SetX (eastMax);
            toPnt->SetY (value);
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision);
            
            // Clip the line to the frame boundary.  The grid line may
            // actually leave, and then reenter, the grid boundary, so the
            // result can be a multi-line sting.
            lineStringCollection = m_FrameBoundary->ClipLineString (lineString);

            // Construct the Grid Line object and add it to the grid
            // line collection object.
            gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::NorthSouth,value);
            gridLine->SetSegmentCollection (lineStringCollection);
            gridLineCollection->Add (gridLine);
        }

        // Given the specification, we double loop, generating lines.
        increment = specs->GetEastingIncrement ();
        for (value = eastMin;value <= eastMax;value += increment)
        {
            fromPnt->SetX (value);
            fromPnt->SetY (northMin);
            toPnt->SetX (value);
            toPnt->SetY (northMax);
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision);

            // Clip the line to the frame boundary.  The grid line may
            // actually leave, and then re-enter, the grid boundary, so the
            // result can be a multi-line sting.
            lineStringCollection = m_FrameBoundary->ClipLineString (lineString);
  
            // Construct the Grid Line object and add it to the grid
            // line collection object.
            gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::EastWest,value);
            gridLine->SetSegmentCollection (lineStringCollection);
            gridLineCollection->Add (gridLine);
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridLines")

    return static_cast<MgCoordinateSystemGridLineCollection*>(gridLineCollection.Detach());
}
MgCoordinateSystemGridTickCollection* CCoordinateSystemOneGrid::GetBoundaryTicks (MgCoordinateSystemGridSpecification* specs)
{
    // Loop through the untransformed boundary.  For each line segment:
    
   // Errors:
   // 1> Convergence of the calculation
   // 2> specification error
   return NULL;
}

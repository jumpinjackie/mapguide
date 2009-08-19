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
const INT32 CCoordinateSystemOneGrid::MaxCurvePoints = 511;
CCoordinateSystemOneGrid::CCoordinateSystemOneGrid () : MgGuardDisposable   (),
                                                        m_UserID            (0),
                                                        m_MaxCurvePoints    (MaxCurvePoints),
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
}
CCoordinateSystemOneGrid::CCoordinateSystemOneGrid (MgCoordinateSystemGridBoundary* frameBoundary,
                                                    MgCoordinateSystem* gridCRS,
                                                    MgCoordinateSystem* frameCRS)
                                                        :
                                                    MgGuardDisposable   (),
                                                    m_UserID            (0),
                                                    m_MaxCurvePoints    (MaxCurvePoints),
                                                    m_RegionLabel       (),
                                                    m_GridCRS           (),
                                                    m_FrameCRS          (),
                                                    m_ToFrameXform      (),
                                                    m_ToGridXform       (),
                                                    m_BoundaryPrecision (0.0),
                                                    m_GridBoundary      (),
                                                    m_FrameBoundary     ()
{
    SetUp (frameBoundary,gridCRS,frameCRS);
}
CCoordinateSystemOneGrid::~CCoordinateSystemOneGrid (void)
{
}
// The following is essentially the constructor.  It's existence is to
// enable a derived class to monkey around with its constructor's
// arguments to come up with the arguments this object needs.  Perhaps
// there is a better way to do this in C++, but I am unaware of it.
void CCoordinateSystemOneGrid::SetUp (MgCoordinateSystemGridBoundary* frameBoundary,
                                      MgCoordinateSystem* gridCRS,
                                      MgCoordinateSystem* frameCRS)
{
    MgCoordinateSystemFactory csFactory;

    m_FrameBoundary = SAFE_ADDREF (frameBoundary);
    m_GridCRS = SAFE_ADDREF (gridCRS);
    m_FrameCRS = SAFE_ADDREF (frameCRS);
    m_ToFrameXform = csFactory.GetTransform (m_GridCRS,m_FrameCRS);
    m_ToGridXform = csFactory.GetTransform (m_FrameCRS,m_GridCRS);
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
        precision = specs->GetCurvePrecision ();
        GenerateGridBoundary (precision);

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
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision,m_MaxCurvePoints);
            
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
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision,m_MaxCurvePoints);

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
CCoordinateSystemGridTickCollection* CCoordinateSystemOneGrid::GetBoundaryTicks (MgCoordinateSystemGridSpecification* specs)
{
    INT32 status;
    INT32 orientation;

    double delta;
    double increment;
    double segLength;
    double ordinateValue;
    double curvePrecision;
    double eastMin, eastMax;
    double northMin, northMax;
    double deltaX, deltaY;

    Ptr<MgPolygon> pPolygon;
    Ptr<MgCoordinate> toPnt;
    Ptr<MgCoordinate> fromPnt;
    Ptr<MgCoordinate> tickPnt;
    Ptr<MgLinearRing> exteriorRing;
    Ptr<MgCoordinateIterator> ringItr;
    Ptr<CCoordinateSystemGridTickCollection> tickCollection;
    MgGeometryFactory mgFactory;

    MG_TRY ()
    
        tickCollection = new CCoordinateSystemGridTickCollection ();

        // Get the grid extents.
        curvePrecision = specs->GetCurvePrecision();
        GetGridExtents (eastMin,eastMax,northMin,northMax,curvePrecision);

        // Expand the extents to become the lowest and highest tick values
        // for each of the ordinates.
        increment = specs->GetTickEastingIncrement();
        delta = fabs(fmod (eastMin,increment));
        eastMin -= (eastMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (eastMax,increment));
        eastMax += (eastMax >= 0.0) ? (increment - delta) : delta;

        increment = specs->GetTickNorthingIncrement();
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

        // Extract the exterior ring from the frame boundary.
        pPolygon = m_FrameBoundary->GetBoundary ();
        exteriorRing = pPolygon->GetExteriorRing ();

        // Loop once for each line segment in the polyline which defines the
        // the frame boundary.
        ringItr = exteriorRing->GetCoordinates ();
        ringItr->MoveNext ();
        toPnt = ringItr->GetCurrent ();
        while (ringItr->MoveNext ())
        {
            fromPnt = toPnt;
            toPnt = ringItr->GetCurrent ();

            // Do the easting ticks; that is tickmarks whose value is an
            // easting ordinate value.
            increment = specs->GetTickEastingIncrement ();
            orientation = MgCoordinateSystemGridOrientation::EastWest;
            for (ordinateValue = eastMin;ordinateValue <= eastMax;ordinateValue += increment)
            {
                Ptr<MgCoordinate> tickPoint = new MgCoordinateXY ();
                status = m_ToGridXform->PositionOfValue (tickPoint,ordinateValue,
                                                                   orientation,
                                                                   fromPnt,
                                                                   toPnt);
                if (status == 0)
                {
                    // Here if we got a real point.  Create a tick object and
                    // add it to the collection.  We're going to need to capture
                    // some information about the boundary line.
                    deltaX = toPnt->GetX () - fromPnt->GetX ();
                    deltaY = toPnt->GetY () - fromPnt->GetY ();
                    segLength = sqrt (deltaX * deltaX + deltaY * deltaY);
                    Ptr<MgCoordinate> unitVector = mgFactory.CreateCoordinateXY (deltaX / segLength,deltaY / segLength);
                    Ptr<CCoordinateSystemGridTick> tickPtr;
                    tickPtr = new CCoordinateSystemGridTick (orientation,ordinateValue);
                    tickPtr->SetDirection (unitVector);
                    tickPtr->SetPosition (tickPoint);
                    tickCollection->Add (tickPtr);
                }
            }

            // Do the easting ticks; that is tickmarks whose value is an
            // easting ordinate value.
            increment = specs->GetTickNorthingIncrement ();
            orientation = MgCoordinateSystemGridOrientation::NorthSouth;
            for (ordinateValue = northMin;ordinateValue <= northMax;ordinateValue += increment)
            {
                Ptr<MgCoordinate> tickPoint = new MgCoordinateXY ();
                status = m_ToGridXform->PositionOfValue (tickPoint,ordinateValue,
                                                                   orientation,
                                                                   fromPnt,
                                                                   toPnt);
                if (status == 0)
                {
                    // Here if we got a real point.  Create a tick object and
                    // add it to the collection.  We're going to need to capture
                    // some information about the boundary line.
                    deltaX = toPnt->GetX () - fromPnt->GetX ();
                    deltaY = toPnt->GetY () - fromPnt->GetY ();
                    segLength = sqrt (deltaX * deltaX + deltaY * deltaY);
                    Ptr<MgCoordinate> unitVector = mgFactory.CreateCoordinateXY (deltaX / segLength,deltaY / segLength);
                    Ptr<CCoordinateSystemGridTick> tickPtr;
                    tickPtr = new CCoordinateSystemGridTick (orientation,ordinateValue);
                    tickPtr->SetDirection (unitVector);
                    tickPtr->SetPosition (tickPoint);
                    tickCollection->Add (tickPtr);
                }
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridLines")
    return tickCollection.Detach ();
}
MgCoordinateSystem* CCoordinateSystemOneGrid::GetFrameCRS ()
{
    return SAFE_ADDREF (m_FrameCRS.p);
}
MgCoordinateSystem* CCoordinateSystemOneGrid::GetGridCRS ()
{
    return SAFE_ADDREF (m_GridCRS.p);
}
void CCoordinateSystemOneGrid::GenerateGridBoundary (double boundaryPrecision)
{
    bool generateGridBoundary;
    Ptr<MgPolygon> pPolygon;

    MG_TRY ()
        generateGridBoundary = (m_GridBoundary == NULL);
        if (!generateGridBoundary)
        {
            generateGridBoundary = (fabs (boundaryPrecision - m_BoundaryPrecision) > 1.0E-12);
        }
        if (generateGridBoundary)
        {
            m_GridBoundary = new CCoordinateSystemGridBoundary ();
            pPolygon = m_FrameBoundary->GetBoundary(m_ToGridXform.p,boundaryPrecision);
            m_GridBoundary->SetBoundaryExtents (pPolygon);
            m_BoundaryPrecision = boundaryPrecision;
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GenerateGridBoundary")
}
void CCoordinateSystemOneGrid::GetGeographicExtents (double& longMin,double& longMax,double& latMin,double& latMax,double precision)
{
    Ptr<MgPolygon> pPolygon;
    Ptr<MgCoordinateSystemGridBoundary> llBoundary;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystemTransform> llTransform;
    MgCoordinateSystemFactory csFactory;

    MG_TRY ()
        llCRS = csFactory.CreateFromCode (L"LL");
        llTransform = csFactory.GetTransform(m_GridCRS,llCRS);
        pPolygon = m_FrameBoundary->GetBoundary (llTransform,precision);
        llBoundary = csFactory.GridBoundary (pPolygon);
        llBoundary->GetBoundaryExtents (longMin,longMax,latMin,latMax);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGeographicExtents")
}
void CCoordinateSystemOneGrid::GetGridExtents (double& eastMin,double& eastMax,double& northMin,double& northMax,double precision)
{
    Ptr<MgPolygon> pPolygon;
    Ptr<MgCoordinateSystemGridBoundary> llBoundary;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystemTransform> llTransform;
    MgCoordinateSystemFactory csFactory;

    MG_TRY ()
        // Make sure the grid boundary has been calculated.
        GenerateGridBoundary (precision);
        m_GridBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridExtents")
}

void CCoordinateSystemOneGrid::Dispose (void)
{
    delete this;
}

//=============================================================================
// This object was invented to carry multiple MGRS grids, but is no longer used
// as there is now a CCoordinateSystemMgrsZone object and a related collection
// object.  WHen we're ready for code complete and we don't seem to need this
// for anything, we should delete it. 
CCoordinateSystemGridCollection::CCoordinateSystemGridCollection (void)
                                        :
                                     MgGuardDisposable   (),
                                     m_OneGridCollection ()
{
    m_OneGridCollection = new MgDisposableCollection();
}
CCoordinateSystemGridCollection::~CCoordinateSystemGridCollection (void)
{
    m_OneGridCollection->Clear ();
}
INT32 CCoordinateSystemGridCollection::GetCount () const
{
    INT32 itemCount = m_OneGridCollection->GetCount ();
    return itemCount;
}
CCoordinateSystemOneGrid* CCoordinateSystemGridCollection::GetItem (INT32 index) const
{
    CCoordinateSystemOneGrid *itemPtr = static_cast<CCoordinateSystemOneGrid*>(m_OneGridCollection->GetItem (index));
    return itemPtr;
}
void CCoordinateSystemGridCollection::RemoveAt (INT32 index)
{
    m_OneGridCollection->RemoveAt (index);
}
void CCoordinateSystemGridCollection::Clear()
{
    m_OneGridCollection->Clear ();
}
void CCoordinateSystemGridCollection::SetItem (INT32 index, CCoordinateSystemOneGrid* value)
{
    m_OneGridCollection->SetItem (index,value);
}
void CCoordinateSystemGridCollection::Add (CCoordinateSystemOneGrid* value)
{
    m_OneGridCollection->Add (value);
}
void CCoordinateSystemGridCollection::Dispose (void)
{
    delete this;
}

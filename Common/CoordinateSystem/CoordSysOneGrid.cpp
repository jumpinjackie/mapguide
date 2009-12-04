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
#include "CoordSysMathComparator.h"
#include "CoordSysTransform.h"              //for CCoordinateSystemTransform
#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"

using namespace CSLibrary;

//=============================================================================
// CCoordinateSystemOneGrid
const INT32 CCoordinateSystemOneGrid::MaxCurvePoints = 511;
const INT32 CCoordinateSystemOneGrid::m_GridLineExceptionLevelK = 50000000L;   // 50MB
const INT32 CCoordinateSystemOneGrid::m_GridTickExceptionLevelK = 20000000L;   // 20MB

CCoordinateSystemOneGrid::CCoordinateSystemOneGrid () : MgGuardDisposable        (),
                                                        m_GridFrameCrsSame       (false),
                                                        m_UserID                 (0),
                                                        m_MaxCurvePoints         (MaxCurvePoints),
                                                        m_Label                  (),
                                                        m_GridLineExceptionLevel (m_GridLineExceptionLevelK),
                                                        m_GridTickExceptionLevel (m_GridTickExceptionLevelK),
                                                        m_GridCRS                (),
                                                        m_FrameCRS               (),
                                                        m_ToFrameXform           (),
                                                        m_ToGridXform            (),
                                                        m_BoundaryPrecision      (0.0),
                                                        m_GridBoundary           (),
                                                        m_FrameBoundary          ()
{
    Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
}
CCoordinateSystemOneGrid::CCoordinateSystemOneGrid (MgCoordinateSystemGridBoundary* frameBoundary,
                                                    MgCoordinateSystem* gridCRS,
                                                    MgCoordinateSystem* frameCRS)
                                                        :
                                                    MgGuardDisposable        (),
                                                    m_GridFrameCrsSame       (false),
                                                    m_UserID                 (0),
                                                    m_MaxCurvePoints         (MaxCurvePoints),
                                                    m_Label                  (),
                                                    m_GridLineExceptionLevel (m_GridLineExceptionLevelK),
                                                    m_GridTickExceptionLevel (m_GridTickExceptionLevelK),
                                                    m_GridCRS                (),
                                                    m_FrameCRS               (),
                                                    m_ToFrameXform           (),
                                                    m_ToGridXform            (),
                                                    m_BoundaryPrecision      (0.0),
                                                    m_GridBoundary           (),
                                                    m_FrameBoundary          ()
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
    CCoordinateSystemMathComparator csMathComparator;

    m_FrameBoundary = SAFE_ADDREF (frameBoundary);
    m_GridCRS = SAFE_ADDREF (gridCRS);
    m_FrameCRS = SAFE_ADDREF (frameCRS);

    m_ToFrameXform = csFactory.GetTransform (m_GridCRS,m_FrameCRS);
    m_ToFrameXform->IgnoreDatumShiftWarning (true);
    m_ToFrameXform->IgnoreOutsideDomainWarning (true);

    m_ToGridXform = csFactory.GetTransform (m_FrameCRS,m_GridCRS);
    m_ToGridXform->IgnoreDatumShiftWarning (true);
    m_ToGridXform->IgnoreOutsideDomainWarning (true);

    m_GridFrameCrsSame = csMathComparator.Same (gridCRS,frameCRS);

}
bool CCoordinateSystemOneGrid::GridFrameCrsAreTheSame ()
{
    return m_GridFrameCrsSame;
}
bool CCoordinateSystemOneGrid::IsGeographic (void)
{
    bool isGeographic;
    INT32 projectionCode;

    projectionCode = m_GridCRS->GetProjectionCode ();
    isGeographic = (projectionCode == MgCoordinateSystemProjectionCode::LL);    
    return isGeographic;
}
INT32 CCoordinateSystemOneGrid::GetUserID (void)
{
    return m_UserID;
}
STRING CCoordinateSystemOneGrid::GetLabel (void)
{
    return m_Label;
}
void CCoordinateSystemOneGrid::SetUserID (INT32 userID)
{
    m_UserID = userID;
}
void CCoordinateSystemOneGrid::SetLabel (CREFSTRING label)
{
    m_Label = label;
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
    INT32 gridCrsUnitCode;

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

    Ptr<CCoordinateSystemGridLineCollection> gridLineCollection = new CCoordinateSystemGridLineCollection (m_GridLineExceptionLevel);

    MG_TRY()
        gridCrsUnitCode = m_GridCRS->GetUnitCode ();

        CCoordinateSystemGridSpecification* mySpecPtr = dynamic_cast<CCoordinateSystemGridSpecification*>(specs);
        coordinate = new MgCoordinateXY ();
        fromPnt    = new MgCoordinateXY ();
        toPnt      = new MgCoordinateXY ();

        // If the boundary has not been converted yet, or if the precision
        // value has been changed, reproduce the m_GridBoundary
        // member from the m_FrameBoundary member.  At this point, we
        // need the curve precision value in Grid system units.
        precision = mySpecPtr->GetCurvePrecision (m_GridCRS);
        GenerateGridBoundary (precision);

        // Get the extents of the frame boundary, and then convert them to
        // grid coordinates.  Doing this here _should_ elimiate the need to
        // arbitrarily expand the grid boundary.
        m_GridBoundary->GetBoundaryExtents (eastMin,eastMax,northMin,northMax);
        
        // Adjust so the the grid limits are exact values with regard to the
        // specified increments.  Do this in an expansive way so the the grid
        // limits always get larger, never smaller.
        //
        // Note that we need to have the increments and base values in terms
        // of the grid coordinate system, regardless of the units used to
        // specify them.
        increment = mySpecPtr->GetEastingIncrement(gridCrsUnitCode);
        delta = fabs(fmod (eastMin,increment));
        eastMin -= (eastMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (eastMax,increment));
        eastMax += (eastMax >= 0.0) ? (increment - delta) : delta;
 
        increment = mySpecPtr->GetNorthingIncrement(gridCrsUnitCode);
        delta = fabs(fmod (northMin,increment));
        northMin -= (northMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (northMax,increment));
        northMax += (northMax >= 0.0) ? (increment - delta) : delta;

        // Adjust for the base.  Again, we always enlarge, never shrink.
        if (mySpecPtr->GetEastingBase () > 0.0)
        {
            increment = mySpecPtr->GetEastingIncrement (gridCrsUnitCode);
            delta = fmod (mySpecPtr->GetEastingBase(gridCrsUnitCode),increment);
            eastMin  += delta - increment;
            eastMax  += delta + increment;
        }
        if (mySpecPtr->GetNorthingBase () > 0.0)
        {
            increment = mySpecPtr->GetNorthingIncrement (gridCrsUnitCode);
            delta = fmod (mySpecPtr->GetNorthingBase(gridCrsUnitCode),increment);
            northMin  += delta - increment;
            northMax  += delta + increment;
        }

        // Given the specification, we double loop, generating lines.
        // Forst loop, generatinf west/east lines starting at the southern edge
        // proceeding to the north.
        increment = mySpecPtr->GetNorthingIncrement (gridCrsUnitCode);
        for (value = northMin;value <= northMax;value += increment)
        {
            fromPnt->SetX (eastMin);
            fromPnt->SetY (value);
            toPnt->SetX (eastMax);
            toPnt->SetY (value);
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision,m_MaxCurvePoints);

            // Clip the line to the frame boundary.  The grid line may
            // actually leave, and then re-enter, the grid boundary, so the
            // result can be a multi-line string.
            lineStringCollection = m_FrameBoundary->ClipLineString (lineString);
            if (lineStringCollection)
            {
                // Construct the Grid Line object and add it to the grid
                // line collection object.
                gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::NorthSouth,value);
                gridLine->SetSegmentCollection (lineStringCollection);
                gridLineCollection->Add (gridLine);
            }
        }

        // Second loop, the south/north lines, starting at the western edged
        // proceeding to the east.
        increment = mySpecPtr->GetEastingIncrement (gridCrsUnitCode);
        for (value = eastMin;value <= eastMax;value += increment)
        {
            fromPnt->SetX (value);
            fromPnt->SetY (northMin);
            toPnt->SetX (value);
            toPnt->SetY (northMax);
            lineString = m_ToFrameXform->GridLine (fromPnt,toPnt,precision,m_MaxCurvePoints);

            // Clip the line to the frame boundary.  The grid line may
            // actually leave, and then re-enter, the grid boundary, so the
            // result can be a multi-line string.
            lineStringCollection = m_FrameBoundary->ClipLineString (lineString);
            if (lineStringCollection)
            {
                // Construct the Grid Line object and add it to the grid
                // line collection object.
                gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::EastWest,value);
                gridLine->SetSegmentCollection (lineStringCollection);
                gridLineCollection->Add (gridLine);
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridLines")

    return static_cast<MgCoordinateSystemGridLineCollection*>(gridLineCollection.Detach());
}
CCoordinateSystemGridTickCollection* CCoordinateSystemOneGrid::GetBoundaryTicks (MgCoordinateSystemGridSpecification* specs)
{
    bool isOnGridLine;

    INT32 status;
    INT32 orientation;
    INT32 gridCrsUnitCode;

    double tmpDbl;
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

        gridCrsUnitCode = m_GridCRS->GetUnitCode ();

        CCoordinateSystemGridSpecification* mySpecPtr = dynamic_cast<CCoordinateSystemGridSpecification*>(specs);
        tickCollection = new CCoordinateSystemGridTickCollection (m_GridTickExceptionLevel);

        // Get the grid extents.
        curvePrecision = mySpecPtr->GetCurvePrecision();
        GetGridExtents (eastMin,eastMax,northMin,northMax,curvePrecision);

        // Expand the extents to become the lowest and highest tick values
        // for each of the ordinates.
        increment = mySpecPtr->GetTickEastingIncrement(gridCrsUnitCode);
        delta = fabs(fmod (eastMin,increment));
        eastMin -= (eastMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (eastMax,increment));
        eastMax += (eastMax >= 0.0) ? (increment - delta) : delta;

        increment = mySpecPtr->GetTickNorthingIncrement(gridCrsUnitCode);
        delta = fabs(fmod (northMin,increment));
        northMin -= (northMin >= 0.0) ? delta : (increment - delta);
        delta = fabs(fmod (northMax,increment));
        northMax += (northMax >= 0.0) ? (increment - delta) : delta;

        // Adjust for the base.  Again, we always enlarge, never shrink.
        if (mySpecPtr->GetEastingBase () > 0.0)
        {
            increment = mySpecPtr->GetEastingIncrement (gridCrsUnitCode);
            delta = fmod (mySpecPtr->GetEastingBase(gridCrsUnitCode),increment);
            eastMin  += delta - increment;
            eastMax  += delta + increment;
        }
        if (mySpecPtr->GetNorthingBase () > 0.0)
        {
            increment = mySpecPtr->GetNorthingIncrement (gridCrsUnitCode);
            delta = fmod (mySpecPtr->GetNorthingBase(gridCrsUnitCode),increment);
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
            increment = mySpecPtr->GetTickEastingIncrement (gridCrsUnitCode);
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
                    tmpDbl = (ordinateValue - specs->GetEastingBase ());
                    tmpDbl = fmod (tmpDbl,specs->GetEastingIncrement ());
                    isOnGridLine = fabs (tmpDbl) < 1.0E-08;
                    tickPtr->SetOnGridLine (isOnGridLine);
                    tickPtr->SetDirection (unitVector);
                    tickPtr->SetPosition (tickPoint);
                    tickCollection->Add (tickPtr);
                }
            }

            // Do the northing ticks; that is tickmarks whose value is an
            // northing ordinate value.
            increment = mySpecPtr->GetTickNorthingIncrement (gridCrsUnitCode);
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
                    tmpDbl = (ordinateValue - specs->GetNorthingBase ());
                    tmpDbl = fmod (tmpDbl,specs->GetNorthingIncrement ());
                    isOnGridLine = fabs (tmpDbl) < 1.0E-08;
                    tickPtr->SetDirection (unitVector);
                    tickPtr->SetPosition (tickPoint);
                    tickCollection->Add (tickPtr);
                }
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::GetGridLines")
    return tickCollection.Detach ();
}

INT32 CCoordinateSystemOneGrid::ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    return 50000;
}
INT32 CCoordinateSystemOneGrid::ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    return 10000;
}

INT32 CCoordinateSystemOneGrid::SetGridLineExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridLineExceptionLevel;
    if (memoryUseMax > 0L)
    {
        m_GridLineExceptionLevel = memoryUseMax;
    }
    return rtnValue;
}
INT32 CCoordinateSystemOneGrid::SetGridTickExceptionLevel (INT32 memoryUseMax)
{
    INT32 rtnValue = m_GridTickExceptionLevel;
    if (memoryUseMax > 0L)
    {
        m_GridTickExceptionLevel = memoryUseMax;
    }
    return rtnValue;
}
MgCoordinateSystemGridBoundary* CCoordinateSystemOneGrid::GetFrameBoundary (void)
{
    return SAFE_ADDREF (m_FrameBoundary.p);
}
MgCoordinateSystem* CCoordinateSystemOneGrid::GetFrameCRS ()
{
    return SAFE_ADDREF (m_FrameCRS.p);
}
MgCoordinateSystem* CCoordinateSystemOneGrid::GetGridCRS ()
{
    return SAFE_ADDREF (m_GridCRS.p);
}
MgCoordinateSystemTransform* CCoordinateSystemOneGrid::GetGridToFrameXform (void)
{
    return SAFE_ADDREF (m_ToFrameXform.p);
}
MgCoordinateSystemTransform* CCoordinateSystemOneGrid::GetFrameToGridXform (void)
{
    return SAFE_ADDREF (m_ToGridXform.p);
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
        llTransform = csFactory.GetTransform(m_FrameCRS,llCRS);
        llTransform->IgnoreDatumShiftWarning (true);
        llTransform->IgnoreOutsideDomainWarning (true);
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

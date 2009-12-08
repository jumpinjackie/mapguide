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
#include "CoordSysUtil.h"       //for Convert_Wide_To_Ascii
#include "CriticalSection.h"    //for SmartCriticalClass

#include "Spatial/MathUtility.h"
#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"
#include "CoordSysMgrsZone.h"
#include "CoordSysMgrs.h"
#include "CoordSysMgrsMajorRegion.h"

using namespace CSLibrary;

#if !defined (_DEBUG)
    // Include heap overhead estinated at 12 bytes in release mode.
    static const INT32 kMgHeapOverhead = 12;
    static const INT32 kMgSizeOfCoordinateXY = sizeof (MgCoordinateXY) + kMgHeapOverhead;
#else
    // Include heap overhead estinated at 48 bytes in release mode.
    static const INT32 kMgHeapOverhead = 36;
    static const INT32 kMgSizeOfCoordinateXY = sizeof (MgCoordinateXY) + kMgHeapOverhead;
#endif

CCoordinateSystemMgrsZone::CCoordinateSystemMgrsZone (MgCoordinateSystemGridBoundary* frameBoundary,
                                                      INT32 utmZoneNbr,
                                                      bool useFrameDatum,
                                                      MgCoordinateSystem* frameCRS,
                                                      INT8 letteringScheme)
                                                        :
                                                      CCoordinateSystemOneGrid (),
                                                      m_UtmZone                (utmZoneNbr),
                                                      m_LetteringScheme        (letteringScheme)
{
    MgCoordinateSystemFactory csFactory;
    Ptr<MgCoordinateSystem> utmZoneCRS;

    STRING utmZoneCode = CCoordinateSystemMgrs::ZoneNbrToUtmCs (m_UtmZone);
    utmZoneCRS = csFactory.CreateFromCode (utmZoneCode);

    SetUp (frameBoundary,utmZoneCRS,frameCRS);
    SetUserID (m_UtmZone);
}
CCoordinateSystemMgrsZone::~CCoordinateSystemMgrsZone (void)
{
}
CCoordinateSystemGridRegionCollection* CCoordinateSystemMgrsZone::GetGridRegions (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                  MgCoordinateSystemGridSpecification* specification,
                                                                                  INT32 exceptionLvl)
{
    Ptr<CCoordinateSystemGridRegionCollection> regionCollection;

    MG_TRY ()
        regionCollection = BuildRegionCollection (frameBoundary,specification,exceptionLvl);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrsZone::GetGridRegions")
    return SAFE_ADDREF(regionCollection.p);
}
CCoordinateSystemGridLineCollection* CCoordinateSystemMgrsZone::GetGridLines (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                              MgCoordinateSystemGridSpecification* specification,
                                                                              INT32 exceptionLvl)
{
    // For now, we just call the generic OneGrid grid line function.
    // Later on we might need to add some special logic for zones 31
    // through 37 in the higher northern longitudes.
    MgCoordinateSystemGridLineCollection* gridLines = CCoordinateSystemOneGrid::GetGridLines (specification);
    return dynamic_cast<CCoordinateSystemGridLineCollection*>(gridLines);
}
CCoordinateSystemGridLineCollection* CCoordinateSystemMgrsZone::GetGraticuleLines (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                   MgCoordinateSystemGridSpecification* specification,
                                                                                   INT32 exceptionLvl)
{
    const INT32 maxPoints = 512;

    INT32 index;
    INT32 regionCount;
    double value;
    double precision;
    double lngMin, lngMax;
    double latMin,latMax;
    STRING designation;
   
    Ptr<MgCoordinateXY> fromPoint;
    Ptr<MgCoordinateXY> toPoint;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystem> frameCRS;
    Ptr<MgCoordinateSystemTransform> toFrameTransform;
    Ptr<CCoordinateSystemMgrsMajorRegion> regionPtr;
    Ptr<CCoordinateSystemMgrsMajorRegionCollection> regionCollection;
    Ptr<MgLineStringCollection> lineStrCollectionPtr;
    Ptr<CCoordinateSystemMgrsZoneCollection> mjrZoneCollection;
    Ptr<MgLineString> lineString;
    Ptr<MgLineStringCollection> lineStringCollection;
    Ptr<CCoordinateSystemGridLine> gridLine;

    MgCoordinateSystemFactory csFactory;

    Ptr<CCoordinateSystemGridLineCollection> gridLineCollection = new CCoordinateSystemGridLineCollection (exceptionLvl);

    MG_TRY ()
        fromPoint = new MgCoordinateXY ();
        toPoint = new MgCoordinateXY ();
        CCoordinateSystemGridSpecification* mySpecPtr = dynamic_cast<CCoordinateSystemGridSpecification*>(specification);
        precision = mySpecPtr->GetCurvePrecision (m_GridCRS);

        // To be are successful, we'll need a Transform which will convert
        // 'LL' to the frame coordinate system.
        llCRS = csFactory.CreateFromCode (L"LL");
        frameCRS = GetFrameCRS ();
        toFrameTransform = csFactory.GetTransform(llCRS,frameCRS);

        // Major regions are solely a function of the extents of the grid in
        // terms of geographic coordinates.  So, we extract the extents of this
        // grid in geographic coordinate form, and work from there.
        GetGeographicExtents (lngMin,lngMax,latMin,latMax);
        
        regionCollection = new CCoordinateSystemMgrsMajorRegionCollection (m_UtmZone,latMin,latMax);
        if (regionCollection != 0)
        {
            // Draw the east/west lines, i.e. the lines of constant latitude.
            regionCount = regionCollection->GetCount ();
            for (index = 0;index < regionCount;index += 1)
            {
                // We have a region.
                regionPtr = regionCollection->GetItem (index);
                value = regionPtr->GetSouthEdgeLat ();
                fromPoint->SetX (regionPtr->GetWestEdgeLng ());
                fromPoint->SetY (value);
                toPoint->SetX (regionPtr->GetEastEdgeLng ());
                toPoint->SetY (value);
                lineString = toFrameTransform->GridLine (fromPoint,toPoint,precision,m_MaxCurvePoints);

                // Clip the line to the frame boundary.  The grid line may
                // actually leave, and then re-enter, the grid boundary, so the
                // result can be a multi-line string.
                lineStringCollection = frameBoundary->ClipLineString (lineString);
                if (lineStringCollection)
                {
                    // Construct the Grid Line object and add it to the grid
                    // line collection object.
                    gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::NorthSouth,value);
                    gridLine->SetSegmentCollection (lineStringCollection);
                    gridLineCollection->Add (gridLine);
                }
                if (index == (regionCount - 1))
                {
                    value = regionPtr->GetNorthEdgeLat ();
                    fromPoint->SetX (regionPtr->GetWestEdgeLng ());
                    fromPoint->SetY (value);
                    toPoint->SetX (regionPtr->GetEastEdgeLng ());
                    toPoint->SetY (value);
                    lineString = toFrameTransform->GridLine (fromPoint,toPoint,precision,m_MaxCurvePoints);

                    // Clip the line to the frame boundary.
                    lineStringCollection = frameBoundary->ClipLineString (lineString);
                    if (lineStringCollection)
                    {
                        // Construct the Grid Line object and add it to the grid
                        // line collection object.
                        gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::NorthSouth,value);
                        gridLine->SetSegmentCollection (lineStringCollection);
                        gridLineCollection->Add (gridLine);
                    }
                }
            }

            // Draw the western north/south lines, i.e. the lines of constant
            // longitude.
            regionCount = regionCollection->GetCount ();
            for (index = 0;index < regionCount;index += 1)
            {
                // We have a region.
                regionPtr = regionCollection->GetItem (index);
                value = regionPtr->GetWestEdgeLng ();
                fromPoint->SetX (value);
                fromPoint->SetY (regionPtr->GetSouthEdgeLat ());
                toPoint->SetX (value);
                toPoint->SetY (regionPtr->GetNorthEdgeLat ());
                lineString = toFrameTransform->GridLine (fromPoint,toPoint,precision,m_MaxCurvePoints);

                // Clip the line to the frame boundary.
                lineStringCollection = frameBoundary->ClipLineString (lineString);
                if (lineStringCollection)
                {
                    // Construct the Grid Line object and add it to the grid
                    // line collection object.
                    gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::EastWest,value);
                    gridLine->SetSegmentCollection (lineStringCollection);
                    gridLineCollection->Add (gridLine);
                }
            }

            // Draw the north/south lines, i.e. the lines of constant longitude.
            regionCount = regionCollection->GetCount ();
            for (index = 0;index < regionCount;index += 1)
            {
                // We have a region.
                regionPtr = regionCollection->GetItem (index);
                value = regionPtr->GetEastEdgeLng ();
                fromPoint->SetX (value);
                fromPoint->SetY (regionPtr->GetSouthEdgeLat ());
                toPoint->SetX (value);
                toPoint->SetY (regionPtr->GetNorthEdgeLat ());
                lineString = toFrameTransform->GridLine (fromPoint,toPoint,precision,m_MaxCurvePoints);

                // Clip the line to the frame boundary.  The grid line may
                // actually leave, and then re-enter, the grid boundary, so the
                // result can be a multi-line string.
                lineStringCollection = frameBoundary->ClipLineString (lineString);
                if (lineStringCollection)
                {
                    // Construct the Grid Line object and add it to the grid
                    // line collection object.
                    gridLine = new CCoordinateSystemGridLine (MgCoordinateSystemGridOrientation::EastWest,value);
                    gridLine->SetSegmentCollection (lineStringCollection);
                    gridLineCollection->Add (gridLine);
                }
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrsZone::GetGraticuleLines")
   return gridLineCollection.Detach ();
}
INT32 CCoordinateSystemMgrsZone::ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification)
{
    INT32 regionSize;
    INT32 regionCount;
    INT32 memoryGuess (0);

    // Estimate the size of a major region object.  Eventually, should include
    // MaxPoints and CurvePrecision from the specification object.  For now, we
    // make some guesses based on the coordinate systems involved.
    if (GridFrameCrsAreTheSame ())
    {
        // Here, for now, we assume that there are 4 lines at two points a piece, plus
        // a polygon with 5 points.  In this case, several of these points are the
        // same actual object in memory; so we reduce the point count by (an arbirtrary
        // value) 3.
        regionSize = sizeof (CCoordinateSystemGridRegion) + sizeof (MgPolygon) +
                                                            sizeof (MgLineString) * 4 +
                                                            kMgSizeOfCoordinateXY * 10;
    }
    else
    {
        // Here, for now, we assume that there are 4 lines at 511 points a piece, plus
        // a polygon with (4 * 511) points; 4088 points altogether.
        regionSize = sizeof (CCoordinateSystemGridRegion) + sizeof (MgPolygon) +
                                                            sizeof (MgLineString) * 4 +
                                                            kMgSizeOfCoordinateXY * 4088;
    }

    MG_TRY ()
        if (specification->GetUnitType () == MgCoordinateSystemUnitType::Angular)
        {
            double eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Degree);
            double northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Degree);
            if (MgMathUtility::DblCmp (eastingIncrement,6.0) &&
                MgMathUtility::DblCmp (northingIncrement,8.0))
            {
                double lngMin,lngMax;
                double latMin,latMax;
                Ptr<CCoordinateSystemMgrsMajorRegionCollection> mjrRegionCollection;

                // Determine the number of regions included in the boundary.
                GetGeographicExtents (lngMin,lngMax,latMin,latMax);
                mjrRegionCollection = new CCoordinateSystemMgrsMajorRegionCollection (m_UtmZone,latMin,latMax);
                regionCount = mjrRegionCollection->GetCount ();

                // Make a guess at the amount of memory required.
                memoryGuess = regionSize * regionCount;
            }
        }
        else if (specification->GetUnitType () == MgCoordinateSystemUnitType::Linear)
        {
            double eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Meter);
            double northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Meter);
            if (MgMathUtility::DblCmp (eastingIncrement,100000.0) &&
                MgMathUtility::DblCmp (northingIncrement,100000.0))
            {
                INT32 beginEast, endEast;
                INT32 beginNorth, endNorth;

                double delta;
                double curvePrecision;
                double  eastMin, eastMax;
                double northMin, northMax;

                curvePrecision = 1.0;
                // Estimate the number of minor regions.
                GetGridExtents (eastMin,eastMax,northMin,northMax,curvePrecision);
 
                delta = fabs (fmod (eastMin,100000.0));
                beginEast = static_cast<INT32>(eastMin - ((eastMin >= 0.0) ? delta : (100000.0 - delta)));
                delta = fabs (fmod (eastMax,100000.0));
                endEast = static_cast<INT32>(eastMax + ((eastMax >= 0.0) ? (100000.0 - delta) : -delta));

                delta = fabs (fmod (northMin,100000.0));
                beginNorth = static_cast<INT32>(northMin - ((northMin >= 0.0) ? delta : (100000.0 - delta)));
                delta = fabs (fmod (northMax,100000.0));
                endNorth = static_cast<INT32>(northMax + ((northMax >= 0.0) ? (100000.0 - delta) : -delta));

                INT32 verticalCount   = (endNorth - beginNorth) / 100000;
                INT32 horizontalCount = (endEast - beginEast) / 100000;
                regionCount = horizontalCount * verticalCount;

                memoryGuess = regionSize * regionCount;
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrsZone::ApproxGridRegionMemoryUsage")

    return memoryGuess;
}
INT32 CCoordinateSystemMgrsZone::GetUtmZoneNbr (void)
{
    // m_UtmZoneNbr is positive for northern hemisphere, negative for the
    // southern hemisphere.  Polar regions are assigned the value of 61.  Zero
    // is the uninitialized/unknown/error value.
    return m_UtmZone;
}
CCoordinateSystemGridRegionCollection* CCoordinateSystemMgrsZone::BuildRegionCollection (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                         MgCoordinateSystemGridSpecification* specification,
                                                                                         INT32 exceptionLvl)
{
    double curvePrecision;
    double eastingIncrement;
    double northingIncrement;
    Ptr<CCoordinateSystemGridRegionCollection> regionCollection;

    MG_TRY ()
        regionCollection = new CCoordinateSystemGridRegionCollection (exceptionLvl);
        curvePrecision = specification->GetCurvePrecision ();
        if (specification->GetUnitType () == MgCoordinateSystemUnitType::Angular)
        {
            eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Degree);
            northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Degree);
            if (MgMathUtility::DblCmp (eastingIncrement,6.0) &&
                MgMathUtility::DblCmp (northingIncrement,8.0))
            {
                BuildMajorRegions (regionCollection,frameBoundary,curvePrecision);
            }
        }
        else if (specification->GetUnitType () == MgCoordinateSystemUnitType::Linear)
        {
            eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Meter);
            northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Meter);
            if (MgMathUtility::DblCmp (eastingIncrement,100000.0) &&
                MgMathUtility::DblCmp (northingIncrement,100000.0))
            {
                BuildMinorRegions (regionCollection,frameBoundary,curvePrecision);
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMgrsZone::GetGraticuleLines")
    return regionCollection.Detach ();
}
void CCoordinateSystemMgrsZone::BuildMajorRegions (CCoordinateSystemGridRegionCollection* regionCollection,
                                                   MgCoordinateSystemGridBoundary* frameBoundary,
                                                   double curvePrecision)
{
    const INT32 maxPoints = 512;

    INT32 index;
    double lngMin, lngMax;
    double latMin,latMax;
    STRING designation;

    Ptr<MgCoordinate> southwest;
    Ptr<MgCoordinate> northeast;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystem> frameCRS;
    Ptr<MgCoordinateSystemTransform> toFrameTransform;
    Ptr<CCoordinateSystemGridBoundary> rgnBoundary;
    Ptr<CCoordinateSystemGridRegion> pMjrRegion;
    Ptr<CCoordinateSystemMgrsMajorRegion> regionPtr;
    Ptr<CCoordinateSystemMgrsMajorRegionCollection> mjrRegionCollection;

    MgCoordinateSystemFactory csFactory;

    MG_TRY ()
        southwest = new MgCoordinateXY ();
        northeast = new MgCoordinateXY ();
 
        // To be are successful, we'll need a Transform which will convert
        // 'LL' to the frame coordinate system.
        llCRS = csFactory.CreateFromCode (L"LL");
        frameCRS = GetFrameCRS ();
        toFrameTransform = csFactory.GetTransform(llCRS,frameCRS);

        // Major regions are solely a function of the extents of the grid in
        // terms of geographic coordinates.  So, we extract the extents of this
        // grid in geographic coordinate form, and work from there.
        GetGeographicExtents (lngMin,lngMax,latMin,latMax);
        
        mjrRegionCollection = new CCoordinateSystemMgrsMajorRegionCollection (m_UtmZone,latMin,latMax);
        if (mjrRegionCollection != 0)
        {
            INT32 mjrRegionCount = mjrRegionCollection->GetCount ();
            for (index = 0;index < mjrRegionCount;index += 1)
            {
                // We have a region.
                regionPtr = mjrRegionCollection->GetItem (index);
                southwest->SetX (regionPtr->GetWestEdgeLng ());
                southwest->SetY (regionPtr->GetSouthEdgeLat ());
                northeast->SetX (regionPtr->GetEastEdgeLng ());
                northeast->SetY (regionPtr->GetNorthEdgeLat ());
                designation = regionPtr->GetDesignation ();
                pMjrRegion = new CCoordinateSystemGridRegion (designation,frameBoundary,
                                                                          toFrameTransform,
                                                                          southwest,
                                                                          northeast,
                                                                          curvePrecision,
                                                                          maxPoints);
                regionCollection->Add (pMjrRegion);
            }
        }
        else if (m_UtmZone == 61)
        {
            //TODO: need to generate regions for the north polar region.
        }
        else if (m_UtmZone == -61)
        {
            //TODO: need to generate regions for the south polar region.
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::BuildMajorRegions")
}
void CCoordinateSystemMgrsZone::BuildMinorRegions (CCoordinateSystemGridRegionCollection* regionCollection,
                                                   MgCoordinateSystemGridBoundary* frameBoundary,
                                                   double curvePrecision)
{
    const INT32 maxPoints = 512;

    INT32 beginEast, endEast;
    INT32 beginNorth, endNorth;
    INT32 eastIndex, northIndex;

    double delta;
    double eastMin, eastMax;
    double northMin, northMax;

    Ptr<MgCoordinate> southwest;
    Ptr<MgCoordinate> northeast;
    Ptr<MgCoordinateSystemTransform> gridToFrameXfrom;
    Ptr<CCoordinateSystemGridBoundary> rgnBoundary;
    Ptr<CCoordinateSystemGridRegion> pMnrRegion;
    MgCoordinateSystemFactory csFactory;

    MG_TRY ()
        southwest = new MgCoordinateXY ();
        northeast = new MgCoordinateXY ();

        // This function is in the OneGrid base class.  We call it just in case
        // the grid boundary has not been generated for this object yet.
        GetGridExtents (eastMin,eastMax,northMin,northMax,curvePrecision);

        // Generation of regions requires conversion of region boundaries to
        // frame coordinates.  The OneGrid base class has this transformation
        // available.
        gridToFrameXfrom = GetGridToFrameXform ();

        // Expand the min/max values to the 100K boundaries of the minor regions.
        // A little overkill here as the extent values should never be negative.
        delta = fabs (fmod (eastMin,100000.0));
        beginEast = static_cast<INT32>(eastMin - ((eastMin >= 0.0) ? delta : (100000.0 - delta)));
        delta = fabs (fmod (eastMax,100000.0));
        endEast = static_cast<INT32>(eastMax + ((eastMax >= 0.0) ? (100000.0 - delta) : -delta));

        delta = fabs (fmod (northMin,100000.0));
        beginNorth = static_cast<INT32>(northMin - ((northMin >= 0.0) ? delta : (100000.0 - delta)));
        delta = fabs (fmod (northMax,100000.0));
        endNorth = static_cast<INT32>(northMax + ((northMax >= 0.0) ? (100000.0 - delta) : -delta));

        // We'll move from south to north, doing each west to east row in turn.
        for (northIndex = beginNorth;northIndex < endNorth;northIndex += 100000)
        {
            for (eastIndex = beginEast;eastIndex < endEast;eastIndex += 100000)
            {
                // Use the current eastIndex and northIndex values to generate
                // a 100KM sqaure.
                southwest->SetX (static_cast<double>(eastIndex));
                southwest->SetY (static_cast<double>(northIndex));
                northeast->SetX (static_cast<double>(eastIndex + 100000));
                northeast->SetY (static_cast<double>(northIndex + 100000));

                // Now we need the designation.  The comes from the MgCoordinateSystemMgrs
                // object that knows about the lettering scheme.  Unfortunately, we don't
                // have a reference to the host Mgrs object, so we're going to have to know
                // about the lettering scheme.
                STRING designation = CCoordinateSystemMgrs::GridSquareDesignation (m_UtmZone,static_cast<double>(eastIndex + 1),
                                                                                             static_cast<double>(northIndex + 1),
                                                                                             m_LetteringScheme);
                if (!designation.empty ())
                {
                    // Construct the region object and add it to the region collection.
                    pMnrRegion = new CCoordinateSystemGridRegion (designation,frameBoundary,
                                                                              gridToFrameXfrom,
                                                                              southwest,
                                                                              northeast,
                                                                              curvePrecision,
                                                                              maxPoints);
                    regionCollection->Add (pMnrRegion);
                }
            }
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemOneGrid::BuildMinorRegions")
}

///////////////////////////////////////////////////////////////////////////////
// An MGRS grid is, essentially, a collection of MGRS Zone grids.  Usually
// just one, but we must be able to do lots.
CCoordinateSystemMgrsZoneCollection::CCoordinateSystemMgrsZoneCollection (void)
                                        :
                                     MgGuardDisposable   (),
                                     m_OneGridCollection ()
{
    m_OneGridCollection = new MgDisposableCollection();
}
CCoordinateSystemMgrsZoneCollection::~CCoordinateSystemMgrsZoneCollection (void)
{
    m_OneGridCollection->Clear ();
}
INT32 CCoordinateSystemMgrsZoneCollection::GetCount () const
{
    INT32 itemCount = m_OneGridCollection->GetCount ();
    return itemCount;
}
CCoordinateSystemMgrsZone* CCoordinateSystemMgrsZoneCollection::GetItem (INT32 index) const
{
    CCoordinateSystemMgrsZone *itemPtr = static_cast<CCoordinateSystemMgrsZone*>(m_OneGridCollection->GetItem (index));
    return itemPtr;
}
void CCoordinateSystemMgrsZoneCollection::RemoveAt (INT32 index)
{
    m_OneGridCollection->RemoveAt (index);
}
void CCoordinateSystemMgrsZoneCollection::Clear()
{
    m_OneGridCollection->Clear ();
}
void CCoordinateSystemMgrsZoneCollection::SetItem (INT32 index, CCoordinateSystemMgrsZone* value)
{
    m_OneGridCollection->SetItem (index,value);
}
void CCoordinateSystemMgrsZoneCollection::Add (CCoordinateSystemMgrsZone* value)
{
    m_OneGridCollection->Add (value);
}
void CCoordinateSystemMgrsZoneCollection::Dispose (void)
{
    delete this;
}

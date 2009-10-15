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
#include "CoordSysMgrs.h"       //for CCoordinateSystemMgrs

using namespace CSLibrary;

CCoordinateSystemMgrsZone::CCoordinateSystemMgrsZone (MgCoordinateSystemGridBoundary* frameBoundary,
                                                      INT32 utmZoneNbr,
                                                      bool useFrameDatum,
                                                      MgCoordinateSystem* frameCRS,
                                                      INT8 letteringScheme)
                                                        :
                                                      CCoordinateSystemOneGrid (),
                                                      m_UtmZone                (utmZoneNbr),
                                                      m_LetteringScheme        (letteringScheme),
                                                      m_RegionCollection       ()
{
    MgCoordinateSystemFactory csFactory;
    Ptr<MgCoordinateSystem> utmZoneCRS;
    m_RegionCollection = new CCoordinateSystemGridRegionCollection ();

    STRING utmZoneCode = CCoordinateSystemMgrs::ZoneNbrToUtmCs (m_UtmZone);
    utmZoneCRS = csFactory.CreateFromCode (utmZoneCode);
    SetUp (frameBoundary,utmZoneCRS,frameCRS);
    
    SetUserID (m_UtmZone);
}
CCoordinateSystemMgrsZone::~CCoordinateSystemMgrsZone (void)
{
}
CCoordinateSystemGridRegionCollection* CCoordinateSystemMgrsZone::GetGridRegions (MgCoordinateSystemGridBoundary* frameBoundary,
                                                                                  MgCoordinateSystemGridSpecification* specification)
{
    BuildRegionCollection (frameBoundary,specification);
    return SAFE_ADDREF(m_RegionCollection.p);
}
INT32 CCoordinateSystemMgrsZone::GetUtmZoneNbr (void)
{
    // m_UtmZoneNbr is positive for northern hemisphere, negative for the
    // southern hemisphere.  Polar regions are assigned the value of 61.  Zero
    // is the uninitialized/unknown/error value.
    return m_UtmZone;
}
void CCoordinateSystemMgrsZone::BuildRegionCollection (MgCoordinateSystemGridBoundary* frameBoundary,
                                                       MgCoordinateSystemGridSpecification* specification)
{
    double curvePrecision;
    double eastingIncrement;
    double northingIncrement;

    curvePrecision = specification->GetCurvePrecision ();

    if (specification->GetUnitType () == MgCoordinateSystemUnitType::Angular)
    {
        eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Degree);
        northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Degree);
        if (MgMathUtility::DblCmp (eastingIncrement,6.0) &&
            MgMathUtility::DblCmp (northingIncrement,8.0))
        {
            BuildMajorRegions (frameBoundary,curvePrecision);
        }
    }
    else if (specification->GetUnitType () == MgCoordinateSystemUnitType::Linear)
    {
        eastingIncrement = specification->GetEastingIncrement (MgCoordinateSystemUnitCode::Meter);
        northingIncrement = specification->GetNorthingIncrement (MgCoordinateSystemUnitCode::Meter);
        if (MgMathUtility::DblCmp (eastingIncrement,100000.0) &&
            MgMathUtility::DblCmp (northingIncrement,100000.0))
        {
            BuildMinorRegions (frameBoundary,curvePrecision);
        }
    }
}
void CCoordinateSystemMgrsZone::BuildMajorRegions (MgCoordinateSystemGridBoundary* frameBoundary,double curvePrecision)
{
    const INT32 maxPoints = 512;

    wchar_t gridZoneLetter;
    INT32 utmZoneNbr;
    INT32 gridZoneIndex;
    INT32 latIdx, firstLat, lastLat;
    double delta;
    double centralMeridian;
    double lngMin, lngMax;
    double latMin,latMax;
    STRING designation;
    wchar_t wcBufr [128];
 
    Ptr<MgCoordinate> southwest;
    Ptr<MgCoordinate> northeast;
    Ptr<MgCoordinateSystem> llCRS;
    Ptr<MgCoordinateSystem> frameCRS;
    Ptr<MgCoordinateSystemTransform> toFrameTransform;
    Ptr<CCoordinateSystemGridBoundary> rgnBoundary;
    Ptr<CCoordinateSystemGridRegion> pMjrRegion;

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

        if ((m_UtmZone) != 0 && (abs (m_UtmZone) <= 60))
        {
            // Here for a normal (i.e. non-polar) UTM zone.  Use some asserts
            // to verify the assumption here that the boundary for this zone
            // does not extend into the polar regions.
            assert (latMin >= -80.0);
            assert (latMax <=  84.0);

            // The grid boundary may have been shrunk to the limits of the frame
            // boundary, so we'll recompute the appropriate minLng and maxLng from
            // the utmZoneNbr data member.
            utmZoneNbr = abs (m_UtmZone);
            centralMeridian = static_cast<double>(-183 + (utmZoneNbr * 6));
            lngMin = centralMeridian - 3.0;
            lngMax = centralMeridian + 3.0;

            // Need to account for the fact that the frame boundary may cross
            // an MGRS Grid Zone Designation boundary (i.e. the 8 degree chunks
            // of latitude).  Thus, there may be more than just one major region.
            delta = fabs (fmod (latMin,8.0));
            firstLat = static_cast<INT32>(latMin - ((latMin >= 0.0) ? delta : (8.0 - delta)));
            delta = fabs (fmod (latMax,8.0));
            lastLat = static_cast<INT32>(latMax + ((latMax >= 0.0) ? (8.0 - delta) : -delta));

            // While the normal UTM zones extend up to 84 degrees
            // north latitude, the northernmost band (Band X) is
            // 12 degrees high (72 to 84 degrees latitude).  So there
            // is no band beginning at 80.
            if (lastLat > 72)
            {
                lastLat = 72;
            }

            for (latIdx = firstLat;latIdx <= lastLat;latIdx += 8)
            {
                // Calculate the envelope of the normal UTM zone.
                latMin = static_cast<double>(latIdx);
                latMax = latMin + 8.0;
                southwest->SetX (lngMin);
                southwest->SetY (latMin);
                northeast->SetX (lngMax);
                northeast->SetY (latMax);

                // Band X (72 thru 84 north latitude includes 12 degrees of
                // latitude.  Also, in this band, zones 32, 34, and 36 do not exist.
                // and zones 31, 33, 35, and 37 have non-standard widths.  We
                // adjust for all that now.
                if (latIdx == 72)
                {
                    if (m_UtmZone == 32 || m_UtmZone == 34 || m_UtmZone == 36)
                    {
                        // These are regions which do not exist.
                        continue;
                    }

                    // For all other regions in this band, the "height" is
                    // 12 degrees.
                    latMin = static_cast<double>(latIdx);
                    latMax = latMin + 12.0;

                    // There are two zones which are 9 degrees wide, and two that are
                    // 12 degrees wide in this band.
                    if (m_UtmZone == 31)
                    {
                        lngMin = centralMeridian - 3.0;
                        lngMax = centralMeridian + 6.0;
                    }
                    else if (m_UtmZone == 33 || m_UtmZone == 35)
                    {
                        lngMin = centralMeridian - 6.0;
                        lngMax = centralMeridian + 6.0;
                    }
                    else if (m_UtmZone == 37)
                    {
                        lngMin = centralMeridian - 6.0;
                        lngMax = centralMeridian + 3.0;
                    }
                    southwest->SetX (lngMin);
                    southwest->SetY (latMin);
                    northeast->SetX (lngMax);
                    northeast->SetY (latMax);
                }

                // One more kludge.  The following is necessary for a specific
                // region south east of Norway.
                if ((latIdx == 56) && (m_UtmZone == 31 || m_UtmZone == 32))
                {
                    if (m_UtmZone == 31)
                    {
                        lngMin = centralMeridian - 3.0;
                        lngMax = centralMeridian;
                    }
                    else if (m_UtmZone == 32)
                    {
                        lngMin = centralMeridian - 6.0;
                        lngMax = centralMeridian + 3.0;
                    }
                    southwest->SetX (lngMin);
                    southwest->SetY (latMin);
                    northeast->SetX (lngMax);
                    northeast->SetY (latMax);
                }

                // We have the polygon, we need the designation.
                gridZoneIndex = CCoordinateSystemMgrs::GridZoneDesignationIndex (latMin + 1.0,centralMeridian);
                gridZoneLetter = CCoordinateSystemMgrs::GridZoneDesignationLetter (gridZoneIndex);
                swprintf (wcBufr,128,L"%d%c",utmZoneNbr,gridZoneLetter);
                STRING designation (wcBufr);

                // Construct the region object and add it to the region collection.
                pMjrRegion = new CCoordinateSystemGridRegion (designation,frameBoundary,
                                                                          toFrameTransform,
                                                                          southwest,
                                                                          northeast,
                                                                          curvePrecision,
                                                                          maxPoints);
                m_RegionCollection->Add (pMjrRegion);
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
void CCoordinateSystemMgrsZone::BuildMinorRegions (MgCoordinateSystemGridBoundary* frameBoundary,double curvePrecision)
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

                // Now we need the designation.  The comes from the MgCoordinateSystemMgrs object that knows about the lettering scheme.
                // Unfortunately, we don't have a reference to the host Mgrs object, so we're going to have to know
                // about the lettering scheme.
                STRING designation = CCoordinateSystemMgrs::GridSquareDesignation (m_UtmZone,static_cast<double>(eastIndex + 1),
                                                                                             static_cast<double>(northIndex + 1),
                                                                                             m_LetteringScheme);

                // Construct the region object and add it to the region collection.
                pMnrRegion = new CCoordinateSystemGridRegion (designation,frameBoundary,
                                                                          gridToFrameXfrom,
                                                                          southwest,
                                                                          northeast,
                                                                          curvePrecision,
                                                                          maxPoints);
                m_RegionCollection->Add (pMnrRegion);
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

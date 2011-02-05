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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CoordSysUtil.h"
#include "CriticalSection.h"

#include "CoordSys.h"
#include "CoordSysGrids.h"
#include "CoordSysOneGrid.h"
#include "CoordSysMgrsZone.h"
#include "CoordSysMgrs.h"
#include "CoordSysMgrsMajorRegion.h"

using namespace CSLibrary;

//=============================================================================
// CCoordinateSystemMgrsMajorRegion
CCoordinateSystemMgrsMajorRegion::CCoordinateSystemMgrsMajorRegion (INT32 utmZoneNbr,INT32 bandIndex)
                                                                        :
                                                                    m_IsValid         (false),
                                                                    m_Designation     (),
                                                                    m_UtmZoneNbr      (utmZoneNbr),
                                                                    m_MgrsBandIndex   (bandIndex),
                                                                    m_CentralMeridian (),
                                                                    m_WestLng         (),
                                                                    m_SouthLat        (),
                                                                    m_EastLng         (),
                                                                    m_NorthLat        ()
{
    INT32 zoneNbr;
    wchar_t gridZoneLetter;
    wchar_t wcBufr [128];

    // This constructor should not be used for the polar regions.
    zoneNbr = abs (m_UtmZoneNbr);
    if ((zoneNbr > 0) && (zoneNbr <= 60) && (bandIndex >= 2) && (bandIndex < 22))
    {
        m_IsValid = true;

        // Do the normal thing which is all we need to do 90% of the time.
        zoneNbr = abs (m_UtmZoneNbr);
        m_CentralMeridian = static_cast<double>(-183 + (zoneNbr * 6));
        m_WestLng = m_CentralMeridian - 3.0;
        m_EastLng = m_CentralMeridian + 3.0;

        gridZoneLetter = CCoordinateSystemMgrs::GridZoneDesignationLetter (bandIndex);
        swprintf (wcBufr,128,L"%d%c",zoneNbr,gridZoneLetter);
        STRING designation (wcBufr);
        m_Designation = designation;

        // -2 gets us past the 'A' and 'B' for the south polar region.
        m_SouthLat = static_cast<double>(-80 + ((bandIndex - 2) * 8));
        m_NorthLat = m_SouthLat + 8.0;

        if (bandIndex == 21)
        {
            m_NorthLat = m_SouthLat + 12.0;
            if (m_UtmZoneNbr == 32 || m_UtmZoneNbr == 34 || m_UtmZoneNbr == 36)
            {
                m_IsValid = false;
            }
            else
            {
                // There are two zones which are 9 degrees wide, and two that are
                // 12 degrees wide in this band.
                if (m_UtmZoneNbr == 31)
                {
                    m_WestLng = m_CentralMeridian - 3.0;
                    m_EastLng = m_CentralMeridian + 6.0;
                }
                else if (m_UtmZoneNbr == 33 || m_UtmZoneNbr == 35)
                {
                    m_WestLng = m_CentralMeridian - 6.0;
                    m_EastLng = m_CentralMeridian + 6.0;
                }
                else if (m_UtmZoneNbr == 37)
                {
                    m_WestLng = m_CentralMeridian - 6.0;
                    m_EastLng = m_CentralMeridian + 3.0;
                }
            }
        }
        else if (bandIndex == 19)
        {
            if (m_UtmZoneNbr == 31)
            {
                m_WestLng = m_CentralMeridian - 3.0;
                m_EastLng = m_CentralMeridian;
            }
            else if (m_UtmZoneNbr == 32)
            {
                m_WestLng = m_CentralMeridian - 6.0;
                m_EastLng = m_CentralMeridian + 3.0;
            }
        }
    }
}
// The following constructor is for polar regions.
//CCoordinateSystemMgrsMajorRegion (INT32 utmZoneNbr,double centralMeridian,INT32 latIndex);
CCoordinateSystemMgrsMajorRegion::~CCoordinateSystemMgrsMajorRegion (void)
{
}
bool CCoordinateSystemMgrsMajorRegion::IsValid (void)
{
    return m_IsValid;
}
STRING CCoordinateSystemMgrsMajorRegion::GetDesignation (void)
{
    return m_Designation;
}
INT32 CCoordinateSystemMgrsMajorRegion::GetUtmZoneNbr (void)
{
    return m_UtmZoneNbr;
}
INT32 CCoordinateSystemMgrsMajorRegion::GetMgrsBandINdex (void)
{
    return m_MgrsBandIndex;
}
double CCoordinateSystemMgrsMajorRegion::GetCentralMeridian (void)
{
    return m_CentralMeridian;
}
double CCoordinateSystemMgrsMajorRegion::GetWestEdgeLng (void)
{
    return m_WestLng;
}
double CCoordinateSystemMgrsMajorRegion::GetSouthEdgeLat (void)
{
    return m_SouthLat;
}
double CCoordinateSystemMgrsMajorRegion::GetEastEdgeLng (void)
{
    return m_EastLng;
}
double CCoordinateSystemMgrsMajorRegion::GetNorthEdgeLat (void)
{
    return m_NorthLat;
}
void CCoordinateSystemMgrsMajorRegion::Dispose (void)
{
    delete this;
}
///////////////////////////////////////////////////////////////////////////////
CCoordinateSystemMgrsMajorRegionCollection::CCoordinateSystemMgrsMajorRegionCollection (void)
                                                    :
                                                 MgGuardDisposable       (),
                                                 m_MajorRegionCollection ()
{
    m_MajorRegionCollection = new MgDisposableCollection();
}
CCoordinateSystemMgrsMajorRegionCollection::CCoordinateSystemMgrsMajorRegionCollection (INT32 utmZoneNbr,double minLat,double maxLat)
                                                                                            :
                                                                                        MgGuardDisposable       (),
                                                                                        m_NorthPolar            (false),
                                                                                        m_SouthPolar            (false),
                                                                                        m_MajorRegionCollection ()
{
    INT32 bandIndex;
    INT32 intMinLat;
    INT32 intMaxLat;
    INT32 southernBandIndex;
    INT32 northernBandIndex;

    double delta;

    // NOTE:  This is a constructor.
    m_MajorRegionCollection = new MgDisposableCollection();

    // Determine the band index of the southernmost region in the latitude range.
    if (minLat < -80.0)
    {
        // Southern limit includes the south UPS region.
        m_SouthPolar = true;
        intMinLat = -90;
        southernBandIndex = 0;
    }
    else if (minLat >= -80.0 && minLat < 72.0)
    {
        // Southern limit is in the normal UTM zone area where regions are 8 degrees
        // of latitude.
        delta = fabs(fmod (minLat,8.0));
        intMinLat = static_cast<INT32>(minLat - ((minLat >= 0.0) ? delta : (8.0 - delta)));
        // +2 two here accounts for the A & B regions of the south pole.
        southernBandIndex = ((intMinLat + 80) / 8) + 2;
    }
    else if (minLat >= 72.0 && minLat <= 84.0)
    {
        // The southern limit is in Band X which is the one zone which is 12 degrees
        // of latitude high.
        intMinLat = 72;
        southernBandIndex = 21;
    }
    else if (minLat > 84.0)
    {
        // The southern limit is in the north UPS regions (i.e. YZ).
        m_NorthPolar = true;
        intMinLat = 84;
        southernBandIndex = 22;
    }

    // Determine the bandindex of the northernmost region in the latitude range.
    if (maxLat < -80.0)
    {
        // Northern limit is in the southern UPS region.
        m_SouthPolar = true;
        intMaxLat = 80;
        northernBandIndex = 1;
    }
    else if (maxLat >= -80.0 && maxLat <= 72.0)
    {
        // Northern limit is in area of normal UTM zones where regions are 8 degrees
        // of latitude high.
        delta = fabs(fmod (maxLat,8.0));
        intMaxLat = static_cast<INT32>(maxLat + ((maxLat >= 0.0) ? (8.0 - delta) : delta));
        northernBandIndex = ((intMaxLat + 80) / 8) + 2;
    }
    else if (maxLat > 72.0 && maxLat <= 84.0)
    {
        // Northern limit is in band X, the region which is 12 degrees high.
        intMaxLat = 84;
        northernBandIndex = 21;
    }
    else if (maxLat > 84.0)
    {
        m_NorthPolar = true;
        intMaxLat = 90;
        northernBandIndex = 22;
    }

    // This function does not return the polar regions, as it is likely that doing
    // in this function which UTM zone specific will cause several duplicates.  So
    // we filter the polar band out.
    if (southernBandIndex <  2) southernBandIndex =  2;
    if (northernBandIndex > 21) northernBandIndex = 21;

    // OK, generate the regions which interect (even to the smallest degree) the
    // the latitude limits we were provided.
    if (southernBandIndex <= northernBandIndex)
    {
        for (bandIndex = southernBandIndex;bandIndex <= northernBandIndex;bandIndex += 1)
        {
            Ptr<CCoordinateSystemMgrsMajorRegion> rgnPtr = new CCoordinateSystemMgrsMajorRegion (utmZoneNbr,bandIndex);
            if (rgnPtr->IsValid ())
            {
                m_MajorRegionCollection->Add (rgnPtr);
            }
        }
    }
}
bool CCoordinateSystemMgrsMajorRegionCollection::IncludesSouthPolarRegion (void)
{
    return m_SouthPolar;
}
bool CCoordinateSystemMgrsMajorRegionCollection::IncludesNorthPolarRegion (void)
{
    return m_NorthPolar;
}
CCoordinateSystemMgrsMajorRegionCollection::~CCoordinateSystemMgrsMajorRegionCollection (void)
{
    m_MajorRegionCollection->Clear ();
}
INT32 CCoordinateSystemMgrsMajorRegionCollection::GetCount () const
{
    INT32 itemCount = m_MajorRegionCollection->GetCount ();
    return itemCount;
}
CCoordinateSystemMgrsMajorRegion* CCoordinateSystemMgrsMajorRegionCollection::GetItem (INT32 index) const
{
    CCoordinateSystemMgrsMajorRegion *itemPtr = static_cast<CCoordinateSystemMgrsMajorRegion*>(m_MajorRegionCollection->GetItem (index));
    return itemPtr;
}
void CCoordinateSystemMgrsMajorRegionCollection::RemoveAt (INT32 index)
{
    m_MajorRegionCollection->RemoveAt (index);
}
void CCoordinateSystemMgrsMajorRegionCollection::Clear()
{
    m_MajorRegionCollection->Clear ();
}
void CCoordinateSystemMgrsMajorRegionCollection::SetItem (INT32 index, CCoordinateSystemMgrsMajorRegion* value)
{
    m_MajorRegionCollection->SetItem (index,value);
}
void CCoordinateSystemMgrsMajorRegionCollection::Add (CCoordinateSystemMgrsMajorRegion* value)
{
    m_MajorRegionCollection->Add (value);
}
void CCoordinateSystemMgrsMajorRegionCollection::Dispose (void)
{
    delete this;
}

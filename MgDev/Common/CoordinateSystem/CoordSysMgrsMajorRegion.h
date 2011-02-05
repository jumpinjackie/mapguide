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

#ifndef _CCOORDINATESYSTEMMGRSMAJORTREGION_H_
#define _CCOORDINATESYSTEMMGRSMAHJORREGION_H_

namespace CSLibrary
{

// All region, major and minor, are rectangular in their native coordinates:
// Lat/Long for the major regions and UTM coordinates for minor regions.  We
// have this helper object for the major regions as there screwy things go on
// at the northern extent.  Specifically, band X is actually 12 degrees
// high, zones 31 thru 37 in band X are screwy, and in band V zones 31 and 32
// are screwy.  We intend to use this object so that there is only one copy of
// the code necessary to deal with all this screwy-ness.
//
// Essentially, given a grid boundary in terms of lat/long min/max, we generate
// a collection of these objects which represents the regions which intersect
// the geographic region provided.  Then, as need dictate, MGRS graticule lines
// and/or Grid Regions are manufactured from the the resulting collection.
//
// A lot of busy work, but required to get this done right.

class CCoordinateSystemMgrsMajorRegion : public MgGuardDisposable
{
public:
    CCoordinateSystemMgrsMajorRegion (INT32 utmZoneNbr,INT32 bandIndex);

    // This constructor is used for polar regions.  The central meridian
    // paremeter is the central longitude of the view port as seen from the
    // the equator and is used to determine what portion of the polar
    // graticules are considered east/west/south/north lines.  This
    // becomes very important for labeling purposes.
    CCoordinateSystemMgrsMajorRegion (INT32 utmZoneNbr,double centralMeridian,INT32 latIndex);
    ~CCoordinateSystemMgrsMajorRegion (void);

    bool IsValid (void);
    STRING GetDesignation (void);
    INT32 GetUtmZoneNbr (void);
    INT32 GetMgrsBandINdex (void);
    double GetCentralMeridian (void);
    double GetWestEdgeLng (void);
    double GetSouthEdgeLat (void);
    double GetEastEdgeLng (void);
    double GetNorthEdgeLat (void);

protected:
    void Dispose (void);

private:
    bool m_IsValid;             // Set to false for non-existent regions
    STRING m_Designation;       // MGRS major region designation.
    INT32 m_UtmZoneNbr;         // positive is north, negative south, 61 polar, 0 == invalid.
    INT32 m_MgrsBandIndex;      // 'A' thru 'Z', sans 'I' and 'O', zero based
    double m_CentralMeridian;   // in degrees, east longitude positive.
    double m_WestLng;           // longitude of western edge of the region, degrees positive east
    double m_SouthLat;          // latitude of the southern edge of the region, degrees positive north
    double m_EastLng;           // longitude of eastern edge of the region, degrees positive east
    double m_NorthLat;          // latitude of the southern edge of the region, degrees positive north

    ///////////////////////////////////////////////////////////////////////////
    // Not implemented
    CCoordinateSystemMgrsMajorRegion (const CCoordinateSystemMgrsMajorRegion& source);
    CCoordinateSystemMgrsMajorRegion& operator= (const CCoordinateSystemMgrsMajorRegion& rhs);
};

// The following is a collection of the above.  The magic here, of course,
// is that the constructor of this object builds the entire collection.
class CCoordinateSystemMgrsMajorRegionCollection : public MgGuardDisposable
{
public:
    CCoordinateSystemMgrsMajorRegionCollection (void);
    CCoordinateSystemMgrsMajorRegionCollection (INT32 utmZoneNbr,double minLat,double maxLat);
    ~CCoordinateSystemMgrsMajorRegionCollection (void);

    bool IncludesSouthPolarRegion (void);
    bool IncludesNorthPolarRegion (void);
    INT32 GetCount () const;
    CCoordinateSystemMgrsMajorRegion* GetItem (INT32 index) const;
    void RemoveAt (INT32 index);
    void Clear();
    void SetItem (INT32 index, CCoordinateSystemMgrsMajorRegion* value);
    void Add (CCoordinateSystemMgrsMajorRegion* value);

protected:
    void Dispose (void);

private:
    // Data Members
    bool m_SouthPolar;
    bool m_NorthPolar;
    Ptr<MgDisposableCollection> m_MajorRegionCollection;

    // Member functions not implemented.
    CCoordinateSystemMgrsMajorRegionCollection  (const CCoordinateSystemMgrsMajorRegionCollection & source);
    CCoordinateSystemMgrsMajorRegionCollection& operator= (const CCoordinateSystemMgrsMajorRegionCollection & rhs);
};

}       // CSLibrary namespace

#endif  // _CCOORDINATESYSTEMMGRSMAJORTREGION_H_

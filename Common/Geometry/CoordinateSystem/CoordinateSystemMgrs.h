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

#ifndef _MGCOORDINATESYSTEMMGRS_H_
#define _MGCOORDINATESYSTEMMGRS_H_

/// \defgroup MgCoordinateSystemMgrs MgCoordinateSystemMgrs
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// This object carries most, if not all, intelligence specific to the MGRS
/// (Military Grid Reference System) grid standard.  It can be used at two
/// distinct levels, depending upon how it is manufactured.
///
/// When manufactured with an ellipsoid/datum specification, this object is
/// suitable only for conversion of geographic coordinates to MGRS strings and
/// vice-versa.
///
/// WHen manufactured with a frame coordinate system with a grid boundary
/// subsequently supplied, the resulting object is capable of NGRS string
/// conversions AND the generation of MGRS grids.
///
/// When manufactured using a frame coordinate system and subsequent
/// provision of a frame boundary (see base class MgCoordinateSystemGridBase)
/// the members derived from the MgCoordinateSystemGridBase base class
/// can be used for MGRS grid drawing purposes.
class MgCoordinateSystemMgrs : public MgCoordinateSystemGridBase
{
PUBLISHED_API:
    // section that reads/writes MGRS coordinates
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a geographic coordinate to MGRS string form.
    /// \param dLongitude (double)
    /// Longitude of the geographic position to be converted, in degrees
    /// relative to Greenwich.  Use negative value for west longitude.
    /// \param dLatitude (double)
    /// Latitude of the geographic position to be converted, in degrees.
    /// Use negative value for south latitude.
    /// \param nPrecision (int)
    /// A value between zero and five, indicates the precision of the
    /// numeric portion of the returned MGRS designation.
    /// \return
    /// The MGRs representation for the given point.
    virtual STRING ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts a geographic coordinate to MGRS string form.
    /// \param pLonLat (String/string)
    /// The geographic position to be converted.  X coordinate is the longitude
    /// in degrees relative to Greenwich, negative indicating west longitude.
    /// Y coordinate is latitude in degrees, negative indicating south latitude.
    /// \param nPrecision (int)
    /// A value between zero and five, indicates the precision of the
    /// numeric portion of the returned MGRS designation.
    /// \return
    /// The MGRs representation for the given point.
    virtual STRING ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts an MGRS string to geographic coordinates.
    /// \param sMgrs (String/string)
    /// The MGRS string to be converted.
    /// \return
    /// The geographic coordinates of the location identified by the provided
    /// MGRS string (center of the grid sqaure).
    virtual MgCoordinate* ConvertToLonLat(CREFSTRING sMgrs)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts an MGRS string to geographic coordinates.
    /// \param sMgrs (String/string)
    /// The MGRS string to be converted.
    /// \param grdSqrPosition (int)
    /// A value from the MgCoordinateSystemMgrsGridSquarePosition enumeration
    /// which indicates the position within the grid sqaure referenced by
    /// the sMgrs argumjent which is to be returned.
    /// \return
    /// The geographic coordinates of the location identified by the provided
    /// MGRS string.
    virtual MgCoordinate* ConvertToLonLat(CREFSTRING sMgrs, INT32 grdSqrPosition)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a value as defined by the MgCoordinateSystemMgrsLetteringScheme
    /// object indicating which lettering scheme is currently active.
    /// \return
    /// Either MgCoordinateSystemMgrsLetteringScheme::Normal or
    /// MgCoordinateSystemMgrsLetteringScheme::Alternative
    virtual INT8 GetLetteringScheme()=0;

INTERNAL_API:
    //section that reads/writes MGRS coordinates
    virtual INT32 ConvertFromLonLat(double dLongitude, double dLatitude, INT32 nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertFromLonLat(MgCoordinate* pLonLat, INT32 nPrecision, REFSTRING sMgrs)=0;
    virtual INT32 ConvertToLonLat(CREFSTRING sMgrs, MgCoordinate* pLonLat, INT32 grdSqrPosition)=0;

protected:
    INT32 GetClassId(){return m_cls_id;};

CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemMgrs;
};

/// \}

#endif //_MGCOORDINATESYSTEMMGRS_H_

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

#ifndef _MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMATIONMETHOD_H_
#define _MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMATIONMETHOD_H_

/// \defgroup MgCoordinateSystemGeodeticStandaloneTransformationMethod MgCoordinateSystemGeodeticStandaloneTransformationMethod
/// \ingroup Coordinate_System_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// This class defines all transformation methods that are built into
/// the transformation engine. That is, when creating such a geodetic transformation
/// the API client cannot specify any parameters because they are well known
/// and are available to the engine already.
/// The constants defined below are only valid for MgCoordinateSystemGeodeticTransformDef instances
/// of type MgCoordinateSystemGeodeticTransformDefType.Standalone.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticStandaloneTransformationMethod
{
PUBLISHED_API:

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constant available for programming convenience only. No specific
    /// geodetic transformation can be created from this constant.
    static const INT32 None = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Describes a geodetic transformation that is actually a NOOP, i.e.
    /// the 2 datums to be transformed between are considered equal.
    static const INT32 NullX = 4097; //cs_DTCMTH_NULLX

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Describes the WGS72 to WGS84 geodetic transformation as published by the DMA (DMA TR 8350.2-B).
    /// All required parameters are known and built-in.
    static const INT32 Wgs72 = 4098; //cs_DTCMTH_WGS72
};
/// \}

#endif //_MGCOORDINATESYSTEMGEODETICSTANDALONETRANSFORMATIONMETHOD_H_

//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGMAPPINGDEFS_H
#define MGMAPPINGDEFS_H

/// \defgroup MgPageUnitsType MgPageUnitsType
/// \ingroup Mapping_Service_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines page size units for the Mapping Service.
///
/// \todo
///   * [[TO DO: I should change my examples to use this
///     instead of hardcoding the string.]]
///
class MG_MAPGUIDE_API MgPageUnitsType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// Page units are in inches.
    static const STRING Inches;           ///\if INTERNAL value("in") \endif

    /// Page units are in millimeters.
    static const STRING Millimeters;      ///\if INTERNAL value("mm") \endif
};
/// \}


/// \defgroup MgCoordinateSpace MgCoordinateSpace
/// \ingroup Mapping_Service_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Specifies the coordinate space for feature geometry returned from QueryFeatures.
/// Can also specify that only non-geometric data should be returned by QueryFeature.
///
class MG_MAPGUIDE_API MgCoordinateSpace
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// \brief
    /// Specifies that geometry properties should not be returned by QueryFeature.
    ///
    static const STRING None;       ///\if INTERNAL value("None") \endif

    /// \brief
    /// Specifies that geometry properties should be returned by QueryFeature in
    /// the coordinate system space of the map they belong to.
    ///
    static const STRING Map;        ///\if INTERNAL value("Map") \endif

    /// \brief
    /// Specifies that geometry properties should be returned by QueryFeature in
    /// the coordinate system space of the display in the client.
    ///
    static const STRING Display;    ///\if INTERNAL value("Display") \endif
};
/// \}


/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \internal
/// \brief
/// Operation IDs for Mapping Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgMappingServiceOpId
{
INTERNAL_API:
    static const int GenerateMap                = 0x1111EE01;
    static const int GenerateMapUpdate          = 0x1111EE02;
    static const int GeneratePlot               = 0x1111EE03;
    static const int GeneratePlotWithLayout     = 0x1111EE04;
    static const int GenerateMultiPlot          = 0x1111EE05;
    static const int GenerateLegendPlot         = 0x1111EE06;
    static const int QueryFeatures              = 0x1111EE07;
    static const int QueryFeatures2             = 0x1111EE08;
    static const int GeneratePlot2              = 0x1111EE09;
    static const int GeneratePlot3              = 0x1111EE0A;
    static const int GenerateLegendImage        = 0x1111EE0D;
};
/// \endcond

#endif

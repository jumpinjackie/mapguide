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

#ifndef DESKTOP_UNITTYPE_H
#define DESKTOP_UNITTYPE_H

/// \ingroup Desktop_Misc_Module
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgdUnitType defines enumerated values used to determine the type
/// of units for plot decorations (scale bar).
class MG_DESKTOP_API MgdUnitType
{
PUBLISHED_API:
    /// \if INTERNAL
    /// The "value(xxx)" comments are used by SWIG to build constants.php.  Do not alter them.
    /// \endif

    /// US English.
    static const STRING USEnglish;  ///\if INTERNAL value("US-English") \endif

    /// Metric.
    static const STRING Metric;     ///\if INTERNAL value("Metric") \endif
};

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines page size units for the Mapping Service.
///
/// \todo
///   * [[TO DO: I should change my examples to use this
///     instead of hardcoding the string.]]
///
class MG_DESKTOP_API MgdPageUnitsType
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


/// \ingroup Desktop_Misc_Module
/// \{
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Specifies the coordinate space for feature geometry returned from QueryFeatures.
/// Can also specify that only non-geometric data should be returned by QueryFeature.
///
class MG_DESKTOP_API MgdCoordinateSpace
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

#endif

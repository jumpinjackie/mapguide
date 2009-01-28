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

#ifndef MGUNITTYPE_H
#define MGUNITTYPE_H

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgUnitType defines enumerated values used to determine the type
/// of units for plot decorations (scale bar).
class MG_MAPGUIDE_API MgUnitType
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

#endif

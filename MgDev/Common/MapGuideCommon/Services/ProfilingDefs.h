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

#ifndef MGPROFILINGDEFS_H
#define MGPROFILINGDEFS_H

/// \cond INTERNAL

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Profiling Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgProfilingServiceOpId
{
INTERNAL_API:
    static const int ProfileRenderMap            = 0x1111E601;
    static const int ProfileRenderDynamicOverlay = 0x1111E602;
};
/// \endcond

#endif

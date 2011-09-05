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

#ifndef MGRENDERINGDEFS_H
#define MGRENDERINGDEFS_H

/// \cond INTERNAL

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Rendering Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgRenderingServiceOpId
{
INTERNAL_API:
    static const int RenderMap1                 = 0x1111E901;
    static const int RenderMap2                 = 0x1111E902;
    static const int RenderMap3                 = 0x1111E903;
    static const int RenderMapLegend            = 0x1111E904;
    static const int QueryFeatures              = 0x1111E907;
    static const int RenderTile                 = 0x1111E908;
    static const int RenderDynamicOverlay       = 0x1111E909;
    static const int QueryFeatureProperties     = 0x1111E90A;
    static const int RenderMap4                 = 0x1111E90B;
    static const int RenderDynamicOverlay2      = 0x1111E90C;
    static const int RenderMap5                 = 0x1111E90D;
};
/// \endcond

#endif

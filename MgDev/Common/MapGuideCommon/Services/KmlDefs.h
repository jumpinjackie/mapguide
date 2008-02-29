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

#ifndef MGKMLDEFS_H
#define MGKMLDEFS_H

/// \cond INTERNAL

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Kml Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgKmlServiceOpId
{
INTERNAL_API:
    static const int GetMapKml                 = 0x1111E701;
    static const int GetLayerKml               = 0x1111E702;
    static const int GetFeaturesKml            = 0x1111E703;
};
/// \endcond

#endif

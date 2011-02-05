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

#ifndef MGDRAWINGDEFS_H
#define MGDRAWINGDEFS_H

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Drawing Service.
/// INTERNAL do not document.
///
class MG_MAPGUIDE_API MgDrawingServiceOpId
{
INTERNAL_API:
    static const int GetDrawing                 = 0x1111EC01;
    static const int DescribeDrawing            = 0x1111EC02;
    static const int GetSection                 = 0x1111EC03;
    static const int GetSectionResource         = 0x1111EC04;
    static const int EnumerateLayers            = 0x1111EC05;
    static const int GetLayer                   = 0x1111EC06;
    static const int EnumerateSections          = 0x1111EC07;
    static const int EnumerateSectionResources  = 0x1111EC08;
    static const int GetCoordinateSpace         = 0x1111EC09;
};
/// \endcond

#endif

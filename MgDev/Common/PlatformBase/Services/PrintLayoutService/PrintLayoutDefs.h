//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MGPRINTLAYOUTDEFS_H
#define MGPRINTLAYOUTDEFS_H

/// \cond INTERNAL

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Operation IDs for Print Layout Service.
/// INTERNAL do not document.
///
class MG_PLATFORMBASE_API MgPrintLayoutServiceOpId
{
INTERNAL_API:
    static const int CreatePrintLayout                  = 0x1111D001;
    static const int CreatePrintLayoutElement           = 0x1111D002;
    static const int CreatePrintLayoutElementDefinition = 0x1111D003;
    static const int GetPrintLayoutElement              = 0x1111D004;
    static const int GetPrintLayoutElementDefinition    = 0x1111D005;
};

/// \endcond

#endif

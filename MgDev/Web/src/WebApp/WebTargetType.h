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

#ifndef _MGWEBTARGETTYPE_H_
#define _MGWEBTARGETTYPE_H_

////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Types of UI target for a command
/// </summary>
class MgWebTargetType
{
PUBLISHED_API:
    static const INT32 TaskPane        = 1;
    static const INT32 NewWindow       = 2;
    static const INT32 SpecifiedFrame  = 3;
};

#endif  // _MGWEBTARGETTYPE_H_

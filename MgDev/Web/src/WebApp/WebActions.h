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

#ifndef _MGWEBACTIONS_H_
#define _MGWEBACTIONS_H_

////////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Enumeration of all the possible actions that can be executed from the viewer.
/// </summary>
class MgWebActions
{
    //NOTE: If you change these values, make sure to update the javascript code accordingly
PUBLISHED_API:
    static const INT32 Pan              =  1;
    static const INT32 PanUp            =  2;
    static const INT32 PanDown          =  3;
    static const INT32 PanRight         =  4;
    static const INT32 PanLeft          =  5;
    static const INT32 Zoom             =  6;
    static const INT32 ZoomIn           =  7;
    static const INT32 ZoomOut          =  8;
    static const INT32 ZoomRectangle    =  9;
    static const INT32 ZoomSelection    = 10;
    static const INT32 FitWindow        = 11;
    static const INT32 ViewPrevious     = 12;
    static const INT32 ViewNext         = 13;
    static const INT32 ViewRestore      = 14;
    static const INT32 Select           = 15;
    static const INT32 SelectRadius     = 16;
    static const INT32 SelectPolygon    = 17;
    static const INT32 SelectClear      = 19;
    static const INT32 Refresh          = 20;
    static const INT32 Copy             = 21;
    static const INT32 About            = 22;
    static const INT32 PrintMap         = 23;
    static const INT32 GetPrintablePage = 24;
    static const INT32 Measure          = 25;
    static const INT32 InvokeUrl        = 26;
    static const INT32 Search           = 27;
    static const INT32 Buffer           = 28;
    static const INT32 ViewOptions      = 30;
    static const INT32 InvokeScript     = 31;
    static const INT32 SelectWithin     = 32;
    static const INT32 Help             = 33;
};

#endif // _MGWEBACTIONS_H_

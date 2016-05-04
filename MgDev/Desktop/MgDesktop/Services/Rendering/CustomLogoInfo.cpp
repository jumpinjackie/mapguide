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

#include "MgDesktop.h"
#include "CustomLogoInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object
/// </summary>
///----------------------------------------------------------------------------

MgdCustomLogoInfo::MgdCustomLogoInfo() : m_dXPosition(0), m_dYPosition(0), m_positionUnits(L""),
    m_dSizeWidth(0), m_dSizeHeight(0), m_sizeUnits(L""),
    m_dRotation(0), m_resId(L""), m_name(L"")
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object
/// </summary>
///----------------------------------------------------------------------------

MgdCustomLogoInfo::~MgdCustomLogoInfo()
{
}


///----------------------------------------------------------------------------
/// <summary>
/// Set the X coordinate to position the logo on the page.
/// </summary>
/// <param name="xCoord">
/// The X coordinate of the logo position on the page (in page units).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetX(double xCoord)
{
    m_dXPosition = xCoord;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the Y coordinate to position the logo on the page.
/// </summary>
/// <param name="yCoord">
/// The Y coordinate of the logo position on the page (in page units).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetY(double yCoord)
{
    m_dYPosition = yCoord;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the units for logo position values.
/// </summary>
/// <param name="width">
/// The units of the position values on the page.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetPositionUnits(STRING units)
{
    m_positionUnits = units;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the width of the logo on the page.
/// </summary>
/// <param name="width">
/// The desired width of the logo on the page (in meters).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetWidth(double width)
{
    m_dSizeWidth = width;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the height of the logo on the page.
/// </summary>
/// <param name="height">
/// The desired height of the logo on the page (in meters).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetHeight(double height)
{
    m_dSizeHeight = height;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the units for defining the size of the logo on the page.
/// </summary>
/// <param name="width">
/// The size units for defining the logo on the page.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetSizeUnits(STRING units)
{
    m_sizeUnits = units;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the rotation angle for the logo on the page.
/// </summary>
/// <param name="angle">
/// The desired rotation angle (in degrees).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetRotation(double angle)
{
    m_dRotation = angle;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the resource id of the SymbolLibrary that contain the logo.
/// </summary>
/// <param name="resId">
/// The resource id that contains the desired symbol.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetResourceId(STRING resId)
{
    m_resId = resId;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the name of the logo to use.
/// </summary>
/// <param name="name">
/// The name of the desired symbol in the SymbolLibrary identified by the resource id.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomLogoInfo::SetName(STRING name)
{
    m_name = name;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the X coordinate for the desired logo position on the page.
/// </summary>
/// <return>
/// The X coordinate of the desired logo position (in page units).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomLogoInfo::GetX()
{
    return m_dXPosition;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the Y coordinate for the desired logo position on the page.
/// </summary>
/// <return>
/// The Y coordinate of the desired logo position (in page units).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomLogoInfo::GetY()
{
    return m_dYPosition;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the units for defining the logo position on the page.
/// </summary>
/// <return>
/// The position units for the logo on the page.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomLogoInfo::GetPositionUnits()
{
    return m_positionUnits;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the desired width of the logo on the page.
/// </summary>
/// <return>
/// The desired width of the logo (in meters).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomLogoInfo::GetWidth()
{
    return m_dSizeWidth;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the desired height of the logo on the page.
/// </summary>
/// <return>
/// The desired height of the logo (in meters).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomLogoInfo::GetHeight()
{
    return m_dSizeHeight;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the units for defining the size of the logo on the page.
/// </summary>
/// <return>
/// The units for the logo size on the page.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomLogoInfo::GetSizeUnits()
{
    return m_sizeUnits;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the desired rotation of the logo on the page.
/// </summary>
/// <return>
/// The desired rotation angle of the logo (in degrees).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomLogoInfo::GetRotation()
{
    return m_dRotation;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the resource identifier that contain the desired logo.
/// </summary>
/// <return>
/// The resource identifier of the SymbolLibrary that contain the desired logo.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomLogoInfo::GetResourceId()
{
    return m_resId;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the name of the desired logo in the SymbolLibrary.
/// </summary>
/// <return>
/// The name of the logo in the SymbolLibrary identified by the resource id.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomLogoInfo::GetName()
{
    return m_name;
}

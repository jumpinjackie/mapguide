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
#include "CustomTextInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Default constructor for the object.
/// </summary>
///----------------------------------------------------------------------------

MgdCustomTextInfo::MgdCustomTextInfo()
{
    m_dXPosition = 0.0;
    m_dYPosition = 0.0;
    m_fontHeight = 0.0;
}

///----------------------------------------------------------------------------
/// <summary>
/// Default destructor for the object
/// </summary>
///----------------------------------------------------------------------------

MgdCustomTextInfo::~MgdCustomTextInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the X coordinate of the custom text on the page.
/// </summary>
/// <param name="xCoord">
/// The X coordinate of the custom text on the page (in page units).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetX(double xCoord)
{
    m_dXPosition = xCoord;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the Y coordinate of the custom text on the page.
/// </summary>
/// <param name="yCoord">
/// The Y coordinate of the custom text on the page (in page units).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetY(double yCoord)
{
    m_dYPosition = yCoord;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the position units for the text on the page
/// </summary>
/// <param name="yCoord">
/// The units for the position values of the text on the page.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetPositionUnits(STRING units)
{
    m_positionUnits = units;
}
///----------------------------------------------------------------------------
/// <summary>
/// The the contents of the custom text to be added to the page.
/// </summary>
/// <param name="value">
/// The contents of the custom text to add to the page.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetValue(STRING value)
{
    m_value = value;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the font to use for the custom text.
/// </summary>
/// <param name="fontName">
/// The name of the font to use for the custom text.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetFontName(STRING fontName)
{
    m_fontName = fontName;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the font height for the custom text.
/// </summary>
/// <param name="height">
/// The height of the font to use for the custom text (in meters).
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetFontHeight(double height)
{
    m_fontHeight = height;
}

///----------------------------------------------------------------------------
/// <summary>
/// Set the font size units for the custom text.
/// </summary>
/// <param name="height">
/// The units for the font size of the custom text on the page.
/// </param>
///----------------------------------------------------------------------------

void MgdCustomTextInfo::SetSizeUnits(STRING units)
{
    m_sizeUnits = units;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the X coordinte of the custom text on the page.
/// </summary>
/// <return>
/// The X coordinate of the custom text (in page units).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomTextInfo::GetX()
{
    return m_dXPosition;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the Y coordiante of the custom text on the page.
/// </summary>
/// <return>
/// The Y coordinate of the custom text (in page units).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomTextInfo::GetY()
{
    return m_dYPosition;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the units for the custom text position values.
/// </summary>
/// <return>
/// The position units for the custom text on the page.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomTextInfo::GetPositionUnits()
{
    return m_positionUnits;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the contents of the custom text to add to the page.
/// </summary>
/// <return>
/// The text to add to the page.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomTextInfo::GetValue()
{
    return m_value;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the name of the font to use for custom text.
/// </summary>
/// <return>
/// The font name of the custom text.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomTextInfo::GetFontName()
{
    return m_fontName;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the font height to use for the custom text.
/// </summary>
/// <return>
/// The font height of the custom text (in meters).
/// </return>
///----------------------------------------------------------------------------

double MgdCustomTextInfo::GetFontHeight()
{
    return m_fontHeight;
}

///----------------------------------------------------------------------------
/// <summary>
/// Retrieve the units for the font size of the custom text.
/// </summary>
/// <return>
/// The font size units for the custom text on the page.
/// </return>
///----------------------------------------------------------------------------

STRING MgdCustomTextInfo::GetSizeUnits()
{
    return m_sizeUnits;
}

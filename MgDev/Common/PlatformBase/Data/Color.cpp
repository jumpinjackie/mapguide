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

#include "PlatformBase.h"

MG_IMPL_DYNCREATE(MgColor);

//////////////////////////////////////////////////////////////////
///<summary>
/// Default Constructor
///</summary>
///
MgColor::MgColor()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgColor object from a Color specified as 0xRRGGBBAA
///</summary>
///<param name="color">Color specified as 0xRRGGBBAA</param>
///
MgColor::MgColor(CREFSTRING color)
{
    UINT32 colorVal = 0;
    swscanf(color.c_str(), L"%10X", &colorVal);
    UINT32 littleEndianColorVal = MG_HTONL(colorVal);
    m_red = (littleEndianColorVal & 0xFF000000) >> 24;
    m_green = (littleEndianColorVal & 0x00FF0000) >> 16;
    m_blue = (littleEndianColorVal & 0x0000FF00) >> 8;
    m_alpha = (littleEndianColorVal & 0x000000FF);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgColor object from red, green, blue values.
///</summary>
///
MgColor::MgColor(INT16 red, INT16 green, INT16 blue) :
    m_red(red), m_green(green), m_blue(blue), m_alpha(255)
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgColor object from red, green, blue, alpha values.
///</summary>
///
MgColor::MgColor(INT16 red, INT16 green, INT16 blue, INT16 alpha) :
    m_red(red), m_green(green), m_blue(blue), m_alpha(alpha)
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgColor object.
///</summary>
///
MgColor::~MgColor()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns Color specified as 0xRRGGBBAA
/// </summary>
STRING MgColor::GetColor()
{
    char rgb[9];
    sprintf(rgb, "%02x%02x%02x%02x", m_red, m_green, m_blue, m_alpha);
    string srgb = rgb;
    return MgUtil::MultiByteToWideChar(srgb);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// return the color red value
/// </summary>
INT16 MgColor::GetRed()
{
    return m_red;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// return the color green value
/// </summary>
INT16 MgColor::GetGreen()
{
    return m_green;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// return the color blue value
/// </summary>
INT16 MgColor::GetBlue()
{
    return m_blue;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// return the color alpha value
/// </summary>
INT16 MgColor::GetAlpha()
{
    return m_alpha;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgColor::Serialize(MgStream* stream)
{
    stream->WriteInt16(m_red);
    stream->WriteInt16(m_green);
    stream->WriteInt16(m_blue);
    stream->WriteInt16(m_alpha);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgColor::Deserialize(MgStream* stream)
{
    stream->GetInt16(m_red);
    stream->GetInt16(m_green);
    stream->GetInt16(m_blue);
    stream->GetInt16(m_alpha);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the class Id.
/// </summary>
/// <returns>
/// Returns the integer value of the class Id.
/// </returns>
INT32 MgColor::GetClassId()
{
    return m_cls_id;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method.
/// </summary>
void MgColor::Dispose()
{
    delete this;
}

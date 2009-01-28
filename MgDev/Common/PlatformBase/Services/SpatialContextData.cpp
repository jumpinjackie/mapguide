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

#include "PlatformBase.h"
#include "SpatialContextData.h"
#include "SpatialContextExtentType.h"
#include "GeometryCommon.h"

MG_IMPL_DYNCREATE(MgSpatialContextData)

//////////////////////////////////////////////////////////////
MgSpatialContextData::MgSpatialContextData()
{
    m_name = L"";
    m_desc = L"";
    m_coord = L"";
    m_extentType = 0;
    m_xyTolerance = 0;
    m_zTolerance = 0;
    m_isActive = false;
}

//////////////////////////////////////////////////////////////
MgSpatialContextData::~MgSpatialContextData()
{
}

//////////////////////////////////////////////////////////////
STRING MgSpatialContextData::GetName()
{
    return m_name;
}

//////////////////////////////////////////////////////////////
STRING MgSpatialContextData::GetDescription()
{
    return m_desc;
}

//////////////////////////////////////////////////////////////
STRING MgSpatialContextData::GetCoordinateSystem()
{
    return m_coord;
}

//////////////////////////////////////////////////////////////
INT32 MgSpatialContextData::GetExtentType()
{
    return m_extentType;
}

//////////////////////////////////////////////////////////////
MgByte* MgSpatialContextData::GetExtent()
{
    return SAFE_ADDREF((MgByte*)m_extent);
}

//////////////////////////////////////////////////////////////
double MgSpatialContextData::GetXYTolerance()
{
    return m_xyTolerance;
}

//////////////////////////////////////////////////////////////
double MgSpatialContextData::GetZTolerance()
{
    return m_zTolerance;
}

//////////////////////////////////////////////////////////////
bool MgSpatialContextData::IsActive()
{
    return m_isActive;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetName(CREFSTRING name)
{
    m_name = name;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetDescription(CREFSTRING desc)
{
    m_desc = desc;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetCoordinateSystem(CREFSTRING coordSystem)
{
    m_coord = coordSystem;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetExtentType(INT32 extentType)
{
    m_extentType = extentType;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetExtent(MgByte* extent)
{
    m_extent = SAFE_ADDREF(extent);
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetXYTolerance(double xyTol)
{
    m_xyTolerance =  xyTol;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetZTolerance(double zTol)
{
    m_zTolerance = zTol;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetActiveStatus(bool active)
{
    m_isActive = active;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::Serialize(MgStream* stream)
{
    stream->WriteString(m_name);
    stream->WriteString(m_desc);
    stream->WriteString(m_coord);
    stream->WriteString(m_wktStr);
    stream->WriteInt32(m_extentType);

    // hack to handle null extent
    Ptr<MgByte> tmpExtent = m_extent;
    if (tmpExtent == NULL)
        tmpExtent = new MgByte();

    Ptr<MgByteSource> byteSource = new MgByteSource(tmpExtent);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    stream->WriteStream(byteReader);

    stream->WriteDouble(m_xyTolerance);
    stream->WriteDouble(m_zTolerance);
    stream->WriteBoolean(m_isActive);
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::Deserialize(MgStream* stream)
{
    stream->GetString(m_name);
    stream->GetString(m_desc);
    stream->GetString(m_coord);
    stream->GetString(m_wktStr);
    stream->GetInt32(m_extentType);

    Ptr<MgByteReader> byteReader = stream->GetStream();
    Ptr<MgByteSink> byteSink = new MgByteSink(byteReader);
    m_extent = byteSink->ToBuffer();

    stream->GetDouble(m_xyTolerance);
    stream->GetDouble(m_zTolerance);
    stream->GetBoolean(m_isActive);
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::SetCoordinateSystemWkt(CREFSTRING wktString)
{
    m_wktStr = wktString;
}

//////////////////////////////////////////////////////////////
STRING MgSpatialContextData::GetCoordinateSystemWkt()
{
    return m_wktStr;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextData::ToXml(string& str)
{
    str += "<SpatialContext IsActive=";

    // Whether it is active or not
    if (this->m_isActive)
    {
        str += "\"true\">";
    }
    else
    {
        str += "\"false\">";
    }

    str += "<Name>" + MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(m_name)) + "</Name>";
    str += "<Description>" + MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(m_desc)) + "</Description>";
    str += "<CoordinateSystemName>" + MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(m_coord)) + "</CoordinateSystemName>";
    str += "<CoordinateSystemWkt>" + MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(m_wktStr)) + "</CoordinateSystemWkt>";

    // Extent Type
    if (m_extentType == MgSpatialContextExtentType::scDynamic)
    {
        str += "<ExtentType>Dynamic</ExtentType>";
    }
    else if (m_extentType == MgSpatialContextExtentType::scStatic)
    {
        str += "<ExtentType>Static</ExtentType>";
    }

    // Geometry data in Hex
    str += "<Extent>";
    if (m_extent != NULL)
    {
        Ptr<MgByteSource> byteSource = new MgByteSource(m_extent);
        Ptr<MgByteReader> byteReader = byteSource->GetReader();

        MgAgfReaderWriter agfReader;
        Ptr<MgGeometry> geom = agfReader.Read(byteReader);
        if (geom != NULL)
        {
            Ptr<MgEnvelope> geomEnvl = geom->Envelope();
            if (geomEnvl != NULL)
            {
                geomEnvl->ToXml(str); // TODO: we need this method
            }
        }
    }
    str += "</Extent>";

    // XYZ Tolerance
    char bufXYTol[128]; bufXYTol[0] = 0;
    char bufZTol[128]; bufZTol[0] = 0;
    sprintf(bufXYTol, "%lf", m_xyTolerance);
    sprintf(bufZTol, "%lf", m_zTolerance);

    str += "<XYTolerance>" + string(bufXYTol) + "</XYTolerance>";
    str += "<ZTolerance>" + string(bufZTol) + "</ZTolerance>";

    str += "</SpatialContext>";
}

//////////////////////////////////////////////////////////////
bool MgSpatialContextData::CanSetName()
{
    return true;
}

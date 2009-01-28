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
#include "SpatialContextReader.h"

MG_IMPL_DYNCREATE(MgSpatialContextReader)

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the spatial context currently being read.
/// </summary>
/// <returns>Returns the name of the spatial context.</returns>
STRING MgSpatialContextReader::GetName()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetName();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the description of the spatial context currently being read.
/// </summary>
/// <returns>Returns the description of the spatial context.</returns>
STRING MgSpatialContextReader::GetDescription()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetDescription();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the coordinate system of the spatial context currently
/// being read. If the result is a null or empty string, the coordinate system is
/// unknown to the spatial context.
/// </summary>
/// <returns>Returns the coordinate system name of the spatial context.</returns>
STRING MgSpatialContextReader::GetCoordinateSystem()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetCoordinateSystem();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the coordinate system in OpenGIS SRS WKT format of the spatial context currently
/// being read. If the result is an empty string, the coordinate system WKT description is
/// not available.</summary>
/// </summary>
/// <returns>Returns the coordinate system name of the spatial context.</returns>
STRING MgSpatialContextReader::GetCoordinateSystemWkt()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetCoordinateSystemWkt();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the extent type of the spatial context currently being read.
/// </summary>
/// <returns>Returns the extent type.</returns>
/// <remarks>The type is a value of SpatialContextExtentType.</remarks>
INT32 MgSpatialContextReader::GetExtentType()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetExtentType();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the extent of the spatial context currently being read.
/// </summary>
/// <returns>Returns the geometry</returns>
MgByteReader* MgSpatialContextReader::GetExtent()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    Ptr<MgByte> extents = sData->GetExtent();

    Ptr<MgByteSource> byteSource = new MgByteSource(extents);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    return SAFE_ADDREF((MgByteReader*)byteReader);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the tolerance value for XY ordinates of the spatial context
/// currently being read. Tolerances are used in some geometric tests,
/// mostly for equality between coordinates.  This occurs frequently in spatial
/// queries, especially with "on boundary" or "just touching" conditions.
/// It is also an objective amount that can be used when formulating step
/// distances for parametric curve interpolation.
/// </summary>
/// <returns>Returns the tolerance</returns>
double MgSpatialContextReader::GetXYTolerance()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetXYTolerance();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the tolerance value for Z ordinates of the spatial context
/// currently being read. Tolerances are used in some geometric tests,
/// mostly for equality between coordinates.  This occurs a frequently in spatial
/// queries, especially with "on boundary" or "just touching" conditions.
/// It is also an objective amount that can be used when formulating step
/// distances for parametric curve interpolation.
/// </summary>
/// <returns>Returns the tolerance</returns>
double MgSpatialContextReader::GetZTolerance()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->GetZTolerance();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns true if the spatial context currently being read is the active
/// spatial context.
/// </summary>
/// <returns>Returns true if the current spatial context is the active one.</returns>
bool MgSpatialContextReader::IsActive()
{
    Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
    return sData->IsActive();
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Advances the reader to the next item. The default position of the reader
/// is prior to the first item. Thus, you must call ReadNext to begin
/// accessing any data.
/// </summary>
/// <returns>Returns true if there is a next item.</returns>
bool MgSpatialContextReader::ReadNext()
{
    bool moreAvailable = false;
    INT32 cnt = m_spatialContextCol.GetCount();
    m_currPos++;

    if (cnt > 0 && m_currPos < cnt)
    {
        moreAvailable = true;
    }

    return moreAvailable;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Closes the SpatialContextReader object, freeing any resources it may
/// be holding.
/// </summary>
void MgSpatialContextReader::Close()
{
    m_spatialContextCol.Clear();
}

//////////////////////////////////////////////////////////////
INT32 MgSpatialContextReader::AddSpatialData(MgSpatialContextData* data)
{
    m_spatialContextCol.Add(data);
    return m_spatialContextCol.GetCount();
}

//////////////////////////////////////////////////////////////
MgSpatialContextReader::MgSpatialContextReader() : m_currPos(-1)
{
}

//////////////////////////////////////////////////////////////
MgSpatialContextReader::~MgSpatialContextReader()
{
    this->Close();
}

//////////////////////////////////////////////////////////////
void MgSpatialContextReader::Serialize(MgStream* stream)
{
    INT32 cnt = m_spatialContextCol.GetCount();
    stream->WriteInt32(cnt);
    stream->WriteString(m_providerName);
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(i);
        stream->WriteObject(sData);
    }
}

//////////////////////////////////////////////////////////////
void MgSpatialContextReader::Deserialize(MgStream* stream)
{
    INT32 cnt = 0;
    stream->GetInt32(cnt);
    stream->GetString(m_providerName);
    for (int i=0; i < cnt; i++)
    {
        Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)stream->GetObject();
        m_spatialContextCol.Add(sData);
    }
}

//////////////////////////////////////////////////////////////
void MgSpatialContextReader::SetProviderName(CREFSTRING providerName)
{
    m_providerName = providerName;
}

//////////////////////////////////////////////////////////////
void MgSpatialContextReader::ToXml(string& str)
{
    // this XML follows the FdoSpatialContextList-1.0.0.xsd schema
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    str += "<FdoSpatialContextList>";
    str += "<ProviderName>" + MgUtil::WideCharToMultiByte(m_providerName) + "</ProviderName>";

    while (this->ReadNext())
    {
        Ptr<MgSpatialContextData> sData = (MgSpatialContextData*)m_spatialContextCol.GetItem(m_currPos);
        sData->ToXml(str);
    }

    str += "</FdoSpatialContextList>";
}

//////////////////////////////////////////////////////////////
MgByteReader* MgSpatialContextReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    byteSource->SetMimeType(MgMimeType::Xml);

    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    return SAFE_ADDREF((MgByteReader*)byteReader);
}

void MgSpatialContextReader::Reset()
{
    m_currPos = -1;
}

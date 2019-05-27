//
//  Copyright (C) 2004-2019 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "Metatile.h"

MG_IMPL_DYNCREATE(MgMetatile)

MgMetatile::MgMetatile() 
    : m_width(0), m_height(0), m_metaTilingFactor(0), m_requestedWidth(0), m_requestedHeight(0)
{

}

MgMetatile::MgMetatile(MgByteReader* image, INT32 width, INT32 height, INT32 requestedWidth, INT32 requestedHeight, CREFSTRING imageFormat, INT32 metaTilingFactor)
    : m_width(width), m_height(height), m_requestedWidth(requestedWidth), m_requestedHeight(requestedHeight), m_imageFormat(imageFormat), m_metaTilingFactor(metaTilingFactor)
{
    m_image = SAFE_ADDREF(image);
}

MgMetatile::~MgMetatile()
{
    m_image = NULL;
}

MgByteReader* MgMetatile::GetImage()
{
    return SAFE_ADDREF(m_image.p);
}

INT32 MgMetatile::GetRequestedWidth()
{
    return m_requestedWidth;
}

INT32 MgMetatile::GetRequestedHeight()
{
    return m_requestedHeight;
}

INT32 MgMetatile::GetWidth()
{
    return m_width;
}

INT32 MgMetatile::GetHeight()
{
    return m_height;
}

STRING MgMetatile::GetTileImageFormat()
{
    return m_imageFormat;
}

INT32 MgMetatile::GetMetaTilingFactor()
{
    return m_metaTilingFactor;
}

void MgMetatile::Serialize(MgStream * stream)
{
    stream->WriteInt32(m_requestedWidth);
    stream->WriteInt32(m_requestedHeight);
    stream->WriteInt32(m_width);
    stream->WriteInt32(m_height);
    stream->WriteInt32(m_metaTilingFactor);
    stream->WriteString(m_imageFormat);
    stream->WriteObject(m_image);
}

void MgMetatile::Deserialize(MgStream * stream)
{
    stream->GetInt32(m_requestedWidth);
    stream->GetInt32(m_requestedHeight);
    stream->GetInt32(m_width);
    stream->GetInt32(m_height);
    stream->GetInt32(m_metaTilingFactor);
    stream->GetString(m_imageFormat);
    m_image = static_cast<MgByteReader*>(stream->GetObject());
}

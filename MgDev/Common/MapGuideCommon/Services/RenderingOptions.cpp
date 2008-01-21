//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "RenderingOptions.h"

MG_IMPL_DYNCREATE(MgRenderingOptions)

MgRenderingOptions::MgRenderingOptions(CREFSTRING format, INT32 behavior, MgColor* selectionColor)
{
    m_format = format;
    m_behavior = behavior;
    m_selectionColor = SAFE_ADDREF(selectionColor);
}

STRING MgRenderingOptions::GetImageFormat()
{
    return m_format;
}

INT32 MgRenderingOptions::GetBehavior()
{
    return m_behavior;
}

MgColor* MgRenderingOptions::GetSelectionColor()
{
    return SAFE_ADDREF((MgColor*)m_selectionColor);
}

void MgRenderingOptions::Serialize(MgStream* stream)
{
    stream->WriteString(m_format);
    stream->WriteInt32(m_behavior);
    stream->WriteObject(m_selectionColor);
}

void MgRenderingOptions::Deserialize(MgStream* stream)
{
    stream->GetString(m_format);
    stream->GetInt32(m_behavior);
    m_selectionColor = (MgColor*)stream->GetObject();
}

MgRenderingOptions::MgRenderingOptions()
{
}

MgRenderingOptions::~MgRenderingOptions()
{
}

INT32 MgRenderingOptions::GetClassId()
{
    return m_cls_id;
}

void MgRenderingOptions::Dispose()
{
    delete this;
}

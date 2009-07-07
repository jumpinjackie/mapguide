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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// MgWebUiSizablePane constructor
//
MgWebUiSizablePane::MgWebUiSizablePane()
: m_width(200)
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the width of this UI pane
//
INT32 MgWebUiSizablePane::GetWidth()
{
    return m_width;
}

///////////////////////////////////////////////////////////////////////////
// Sets the width of this pane
//
void MgWebUiSizablePane::SetWidth(INT32 width)
{
    m_width = width;
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebUiSizablePane::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebUiSizablePane::Dispose()
{
    delete this;
}

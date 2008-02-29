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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// Returns the collection of widgets in this toolbar
//
MgWebWidgetCollection* MgWebToolBar::GetWidgets()
{
    return SAFE_ADDREF((MgWebWidgetCollection*)m_widgets);
}

///////////////////////////////////////////////////////////////////////////
// Constructor
//
MgWebToolBar::MgWebToolBar()
{
    if(!(m_widgets = new MgWebWidgetCollection()))
        throw new MgOutOfMemoryException(L"MgWebToolBar.MgWebToolBar", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebToolBar::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object.
//
void MgWebToolBar::Dispose()
{
    delete this;
}

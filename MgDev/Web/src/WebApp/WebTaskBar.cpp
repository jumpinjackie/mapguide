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

#include "WebApp.h"

///////////////////////////////////////////////////////////////////////////
// ctor
//
MgWebTaskBar::MgWebTaskBar()
{
    if (!(m_taskList = new MgWebWidgetCollection()) || !(m_taskButtons = new MgWebWidgetCollection()))
        throw new MgOutOfMemoryException(L"MgWebTaskBar.MgWebTaskBar", __LINE__, __WFILE__, NULL, L"", NULL);

    for (int i=0; i<4; ++i)
    {
        Ptr<MgWebTaskBarWidget> btn = new MgWebTaskBarWidget();
        if (btn == NULL)
            throw new MgOutOfMemoryException(L"MgWebTaskBar.MgWebTaskBar", __LINE__, __WFILE__, NULL, L"", NULL);
        m_taskButtons->Add(btn);
    }
}

///////////////////////////////////////////////////////////////////////////
// Gets the task list
//
MgWebWidgetCollection* MgWebTaskBar::GetTaskList()
{
    return SAFE_ADDREF((MgWebWidgetCollection*)m_taskList);
}

///////////////////////////////////////////////////////////////////////////
// Gets the task buttons
//
MgWebWidgetCollection* MgWebTaskBar::GetTaskButtons()
{
    return SAFE_ADDREF((MgWebWidgetCollection*)m_taskButtons);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebTaskBar::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebTaskBar::Dispose()
{
    delete this;
}

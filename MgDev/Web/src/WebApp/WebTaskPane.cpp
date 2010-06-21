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
MgWebTaskPane::MgWebTaskPane()
{
    if (!(m_taskBar = new MgWebTaskBar()))
        throw new MgOutOfMemoryException(L"MgWebTaskPane.MgWebTaskPane", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Gets the task bar
//
MgWebTaskBar* MgWebTaskPane::GetTaskBar()
{
    return SAFE_ADDREF((MgWebTaskBar*)m_taskBar);
}

///////////////////////////////////////////////////////////////////////////
// Returns the URL of the initial task. Defaults to the 'Getting started' page
//
STRING MgWebTaskPane::GetInitialTaskUrl()
{
    return m_initialTaskUrl;
}

///////////////////////////////////////////////////////////////////////////
// Set the URL of the initial task
//
void MgWebTaskPane::SetInitialTaskUrl(CREFSTRING url)
{
    m_initialTaskUrl = url;
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebTaskPane::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebTaskPane::Dispose()
{
    delete this;
}

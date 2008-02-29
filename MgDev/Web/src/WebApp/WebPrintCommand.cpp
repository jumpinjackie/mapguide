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
// Constructor
//
MgWebPrintCommand::MgWebPrintCommand()
{
    m_action = MgWebActions::PrintMap;
    m_printLayouts = new MgStringCollection();
    if(m_printLayouts == NULL)
        throw new MgOutOfMemoryException(L"MgWebPrintCommand.MgWebPrintCommand", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Returns the number of print layouts to choose from for this command
//
INT32 MgWebPrintCommand::GetPrintLayoutCount()
{
    return m_printLayouts->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the resource id of the print layout at the specified position
//
STRING MgWebPrintCommand::GetPrintLayoutAt(INT32 index)
{
    return m_printLayouts->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Get the print layout collection
//
MgStringCollection* MgWebPrintCommand::GetPrintLayouts()
{
    return SAFE_ADDREF((MgStringCollection*)m_printLayouts);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebPrintCommand::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebPrintCommand::Dispose()
{
    delete this;
}

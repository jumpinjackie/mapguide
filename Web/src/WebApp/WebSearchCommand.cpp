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
// Constructor
//
MgWebSearchCommand::MgWebSearchCommand()
: m_matchLimit(100)
{
    m_action = MgWebActions::Search;
    m_resultColumns = new MgPropertyCollection();
    if(m_resultColumns == NULL)
        throw new MgOutOfMemoryException(L"MgWebSearchCommand.MgWebSearchCommand", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Returns the layer to search.
//
STRING MgWebSearchCommand::GetLayer()
{
    return m_layer;
}

///////////////////////////////////////////////////////////////////////////
// Returns the string displayed to the user to prompt to enter the keyword
//
STRING MgWebSearchCommand::GetPrompt()
{
    return m_prompt;
}

///////////////////////////////////////////////////////////////////////////
// Returns the text of filter expression to be applied
//
STRING MgWebSearchCommand::GetFilter()
{
    return m_filter;
}

///////////////////////////////////////////////////////////////////////////
// Returns the maximum number of result to be returned by the search
//
INT32 MgWebSearchCommand::GetMatchLimit()
{
    return m_matchLimit;
}

///////////////////////////////////////////////////////////////////////////
// Returns the number of result columns
//
INT32 MgWebSearchCommand::GetResultColumnCount()
{
    return m_resultColumns->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Returns the column display name at a given index
//
STRING MgWebSearchCommand::GetColumnDisplayNameAt(INT32 index)
{
    Ptr<MgStringProperty> column = (MgStringProperty*)m_resultColumns->GetItem(index);
    return column->GetValue();
}

///////////////////////////////////////////////////////////////////////////
// Returns the result property name at a given index
//
STRING MgWebSearchCommand::GetColumnPropertyNameAt(INT32 index)
{
    Ptr<MgStringProperty> column = (MgStringProperty*)m_resultColumns->GetItem(index);
    return column->GetName();
}

///////////////////////////////////////////////////////////////////////////
// Set the layer to search
//
void MgWebSearchCommand::SetLayer(CREFSTRING layer)
{
    m_layer = layer;
}

///////////////////////////////////////////////////////////////////////////
// Set the string displayed to the user to prompt to enter the keyword
//
void MgWebSearchCommand::SetPrompt(CREFSTRING prompt)
{
    m_prompt = prompt;
}

///////////////////////////////////////////////////////////////////////////
// Set the filter expression
//
void MgWebSearchCommand::SetFilter(CREFSTRING filter)
{
    m_filter = filter;
}

///////////////////////////////////////////////////////////////////////////
// Set the maximum number of result returned
//
void MgWebSearchCommand::SetMatchLimit(INT32 matchLimit)
{
    m_matchLimit = matchLimit;
}

///////////////////////////////////////////////////////////////////////////
// Get the result columns collection
//
MgPropertyCollection* MgWebSearchCommand::GetResultColumns()
{
    return SAFE_ADDREF((MgPropertyCollection*)m_resultColumns);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebSearchCommand::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebSearchCommand::Dispose()
{
    delete this;
}

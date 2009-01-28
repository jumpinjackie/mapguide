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
// contructors
//
MgWebCommand::MgWebCommand(CREFSTRING name, STRING iconUrl, STRING tooltip)
: m_isUsed(false)
{
    m_name = name;
    m_tooltip = tooltip;
    m_iconUrl = iconUrl;
    m_action = 0;
    m_targetViewerType = 0;
}

MgWebCommand::MgWebCommand()
: m_isUsed(false)
{
}

///////////////////////////////////////////////////////////////////////////
// Returns the command name
//
STRING MgWebCommand::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////
// Returns the command label
//
STRING MgWebCommand::GetLabel()
{
    return m_label;
}

///////////////////////////////////////////////////////////////////////////
// Returns the action associated with this command
//
INT32 MgWebCommand::GetAction()
{
    return m_action;
}

///////////////////////////////////////////////////////////////////////////
// Returns the url of the icon representing this command
//
STRING MgWebCommand::GetIconUrl()
{
    return m_iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Returns the url of the icon representing this command in disabled state
//
STRING MgWebCommand::GetDisabledIconUrl()
{
    return m_disabledIconUrl;
}


///////////////////////////////////////////////////////////////////////////
// Returns the tooltip describing this command
//
STRING MgWebCommand::GetTooltip()
{
    return m_tooltip;
}

///////////////////////////////////////////////////////////////////////////
// Returns the long description of this command
//
STRING MgWebCommand::GetDescription()
{
    return m_description;
}

///////////////////////////////////////////////////////////////////////////
// Returns the type of viewer this command is intended for
//
INT32 MgWebCommand::GetTargetViewerType()
{
    return m_targetViewerType;
}

///////////////////////////////////////////////////////////////////////////
// Tells if this commands is used by at least one button, task or menu option
//
bool MgWebCommand::IsUsed()
{
    return m_isUsed;
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebCommand::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
//Dispose this object
//
void MgWebCommand::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Set this command name
//
void MgWebCommand::SetName(CREFSTRING name)
{
    m_name = name;
}

///////////////////////////////////////////////////////////////////////////
// Set this command label
//
void MgWebCommand::SetLabel(CREFSTRING label)
{
    m_label = label;
}

///////////////////////////////////////////////////////////////////////////
// Set this command action
//
void MgWebCommand::SetAction(INT32 action)
{
    m_action = action;
}

///////////////////////////////////////////////////////////////////////////
// Set this command icon URL
//
void MgWebCommand::SetIconUrl(CREFSTRING iconUrl)
{
    m_iconUrl = iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Set this command disabled icon URL
//
void MgWebCommand::SetDisabledIconUrl(CREFSTRING disabledIconUrl)
{
    m_disabledIconUrl = disabledIconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Set this command tooltip
//
void MgWebCommand::SetTooltip(CREFSTRING tooltipText)
{
    m_tooltip = tooltipText;
}

///////////////////////////////////////////////////////////////////////////
// Set this command long description
//
void MgWebCommand::SetDescription(CREFSTRING description)
{
    m_description = description;
}

///////////////////////////////////////////////////////////////////////////
// Set the type of viewer this command is intended for
//
void MgWebCommand::SetTargetViewerType(INT32 targetViewerType)
{
    m_targetViewerType = targetViewerType;
}

///////////////////////////////////////////////////////////////////////////
// Instructs that this command is used
//
void MgWebCommand::SetUsed(bool used)
{
    m_isUsed = used;
}

///////////////////////////////////////////////////////////////////////////
// Serialize data to TCP/IP stream
//
void MgWebCommand::Serialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgWebCommand.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Deserialize data from TCP/IP stream
//
void MgWebCommand::Deserialize(MgStream* stream)
{
    throw new MgNotImplementedException(L"MgWebCommand.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgWebCommand::CanSetName()
{
    return true;
}

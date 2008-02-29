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
MgWebUiTargetCommand::MgWebUiTargetCommand()
:  m_target(MgWebTargetType::TaskPane)
{
}

///////////////////////////////////////////////////////////////////////////
// Returns the UI target for the command. One value of the MgWebTargetType class.
//
INT32 MgWebUiTargetCommand::GetTarget()
{
    return m_target;
}

///////////////////////////////////////////////////////////////////////////
// Returns the name of the target for the command.
// Applicable only if the type of target is 'SpecifiedFrame'
//
STRING MgWebUiTargetCommand::GetTargetName()
{
    return m_targetName;
}

///////////////////////////////////////////////////////////////////////////
// Set the UI target
//
void MgWebUiTargetCommand::SetTarget(INT32 targetType)
{
    m_target = targetType;
}

///////////////////////////////////////////////////////////////////////////
// Set the UI target name
//
void MgWebUiTargetCommand::SetTargetName(STRING targetName)
{
    m_targetName = targetName;
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebUiTargetCommand::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebUiTargetCommand::Dispose()
{
    delete this;
}

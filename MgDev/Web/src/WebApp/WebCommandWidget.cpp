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
// Gets the command associated with this widget
//
MgWebCommand* MgWebCommandWidget::GetCommand()
{
    return SAFE_ADDREF((MgWebCommand*)m_command);
}

///////////////////////////////////////////////////////////////////////////
// ctor
//
MgWebCommandWidget::MgWebCommandWidget() : MgWebWidget(MgWebWidgetType::Command)
{
}

///////////////////////////////////////////////////////////////////////////
// set the command associated with this item
//
void MgWebCommandWidget::SetCommand(MgWebCommand* command)
{
    m_command = SAFE_ADDREF(command);
    if(m_command)
        m_command->SetUsed(true);
}

///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebCommandWidget::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebCommandWidget::Dispose()
{
    delete this;
}

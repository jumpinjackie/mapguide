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
// Gets the name of the button.
//
STRING MgWebTaskBarWidget::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Url of the icon to appear in the button.
//
STRING MgWebTaskBarWidget::GetIconUrl()
{
    return m_iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Url of the disabled icon to appear in the button.
//
STRING MgWebTaskBarWidget::GetDisabledIconUrl()
{
    return m_disabledIconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Returns the tooltip describing this button
//
STRING MgWebTaskBarWidget::GetTooltip()
{
    return m_tooltip;
}

///////////////////////////////////////////////////////////////////////////
// Returns the long description of this button
//
STRING MgWebTaskBarWidget::GetDescription()
{
    return m_description;
}

///////////////////////////////////////////////////////////////////////////
// ctor
//
MgWebTaskBarWidget::MgWebTaskBarWidget() : MgWebWidget(MgWebWidgetType::TaskButton)
{
}

///////////////////////////////////////////////////////////////////////////
// set the name
//
void MgWebTaskBarWidget::SetName(CREFSTRING name)
{
    m_name = name;
}

///////////////////////////////////////////////////////////////////////////
// set the icon url
//
void MgWebTaskBarWidget::SetIconUrl(CREFSTRING iconUrl)
{
    m_iconUrl = iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// set the icon url
//
void MgWebTaskBarWidget::SetDisabledIconUrl(CREFSTRING iconUrl)
{
    m_disabledIconUrl = iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Set this flyout tooltip
//
void MgWebTaskBarWidget::SetTooltip(CREFSTRING tooltipText)
{
    m_tooltip = tooltipText;
}

///////////////////////////////////////////////////////////////////////////
// Set this flyout long description
//
void MgWebTaskBarWidget::SetDescription(CREFSTRING description)
{
    m_description = description;
}


///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebTaskBarWidget::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebTaskBarWidget::Dispose()
{
    delete this;
}

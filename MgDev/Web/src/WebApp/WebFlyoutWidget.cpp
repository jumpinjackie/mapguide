//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
// Gets the label of the flyout widget.
//
STRING MgWebFlyoutWidget::GetLabel()
{
    return m_label;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Url of the icon to appear in the flyout widget.
//
STRING MgWebFlyoutWidget::GetIconUrl()
{
    return m_iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Gets the Url of the disabled icon to appear in the flyout widget.
//
STRING MgWebFlyoutWidget::GetDisabledIconUrl()
{
    return m_disabledIconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Gets the collection of sub items.
//
MgWebWidgetCollection* MgWebFlyoutWidget::GetSubItems()
{
    return SAFE_ADDREF((MgWebWidgetCollection*)m_subItems);
}

///////////////////////////////////////////////////////////////////////////
// Returns the tooltip describing this flyout
//
STRING MgWebFlyoutWidget::GetTooltip()
{
    return m_tooltip;
}

///////////////////////////////////////////////////////////////////////////
// Returns the long description of this flyout
//
STRING MgWebFlyoutWidget::GetDescription()
{
    return m_description;
}

///////////////////////////////////////////////////////////////////////////
// ctor
//
MgWebFlyoutWidget::MgWebFlyoutWidget() : MgWebWidget(MgWebWidgetType::Flyout)
{
}

///////////////////////////////////////////////////////////////////////////
// Add a subItem to this item
//
void MgWebFlyoutWidget::AddSubItem(MgWebWidget* item)
{
    if(m_subItems == NULL)
    {
        m_subItems = new MgWebWidgetCollection();
        if (m_subItems == NULL)
            throw new MgOutOfMemoryException(L"MgWebLayout.AddSubItems", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    m_subItems->Add(item);
}

///////////////////////////////////////////////////////////////////////////
// set the label
//
void MgWebFlyoutWidget::SetLabel(CREFSTRING label)
{
    m_label = label;
}

///////////////////////////////////////////////////////////////////////////
// set the icon url
//
void MgWebFlyoutWidget::SetIconUrl(CREFSTRING iconUrl)
{
    m_iconUrl = iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// set the icon url
//
void MgWebFlyoutWidget::SetDisabledIconUrl(CREFSTRING iconUrl)
{
    m_disabledIconUrl = iconUrl;
}

///////////////////////////////////////////////////////////////////////////
// Set this flyout tooltip
//
void MgWebFlyoutWidget::SetTooltip(CREFSTRING tooltipText)
{
    m_tooltip = tooltipText;
}

///////////////////////////////////////////////////////////////////////////
// Set this flyout long description
//
void MgWebFlyoutWidget::SetDescription(CREFSTRING description)
{
    m_description = description;
}


///////////////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgWebFlyoutWidget::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgWebFlyoutWidget::Dispose()
{
    delete this;
}

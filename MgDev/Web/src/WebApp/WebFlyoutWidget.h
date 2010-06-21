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

#ifndef _MGWEBFLYOUTWIDGET_
#define _MGWEBFLYOUTWIDGET_

class MgWebWidgetCollection;

class MgWebFlyoutWidget;
template class MG_WEBAPP_API Ptr<MgWebFlyoutWidget>;

class MG_WEBAPP_API MgWebFlyoutWidget : public MgWebWidget
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the floput label
    /// </summary>
    /// <returns>
    /// Command name
    /// </returns>
    STRING GetLabel();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Url of the icon to appear in the flyout widget.
    /// </summary>
    /// <returns>
    /// Icon URL
    /// </returns>
    STRING GetIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Url of the disabled icon to appear in the flyout widget.
    /// </summary>
    /// <returns>
    /// Icon URL
    /// </returns>
    STRING GetDisabledIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the collection of sub items
    /// </summary>
    /// <returns>
    /// Collection of sub items
    /// </returns>
    MgWebWidgetCollection* GetSubItems();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the tooltip describing this command
    /// </summary>
    /// <returns>
    /// Tooltip text
    /// </returns>
    STRING GetTooltip();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the long description of this comand. Appears in the status bar
    /// </summary>
    /// <returns>
    /// Tooltip text
    /// </returns>
    STRING GetDescription();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebFlyoutWidget();

    ///////////////////////////////////////////////////////////////////////////
    // set the label of a flyout widget
    //
    void SetLabel(CREFSTRING label);

    ///////////////////////////////////////////////////////////////////////////
    // set the icon url of a flyout widget
    //
    void SetIconUrl(CREFSTRING iconUrl);

    ///////////////////////////////////////////////////////////////////////////
    // set the disabled icon url of a flyout widget
    //
    void SetDisabledIconUrl(CREFSTRING iconUrl);

    ///////////////////////////////////////////////////////////////////////////
    // Add a subItem to this item
    //
    void AddSubItem(MgWebWidget* item);

    ///////////////////////////////////////////////////////////////////////////
    // set the tooltip text
    //
    void SetTooltip(CREFSTRING tooltipText);

    ///////////////////////////////////////////////////////////////////////////
    // set the description text
    //
    void SetDescription(CREFSTRING description);

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object
    //
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_FlyoutWidget;

private:
    Ptr<MgWebWidgetCollection>  m_subItems;
    STRING                      m_label;
    STRING                      m_iconUrl;
    STRING                      m_disabledIconUrl;
    STRING                      m_tooltip;
    STRING                      m_description;
};

#endif // _MGWEBFLYOUTWIDGET_

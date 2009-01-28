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

#ifndef _MGWEBTASKBARWIDGET_
#define _MGWEBTASKBARWIDGET_

class MgWebWidgetCollection;

class MgWebTaskBarWidget;
template class MG_WEBAPP_API Ptr<MgWebTaskBarWidget>;

class MG_WEBAPP_API MgWebTaskBarWidget : public MgWebWidget
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the button name
    /// </summary>
    /// <returns>
    /// Name
    /// </returns>
    STRING GetName();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Url of the icon to appear in the button.
    /// </summary>
    /// <returns>
    /// Icon URL
    /// </returns>
    STRING GetIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Url of the disabled icon to appear in the button.
    /// </summary>
    /// <returns>
    /// Icon URL
    /// </returns>
    STRING GetDisabledIconUrl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the tooltip describing this button
    /// </summary>
    /// <returns>
    /// Tooltip text
    /// </returns>
    STRING GetTooltip();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the long description of this button. Appears in the status bar
    /// </summary>
    /// <returns>
    /// Description
    /// </returns>
    STRING GetDescription();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebTaskBarWidget();

    ///////////////////////////////////////////////////////////////////////////
    // set the name of the button
    //
    void SetName(CREFSTRING name);

    ///////////////////////////////////////////////////////////////////////////
    // set the url of the icon appearing in the button
    //
    void SetIconUrl(CREFSTRING iconUrl);

    ///////////////////////////////////////////////////////////////////////////
    // set the url of the disabled icon appearing in the button
    //
    void SetDisabledIconUrl(CREFSTRING iconUrl);

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
    static const INT32 m_cls_id = WebApp_TaskBarWidget;

private:
    STRING                      m_name;
    STRING                      m_tooltip;
    STRING                      m_description;
    STRING                      m_iconUrl;
    STRING                      m_disabledIconUrl;
};

#endif // _MGWEBTASKBARWIDGET_

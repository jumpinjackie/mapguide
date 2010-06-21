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

#ifndef _MGWEBCONTEXTMENU_H_
#define _MGWEBCONTEXTMENU_H_

class MgWebContextMenu;
template class MG_WEBAPP_API Ptr<MgWebContextMenu>;

class MG_WEBAPP_API MgWebContextMenu : public MgWebWidgetCollection
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Checks if the context menu is visible
    /// </summary>
    /// <returns>
    /// true if the context menu is visible
    /// </returns>
    bool IsVisible();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebContextMenu();

    ///////////////////////////////////////////////////////////////////////////
    // Set the pane visibility
    //
    void SetVisible(bool visible);

protected:
    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object.
    //
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_ContextMenu;

private:
    bool       m_visible;
};

#endif

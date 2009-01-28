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

#ifndef _MGWEBUIPANE_H_
#define _MGWEBUIPANE_H_

class MgWebUiPane;
template class MG_WEBAPP_API Ptr<MgWebUiPane>;

class MG_WEBAPP_API MgWebUiPane : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Checks if the UI pane is visible
    /// </summary>
    /// <returns>
    /// true if the pane is visible
    /// </returns>
    bool IsVisible();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // MgWebUiPane constructor
    //
    MgWebUiPane();

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
    // Dispose this object
    //
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_UiPane;

protected:
    bool       m_visible;
};

#endif  // _MGWEBUIPANE_H_

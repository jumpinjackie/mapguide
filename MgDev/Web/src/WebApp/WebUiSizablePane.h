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

#ifndef _MGWEBUISIZABLEPANE_H_
#define _MGWEBUISIZABLEPANE_H_

class MgWebUiSizablePane;
template class MG_WEBAPP_API Ptr<MgWebUiSizablePane>;

class MG_WEBAPP_API MgWebUiSizablePane : public MgWebUiPane
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the width of this UI pane
    /// </summary>
    /// <returns>
    /// Width of this UI pane
    /// </returns>
    INT32 GetWidth();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    // MgWebUiSizablePane constructor
    //
    MgWebUiSizablePane();

    ///////////////////////////////////////////////////////////////////////////
    // Sets the width of this pane
    //
    void SetWidth(INT32 width);

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
    static const INT32 m_cls_id = WebApp_UiSizablePane;

protected:
    INT32       m_width;
};

#endif  // _MGWEBUISIZABLEPANE_H_

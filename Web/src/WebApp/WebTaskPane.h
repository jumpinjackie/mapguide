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

#ifndef _MGWEBTASKPANE_
#define _MGWEBTASKPANE_

class MgWebWidgetCollection;

class MgWebTaskPane;
template class MG_WEBAPP_API Ptr<MgWebTaskPane>;

class MG_WEBAPP_API MgWebTaskPane : public MgWebUiSizablePane
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the task bar
    /// </summary>
    /// <returns>
    /// Task bar
    /// </returns>
    MgWebTaskBar* GetTaskBar();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the URL of the initial task. Defaults to the 'Getting started' page
    /// </summary>
    /// <returns>
    /// URL of the initial task
    /// </returns>
    STRING GetInitialTaskUrl();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebTaskPane();

    ///////////////////////////////////////////////////////////////////////////
    // Set the URL of the initial task
    //
    void SetInitialTaskUrl(CREFSTRING url);

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
    static const INT32 m_cls_id = WebApp_TaskPane;

private:
    Ptr<MgWebTaskBar>   m_taskBar;
    STRING              m_initialTaskUrl;
};

#endif // _MGWEBTASKPANE_

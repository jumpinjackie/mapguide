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

#ifndef _MGWEBTASKBAR_
#define _MGWEBTASKBAR_

class MgWebWidgetCollection;

class MgWebTaskBar;
template class MG_WEBAPP_API Ptr<MgWebTaskBar>;

class MG_WEBAPP_API MgWebTaskBar : public MgWebUiPane
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the task list
    /// </summary>
    /// <returns>
    /// Task list
    /// </returns>
    MgWebWidgetCollection* GetTaskList();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the task buttons
    /// </summary>
    /// <returns>
    /// Task list
    /// </returns>
    MgWebWidgetCollection* GetTaskButtons();

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebTaskBar();

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
    static const INT32 m_cls_id = WebApp_TaskBar;

private:
    Ptr<MgWebWidgetCollection> m_taskList;
    Ptr<MgWebWidgetCollection> m_taskButtons;
};

#endif // _MGWEBTASKBAR_
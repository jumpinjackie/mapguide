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

#ifndef _MGWEBSEPARATORWIDGET_
#define _MGWEBSEPARATORWIDGET_

class MgWebWidgetCollection;

class MgWebSeparatorWidget;
template class MG_WEBAPP_API Ptr<MgWebSeparatorWidget>;

class MG_WEBAPP_API MgWebSeparatorWidget : public MgWebWidget
{
EXTERNAL_API:
    virtual ~MgWebSeparatorWidget() {}      //not needed by C++ code, but workaround SWIG bug

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebSeparatorWidget();

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
    static const INT32 m_cls_id = WebApp_SeparatorWidget;
};

#endif // _MGWEBSEPARATORWIDGET_

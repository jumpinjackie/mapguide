//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MGWEBWIDGETCOLLECTION_H_
#define _MGWEBWIDGETCOLLECTION_H_

class MgWebWidgetCollection;
template class MG_WEBAPP_API Ptr<MgWebWidgetCollection>;

class MG_WEBAPP_API MgWebWidgetCollection : public MgGuardDisposable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the number of widgets in the collection
    /// </summary>
    /// <returns>
    /// Number of widgets
    /// </returns>
    INT32 GetCount();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the widget at the specified index
    /// Throws an out of range exception if the index is invalid
    /// </summary>
    /// <param name="index">
    /// Index of the widget
    /// </param>
    /// <returns>
    /// Command
    /// </returns>
    MgWebWidget* GetWidget(INT32 index);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns true if the collection contains the specified widget, false otherwise
    /// </summary>
    /// <param name="value">
    /// Value of the widget
    /// </param>
    /// <returns>
    /// true if the collection contains the specified widget, false otherwise
    /// </returns>
    bool Contains(MgWebWidget* value);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the index of the specified widget in the collection or -1 if the widget does not exist.
    /// </summary>
    /// <param name="value">
    /// Value of the widget
    /// </param>
    /// <returns>
    /// index if the collection contains the specified widget, false otherwise
    /// </returns>
    INT32 IndexOf(MgWebWidget* value);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an empty MgWebWidgetCollection object.
    /// </summary>
    MgWebWidgetCollection();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Adds the specified widget to the end of the collection.
    /// </summary>
    /// <param name="value">
    /// Command
    /// </param>
    /// <returns>
    /// index of the newly added widget
    /// </returns>
    void Add(MgWebWidget* value);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Inserts the specified widget at the specified index within the collection.
    /// Widgets following the insertion point are moved down to accommodate the new widget.
    /// Throws an out of range argument exception if the specified index is out of range
    /// </summary>
    /// <param name="index">
    /// Position in the collection
    /// </param>
    /// <param name="value">
    /// Command to be inserted in the collection
    /// </param>
    void Insert(INT32 index, MgWebWidget* value);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Removes all widgets from the collection
    /// </summary>
    void Clear();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Removes an widget from the collection
    /// <param name="value">
    /// Command to be removed
    /// </param>
    bool Remove(MgWebWidget* value);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Removes an widget from the collection at the specified index
    /// Throws an out of range exception if the widget does not exist within the collection.
    /// </summary>
    /// <param name="index">
    /// index of the widget to be removed
    /// </param>
    void RemoveAt(INT32 index);

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
    static const INT32 m_cls_id = WebApp_WidgetCollection;

private:
    Ptr<MgDisposableCollection>   m_widgets;
};

#endif

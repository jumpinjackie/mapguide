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

#ifndef _MGWEBTPRINTCOMMAND_H_
#define _MGWEBTPRINTCOMMAND_H_

class MgWebPrintCommand;
template class MG_WEBAPP_API Ptr<MgWebPrintCommand>;

class MG_WEBAPP_API MgWebPrintCommand : public MgWebCommand
{
EXTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the number of print layouts to choose from for this command
    /// </summary>
    /// <returns>
    /// Number of print layouts
    /// </returns>
    INT32 GetPrintLayoutCount();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the resource id of the print layout at the specified position
    /// </summary>
    /// <returns>
    /// Print layout resource id
    /// </returns>
    /// <remarks>
    /// Throws an MgIndexOutOfRangeException if index is out of range
    /// </remarks>
    STRING GetPrintLayoutAt(INT32 index);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    // ctor
    //
    MgWebPrintCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Get the print layout collection
    //
    MgStringCollection* GetPrintLayouts();

CLASS_ID:
    static const INT32 m_cls_id = WebApp_PrintCommand;

protected:

    ///////////////////////////////////////////////////////////////////////////
    // Get the unique identifier for the class
    //
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    // Dispose this object
    //
    virtual void Dispose();

protected:
    Ptr<MgStringCollection>   m_printLayouts;
};

#endif // _MGWEBTPRINTCOMMAND_H_

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

#ifndef _MGWEBINVOKESCRIPTCOMMAND_H_
#define _MGWEBINVOKESCRIPTCOMMAND_H_

class MgWebInvokeScriptCommand;
template class MG_WEBAPP_API Ptr<MgWebInvokeScriptCommand>;

class MG_WEBAPP_API MgWebInvokeScriptCommand : public MgWebUiTargetCommand
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the script code necessary to invoke the desired function
    /// </summary>
    /// <returns>
    /// Javascript code
    /// </returns>
    STRING GetScriptCode();

INTERNAL_API:

    MgWebInvokeScriptCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Set the javascript code
    //
    void SetScriptCode(CREFSTRING code);

CLASS_ID:
    static const INT32 m_cls_id = WebApp_InvokeScriptCommand;

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
    STRING      m_code;
};

#endif // _MGWEBINVOKESCRIPTCOMMAND_H_

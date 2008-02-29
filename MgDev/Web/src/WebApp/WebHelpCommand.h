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

#ifndef _MGWEBHELPCOMMAND_H_
#define _MGWEBHELPCOMMAND_H_

class MgWebHelpCommand;
template class MG_WEBAPP_API Ptr<MgWebHelpCommand>;

class MG_WEBAPP_API MgWebHelpCommand : public MgWebUiTargetCommand
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the url that starts the help
    /// </summary>
    /// <returns>
    /// Url to help
    /// </returns>
    STRING GetUrl();

INTERNAL_API:

    MgWebHelpCommand();

    ///////////////////////////////////////////////////////////////////////////
    // Set the Url for this help command
    //
    void SetUrl(CREFSTRING url);

CLASS_ID:
    static const INT32 m_cls_id = WebApp_HelpCommand;

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
    STRING      m_url;
};

#endif // _MGWEBHELPCOMMAND_H_

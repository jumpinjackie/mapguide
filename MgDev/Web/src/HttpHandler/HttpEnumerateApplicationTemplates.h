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

#ifndef _MGHTTPENUMAPPTEMPLATES_H_
#define _MGHTTPENUMAPPTEMPLATES_H_

#include <vector>
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
using namespace std;

class MgHttpEnumerateApplicationTemplates : public MgHttpRequestResponseHandler
{
HTTP_DECLARE_CREATE_OBJECT()

public:
    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="name">Input
    /// MgHttpRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>
    MgHttpEnumerateApplicationTemplates(MgHttpRequest *hRequest);

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(MgHttpResponse& hResponse);

private:

    void ReadTemplateInfo();
    void FindTemplates(MgStringCollection* templates, STRING fulldir);
    string GetStringFromElement(DOMElement* elt);

    string CreateOpenElement(const string name, int insetDepth, bool linebreak);
    string CreateCloseElement(const string name, int insetDepth, bool linebreak);

    //Overloaded version for wide char element names
    string CreateOpenElement(const wstring name, int insetDepth, bool linebreak);
    string CreateCloseElement(const wstring name, int insetDepth, bool linebreak);

    string GetXmlResponse();
};

#endif  //_MGHTTPENUMAPPTEMPLATES_H_

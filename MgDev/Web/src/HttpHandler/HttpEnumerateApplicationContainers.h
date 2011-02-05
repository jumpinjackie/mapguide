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

#ifndef _MGHTTPENUMAPPCONTAINERS_H_
#define _MGHTTPENUMAPPCONTAINERS_H_

#include <vector>
using namespace std;

struct ContainerInfo
{
    string type;
    string localizedType;
    string description;
    string previewImageUrl;
};
typedef vector<ContainerInfo*> ContainerInfoVector;

class MgHttpEnumerateApplicationContainers : public MgHttpRequestResponseHandler
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
    MgHttpEnumerateApplicationContainers(MgHttpRequest *hRequest);

    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Input
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>
    void Execute(MgHttpResponse& hResponse);

private:
    bool m_refresh;

    ContainerInfoVector m_containerInfoVector;

    void ReadContainerInfo();
    void FindContainers(MgStringCollection* widgets, STRING rootFolder);
    string GetXmlResponse();
    string GetStringFromElement(DOMElement* elt);
};

#endif  //_MGHTTPENUMAPPCONTAINERS_H_

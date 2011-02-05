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

#ifndef MGHTTPCOPYRESOURCE_H_
#define MGHTTPCOPYRESOURCE_H_

class MgHttpCopyResource : public MgHttpRequestResponseHandler
{
HTTP_DECLARE_CREATE_OBJECT()

public:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Initializes the common parameters of the request.
    /// </summary>
    /// <param name="hRequest">Input
    /// MgHttpRequest
    /// This contains all the parameters of the request.
    /// </param>
    /// <returns>
    /// nothing
    /// </returns>

    MgHttpCopyResource(MgHttpRequest* hRequest);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Executes the specific request.
    /// </summary>
    /// <param name="hResponse">Output
    /// This contains the response (including MgHttpResult and StatusCode) from the server.
    /// </param>

    void Execute(MgHttpResponse& hResponse);

private:

    STRING m_sourceResourceId;
    STRING m_destResourceId;
    bool m_overwrite;
};

#endif

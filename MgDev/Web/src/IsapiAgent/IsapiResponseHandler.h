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

#ifndef ISAPI_RESPONSE_HANDLER_H
#define ISAPI_RESPONSE_HANDLER_H

#include "HttpExt.h"

// This class transforms MgHttpResponse data into an HTTP compliant
// response which is then send back to the web server.
class IsapiResponseHandler
{
public:
    IsapiResponseHandler(EXTENSION_CONTROL_BLOCK *pECB);
    ~IsapiResponseHandler();
    void SendResponse(MgHttpResponse* response);
    void SendError(MgException* e);
    void RequestAuth();

private:
    void WriteContext(const char *pszFormat, ...);
    void WriteHeader(const char* szBuffer, const char* szStatusBuffer = NULL);

    EXTENSION_CONTROL_BLOCK *m_pECB;
};

#endif

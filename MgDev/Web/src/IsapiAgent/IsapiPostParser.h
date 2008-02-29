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

#ifndef ISAPI_POST_PARSER_H
#define ISAPI_POST_PARSER_H

#include "httpext.h"

// This class parsers HTTP POST requests.  It makes use of IsapiGetParser
// if the post request is form/url-encoded
class IsapiPostParser
{
public:
    IsapiPostParser(EXTENSION_CONTROL_BLOCK* pECB);
    ~IsapiPostParser();
    void Parse(MgHttpRequestParam* params);

private:
    size_t m_bufSize;

    EXTENSION_CONTROL_BLOCK* m_pECB;
    BYTE* m_pBuffer;
    DWORD m_dwPos;

};

#endif

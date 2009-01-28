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

#ifndef APACHE_POST_PARSER_H
#define APACHE_POST_PARSER_H

#include "httpd.h"
#include "http_config.h"
#include "http_core.h"
#include "http_log.h"
#include "http_main.h"
#include "http_protocol.h"
#include "http_request.h"

#ifdef strtoul
#undef strtoul
#endif
#define strtoul strtoul

#include "HttpHandler.h"

// This class parsers HTTP POST requests.  It makes use of MapAgentGetParser
// if the post request is form/url-encoded
class ApachePostParser
{
public:
    ApachePostParser(request_rec* r);
    ~ApachePostParser();
    void Parse(MgHttpRequestParam* params);

private:

    BYTE* m_pBuffer;
    request_rec *m_r;

};

#endif

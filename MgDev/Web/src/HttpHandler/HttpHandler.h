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

#ifndef MG_HTTP_HANDLER_H
#define MG_HTTP_HANDLER_H

// Forward declarations
class ACE_SOCK_Stream;
class MgHttpRequestResponseHandler;
class MgHttpResponse;

// External headers
#include "MapGuideCommon.h"

#include <assert.h>

#include "HttpHandlerClassId.h"

// Common HttpHandler headers
#include "HttpHandlerApiDllExport.h"
#include "HttpHeader.h"
#include "HttpRequestParam.h"
#include "HttpRequestMetadata.h"
#include "HttpStatusCodes.h"
#include "HttpResult.h"
#include "HttpResourceStrings.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpRequestResponseHandler.h"
#include "HttpPrimitiveValue.h"
#include "HttpUtil.h"

// dynamic creation macros
#define HTTP_DECLARE_CREATE_OBJECT() \
public: \
    static MgHttpRequestResponseHandler* CreateObject(MgHttpRequest*);

#define HTTP_IMPLEMENT_CREATE_OBJECT(class_name) \
    MgHttpRequestResponseHandler* class_name::CreateObject(MgHttpRequest* request) \
        { return new class_name(request); }

#endif // MG_HTTP_HANDLER_H

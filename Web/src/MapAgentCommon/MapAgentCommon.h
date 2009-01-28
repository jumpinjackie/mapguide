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

#ifndef MGMAPAGENTCOMMON_H
#define MGMAPAGENTCOMMON_H

#include "HttpHandler.h"

class MapAgentCommon
{
public:
    MapAgentCommon();
    ~MapAgentCommon();

    static bool ParseAuth(char* AuthString, MgHttpRequestParam* params);
    static bool AuthenticateOgcRequest(MgHttpRequestParam* params);
    static bool IsOgcRequest(MgHttpRequestParam* params);

    static bool IsXmlPi(char* buf);

    static void ScanHeaders(char* partHdrStart, char* partHdrEnd, STRING& paramName, STRING& paramType, bool& bIsFile);
    static void PopulateData(char* partHdrEnd, char** curBuf, char* endBuf, string& dataEndTag,
        STRING& paramName, STRING& paramType, MgHttpRequestParam* params, bool& bIsFile);
};

#endif

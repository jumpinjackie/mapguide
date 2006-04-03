//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "HttpHandler.h"
#include "CgiGetParser.h"

CgiGetParser::CgiGetParser(void)
{
}

CgiGetParser::~CgiGetParser(void)
{
}

void CgiGetParser::Parse(char* queryString, MgHttpRequestParam* params)
{
    MG_TRY()
    string query;
    MgUtil::UnEscapeUrl(queryString, query);
    string::size_type startPos = 0;
    string::size_type sepPos = 0;
    while (sepPos != query.npos)
    {
        sepPos = query.find('=', startPos);
        if (sepPos != query.npos)
        {
            string paramName = query.substr(startPos, sepPos-startPos);
            string paramValue;
            startPos = sepPos + 1;
            sepPos = query.find('&', startPos);
            if (sepPos != query.npos)
            {
                paramValue = query.substr(startPos, sepPos-startPos);
                startPos = sepPos + 1;
            }
            else
            {
                paramValue = query.substr(startPos);
            }

            if (paramName.length() > 0 && paramValue.length() > 0)
            {
                STRING name = MgUtil::MultiByteToWideChar(paramName);
                STRING value = MgUtil::MultiByteToWideChar(paramValue);
                params->AddParameter(name, value);
            }
        }
    }
    MG_CATCH_AND_THROW(L"CgiGetParser.Parse");
}

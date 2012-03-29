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

#include "HttpHandler.h"
#include "MapAgentGetParser.h"


MapAgentGetParser::MapAgentGetParser()
{
}


MapAgentGetParser::~MapAgentGetParser()
{
}


void MapAgentGetParser::Parse(const char* pszQuery, MgHttpRequestParam* params)
{
    MG_TRY()

    // but we need the support of a string class, so we wrap it.
    string sQuery(pszQuery);

    string::size_type startPos = 0;
    string::size_type sepPos = 0;
    while (sepPos != sQuery.npos)
    {
        sepPos = sQuery.find('&',startPos);
        if(sepPos != sQuery.npos)
        {
            // Extract one parameter.
            string sParameter = sQuery.substr(startPos,sepPos - startPos);

            // Update the next start to be past the ampersand.
            startPos = sepPos + 1;

            // Now, digest the parameter.
            ParseOneParameter(sParameter,params);
        }
    }

    ParseOneParameter(sQuery.substr(startPos),params);

    MG_CATCH_AND_THROW(L"MapAgentGetParser.Parse");
}


void MapAgentGetParser::ParseOneParameter(string sParameter, MgHttpRequestParam* params)
{
    if(sParameter.length() == 0)
        return;

    string sName;
    string sValue;
    string::size_type sepPosEqual = sParameter.find('=');
    if(sepPosEqual != sParameter.npos) { // A name/value pair.
        // Unescape any decorations encoded within the name or value.
        MgUtil::UnEscapeUrl((char*)sParameter.substr(0,sepPosEqual).c_str(),sName);
        MgUtil::UnEscapeUrl((char*)sParameter.substr(sepPosEqual+1).c_str(),sValue);
    }
    else { // just a name, no value.
        MgUtil::UnEscapeUrl((char*)sParameter.c_str(),sName);
        sValue = "";
    }
    STRING wsName  = MgUtil::MultiByteToWideChar(sName);
    STRING wsValue = MgUtil::MultiByteToWideChar(sValue);
    params->AddParameter(wsName, wsValue);
}

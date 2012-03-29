//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "IOProfileRenderLabelsResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderLabelsResult);
ELEM_MAP_ENTRY(2, RenderTime);
ELEM_MAP_ENTRY(3, Error);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOProfileRenderLabelsResult::IOProfileRenderLabelsResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLabelsResult = NULL;
}


IOProfileRenderLabelsResult::IOProfileRenderLabelsResult(ProfileRenderLabelsResult* profileRenderLabelsResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLabelsResult = profileRenderLabelsResult;
}


IOProfileRenderLabelsResult::~IOProfileRenderLabelsResult()
{
}


void IOProfileRenderLabelsResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLabelsResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLabelsResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLabelsResult::Write(MdfStream& fd, ProfileRenderLabelsResult* profileRenderLabelsResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderLabelsResult) << std::endl;
    tab.inctab();

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderLabelsResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: Error
    if(!profileRenderLabelsResult->GetError().empty())
    {
        fd << tab.tab() << startStr(sError);
        fd << EncodeString(profileRenderLabelsResult->GetError());
        fd << endStr(sError) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderLabelsResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderLabelsResult) << std::endl;
}

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
#include "IOProfileRenderWatermarkResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderWatermarkResult);
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, PositionType);
ELEM_MAP_ENTRY(4, RenderTime);
ELEM_MAP_ENTRY(5, Error);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOProfileRenderWatermarkResult::IOProfileRenderWatermarkResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderWatermarkResult = NULL;
}


IOProfileRenderWatermarkResult::IOProfileRenderWatermarkResult(ProfileRenderWatermarkResult* profileRenderWatermarkResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderWatermarkResult = profileRenderWatermarkResult;
}


IOProfileRenderWatermarkResult::~IOProfileRenderWatermarkResult()
{
}


void IOProfileRenderWatermarkResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarkResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarkResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarkResult::Write(MdfStream& fd, ProfileRenderWatermarkResult* profileRenderWatermarkResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderWatermarkResult) << std::endl;
    tab.inctab();

    // Property: ResourceID
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(profileRenderWatermarkResult->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: PositionType
    fd << tab.tab() << startStr(sPositionType);
    fd << EncodeString(profileRenderWatermarkResult->GetPositionType());
    fd << endStr(sPositionType) << std::endl;

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderWatermarkResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: Error
    if(!profileRenderWatermarkResult->GetError().empty())
    {
        fd << tab.tab() << startStr(sError);
        fd << EncodeString(profileRenderWatermarkResult->GetError());
        fd << endStr(sError) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderWatermarkResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderWatermarkResult) << std::endl;
}

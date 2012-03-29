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
#include "IOProfileRenderWatermarksResult.h"
#include "IOProfileRenderWatermarkResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderWatermarksResult);
ELEM_MAP_ENTRY(2, RenderTime);
ELEM_MAP_ENTRY(3, ProfileRenderWatermarkResult);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOProfileRenderWatermarksResult::IOProfileRenderWatermarksResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderWatermarksResult = NULL;
}


IOProfileRenderWatermarksResult::IOProfileRenderWatermarksResult(ProfileRenderWatermarksResult* profileRenderWatermarksResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderWatermarksResult = profileRenderWatermarksResult;
}


IOProfileRenderWatermarksResult::~IOProfileRenderWatermarksResult()
{
}


void IOProfileRenderWatermarksResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarksResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarksResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderWatermarksResult::Write(MdfStream& fd, ProfileRenderWatermarksResult* profileRenderWatermarksResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderWatermarksResult) << std::endl;
    tab.inctab();

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderWatermarksResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: ProfileRenderWatermarkResult
    for (int i=0; i<profileRenderWatermarksResult->GetProfileRenderWatermarkResults()->GetCount(); ++i)
        IOProfileRenderWatermarkResult::Write(fd, profileRenderWatermarksResult->GetProfileRenderWatermarkResults()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderWatermarksResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderWatermarksResult) << std::endl;
}

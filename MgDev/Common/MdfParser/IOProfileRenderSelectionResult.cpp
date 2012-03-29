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
#include "IOProfileRenderSelectionResult.h"
#include "IOProfileRenderLayerResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderSelectionResult);
ELEM_MAP_ENTRY(2, RenderTime);
ELEM_MAP_ENTRY(3, ProfileRenderLayerResult);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOProfileRenderSelectionResult::IOProfileRenderSelectionResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderSelectionResult = NULL;
}


IOProfileRenderSelectionResult::IOProfileRenderSelectionResult(ProfileRenderSelectionResult* profileRenderSelectionResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderSelectionResult = profileRenderSelectionResult;
}


IOProfileRenderSelectionResult::~IOProfileRenderSelectionResult()
{
}


void IOProfileRenderSelectionResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderSelectionResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderSelectionResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderSelectionResult::Write(MdfStream& fd, ProfileRenderSelectionResult* profileRenderSelectionResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderSelectionResult) << std::endl;
    tab.inctab();

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderSelectionResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: ProfileRenderLayerResult
    for (int i=0; i<profileRenderSelectionResult->GetProfileRenderLayerResults()->GetCount(); ++i)
        IOProfileRenderLayerResult::Write(fd, profileRenderSelectionResult->GetProfileRenderLayerResults()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderSelectionResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderSelectionResult) << std::endl;
}

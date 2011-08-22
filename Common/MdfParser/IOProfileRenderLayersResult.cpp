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
#include "IOProfileRenderLayersResult.h"
#include "IOProfileRenderLayerResult.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderLayersResult);
ELEM_MAP_ENTRY(2, RenderTime);
ELEM_MAP_ENTRY(3, ProfileRenderLayerResult);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOProfileRenderLayersResult::IOProfileRenderLayersResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLayersResult = NULL;
}


IOProfileRenderLayersResult::IOProfileRenderLayersResult(ProfileRenderLayersResult* profileRenderLayersResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLayersResult = profileRenderLayersResult;
}


IOProfileRenderLayersResult::~IOProfileRenderLayersResult()
{
}


void IOProfileRenderLayersResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayersResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayersResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayersResult::Write(MdfStream& fd, ProfileRenderLayersResult* profileRenderLayersResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderLayersResult) << std::endl;
    tab.inctab();

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderLayersResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: ProfileRenderLayerResult
    for (int i=0; i<profileRenderLayersResult->GetProfileRenderLayerResults()->GetCount(); ++i)
        IOProfileRenderLayerResult::Write(fd, profileRenderLayersResult->GetProfileRenderLayerResults()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderLayersResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderLayersResult) << std::endl;
}

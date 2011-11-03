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
#include "IOProfileRenderLayerResult.h"
#include "IOScaleRange.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderLayerResult);
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, LayerName);
ELEM_MAP_ENTRY(4, LayerType);
ELEM_MAP_ENTRY(5, FeatureClassName);
ELEM_MAP_ENTRY(6, CoordinateSystem);
ELEM_MAP_ENTRY(7, ScaleRange);
ELEM_MAP_ENTRY(8, Filter);
ELEM_MAP_ENTRY(9, RenderTime);
ELEM_MAP_ENTRY(10, Error);
ELEM_MAP_ENTRY(11, ExtendedData1);


IOProfileRenderLayerResult::IOProfileRenderLayerResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLayerResult = NULL;
}


IOProfileRenderLayerResult::IOProfileRenderLayerResult(ProfileRenderLayerResult* profileRenderLayerResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderLayerResult = profileRenderLayerResult;
}


IOProfileRenderLayerResult::~IOProfileRenderLayerResult()
{
}


void IOProfileRenderLayerResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayerResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayerResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderLayerResult::Write(MdfStream& fd, ProfileRenderLayerResult* profileRenderLayerResult, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sProfileRenderLayerResult) << std::endl;
    tab.inctab();

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(profileRenderLayerResult->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: LayerName
    fd << tab.tab() << startStr(sLayerName);
    fd << EncodeString(profileRenderLayerResult->GetLayerName());
    fd << endStr(sLayerName) << std::endl;

    // Property: LayerType
    fd << tab.tab() << startStr(sLayerType);
    fd << EncodeString(profileRenderLayerResult->GetLayerType());
    fd << endStr(sLayerType) << std::endl;

    // Property: FeatureClassName
    fd << tab.tab() << startStr(sFeatureClassName);
    fd << EncodeString(profileRenderLayerResult->GetFeatureClassName());
    fd << endStr(sFeatureClassName) << std::endl;

    // Property: CoordinateSystem
    fd << tab.tab() << startStr(sCoordinateSystem);
    fd << EncodeString(profileRenderLayerResult->GetCoordinateSystem());
    fd << endStr(sCoordinateSystem) << std::endl;

    // Property: ScaleRange
    ScaleRange* scaleRange = profileRenderLayerResult->GetScaleRange();
    if (scaleRange)
        IOScaleRange::Write(fd, scaleRange, version, tab);

    // Property: Filter
    fd << tab.tab() << startStr(sFilter);
    fd << EncodeString(profileRenderLayerResult->GetFilter());
    fd << endStr(sFilter) << std::endl;

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderLayerResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: Error
    if(!profileRenderLayerResult->GetError().empty())
    {
        fd << tab.tab() << startStr(sError);
        fd << EncodeString(profileRenderLayerResult->GetError());
        fd << endStr(sError) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderLayerResult->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sProfileRenderLayerResult) << std::endl;
}

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
#include "IOProfileRenderMapResult.h"
#include "IOProfileRenderLayersResult.h"
#include "IOProfileRenderSelectionResult.h"
#include "IOProfileRenderWatermarksResult.h"
#include "IOProfileRenderLabelsResult.h"
#include "IOExtra.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ProfileRenderMapResult);
ELEM_MAP_ENTRY(2, ProfileRenderDynamicOverlayResult);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, CoordinateSystem);
ELEM_MAP_ENTRY(5, Extents);
ELEM_MAP_ENTRY(6, Scale);
ELEM_MAP_ENTRY(7, LayerCount);
ELEM_MAP_ENTRY(8, ImageFormat);
ELEM_MAP_ENTRY(9, RendererType);
ELEM_MAP_ENTRY(10, RenderTime);
ELEM_MAP_ENTRY(11, ProfileRenderLayersResult);
ELEM_MAP_ENTRY(13, ProfileRenderSelectionResult);
ELEM_MAP_ENTRY(13, ProfileRenderWatermarksResult);
ELEM_MAP_ENTRY(14, ProfileRenderLabelsResult);
ELEM_MAP_ENTRY(15, CreateImageTime);
ELEM_MAP_ENTRY(16, Error);
ELEM_MAP_ENTRY(17, ExtendedData1);


IOProfileRenderMapResult::IOProfileRenderMapResult(Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderMapResult = NULL;
}


IOProfileRenderMapResult::IOProfileRenderMapResult(ProfileRenderMapResult* profileRenderMapResult, Version& version) : SAX2ElementHandler(version)
{
    this->m_profileRenderMapResult = profileRenderMapResult;
}


IOProfileRenderMapResult::~IOProfileRenderMapResult()
{
}


void IOProfileRenderMapResult::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderMapResult::ElementChars(const wchar_t* ch)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderMapResult::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    //TODO: Implement this while deserialization is needed.
}


void IOProfileRenderMapResult::Write(MdfStream& fd, ProfileRenderMapResult* profileRenderMapResult, Version* version, MgTab& tab)
{
    ProfileResult::ProfileResultType type = profileRenderMapResult->GetProfileResultType();
    if (ProfileResult::ProfileRenderMap == type)
        fd << tab.tab() << startStr(sProfileRenderMapResult) << std::endl;
    else //ProfileResult::ProfileRenderDynamicOverlay
        fd << tab.tab() << startStr(sProfileRenderDynamicOverlayResult) << std::endl;
    tab.inctab();

    // Property: ResourceID
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(profileRenderMapResult->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: CoordinateSystem
    fd << tab.tab() << startStr(sCoordinateSystem);
    fd << EncodeString(profileRenderMapResult->GetCoordinateSystem());
    fd << endStr(sCoordinateSystem) << std::endl;

    // Property: Extents
    IOExtra::WriteBox2D(fd, profileRenderMapResult->GetExtents(), false, version, tab);

    // Property: Scale
    fd << tab.tab() << startStr(sScale);
    fd << DoubleToStr(profileRenderMapResult->GetScale());
    fd << endStr(sScale) << std::endl;

    // Property: LayerCount
    fd << tab.tab() << startStr(sLayerCount);
    fd << IntToStr(profileRenderMapResult->GetLayerCount());
    fd << endStr(sLayerCount) << std::endl;

    // Property: ImageFormat
    fd << tab.tab() << startStr(sImageFormat);
    fd << EncodeString(profileRenderMapResult->GetImageFormat());
    fd << endStr(sImageFormat) << std::endl;

    // Property: RendererType
    fd << tab.tab() << startStr(sRendererType);
    fd << EncodeString(profileRenderMapResult->GetRendererType());
    fd << endStr(sRendererType) << std::endl;

    // Property: RenderTime
    fd << tab.tab() << startStr(sRenderTime);
    fd << DoubleToStr(profileRenderMapResult->GetRenderTime());
    fd << endStr(sRenderTime) << std::endl;

    // Property: ProfileRenderLayersResult
    ProfileRenderLayersResult* pProfileRenderLayersResult = profileRenderMapResult->GetProfileRenderLayersResult();
    if (pProfileRenderLayersResult)
        IOProfileRenderLayersResult::Write(fd, pProfileRenderLayersResult, version, tab);

    // Property: ProfileRenderSelectionResult
    ProfileRenderSelectionResult* pProfileRenderSelectionResult = profileRenderMapResult->GetProfileRenderSelectionResult();
    if (pProfileRenderSelectionResult)
        IOProfileRenderSelectionResult::Write(fd, pProfileRenderSelectionResult, version, tab);

    // Property: ProfileRenderWatermarksResult
    ProfileRenderWatermarksResult* pProfileRenderWatermarksResult = profileRenderMapResult->GetProfileRenderWatermarksResult();
    if (pProfileRenderWatermarksResult)
        IOProfileRenderWatermarksResult::Write(fd, pProfileRenderWatermarksResult, version, tab);

    // Property: ProfileRenderLabelsResult
    ProfileRenderLabelsResult* pProfileRenderLabelsResult = profileRenderMapResult->GetProfileRenderLabelsResult();
    if (pProfileRenderLabelsResult)
        IOProfileRenderLabelsResult::Write(fd, pProfileRenderLabelsResult, version, tab);

    // Property: CreateImageTime
    fd << tab.tab() << startStr(sCreateImageTime);
    fd << DoubleToStr(profileRenderMapResult->GetCreateImageTime());
    fd << endStr(sCreateImageTime) << std::endl;

    // Property: Error
    if(!profileRenderMapResult->GetError().empty())
    {
        fd << tab.tab() << startStr(sError);
        fd << EncodeString(profileRenderMapResult->GetError());
        fd << endStr(sError) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, profileRenderMapResult->GetUnknownXml(), version, tab);

    tab.dectab();
    if (ProfileResult::ProfileRenderMap == type)
        fd << tab.tab() << endStr(sProfileRenderMapResult) << std::endl;
    else //ProfileResult::ProfileRenderDynamicOverlay
        fd << tab.tab() << endStr(sProfileRenderDynamicOverlayResult) << std::endl;
}

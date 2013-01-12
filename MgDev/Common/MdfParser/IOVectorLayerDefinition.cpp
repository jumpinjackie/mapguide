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

#include "stdafx.h"
#include "IOVectorLayerDefinition.h"
#include "IONameStringPair.h"
#include "IOVectorScaleRange.h"
#include "IOWatermarkInstance.h"
#include "IOURLData.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, VectorLayerDefinition);
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, Opacity);
ELEM_MAP_ENTRY(4, FeatureName);
ELEM_MAP_ENTRY(5, FeatureNameType);
ELEM_MAP_ENTRY(6, Filter);
ELEM_MAP_ENTRY(7, PropertyMapping);
ELEM_MAP_ENTRY(8, Geometry);
ELEM_MAP_ENTRY(9, Url);
ELEM_MAP_ENTRY(10, ToolTip);
ELEM_MAP_ENTRY(11, VectorScaleRange);
ELEM_MAP_ENTRY(12, ExtendedData1);
ELEM_MAP_ENTRY(13, Watermarks);
ELEM_MAP_ENTRY(14, Watermark);
ELEM_MAP_ENTRY(15, UrlData);


IOVectorLayerDefinition::IOVectorLayerDefinition(Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = NULL;
}


IOVectorLayerDefinition::IOVectorLayerDefinition(VectorLayerDefinition* layer, Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = layer;
}


IOVectorLayerDefinition::~IOVectorLayerDefinition()
{
}


void IOVectorLayerDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eVectorLayerDefinition:
        this->m_startElemName = name;
        break;

    case eWatermark:
        {
            Version wdVersion;
            if (!IOVectorLayerDefinition::GetWatermarkDefinitionVersion(&this->m_version, wdVersion))
                return;

            WatermarkInstance* watermark = new WatermarkInstance(L"", L"");
            this->m_layer->GetWatermarks()->Adopt(watermark);
            IOWatermarkInstance* IO = new IOWatermarkInstance(watermark, wdVersion);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case ePropertyMapping:
        {
            IONameStringPair* IO = new IONameStringPair(this->m_layer, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eUrlData:
        {
            IOURLData* IO = new IOURLData(this->m_layer, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eVectorScaleRange:
        {
            IOVectorScaleRange* IO = new IOVectorScaleRange(this->m_layer, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOVectorLayerDefinition::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eResourceId:
        this->m_layer->SetResourceID(ch);
        break;

    case eOpacity:
        this->m_layer->SetOpacity(wstrToDouble(ch));
        break;

    case eFeatureName:
        this->m_layer->SetFeatureName(ch);
        break;

    case eFeatureNameType:
        if (::wcscmp(ch, L"FeatureClass") == 0) // NOXLATE
            this->m_layer->SetFeatureNameType(VectorLayerDefinition::FeatureClass);
        else if (::wcscmp(ch, L"NamedExtension") == 0) // NOXLATE
            this->m_layer->SetFeatureNameType(VectorLayerDefinition::NamedExtension);
        break;

    case eFilter:
        this->m_layer->SetFilter(ch);
        break;

    case eGeometry:
        this->m_layer->SetGeometry(ch);
        break;

    case eUrl:
        // Handle layer definition <= 2.3.0
        if (m_version <= Version(2, 3, 0))
        {
            URLData* urlData = this->m_layer->GetUrlData();
            if (!urlData)
            {
                urlData = new URLData();
                this->m_layer->AdoptUrlData(urlData);
            }
            urlData->SetUrlContent(ch);
        }
        break;

    case eToolTip:
        this->m_layer->SetToolTip(ch);
        break;
    }
}


void IOVectorLayerDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_layer->SetUnknownXml(this->m_unknownXml);

        this->m_layer = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


// Determine which WatermarkDefinition schema version to use based
// on the supplied LayerDefinition version:
// * LDF version == 2.4.0  =>  WD version 2.4.0
// * LDF version <= 2.3.0  =>  WD version 2.3.0
bool IOVectorLayerDefinition::GetWatermarkDefinitionVersion(Version* ldfVersion, Version& wdVersion)
{
    if (!ldfVersion || *ldfVersion >= Version(2, 4, 0))
        wdVersion = Version(2, 4, 0);
    else if (*ldfVersion <= Version(2, 3, 0))
        wdVersion = Version(2, 3, 0);

    return true;
}


void IOVectorLayerDefinition::Write(MdfStream& fd, VectorLayerDefinition* vectorLayer, Version* version, MgTab& tab)
{
    // verify the LDF version
    MdfString strVersion;
    if (version)
    {
        if (*version == Version(0, 9, 0))
        {
            // LDF in MapGuide 2006
            strVersion = L"1.0.0";
        }
        else if ((*version >= Version(1, 0, 0)) && (*version <= Version(2, 4, 0)))
        {
            // LDF in MapGuide 2007 - current
            strVersion = version->ToString();
        }
        else
        {
            // unsupported LDF version
            // TODO - need a way to return error information
            _ASSERT(false);
            return;
        }
    }
    else
    {
        // use the current highest version
        strVersion = L"2.4.0";
    }

    fd << tab.tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    tab.inctab();

    fd << tab.tab() << startStr(sVectorLayerDefinition) << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(vectorLayer->GetResourceID());
    fd << endStr(sResourceId) << std::endl;

    // Property: Opacity (optional)
    if (vectorLayer->GetOpacity() != 1.0)
    {
        fd << tab.tab() << startStr(sOpacity);
        fd << DoubleToStr(vectorLayer->GetOpacity());
        fd << endStr(sOpacity) << std::endl;
    }

    // Property: Watermarks (optional)
    int watermarkCount = vectorLayer->GetWatermarks()->GetCount();
    if (watermarkCount != 0)
    {
        if (!version || (*version >= Version(2, 3, 0)))
        {
            // only write Watermarks if the LDF version is 2.3.0 or greater
            fd << tab.tab() << startStr(sWatermarks) << std::endl;
            tab.inctab();
            for (int i=0; i<watermarkCount; ++i)
                IOWatermarkInstance::Write(fd, vectorLayer->GetWatermarks()->GetAt(i), version, tab);
            tab.dectab();
            fd << tab.tab() << endStr(sWatermarks) << std::endl;
        }
        else if (*version >= Version(1, 0, 0))
        {
            // save Watermarks as extended data for LDF versions 1.0.0 - 1.3.0
            tab.inctab();

            fdExtData << tab.tab() << startStr(sWatermarks) << std::endl;
            tab.inctab();
            for (int i=0; i<watermarkCount; ++i)
                IOWatermarkInstance::Write(fdExtData, vectorLayer->GetWatermarks()->GetAt(i), version, tab);
            tab.dectab();
            fdExtData << tab.tab() << endStr(sWatermarks) << std::endl;

            tab.dectab();
        }
    }

    // Property: FeatureName
    fd << tab.tab() << startStr(sFeatureName);
    fd << EncodeString(vectorLayer->GetFeatureName());
    fd << endStr(sFeatureName) << std::endl;

    // Property: FeatureNameType
    fd << tab.tab() << startStr(sFeatureNameType);
    if (vectorLayer->GetFeatureNameType() == VectorLayerDefinition::FeatureClass)
        fd << "FeatureClass"; // NOXLATE
    else
        fd << "NamedExtension"; // NOXLATE
    fd << endStr(sFeatureNameType) << std::endl;

    // Property: Filter
    if (!vectorLayer->GetFilter().empty())
    {
        fd << tab.tab() << startStr(sFilter);
        fd << EncodeString(vectorLayer->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: PropertyMappings
    for (int i=0; i<vectorLayer->GetPropertyMappings()->GetCount(); ++i)
        IONameStringPair::Write(fd, sPropertyMapping, vectorLayer->GetPropertyMappings()->GetAt(i), version, tab);

    // Property: Geometry
    fd << tab.tab() << startStr(sGeometry);
    fd << EncodeString(vectorLayer->GetGeometry());
    fd << endStr(sGeometry) << std::endl;

    // Property: Url / UrlData
    URLData* urlData = vectorLayer->GetUrlData();
    if (urlData)
    {
        if (!version || (*version >= Version(2, 4, 0)))
        {
            // write new version 2.4.0 property
            IOURLData::Write(fd, urlData, version, tab);
        }
        else
        {
            // save original url property for LDF versions <= 2.3.0
            if (!urlData->GetUrlContent().empty())
            {
                fd << tab.tab() << startStr(sUrl);
                fd << EncodeString(urlData->GetUrlContent());
                fd << endStr(sUrl) << std::endl;
            }

            // save new property as extended data for LDF versions <= 2.3.0
            tab.inctab();
            IOURLData::Write(fdExtData, urlData, version, tab);
            tab.dectab();
        }
    }

    // Property: ToolTip
    if (!vectorLayer->GetToolTip().empty())
    {
        fd << tab.tab() << startStr(sToolTip);
        fd << EncodeString(vectorLayer->GetToolTip());
        fd << endStr(sToolTip) << std::endl;
    }

    // Property: VectorScaleRange
    for (int i=0; i<vectorLayer->GetScaleRanges()->GetCount(); ++i)
        IOVectorScaleRange::Write(fd, vectorLayer->GetScaleRanges()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, vectorLayer->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sVectorLayerDefinition) << std::endl;

    tab.dectab();
    fd << tab.tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

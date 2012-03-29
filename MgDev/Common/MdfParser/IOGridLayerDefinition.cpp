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
#include "IOGridLayerDefinition.h"
#include "IOGridScaleRange.h"
#include "IOWatermarkInstance.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, GridLayerDefinition);
ELEM_MAP_ENTRY(2, ResourceId);
ELEM_MAP_ENTRY(3, Opacity);
ELEM_MAP_ENTRY(4, FeatureName);
ELEM_MAP_ENTRY(5, Geometry);
ELEM_MAP_ENTRY(6, Filter);
ELEM_MAP_ENTRY(7, GridScaleRange);
ELEM_MAP_ENTRY(8, ExtendedData1);
ELEM_MAP_ENTRY(9, Watermarks);
ELEM_MAP_ENTRY(10, Watermark);


IOGridLayerDefinition::IOGridLayerDefinition(Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = NULL;
}


IOGridLayerDefinition::IOGridLayerDefinition(GridLayerDefinition* layer, Version& version) : SAX2ElementHandler(version)
{
    this->m_layer = layer;
}


IOGridLayerDefinition::~IOGridLayerDefinition()
{
}


void IOGridLayerDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eGridLayerDefinition:
        this->m_startElemName = name;
        break;

    case eWatermark:
        {
            Version wdVersion;
            if (!IOGridLayerDefinition::GetWatermarkDefinitionVersion(&this->m_version, wdVersion))
                return;

            WatermarkInstance* watermark = new WatermarkInstance(L"", L"");
            this->m_layer->GetWatermarks()->Adopt(watermark);
            IOWatermarkInstance* IO = new IOWatermarkInstance(watermark, wdVersion);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eGridScaleRange:
        {
            IOGridScaleRange* IO = new IOGridScaleRange(this->m_layer, this->m_version);
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


void IOGridLayerDefinition::ElementChars(const wchar_t* ch)
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

    case eGeometry:
        this->m_layer->SetGeometry(ch);
        break;

    case eFilter:
        this->m_layer->SetFilter(ch);
        break;
    }
}


void IOGridLayerDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
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
bool IOGridLayerDefinition::GetWatermarkDefinitionVersion(Version* ldfVersion, Version& wdVersion)
{
    if (!ldfVersion || *ldfVersion >= Version(2, 4, 0))
        wdVersion = Version(2, 4, 0);
    else if (*ldfVersion <= Version(2, 3, 0))
        wdVersion = Version(2, 3, 0);

    return true;
}


void IOGridLayerDefinition::Write(MdfStream& fd, GridLayerDefinition* gridLayer, Version* version, MgTab& tab)
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

    fd << tab.tab() << startStr(sGridLayerDefinition) << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(gridLayer->GetResourceID());
    fd << endStr(sResourceId) << std::endl;

    // Property: Opacity (optional)
    if (gridLayer->GetOpacity() != 1.0)
    {
        fd << tab.tab() << startStr(sOpacity);
        fd << DoubleToStr(gridLayer->GetOpacity());
        fd << endStr(sOpacity) << std::endl;
    }

    // Property: Watermarks (optional)
    int watermarkCount = gridLayer->GetWatermarks()->GetCount();
    if (watermarkCount != 0)
    {
        if (!version || (*version >= Version(2, 3, 0)))
        {
            // only write Watermarks if the LDF version is 2.3.0 or greater
            fd << tab.tab() << startStr(sWatermarks) << std::endl;
            tab.inctab();
            for (int i=0; i<watermarkCount; ++i)
                IOWatermarkInstance::Write(fd, gridLayer->GetWatermarks()->GetAt(i), version, tab);
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
                IOWatermarkInstance::Write(fdExtData, gridLayer->GetWatermarks()->GetAt(i), version, tab);
            tab.dectab();
            fdExtData << tab.tab() << endStr(sWatermarks) << std::endl;

            tab.dectab();
        }
    }

    // Property: FeatureName
    fd << tab.tab() << startStr(sFeatureName);
    fd << EncodeString(gridLayer->GetFeatureName());
    fd << endStr(sFeatureName) << std::endl;

    // Property: Geometry
    fd << tab.tab() << startStr(sGeometry);
    fd << EncodeString(gridLayer->GetGeometry());
    fd << endStr(sGeometry) << std::endl;

    // Property: Filter
    if (!gridLayer->GetFilter().empty())
    {
        fd << tab.tab() << startStr(sFilter);
        fd << EncodeString(gridLayer->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: GridScaleRange
    for (int i=0; i<gridLayer->GetScaleRanges()->GetCount(); ++i)
        IOGridScaleRange::Write(fd, gridLayer->GetScaleRanges()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, gridLayer->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sGridLayerDefinition) << std::endl;

    tab.dectab();
    fd << tab.tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

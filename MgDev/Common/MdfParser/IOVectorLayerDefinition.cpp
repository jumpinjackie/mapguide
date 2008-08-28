//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

    case ePropertyMapping:
        {
            IONameStringPair* IO = new IONameStringPair(this->m_layer, this->m_version);
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
        this->m_layer->SetUrl(ch);
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


void IOVectorLayerDefinition::Write(MdfStream& fd, VectorLayerDefinition* vectorLayer, Version* version)
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
        else if ((*version >= Version(1, 0, 0)) && (*version <= Version(1, 3, 0)))
        {
            // LDF in MapGuide 2007 / 2008 / 2009 / 2010
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
        strVersion = L"1.3.0";
    }

    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << startStr(sVectorLayerDefinition) << std::endl;
    inctab();

    // Property: ResourceId
    fd << tab() << startStr(sResourceId);
    fd << EncodeString(vectorLayer->GetResourceID());
    fd << endStr(sResourceId) << std::endl;

    // Property: Opacity (optional)
    if (vectorLayer->GetOpacity() != 1.0)
    {
        fd << tab() << startStr(sOpacity);
        fd << DoubleToStr(vectorLayer->GetOpacity());
        fd << endStr(sOpacity) << std::endl;
    }

    // Property: FeatureName
    fd << tab() << startStr(sFeatureName);
    fd << EncodeString(vectorLayer->GetFeatureName());
    fd << endStr(sFeatureName) << std::endl;

    // Property: FeatureNameType
    fd << tab() << startStr(sFeatureNameType);
    if (vectorLayer->GetFeatureNameType() == VectorLayerDefinition::FeatureClass)
        fd << "FeatureClass"; // NOXLATE
    else
        fd << "NamedExtension"; // NOXLATE
    fd << endStr(sFeatureNameType) << std::endl;

    // Property: Filter
    if (!vectorLayer->GetFilter().empty())
    {
        fd << tab() << startStr(sFilter);
        fd << EncodeString(vectorLayer->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: PropertyMappings
    for (int i=0; i<vectorLayer->GetPropertyMappings()->GetCount(); ++i)
        IONameStringPair::Write(fd, sPropertyMapping, vectorLayer->GetPropertyMappings()->GetAt(i), version);

    // Property: Geometry
    fd << tab() << startStr(sGeometry);
    fd << EncodeString(vectorLayer->GetGeometry());
    fd << endStr(sGeometry) << std::endl;

    // Property: Url
    if (!vectorLayer->GetUrl().empty())
    {
        fd << tab() << startStr(sUrl);
        fd << EncodeString(vectorLayer->GetUrl());
        fd << endStr(sUrl) << std::endl;
    }

    // Property: ToolTip
    if (!vectorLayer->GetToolTip().empty())
    {
        fd << tab() << startStr(sToolTip);
        fd << EncodeString(vectorLayer->GetToolTip());
        fd << endStr(sToolTip) << std::endl;
    }

    // Property: VectorScaleRange
    for (int i=0; i<vectorLayer->GetScaleRanges()->GetCount(); ++i)
        IOVectorScaleRange::Write(fd, vectorLayer->GetScaleRanges()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, vectorLayer->GetUnknownXml(), version);

    dectab();
    fd << tab() << endStr(sVectorLayerDefinition) << std::endl;

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

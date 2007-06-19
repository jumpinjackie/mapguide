//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, VectorLayerDefinition);
ELEM_MAP_ENTRY(2, PropertyMapping);
ELEM_MAP_ENTRY(3, VectorScaleRange);
ELEM_MAP_ENTRY(4, Opacity);
ELEM_MAP_ENTRY(5, ResourceId);
ELEM_MAP_ENTRY(6, FeatureName);
ELEM_MAP_ENTRY(7, FeatureNameType);
ELEM_MAP_ENTRY(8, Geometry);
ELEM_MAP_ENTRY(9, Url);
ELEM_MAP_ENTRY(10, ToolTip);
ELEM_MAP_ENTRY(11, Filter);


IOVectorLayerDefinition::IOVectorLayerDefinition()
{
    this->_layer = NULL;
}


IOVectorLayerDefinition::IOVectorLayerDefinition(VectorLayerDefinition* layer)
{
    this->_layer = layer;
}


IOVectorLayerDefinition::~IOVectorLayerDefinition()
{
}


void IOVectorLayerDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId) {
        case eVectorLayerDefinition:
            m_startElemName = name;
            break;

        case ePropertyMapping:
            {
                IONameStringPair* IO = new IONameStringPair(this->_layer);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eVectorScaleRange:
            {
                IOVectorScaleRange* IO = new IOVectorScaleRange(this->_layer);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eUnknown:
            ParseUnknownXml(name, handlerStack);
            break;

        default:
            break;
    }
}


void IOVectorLayerDefinition::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId) {
        case eOpacity:
            (this->_layer)->SetOpacity(wstrToDouble(ch));
            break;

        case eResourceId:
            (this->_layer)->SetResourceID(ch);
            break;

        case eFeatureName:
            (this->_layer)->SetFeatureName(ch);
            break;

        case eFeatureNameType:
            if (::wcscmp(ch, L"FeatureClass") == 0) // NOXLATE
                _layer->SetFeatureNameType(VectorLayerDefinition::FeatureClass);
            else if (::wcscmp(ch, L"NamedExtension") == 0) // NOXLATE
                _layer->SetFeatureNameType(VectorLayerDefinition::NamedExtension);
            break;

        case eGeometry:
            (this->_layer)->SetGeometry(ch);
            break;

        case eUrl:
            (this->_layer)->SetUrl(ch);
            break;

        case eToolTip:
            (this->_layer)->SetToolTip(ch);
            break;

        case eFilter:
            (this->_layer)->SetFilter(ch);
            break;

        default:
            break;
    }
}


void IOVectorLayerDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_layer->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->_layer = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOVectorLayerDefinition::Write(MdfStream& fd, VectorLayerDefinition* featureLayer, Version* version)
{
    // we currently only support version 1.0.0 and 1.1.0
    if (version && (*version != Version(1, 0, 0)) && (*version != Version(1, 1, 0)))
    {
        // TODO - need a way to return error information
        _ASSERT(false);
        return;
    }

    MdfString strVersion = version? version->ToString() : L"1.1.0";
    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << startStr(sVectorLayerDefinition) << std::endl;
    inctab();

    //Property: ResourceId
    fd << tab() << startStr(sResourceId);
    fd << EncodeString(featureLayer->GetResourceID());
    fd << endStr(sResourceId) << std::endl;

    //Property: Opacity (optional)
    if (featureLayer->GetOpacity() != 1.0)
    {
        fd << tab() << startStr(sOpacity);
        fd << DoubleToStr(featureLayer->GetOpacity());
        fd << endStr(sOpacity) << std::endl;
    }

    //Property: FeatureName
    fd << tab() << startStr(sFeatureName);
    fd << EncodeString(featureLayer->GetFeatureName());
    fd << endStr(sFeatureName) << std::endl;

    //Property: FeatureNameType
    fd << tab() << startStr(sFeatureNameType);
    if (featureLayer->GetFeatureNameType() == VectorLayerDefinition::FeatureClass)
        fd << "FeatureClass"; // NOXLATE
    else
        fd << "NamedExtension"; // NOXLATE
    fd << endStr(sFeatureNameType) << std::endl;

    //Property: Filter
    if (featureLayer->GetFilter().length() > 0)
    {
        fd << tab() << startStr(sFilter);
        fd << EncodeString(featureLayer->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    //Property: PropertyMappings
    for (int i=0; i<featureLayer->GetPropertyMappings()->GetCount(); ++i)
    {
        fd << tab() << startStr(sPropertyMapping) << std::endl;
        inctab();
        IONameStringPair::Write(fd, featureLayer->GetPropertyMappings()->GetAt(i), version);
        dectab();
        fd << tab() << endStr(sPropertyMapping) << std::endl;
    }

    //Property: Geometry
    fd << tab() << startStr(sGeometry);
    fd << EncodeString(featureLayer->GetGeometry());
    fd << endStr(sGeometry) << std::endl;

    //Property: Url
    if (featureLayer->GetUrl().length() > 0)
    {
        fd << tab() << startStr(sUrl);
        fd << EncodeString(featureLayer->GetUrl());
        fd << endStr(sUrl) << std::endl;
    }

    //Property: ToolTip
    if (featureLayer->GetToolTip().length() > 0)
    {
        fd << tab() << startStr(sToolTip);
        fd << EncodeString(featureLayer->GetToolTip());
        fd << endStr(sToolTip) << std::endl;
    }

    //Property: VectorScaleRange
    for (int i=0; i<featureLayer->GetScaleRanges()->GetCount(); ++i)
        IOVectorScaleRange::Write(fd, featureLayer->GetScaleRanges()->GetAt(i), version);

    // Write any previously found unknown XML
    if (!featureLayer->GetUnknownXml().empty())
        fd << tab() << toCString(featureLayer->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << endStr(sVectorLayerDefinition) << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

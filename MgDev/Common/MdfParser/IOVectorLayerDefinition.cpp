//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

IOVectorLayerDefinition::IOVectorLayerDefinition()
{
    this->_layer = NULL;
}

IOVectorLayerDefinition::IOVectorLayerDefinition(VectorLayerDefinition *layer)
{
    this->_layer = layer;
}

IOVectorLayerDefinition::~IOVectorLayerDefinition()
{
}

void IOVectorLayerDefinition::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"VectorLayerDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"PropertyMapping") // NOXLATE
    {
        IONameStringPair *IO = new IONameStringPair(this->_layer);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"VectorScaleRange") // NOXLATE
    {
        IOVectorScaleRange *IO = new IOVectorScaleRange(this->_layer);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOVectorLayerDefinition::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Opacity") // NOXLATE
        (this->_layer)->SetOpacity(wstrToDouble(ch));
    else if (m_currElemName == L"ResourceId") // NOXLATE
        (this->_layer)->SetResourceID(ch);
    else if (m_currElemName == L"FeatureName") // NOXLATE
        (this->_layer)->SetFeatureName(ch);
    else if (m_currElemName == L"FeatureNameType") // NOXLATE
    {
        if (::wcscmp(ch, L"FeatureClass") == 0) // NOXLATE
            _layer->SetFeatureNameType(VectorLayerDefinition::FeatureClass);
        else if (::wcscmp(ch, L"NamedExtension") == 0) // NOXLATE
            _layer->SetFeatureNameType(VectorLayerDefinition::NamedExtension);
    }
    else if (m_currElemName == L"Geometry") // NOXLATE
        (this->_layer)->SetGeometry(ch); // NOXLATE
    else if (m_currElemName == L"Url") // NOXLATE
        (this->_layer)->SetUrl(ch);
    else if (m_currElemName == L"ToolTip") // NOXLATE
        (this->_layer)->SetToolTip(ch);
    else if (m_currElemName == L"Filter")  // NOXLATE
        (this->_layer)->SetFilter(ch);
}

void IOVectorLayerDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->_layer = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOVectorLayerDefinition::Write(MdfStream &fd, VectorLayerDefinition *featureLayer)
{
    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<VectorLayerDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>";  // NOXLATE
    fd << EncodeString(featureLayer->GetResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Opacity (optional)
    if (featureLayer->GetOpacity() != 1.0)
    {
        fd << tab() << "<Opacity>"; // NOXLATE
        fd << DoubleToStr(featureLayer->GetOpacity());
        fd << "</Opacity>" << std::endl; // NOXLATE
    }

    //Property: FeatureName
    fd << tab() << "<FeatureName>";  // NOXLATE
    fd << EncodeString(featureLayer->GetFeatureName());
    fd << "</FeatureName>" << std::endl; // NOXLATE

    //Property: FeatureNameType
    fd << tab() << "<FeatureNameType>";  // NOXLATE
    if (featureLayer->GetFeatureNameType() == VectorLayerDefinition::FeatureClass)
        fd << "FeatureClass"; // NOXLATE
    else
        fd << "NamedExtension"; // NOXLATE
    fd << "</FeatureNameType>" << std::endl; // NOXLATE

    //Property: Filter
    if (featureLayer->GetFilter().length() > 0)
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(featureLayer->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    //Property: PropertyMappings
    for (int x = 0; x < featureLayer->GetPropertyMappings()->GetCount(); x++)
    {
        fd << tab() << "<PropertyMapping>" << std::endl; // NOXLATE
        inctab();
        IONameStringPair * IO = new IONameStringPair();
        IO->Write(fd, featureLayer->GetPropertyMappings()->GetAt(x));
        delete IO;
        dectab();
        fd << tab() << "</PropertyMapping>" << std::endl; // NOXLATE
    }

    //Property: Geometry
    fd << tab() << "<Geometry>";  // NOXLATE
    fd << EncodeString(featureLayer->GetGeometry());
    fd << "</Geometry>" << std::endl; // NOXLATE

    //Property: Url
    if (featureLayer->GetUrl().length() > 0)
    {
        fd << tab() << "<Url>";  // NOXLATE
        fd << EncodeString(featureLayer->GetUrl());
        fd << "</Url>" << std::endl; // NOXLATE
    }

    //Property: ToolTip
    if (featureLayer->GetToolTip().length() > 0)
    {
        fd << tab() << "<ToolTip>";  // NOXLATE
        fd << EncodeString(featureLayer->GetToolTip());
        fd << "</ToolTip>" << std::endl; // NOXLATE
    }

    //Property: VectorScaleRange
    for (int x = 0; x < featureLayer->GetScaleRanges()->GetCount(); x++)
    {
        IOVectorScaleRange * IO = new IOVectorScaleRange();
        IO->Write(fd, featureLayer->GetScaleRanges()->GetAt(x));
        delete IO;
    }

    dectab();
    fd << tab() << "</VectorLayerDefinition>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

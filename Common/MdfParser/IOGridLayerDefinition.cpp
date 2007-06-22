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
#include "IOGridLayerDefinition.h"
#include "IOGridScaleRange.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, GridLayerDefinition);
ELEM_MAP_ENTRY(2, GridScaleRange);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, FeatureName);
ELEM_MAP_ENTRY(5, Geometry);
ELEM_MAP_ENTRY(6, Filter);
ELEM_MAP_ENTRY(7, Opacity);


IOGridLayerDefinition::IOGridLayerDefinition()
{
    this->m_layer = NULL;
}


IOGridLayerDefinition::IOGridLayerDefinition(GridLayerDefinition* layer)
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

    case eGridScaleRange:
        {
            IOGridScaleRange* IO = new IOGridScaleRange(this->m_layer);
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


void IOGridLayerDefinition::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"ResourceId") // NOXLATE
        this->m_layer->SetResourceID(ch);
    else if (this->m_currElemName == L"FeatureName") // NOXLATE
        this->m_layer->SetFeatureName(ch);
    else if (this->m_currElemName == L"Geometry") // NOXLATE
        this->m_layer->SetGeometry(ch); // NOXLATE
    else if (this->m_currElemName == L"Filter") // NOXLATE
        this->m_layer->SetFilter(ch);
    else if (this->m_currElemName == L"Opacity") // NOXLATE
        this->m_layer->SetOpacity(wstrToDouble(ch));
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
}


void IOGridLayerDefinition::Write(MdfStream& fd, GridLayerDefinition* gridLayer, Version* version)
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
        else if ((*version == Version(1, 0, 0)) || (*version == Version(1, 1, 0)))
        {
            // LDF in MapGuide 2007 / 2008
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
        strVersion = L"1.1.0";
    }

    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<GridLayerDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(gridLayer->GetResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Opacity (optional)
    if (gridLayer->GetOpacity() != 1.0)
    {
        fd << tab() << "<Opacity>"; // NOXLATE
        fd << DoubleToStr(gridLayer->GetOpacity());
        fd << "</Opacity>" << std::endl; // NOXLATE
    }

    //Property: FeatureName
    fd << tab() << "<FeatureName>"; // NOXLATE
    fd << EncodeString(gridLayer->GetFeatureName());
    fd << "</FeatureName>" << std::endl; // NOXLATE

    //Property: Geometry
    fd << tab() << "<Geometry>"; // NOXLATE
    fd << EncodeString(gridLayer->GetGeometry());
    fd << "</Geometry>" << std::endl; // NOXLATE

    //Property: Filter
    if (gridLayer->GetFilter() != toMdfString(""))
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(gridLayer->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    //Property: GridScaleRange
    for (int i=0; i<gridLayer->GetScaleRanges()->GetCount(); ++i)
        IOGridScaleRange::Write(fd, gridLayer->GetScaleRanges()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, gridLayer->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</GridLayerDefinition>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

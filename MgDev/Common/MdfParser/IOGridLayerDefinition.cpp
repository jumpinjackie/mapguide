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
#include "IOGridLayerDefinition.h"
#include "IOGridScaleRange.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOGridLayerDefinition::IOGridLayerDefinition():_layer(NULL)
{
}

IOGridLayerDefinition::IOGridLayerDefinition(GridLayerDefinition *layer):_layer(layer)
{
}

IOGridLayerDefinition::~IOGridLayerDefinition()
{
}

void IOGridLayerDefinition::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"GridLayerDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"GridScaleRange") // NOXLATE
    {
        IOGridScaleRange *IO = new IOGridScaleRange(this->_layer);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOGridLayerDefinition::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"ResourceId") // NOXLATE
        (this->_layer)->SetResourceID(ch);
    else if (m_currElemName == L"FeatureName") // NOXLATE
        (this->_layer)->SetFeatureName(ch);
    else if (m_currElemName == L"Geometry") // NOXLATE
        (this->_layer)->SetGeometry(ch); // NOXLATE
    else if (m_currElemName == L"Filter")  // NOXLATE
        (this->_layer)->SetFilter(ch);
    else if (m_currElemName == L"Opacity")  // NOXLATE
        (this->_layer)->SetOpacity(wstrToDouble(ch));
}

void IOGridLayerDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->_layer = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOGridLayerDefinition::Write(MdfStream &fd, GridLayerDefinition *gridLayer)
{
    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<GridLayerDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>";  // NOXLATE
    fd << EncodeString(gridLayer->GetResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Opacity (optional)
    if (gridLayer->GetOpacity() != 1.0)
    {
        fd << tab() << "<Opacity>";  // NOXLATE
        fd << DoubleToStr(gridLayer->GetOpacity());
        fd << "</Opacity>" << std::endl; // NOXLATE
    }

    //Property: FeatureName
    fd << tab() << "<FeatureName>";  // NOXLATE
    fd << EncodeString(gridLayer->GetFeatureName());
    fd << "</FeatureName>" << std::endl; // NOXLATE

    //Property: Geometry
    fd << tab() << "<Geometry>";  // NOXLATE
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
    for (int x = 0; x < gridLayer->GetScaleRanges()->GetCount(); x++)
    {
        IOGridScaleRange * IO = new IOGridScaleRange();
        IO->Write(fd, gridLayer->GetScaleRanges()->GetAt(x));
        delete IO;
    }

    dectab();
    fd << tab() << "</GridLayerDefinition>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

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
#include "IODrawingLayerDefinition.h"
#include "VectorScaleRange.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, DrawingLayerDefinition);
ELEM_MAP_ENTRY(2, Opacity);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Sheet);
ELEM_MAP_ENTRY(5, LayerFilter);
ELEM_MAP_ENTRY(6, MinScale);
ELEM_MAP_ENTRY(7, MaxScale);


IODrawingLayerDefinition::IODrawingLayerDefinition()
{
    this->m_layer = NULL;
}


IODrawingLayerDefinition::IODrawingLayerDefinition(DrawingLayerDefinition* layer)
{
    this->m_layer = layer;
}


IODrawingLayerDefinition::~IODrawingLayerDefinition()
{
}


void IODrawingLayerDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eDrawingLayerDefinition:
        this->m_startElemName = name;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IODrawingLayerDefinition::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Opacity") // NOXLATE
        this->m_layer->SetOpacity(wstrToDouble(ch));
    else if (this->m_currElemName == L"ResourceId") // NOXLATE
        this->m_layer->SetResourceID(ch);
    else if (this->m_currElemName == L"Sheet") // NOXLATE
        this->m_layer->SetSheet(ch);
    else if (this->m_currElemName == L"LayerFilter") // NOXLATE
        this->m_layer->SetLayerFilter(ch);
    else if (this->m_currElemName == L"MinScale") // NOXLATE
        this->m_layer->SetMinScale(wstrToDouble(ch));
    else if (this->m_currElemName == L"MaxScale") // NOXLATE
        this->m_layer->SetMaxScale(wstrToDouble(ch));
}


void IODrawingLayerDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
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


void IODrawingLayerDefinition::Write(MdfStream& fd, DrawingLayerDefinition* drawingLayer, Version* version)
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

    fd << tab() << "<DrawingLayerDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(drawingLayer->GetResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Opacity (optional)
    if (drawingLayer->GetOpacity() != 1.0)
    {
        fd << tab() << "<Opacity>"; // NOXLATE
        fd << DoubleToStr(drawingLayer->GetOpacity());
        fd << "</Opacity>" << std::endl; // NOXLATE
    }

    // Property: Sheet
    fd << tab() << "<Sheet>"; // NOXLATE
    fd << EncodeString(drawingLayer->GetSheet());
    fd << "</Sheet>" << std::endl; // NOXLATE

    //Property: LayerFilter (optional)
    if (drawingLayer->GetLayerFilter().length() > 0)
    {
        fd << tab() << "<LayerFilter>"; // NOXLATE
        fd << EncodeString(drawingLayer->GetLayerFilter());
        fd << "</LayerFilter>" << std::endl; // NOXLATE
    }

    //Property: MinScale (optional)
    if (drawingLayer->GetMinScale() != 0.0)
    {
        fd << tab() << "<MinScale>"; // NOXLATE
        fd << DoubleToStr(drawingLayer->GetMinScale());
        fd << "</MinScale>" << std::endl; // NOXLATE
    }

    //Property: MaxScale (optional)
    if (drawingLayer->GetMaxScale() != VectorScaleRange::MAX_MAP_SCALE)
    {
        fd << tab() << "<MaxScale>"; // NOXLATE
        fd << DoubleToStr(drawingLayer->GetMaxScale());
        fd << "</MaxScale>" << std::endl; // NOXLATE
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, drawingLayer->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</DrawingLayerDefinition>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

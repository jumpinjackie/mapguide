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
#include "IODrawingLayerDefinition.h"
#include "VectorScaleRange.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IODrawingLayerDefinition::IODrawingLayerDefinition()
{
}

IODrawingLayerDefinition::IODrawingLayerDefinition(DrawingLayerDefinition * layer)
{
    this->_layer = layer;
}

IODrawingLayerDefinition::~IODrawingLayerDefinition()
{
}

void IODrawingLayerDefinition::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"DrawingLayerDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
}

void IODrawingLayerDefinition::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Opacity") // NOXLATE
        (this->_layer)->SetOpacity(wstrToDouble(ch));
    else if (m_currElemName == L"ResourceId") // NOXLATE
        (this->_layer)->SetResourceID(ch);
    else if (m_currElemName == L"Sheet") // NOXLATE
        (this->_layer)->SetSheet(ch);
    else if (m_currElemName == L"LayerFilter") // NOXLATE
        (this->_layer)->SetLayerFilter(ch);
    else if (m_currElemName == L"MinScale") // NOXLATE
        (this->_layer)->SetMinScale(wstrToDouble(ch));
    else if (m_currElemName == L"MaxScale") // NOXLATE
        (this->_layer)->SetMaxScale(wstrToDouble(ch));
}

void IODrawingLayerDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->_layer = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IODrawingLayerDefinition::Write(MdfStream &fd, DrawingLayerDefinition *drawingLayer)
{
    fd << tab() << "<LayerDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xsi:noNamespaceSchemaLocation=\"LayerDefinition-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<DrawingLayerDefinition>" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>";  // NOXLATE
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
    fd << tab() << "<Sheet>";  // NOXLATE
    fd << EncodeString(drawingLayer->GetSheet());
    fd << "</Sheet>" << std::endl; // NOXLATE

    //Property: LayerFilter (optional)
    if (drawingLayer->GetLayerFilter().length() > 0)
    {
        fd << tab() << "<LayerFilter>";  // NOXLATE
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

    dectab();
    fd << tab() << "</DrawingLayerDefinition>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</LayerDefinition>" << std::endl; // NOXLATE
}

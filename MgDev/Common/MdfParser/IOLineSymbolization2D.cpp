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
#include "IOLineSymbolization2D.h"
#include "IOStroke.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineSymbolization2D);
ELEM_MAP_ENTRY(2, LineStyle);
ELEM_MAP_ENTRY(3, Thickness);
ELEM_MAP_ENTRY(4, Color);
ELEM_MAP_ENTRY(5, Unit);
ELEM_MAP_ENTRY(6, SizeContext);


IOLineSymbolization2D::IOLineSymbolization2D()
{
    this->_lineSymbolization = NULL;
    this->lineRule = NULL;
}


IOLineSymbolization2D::IOLineSymbolization2D(LineRule* lineRule)
{
    this->_lineSymbolization = NULL;
    this->lineRule = lineRule;
}


IOLineSymbolization2D::~IOLineSymbolization2D()
{
}


void IOLineSymbolization2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eLineSymbolization2D:
        m_startElemName = name;
        this->_lineSymbolization = new LineSymbolization2D();
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOLineSymbolization2D::ElementChars(const wchar_t* ch)
{
    Stroke* stroke = this->_lineSymbolization->GetStroke();

    if (m_currElemName == swLineStyle)
        stroke->SetLineStyle(ch);
    else if (m_currElemName == swThickness)
        stroke->SetThickness(ch);
    else if (m_currElemName == swColor)
        stroke->SetColor(ch);
    else if (m_currElemName == swUnit)
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        stroke->SetUnit(unit);
    }
    else if (m_currElemName == swSizeContext)
    {
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            stroke->SetSizeContext(MdfModel::MappingUnits);
        else // "DeviceUnits" & default
            stroke->SetSizeContext(MdfModel::DeviceUnits);
    }
}


void IOLineSymbolization2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_lineSymbolization->GetStroke()->SetUnknownXml(UnknownXml());

        this->lineRule->GetSymbolizations()->Adopt(this->_lineSymbolization);
        handlerStack->pop();
        this->lineRule= NULL;
        this->_lineSymbolization = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOLineSymbolization2D::Write(MdfStream& fd, LineSymbolization2D* lineSymbolization, Version* version)
{
    // a LineSymbolization2D is just a Stroke
    IOStroke::Write(fd, lineSymbolization->GetStroke(), sLineSymbolization2D, version);
}

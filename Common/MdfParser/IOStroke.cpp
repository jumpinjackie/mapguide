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
#include "IOStroke.h"
#include "IOExtra.h"
#include "IOFill.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOStroke::IOStroke(std::wstring elementName)
{
    this->_stroke = NULL;
    this->m_elementName = elementName;
}

IOStroke::IOStroke(Stroke * stroke, std::wstring elementName)
{
    this->_stroke = stroke;
    this->m_elementName = elementName;
}

IOStroke::~IOStroke()
{
}

void IOStroke::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == this->m_elementName)
        m_startElemName = name;
}

void IOStroke::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LineStyle") // NOXLATE
        (this->_stroke)->SetLineStyle(ch);
    else if (m_currElemName == L"Thickness") // NOXLATE
        (this->_stroke)->SetThickness(ch);
    else if (m_currElemName == L"Color") // NOXLATE
        (this->_stroke)->SetColor(ch);
    else if (m_currElemName == L"Unit") // NOXLATE
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        this->_stroke->SetUnit(unit);
    }
}

void IOStroke::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->_stroke = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOStroke::Write(MdfStream &fd, Stroke *stroke, std::string name)
{
    fd << tab() << "<" << name << ">" << std::endl; // NOXLATE
    inctab();

    //Property: LineStyle
    fd << tab() << "<LineStyle>"; // NOXLATE
    fd << EncodeString(stroke->GetLineStyle());
    fd << "</LineStyle>" << std::endl; // NOXLATE

    //Property: Thickness
    fd << tab() << "<Thickness>"; // NOXLATE
    fd << EncodeString(stroke->GetThickness());
    fd << "</Thickness>" << std::endl; // NOXLATE

    //Property: ForegroundColor
    fd << tab() << "<Color>"; // NOXLATE
    fd << EncodeString(stroke->GetColor());
    fd << "</Color>" << std::endl; // NOXLATE

    //Property: Unit
    fd << tab() << "<Unit>"; // NOXLATE
    fd << EncodeString(*LengthConverter::UnitToEnglish(stroke->GetUnit()));
    fd << "</Unit>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</" << name << ">" << std::endl; // NOXLATE
}

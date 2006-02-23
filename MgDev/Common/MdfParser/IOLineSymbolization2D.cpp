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
#include "IOLineSymbolization2D.h"
#include "IOStroke.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOLineSymbolization2D::IOLineSymbolization2D()
{
}

IOLineSymbolization2D::IOLineSymbolization2D(LineRule * lineRule)
{
    this->lineRule = lineRule;
}

IOLineSymbolization2D::~IOLineSymbolization2D()
{
}

void IOLineSymbolization2D::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"LineSymbolization2D") // NOXLATE
    {
        m_startElemName = name;
        this->_lineSymbolization = new LineSymbolization2D();
    }
}

void IOLineSymbolization2D::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LineStyle") // NOXLATE
        this->_lineSymbolization->GetStroke()->SetLineStyle(ch);
    else if (m_currElemName == L"Thickness") // NOXLATE
        this->_lineSymbolization->GetStroke()->SetThickness(ch);
    else if (m_currElemName == L"Color") // NOXLATE
        this->_lineSymbolization->GetStroke()->SetColor(ch);
    else if (m_currElemName == L"Unit") // NOXLATE
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        this->_lineSymbolization->GetStroke()->SetUnit(unit);
    }
}

void IOLineSymbolization2D::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->lineRule->GetSymbolizations()->Adopt(this->_lineSymbolization);
        handlerStack->pop();
        this->lineRule= NULL;
        this->_lineSymbolization = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOLineSymbolization2D::Write(MdfStream &fd, LineSymbolization2D *lineSymbolization)
{
    //Property: Stroke
    if (lineSymbolization->GetStroke())
        IOStroke::Write(fd, lineSymbolization->GetStroke(), "LineSymbolization2D"); // NOXLATE
}

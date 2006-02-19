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
#include "IOPointRule.h"
#include "IOLabel.h"
#include "IOPointSymbolization2D.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOPointRule::IOPointRule()
{
}

IOPointRule::IOPointRule(PointTypeStyle * pointTypeStyle)
{
    this->pointTypeStyle = pointTypeStyle;
}

IOPointRule::~IOPointRule()
{
}

void IOPointRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"PointRule") // NOXLATE
    {
        m_startElemName = name;
        this->_pointRule = new PointRule();
    }
    else if (m_currElemName == L"PointSymbolization2D") // NOXLATE
    {
        IOPointSymbolization2D *IO = new IOPointSymbolization2D(this->_pointRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"Label") // NOXLATE
    {
        IOLabel *IO = new IOLabel(this->_pointRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOPointRule::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LegendLabel") // NOXLATE
        (this->_pointRule)->SetLegendLabel(ch);
    else if (m_currElemName == L"Filter") // NOXLATE
        (this->_pointRule)->SetFilter(ch);
}

void IOPointRule::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->pointTypeStyle->GetRules()->Adopt(this->_pointRule);
        handlerStack->pop();
        this->pointTypeStyle= NULL;
        this->_pointRule = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOPointRule::Write(MdfStream &fd, PointRule *pointRule)
{
    fd << tab() << "<PointRule>" << std::endl; // NOXLATE
    inctab();

    //Property: LegendLabel
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(pointRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if (pointRule->GetFilter().length() > 0)
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(pointRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }
    //Property: Label
    if (pointRule->GetLabel() != NULL && pointRule->GetLabel()->GetSymbol() != NULL)
    {
        IOLabel *IO2 = new IOLabel();
        IO2->Write(fd, pointRule->GetLabel());
        delete IO2;
    }

    //Property: Symbolization
    PointSymbolization2D *symbolization2d = static_cast<PointSymbolization2D*>(pointRule->GetSymbolization());
    if (symbolization2d != NULL)
    {
        IOPointSymbolization2D *IO = new IOPointSymbolization2D();
        IO->Write(fd, symbolization2d);
        delete IO;
    }

    dectab();
    fd << tab() << "</PointRule>" << std::endl; // NOXLATE
}

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
#include "IOGridColorRule.h"
#include "IOGridColor.h"
#include "IOExtra.h"
#include "IOLabel.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOGridColorRule::IOGridColorRule():colorStyle(NULL), colorRule(NULL)
{
}

IOGridColorRule::IOGridColorRule(GridColorStyle * pColorStyle):colorStyle(pColorStyle), colorRule(NULL)
{
}

IOGridColorRule::~IOGridColorRule()
{
}

void IOGridColorRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"ColorRule") // NOXLATE
    {
        m_startElemName = name;
        this->colorRule = new GridColorRule();
    }
    else if (m_currElemName == L"Color")  // NOXLATE
    {
        IOGridColor *IO = new IOGridColor(this->colorRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"Label") // NOXLATE
    {
        IOLabel *IO = new IOLabel(this->colorRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOGridColorRule::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LegendLabel") // NOXLATE
        (this->colorRule)->SetLegendLabel(ch);
    else if (m_currElemName == L"Filter") // NOXLATE
        (this->colorRule)->SetFilter(ch);
}

void IOGridColorRule::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->colorStyle->GetRules()->Adopt(this->colorRule);
        handlerStack->pop();
        this->colorStyle = NULL;
        this->colorRule = NULL;
        m_startElemName = L"";
        delete this;
    }
}
void IOGridColorRule::Write(MdfStream &fd,  GridColorRule *pColorRule)
{
    fd << tab() << "<ColorRule>" << std::endl; // NOXLATE
    inctab();

    //Property: Legend Lable
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(pColorRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if(pColorRule->GetFilter() != toMdfString(""))
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(pColorRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    // Property: Label
    if (pColorRule->GetLabel() != NULL)
    {
        IOLabel *IO = new IOLabel();
        IO->Write(fd, pColorRule->GetLabel());
        delete IO;
    }

    // Property : GridColor
    IOGridColor* pIO = new IOGridColor();
    pIO->Write(fd, pColorRule->GetGridColor());
    delete pIO;

    dectab();
    fd << tab() << "</ColorRule>" << std::endl; // NOXLATE
}

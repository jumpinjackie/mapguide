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
#include "IOGridColorStyle.h"
#include "IOGridColorRule.h"
#include "IOHillShade.h"
#include "IOExtra.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ColorStyle);
ELEM_MAP_ENTRY(2, HillShade);
ELEM_MAP_ENTRY(3, ColorRule);
ELEM_MAP_ENTRY(4, TransparencyColor);
ELEM_MAP_ENTRY(5, BrightnessFactor);
ELEM_MAP_ENTRY(6, ContrastFactor);


IOGridColorStyle::IOGridColorStyle()
{
    this->m_colorStyle = NULL;
    this->m_scaleRange = NULL;
}


IOGridColorStyle::IOGridColorStyle(GridScaleRange* scaleRange)
{
    this->m_colorStyle = NULL;
    this->m_scaleRange = scaleRange;
}


IOGridColorStyle::~IOGridColorStyle()
{
}


void IOGridColorStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eColorStyle:
        this->m_startElemName = name;
        this->m_colorStyle = new GridColorStyle();
        break;

    case eHillShade:
        {
            IOHillShade* IO = new IOHillShade(this->m_colorStyle);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColorRule:
        {
            IOGridColorRule* IO = new IOGridColorRule(this->m_colorStyle);
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


void IOGridColorStyle::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"TransparencyColor") // NOXLATE
        this->m_colorStyle->SetTransparencyColor(ch);
    else if (this->m_currElemName == L"BrightnessFactor") // NOXLATE
        this->m_colorStyle->SetBrightnessFactor(wstrToDouble(ch));
    else if (this->m_currElemName == L"ContrastFactor") // NOXLATE
        this->m_colorStyle->SetContrastFactor(wstrToDouble(ch));
}


void IOGridColorStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_colorStyle->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->AdoptColorStyle(this->m_colorStyle);
        this->m_scaleRange = NULL;
        this->m_colorStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOGridColorStyle::Write(MdfStream& fd, GridColorStyle* colorStyle, Version* version)
{
    fd << tab() << "<ColorStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: HillShade
    if (NULL != colorStyle->GetHillShade())
        IOHillShade::Write(fd, colorStyle->GetHillShade(), version);

    //Property: TransparencyColor (optional)
    if (colorStyle->GetTransparencyColor() != toMdfString(""))
    {
        fd << tab() << "<TransparencyColor>"; // NOXLATE
        fd << EncodeString(colorStyle->GetTransparencyColor());
        fd << "</TransparencyColor>" << std::endl; // NOXLATE
    }

    //Property: BrightnessFactor (optional)
    if (colorStyle->GetBrightnessFactor() != 0.0)
    {
        fd << tab() << "<BrightnessFactor>"; // NOXLATE
        fd << DoubleToStr(colorStyle->GetBrightnessFactor());
        fd << "</BrightnessFactor>" << std::endl; // NOXLATE
    }

    //Property: ContrastFactor (optional)
    if (colorStyle->GetContrastFactor() != 0.0)
    {
        fd << tab() << "<ContrastFactor>"; // NOXLATE
        fd << DoubleToStr(colorStyle->GetContrastFactor());
        fd << "</ContrastFactor>" << std::endl; // NOXLATE
    }

    //Property: Color Rules
    RuleCollection* colorRules = colorStyle->GetRules();
    for (int i=0; i<colorRules->GetCount(); ++i)
    {
        GridColorRule* colorRule = static_cast<GridColorRule*>(colorRules->GetAt(i));
        if (NULL != colorRule)
            IOGridColorRule::Write(fd, colorRule, version);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorStyle->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</ColorStyle>" << std::endl; // NOXLATE
}

//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ColorStyle);
ELEM_MAP_ENTRY(2, HillShade);
ELEM_MAP_ENTRY(3, TransparencyColor);
ELEM_MAP_ENTRY(4, BrightnessFactor);
ELEM_MAP_ENTRY(5, ContrastFactor);
ELEM_MAP_ENTRY(6, ColorRule);
ELEM_MAP_ENTRY(7, ExtendedData1);


IOGridColorStyle::IOGridColorStyle(Version& version) : SAX2ElementHandler(version)
{
    this->m_colorStyle = NULL;
    this->m_scaleRange = NULL;
}


IOGridColorStyle::IOGridColorStyle(GridScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
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
            IOHillShade* IO = new IOHillShade(this->m_colorStyle, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColorRule:
        {
            IOGridColorRule* IO = new IOGridColorRule(this->m_colorStyle, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOGridColorStyle::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eTransparencyColor:
        this->m_colorStyle->SetTransparencyColor(ch);
        break;

    case eBrightnessFactor:
        this->m_colorStyle->SetBrightnessFactor(wstrToDouble(ch));
        break;

    case eContrastFactor:
        this->m_colorStyle->SetContrastFactor(wstrToDouble(ch));
        break;
    }
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridColorStyle::Write(MdfStream& fd, GridColorStyle* colorStyle, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sColorStyle) << std::endl;
    tab.inctab();

    // Property: HillShade
    if (colorStyle->GetHillShade())
        IOHillShade::Write(fd, colorStyle->GetHillShade(), version, tab);

    // Property: TransparencyColor (optional)
    if (!colorStyle->GetTransparencyColor().empty())
    {
        fd << tab.tab() << startStr(sTransparencyColor);
        fd << EncodeString(colorStyle->GetTransparencyColor());
        fd << endStr(sTransparencyColor) << std::endl;
    }

    // Property: BrightnessFactor (optional)
    if (colorStyle->GetBrightnessFactor() != 0.0)
    {
        fd << tab.tab() << startStr(sBrightnessFactor);
        fd << DoubleToStr(colorStyle->GetBrightnessFactor());
        fd << endStr(sBrightnessFactor) << std::endl;
    }

    // Property: ContrastFactor (optional)
    if (colorStyle->GetContrastFactor() != 0.0)
    {
        fd << tab.tab() << startStr(sContrastFactor);
        fd << DoubleToStr(colorStyle->GetContrastFactor());
        fd << endStr(sContrastFactor) << std::endl;
    }

    // Property: Color Rules
    RuleCollection* colorRules = colorStyle->GetRules();
    for (int i=0; i<colorRules->GetCount(); ++i)
    {
        GridColorRule* colorRule = static_cast<GridColorRule*>(colorRules->GetAt(i));
        if (colorRule)
            IOGridColorRule::Write(fd, colorRule, version, tab);
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorStyle->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sColorStyle) << std::endl;
}

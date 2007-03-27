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

IOGridColorStyle::IOGridColorStyle():colorStyle(NULL), scaleRange(NULL)
{
}

IOGridColorStyle::IOGridColorStyle(GridScaleRange * pScaleRange):colorStyle(NULL), scaleRange(pScaleRange)
{
}

IOGridColorStyle::~IOGridColorStyle()
{
}

void IOGridColorStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eColorStyle:
        m_startElemName = name;
        this->colorStyle = new GridColorStyle();
        break;

    case eHillShade:
        {
            IOHillShade *IO = new IOHillShade(this->colorStyle);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColorRule:
        {
            IOGridColorRule *IO = new IOGridColorRule(this->colorStyle);
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

void IOGridColorStyle::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"TransparencyColor") // NOXLATE
        (this->colorStyle)->SetTransparencyColor(ch);
    else if (m_currElemName == L"BrightnessFactor") // NOXLATE
        (this->colorStyle)->SetBrightnessFactor(wstrToDouble(ch));
    else if (m_currElemName == L"ContrastFactor") // NOXLATE
        (this->colorStyle)->SetContrastFactor(wstrToDouble(ch));
}

void IOGridColorStyle::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->colorStyle->SetUnknownXml(UnknownXml());

        this->scaleRange->AdoptColorStyle(this->colorStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->colorStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOGridColorStyle::Write(MdfStream &fd,  GridColorStyle *pColorStyle)
{
    fd << tab() << "<ColorStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: HillShade
    if (NULL != pColorStyle->GetHillShade())
    {
        IOHillShade* pIOHillShade = new IOHillShade();
        pIOHillShade->Write(fd, pColorStyle->GetHillShade());
        delete pIOHillShade;
    }

    //Property: TransparencyColor (optional)
    if (pColorStyle->GetTransparencyColor() != toMdfString(""))
    {
        fd << tab() << "<TransparencyColor>"; // NOXLATE
        fd << EncodeString(pColorStyle->GetTransparencyColor());
        fd << "</TransparencyColor>" << std::endl; // NOXLATE
    }

    //Property: BrightnessFactor (optional)
    if (pColorStyle->GetBrightnessFactor() != 0.0)
    {
        fd << tab() << "<BrightnessFactor>"; // NOXLATE
        fd << DoubleToStr(pColorStyle->GetBrightnessFactor());
        fd << "</BrightnessFactor>" << std::endl; // NOXLATE
    }

    //Property: ContrastFactor (optional)
    if (pColorStyle->GetContrastFactor() != 0.0)
    {
        fd << tab() << "<ContrastFactor>"; // NOXLATE
        fd << DoubleToStr(pColorStyle->GetContrastFactor());
        fd << "</ContrastFactor>" << std::endl; // NOXLATE
    }

    //Property: Color Rules
    RuleCollection *pColorRules = pColorStyle->GetRules();
    for (int x = 0; x < pColorRules->GetCount(); ++x)
    {
        GridColorRule *pColorRule = static_cast<GridColorRule*>(pColorRules->GetAt(x));
        if (NULL != pColorRule)
        {
            IOGridColorRule * IO = new IOGridColorRule();
            IO->Write(fd, pColorRule);
            delete IO;
        }
    }

    // Write any previously found unknown XML
    if (!pColorStyle->GetUnknownXml().empty())
    {
        fd << toCString(pColorStyle->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</ColorStyle>" << std::endl; // NOXLATE
}

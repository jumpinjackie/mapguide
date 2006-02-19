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
#include "IOGridColorStyle.h"
#include "IOGridColorRule.h"
#include "IOHillShade.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOGridColorStyle::IOGridColorStyle():scaleRange(NULL), colorStyle(NULL)
{
}

IOGridColorStyle::IOGridColorStyle(GridScaleRange * pScaleRange):scaleRange(pScaleRange), colorStyle(NULL)
{
}

IOGridColorStyle::~IOGridColorStyle()
{
}

void IOGridColorStyle::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"ColorStyle") // NOXLATE
    {
        m_startElemName = name;
        this->colorStyle = new GridColorStyle();
    }
    else if (m_currElemName == L"HillShade") // NOXLATE
    {
        IOHillShade *IO = new IOHillShade(this->colorStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ColorRule") // NOXLATE
    {
        IOGridColorRule *IO = new IOGridColorRule(this->colorStyle);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
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

    dectab();
    fd << tab() << "</ColorStyle>" << std::endl; // NOXLATE
}

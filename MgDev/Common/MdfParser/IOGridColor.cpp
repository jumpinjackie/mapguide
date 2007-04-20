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
#include "IOGridColor.h"
#include "IOChannelBand.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Color);
ELEM_MAP_ENTRY(2, Bands);
ELEM_MAP_ENTRY(3, ExplicitColor);
ELEM_MAP_ENTRY(4, Band);

IOGridColor::IOGridColor():colorRule(NULL)
{
}

IOGridColor::IOGridColor(GridColorRule * pColorRule):colorRule(pColorRule)
{
}

IOGridColor::~IOGridColor()
{
}

void IOGridColor::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eColor:
        m_startElemName = name;
        break;

    case eBands:
        {
            IOGridColorBands* pIO = new IOGridColorBands(colorRule);
            handlerStack->push(pIO);
            pIO->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOGridColor::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"ExplicitColor") // NOXLATE
    {
        MdfModel::GridColorExplicit *pColor = new MdfModel::GridColorExplicit();
        pColor->SetExplicitColor(ch);
        this->colorRule->AdoptGridColor(pColor);
    }
    else if (m_currElemName == L"Band") // NOXLATE
    {
        MdfModel::GridColorBand *pColor = new MdfModel::GridColorBand();
        pColor->SetBand(ch);
        this->colorRule->AdoptGridColor(pColor);
    }
}

void IOGridColor::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->colorRule->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->colorRule = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOGridColor::Write(MdfStream &fd,  GridColor *pColor)
{
    fd << tab() << "<Color>" << std::endl; // NOXLATE
    inctab();

    GridColorExplicit* pColorExplicit = dynamic_cast<GridColorExplicit*>(pColor);
    if (NULL != pColorExplicit)
    {
        fd << tab() << "<ExplicitColor>"; // NOXLATE
        fd << EncodeString(pColorExplicit->GetExplicitColor());
        fd << "</ExplicitColor>" << std::endl; // NOXLATE
    }

    GridColorBand* pColorBand = dynamic_cast<GridColorBand*>(pColor);
    if (NULL != pColorBand)
    {
        fd << tab() << "<Band>"; // NOXLATE
        fd << EncodeString(pColorBand->GetBand());
        fd << "</Band>" << std::endl; // NOXLATE
    }

    GridColorBands* pColorBands = dynamic_cast<GridColorBands*>(pColor);
    if (NULL != pColorBands)
    {
        IOGridColorBands* pIO = new IOGridColorBands();
        pIO->Write(fd, pColorBands);
        delete pIO;
    }

    // Write any previously found unknown XML
    if (!pColor->GetUnknownXml().empty())
    {
        fd << toCString(pColor->GetUnknownXml());
    }

    dectab();
    fd << tab() << "</Color>" << std::endl; // NOXLATE
}


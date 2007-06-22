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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Color);
ELEM_MAP_ENTRY(2, Bands);
ELEM_MAP_ENTRY(3, ExplicitColor);
ELEM_MAP_ENTRY(4, Band);


IOGridColor::IOGridColor()
{
    this->m_colorRule = NULL;
}


IOGridColor::IOGridColor(GridColorRule* colorRule)
{
    this->m_colorRule = colorRule;
}


IOGridColor::~IOGridColor()
{
}


void IOGridColor::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eColor:
        this->m_startElemName = name;
        break;

    case eBands:
        {
            IOGridColorBands* IO = new IOGridColorBands(this->m_colorRule);
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


void IOGridColor::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"ExplicitColor") // NOXLATE
    {
        MdfModel::GridColorExplicit* color = new MdfModel::GridColorExplicit();
        color->SetExplicitColor(ch);
        this->m_colorRule->AdoptGridColor(color);
    }
    else if (this->m_currElemName == L"Band") // NOXLATE
    {
        MdfModel::GridColorBand* color = new MdfModel::GridColorBand();
        color->SetBand(ch);
        this->m_colorRule->AdoptGridColor(color);
    }
}


void IOGridColor::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_colorRule->SetUnknownXml(this->m_unknownXml);

        this->m_colorRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOGridColor::Write(MdfStream& fd, GridColor* color, Version* version)
{
    fd << tab() << "<Color>" << std::endl; // NOXLATE
    inctab();

    GridColorExplicit* colorExplicit = dynamic_cast<GridColorExplicit*>(color);
    if (NULL != colorExplicit)
    {
        fd << tab() << "<ExplicitColor>"; // NOXLATE
        fd << EncodeString(colorExplicit->GetExplicitColor());
        fd << "</ExplicitColor>" << std::endl; // NOXLATE
    }

    GridColorBand* colorBand = dynamic_cast<GridColorBand*>(color);
    if (NULL != colorBand)
    {
        fd << tab() << "<Band>"; // NOXLATE
        fd << EncodeString(colorBand->GetBand());
        fd << "</Band>" << std::endl; // NOXLATE
    }

    GridColorBands* colorBands = dynamic_cast<GridColorBands*>(color);
    if (NULL != colorBands)
        IOGridColorBands::Write(fd, colorBands, version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, color->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Color>" << std::endl; // NOXLATE
}

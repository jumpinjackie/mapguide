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
#include "IOGridColor.h"
#include "IOChannelBand.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Color);
ELEM_MAP_ENTRY(2, ExplicitColor);
ELEM_MAP_ENTRY(3, Band);
ELEM_MAP_ENTRY(4, Bands);
ELEM_MAP_ENTRY(5, ExtendedData1);


IOGridColor::IOGridColor(Version& version) : SAX2ElementHandler(version)
{
    this->m_colorRule = NULL;
}


IOGridColor::IOGridColor(GridColorRule* colorRule, Version& version) : SAX2ElementHandler(version)
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
            IOGridColorBands* IO = new IOGridColorBands(this->m_colorRule, this->m_version);
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


void IOGridColor::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eExplicitColor:
        {
            GridColorExplicit* color = new GridColorExplicit();
            color->SetExplicitColor(ch);
            this->m_colorRule->AdoptGridColor(color);
        }
        break;

    case eBand:
        {
            GridColorBand* color = new GridColorBand();
            color->SetBand(ch);
            this->m_colorRule->AdoptGridColor(color);
        }
        break;
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridColor::Write(MdfStream& fd, GridColor* color, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sColor) << std::endl;
    tab.inctab();

    GridColorExplicit* colorExplicit = dynamic_cast<GridColorExplicit*>(color);
    if (colorExplicit)
    {
        fd << tab.tab() << startStr(sExplicitColor);
        fd << EncodeString(colorExplicit->GetExplicitColor());
        fd << endStr(sExplicitColor) << std::endl;
    }

    GridColorBand* colorBand = dynamic_cast<GridColorBand*>(color);
    if (colorBand)
    {
        fd << tab.tab() << startStr(sBand);
        fd << EncodeString(colorBand->GetBand());
        fd << endStr(sBand) << std::endl;
    }

    GridColorBands* colorBands = dynamic_cast<GridColorBands*>(color);
    if (colorBands)
        IOGridColorBands::Write(fd, colorBands, version, tab);

    // Write any unknown XML / extended data
    if(color)
        IOUnknown::Write(fd, color->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sColor) << std::endl;
}

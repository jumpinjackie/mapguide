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
ELEM_MAP_ENTRY(1, Bands);
ELEM_MAP_ENTRY(2, RedBand);
ELEM_MAP_ENTRY(3, GreenBand);
ELEM_MAP_ENTRY(4, BlueBand);
ELEM_MAP_ENTRY(5, ExtendedData1);


IOGridColorBands::IOGridColorBands(Version& version) : IOGridColor(version)
{
    this->m_colorBands = NULL;
    this->m_redChannel = NULL;
    this->m_greenChannel = NULL;
    this->m_blueChannel = NULL;
}


IOGridColorBands::IOGridColorBands(GridColorRule* colorRule, Version& version) : IOGridColor(colorRule, version)
{
    this->m_colorBands = NULL;
    this->m_redChannel = NULL;
    this->m_greenChannel = NULL;
    this->m_blueChannel = NULL;
}


IOGridColorBands::~IOGridColorBands()
{
}


void IOGridColorBands::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eBands:
        this->m_startElemName = name;
        this->m_colorBands = new GridColorBands();
        break;

    case eRedBand:
        {
            this->m_redChannel = new ChannelBand();
            IOChannelBand* IO = new IOChannelBand(this->m_redChannel, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eGreenBand:
        {
            this->m_greenChannel = new ChannelBand();
            IOChannelBand* IO = new IOChannelBand(this->m_greenChannel, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eBlueBand:
        {
            this->m_blueChannel = new ChannelBand();
            IOChannelBand* IO = new IOChannelBand(this->m_blueChannel, this->m_version);
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


void IOGridColorBands::ElementChars(const wchar_t* ch)
{
}


void IOGridColorBands::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_colorBands->SetUnknownXml(this->m_unknownXml);

        this->m_colorBands->SetRedBand(*this->m_redChannel);
        this->m_colorBands->SetGreenBand(*this->m_greenChannel);
        this->m_colorBands->SetBlueBand(*this->m_blueChannel);
        this->m_colorRule->AdoptGridColor(this->m_colorBands);
        this->m_colorRule = NULL;
        this->m_colorBands = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridColorBands::Write(MdfStream& fd, GridColorBands* colorBands, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sBands) << std::endl;
    tab.inctab();

    IOChannelBand::Write(fd, &(colorBands->GetRedBand()), sRedBand, version, tab);
    IOChannelBand::Write(fd, &(colorBands->GetGreenBand()), sGreenBand, version, tab);
    IOChannelBand::Write(fd, &(colorBands->GetBlueBand()), sBlueBand, version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorBands->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sBands) << std::endl;
}

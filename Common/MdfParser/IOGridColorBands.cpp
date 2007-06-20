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
ELEM_MAP_ENTRY(1, Bands);
ELEM_MAP_ENTRY(2, RedBand);
ELEM_MAP_ENTRY(3, GreenBand);
ELEM_MAP_ENTRY(4, BlueBand);


IOGridColorBands::IOGridColorBands()
: colorBands(NULL)
, redChannel(NULL)
, greenChannel(NULL)
, blueChannel(NULL)
{
}


IOGridColorBands::IOGridColorBands(GridColorRule* colorRule)
: IOGridColor(colorRule)
, colorBands(NULL)
, redChannel(NULL)
, greenChannel(NULL)
, blueChannel(NULL)
{
}


IOGridColorBands::~IOGridColorBands()
{
}


void IOGridColorBands::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eBands:
        m_startElemName = name;
        this->colorBands = new GridColorBands();
        break;

    case eRedBand:
        {
            redChannel = new ChannelBand();
            IOChannelBand* pIO = new IOChannelBand(redChannel);
            handlerStack->push(pIO);
            pIO->StartElement(name, handlerStack);
        }
        break;

    case eGreenBand:
        {
            greenChannel = new ChannelBand();
            IOChannelBand* pIO = new IOChannelBand(greenChannel);
            handlerStack->push(pIO);
            pIO->StartElement(name, handlerStack);
        }
        break;

    case eBlueBand:
        {
            blueChannel = new ChannelBand();
            IOChannelBand* pIO = new IOChannelBand(blueChannel);
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


void IOGridColorBands::ElementChars(const wchar_t* ch)
{
}


void IOGridColorBands::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->colorBands->SetUnknownXml(UnknownXml());

        this->colorBands->SetRedBand(*redChannel);
        this->colorBands->SetGreenBand(*greenChannel);
        this->colorBands->SetBlueBand(*blueChannel);
        this->colorRule->AdoptGridColor(colorBands);
        handlerStack->pop();
        this->colorRule = NULL;
        this->colorBands = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOGridColorBands::Write(MdfStream& fd, GridColorBands* colorBands, Version* version)
{
    fd << tab() << "<Bands>" << std::endl; // NOXLATE
    inctab();

    IOChannelBand::Write(fd, &(colorBands->GetRedBand()), "RedBand", version); // NOXLATE
    IOChannelBand::Write(fd, &(colorBands->GetGreenBand()), "GreenBand", version); // NOXLATE
    IOChannelBand::Write(fd, &(colorBands->GetBlueBand()), "BlueBand", version); // NOXLATE

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorBands->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Bands>" << std::endl; // NOXLATE
}

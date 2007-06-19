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
ELEM_MAP_ENTRY(1, Bands);
ELEM_MAP_ENTRY(2, RedBand);
ELEM_MAP_ENTRY(3, GreenBand);
ELEM_MAP_ENTRY(4, BlueBand);


IOGridColorBands::IOGridColorBands()
: color(NULL)
, redChannel(NULL)
, greenChannel(NULL)
, blueChannel(NULL)
{
}


IOGridColorBands::IOGridColorBands(GridColorRule* colorRule)
: IOGridColor(colorRule)
, color(NULL)
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
        this->color = new GridColorBands();
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
        if (!UnknownXml().empty())
            this->color->SetUnknownXml(UnknownXml());

        this->color->SetRedBand(*redChannel);
        this->color->SetGreenBand(*greenChannel);
        this->color->SetBlueBand(*blueChannel);
        this->colorRule->AdoptGridColor(color);
        handlerStack->pop();
        this->colorRule = NULL;
        this->color     = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOGridColorBands::Write(MdfStream& fd, GridColorBands* pColor, Version* version)
{
    fd << tab() << "<Bands>" << std::endl; // NOXLATE
    inctab();

    IOChannelBand::Write(fd, &(pColor->GetRedBand()), "RedBand", version); // NOXLATE
    IOChannelBand::Write(fd, &(pColor->GetGreenBand()), "GreenBand", version); // NOXLATE
    IOChannelBand::Write(fd, &(pColor->GetBlueBand()), "BlueBand", version); // NOXLATE

    // Write any previously found unknown XML
    if (!pColor->GetUnknownXml().empty())
        fd << tab() << toCString(pColor->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</Bands>" << std::endl; // NOXLATE
}

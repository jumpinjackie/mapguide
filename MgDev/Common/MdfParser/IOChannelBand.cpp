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
#include "IOChannelBand.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
// From Grid Color - Start Elements
ELEM_MAP_ENTRY(1, RedBand);
ELEM_MAP_ENTRY(2, GreenBand);
ELEM_MAP_ENTRY(3, BlueBand);
// Local Elements
ELEM_MAP_ENTRY(4, Band);
ELEM_MAP_ENTRY(5, LowBand);
ELEM_MAP_ENTRY(6, HighBand);
ELEM_MAP_ENTRY(7, LowChannel);
ELEM_MAP_ENTRY(8, HighChannel);
ELEM_MAP_ENTRY(9, ExtendedData1);


IOChannelBand::IOChannelBand(Version& version) : SAX2ElementHandler(version)
{
    this->m_channel = NULL;
}


IOChannelBand::IOChannelBand(ChannelBand* channel, Version& version) : SAX2ElementHandler(version)
{
    this->m_channel = channel;
}


IOChannelBand::~IOChannelBand()
{
}


void IOChannelBand::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eRedBand:
    case eGreenBand:
    case eBlueBand:
        this->m_startElemName = name;
        if (this->m_channel == NULL)
            this->m_channel = new ChannelBand(); // m_channel maybe is created outside, then passed in
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOChannelBand::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eBand:
        this->m_channel->SetBand(ch);
        break;

    case eLowBand:
        this->m_channel->SetLowBand(wstrToDouble(ch));
        break;

    case eHighBand:
        this->m_channel->SetHighBand(wstrToDouble(ch));
        break;

    case eLowChannel:
        this->m_channel->SetLowChannel(wstrToInt(ch));
        break;

    case eHighChannel:
        this->m_channel->SetHighChannel(wstrToInt(ch));
        break;
    }
}


void IOChannelBand::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_channel->SetUnknownXml(this->m_unknownXml);

        this->m_channel = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOChannelBand::Write(MdfStream& fd, const ChannelBand* channel, std::string name, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Band
    fd << tab.tab() << startStr(sBand);
    fd << EncodeString(channel->GetBand());
    fd << endStr(sBand) << std::endl;

    // Property: LowBand
    // TODO: this is optional - it defaults to the low value in the band
    fd << tab.tab() << startStr(sLowBand);
    fd << DoubleToStr(channel->GetLowBand());
    fd << endStr(sLowBand) << std::endl;

    // Property: HighBand
    // TODO: this is optional - it defaults to the high value in the band
    fd << tab.tab() << startStr(sHighBand);
    fd << DoubleToStr(channel->GetHighBand());
    fd << endStr(sHighBand) << std::endl;

    // Property: LowChannel (optional)
    if (channel->GetLowChannel() != 0)
    {
        fd << tab.tab() << startStr(sLowChannel);
        fd << DoubleToStr(channel->GetLowChannel());
        fd << endStr(sLowChannel) << std::endl;
    }

    // Property: HighChannel (optional)
    if (channel->GetHighChannel() != 255)
    {
        fd << tab.tab() << startStr(sHighChannel);
        fd << DoubleToStr(channel->GetHighChannel());
        fd << endStr(sHighChannel) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, channel->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}

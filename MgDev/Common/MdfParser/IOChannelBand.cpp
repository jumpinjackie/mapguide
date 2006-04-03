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
#include "IOChannelBand.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOChannelBand::IOChannelBand(const std::wstring &strElemName)
: m_pChannel(NULL), m_strElemName(strElemName)
{
}

IOChannelBand::IOChannelBand(ChannelBand *pChannel, const std::wstring &strElemName)
: m_pChannel(pChannel), m_strElemName(strElemName)
{
}

IOChannelBand::~IOChannelBand()
{
}

void IOChannelBand::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == m_strElemName)
    {
        m_startElemName = name;
        if (NULL == this->m_pChannel)
            m_pChannel = new ChannelBand();
    }
}

void IOChannelBand::ElementChars(const wchar_t *ch)
{
    if (NULL == m_pChannel)
    {
        return;
    }

    if (m_currElemName == L"Band") // NOXLATE
        (this->m_pChannel)->SetBand(ch);
    else if (m_currElemName == L"LowBand") // NOXLATE
        (this->m_pChannel)->SetLowBand(wstrToDouble(ch));
    else if (m_currElemName == L"HighBand") // NOXLATE
        (this->m_pChannel)->SetHighBand(wstrToDouble(ch));
    else if (m_currElemName == L"LowChannel") // NOXLATE
        (this->m_pChannel)->SetLowChannel(wstrToInt(ch));
    else if (m_currElemName == L"HighChannel") // NOXLATE
        (this->m_pChannel)->SetHighChannel(wstrToInt(ch));
}

void IOChannelBand::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->m_pChannel     = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOChannelBand::Write(MdfStream &fd, const ChannelBand *pChannel)
{
    fd << tab() << '<' << EncodeString(this->m_strElemName) << '>' << std::endl; // NOXLATE
    inctab();

    //Property: Band
    fd << tab() << "<Band>"; // NOXLATE
    fd << EncodeString(pChannel->GetBand());
    fd << "</Band>" << std::endl; // NOXLATE

    //Property: LowBand
    // TODO: this is optional - it defaults to the low value in the band
    fd << tab() << "<LowBand>"; // NOXLATE
    fd << DoubleToStr(pChannel->GetLowBand());
    fd << "</LowBand>" << std::endl; // NOXLATE

    //Property: HighBand
    // TODO: this is optional - it defaults to the high value in the band
    fd << tab() << "<HighBand>"; // NOXLATE
    fd << DoubleToStr(pChannel->GetHighBand());
    fd << "</HighBand>" << std::endl; // NOXLATE

    //Property: LowChannel (optional)
    if (pChannel->GetLowChannel() != 0)
    {
        fd << tab() << "<LowChannel>"; // NOXLATE
        fd << DoubleToStr(pChannel->GetLowChannel());
        fd << "</LowChannel>" << std::endl; // NOXLATE
    }

    //Property: HighChannel (optional)
    if (pChannel->GetHighChannel() != 255)
    {
        fd << tab() << "<HighChannel>"; // NOXLATE
        fd << DoubleToStr(pChannel->GetHighChannel());
        fd << "</HighChannel>" << std::endl; // NOXLATE
    }

    dectab();
    fd << tab() << "</" << EncodeString(this->m_strElemName) << '>'  << std::endl; // NOXLATE
}

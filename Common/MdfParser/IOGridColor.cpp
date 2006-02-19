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
#include "IOGridColor.h"
#include "IOChannelBand.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

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
    if (m_currElemName == L"Color") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"Bands") // NOXLATE
    {
        IOGridColorBands* pIO = new IOGridColorBands(colorRule);
        handlerStack->push(pIO);
        pIO->StartElement(name, handlerStack);
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

    dectab();
    fd << tab() << "</Color>" << std::endl; // NOXLATE
}

//
// IOGridColorBands
//
IOGridColorBands::IOGridColorBands():color(NULL),redChannel(NULL), greenChannel(NULL), blueChannel(NULL)
{
}

IOGridColorBands::IOGridColorBands(GridColorRule * colorRule):  IOGridColor(colorRule),color(NULL),
                                                                    redChannel(NULL), greenChannel(NULL), blueChannel(NULL)
{
}

IOGridColorBands::~IOGridColorBands()
{

}

void IOGridColorBands::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"Bands") // NOXLATE
    {
        m_startElemName = name;
        this->color = new GridColorBands();
    }
    else if (m_currElemName == L"RedBand") // NOXLATE
    {
        redChannel = new ChannelBand();
        IOChannelBand* pIO = new IOChannelBand(redChannel, m_currElemName);
        handlerStack->push(pIO);
        pIO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"GreenBand") // NOXLATE
    {
        greenChannel = new ChannelBand();
        IOChannelBand* pIO = new IOChannelBand(greenChannel, m_currElemName);
        handlerStack->push(pIO);
        pIO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"BlueBand") // NOXLATE
    {
        blueChannel = new ChannelBand();
        IOChannelBand* pIO = new IOChannelBand(blueChannel, m_currElemName);
        handlerStack->push(pIO);
        pIO->StartElement(name, handlerStack);
    }
}

void IOGridColorBands::ElementChars(const wchar_t *ch)
{

}

void IOGridColorBands::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
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

void IOGridColorBands::Write(MdfStream &fd,  GridColorBands * pColor)
{
    fd << tab() << "<Bands>" << std::endl; // NOXLATE
    inctab();

    std::auto_ptr<IOChannelBand> spIO;

    spIO.reset(new IOChannelBand(L"RedBand")); // NOXLATE
    spIO->Write(fd, &(pColor->GetRedBand()));

    spIO.reset(new IOChannelBand(L"GreenBand")); // NOXLATE
    spIO->Write(fd, &(pColor->GetGreenBand()));

    spIO.reset(new IOChannelBand(L"BlueBand")); // NOXLATE
    spIO->Write(fd, &(pColor->GetBlueBand()));

    dectab();
    fd << tab() << "</Bands>" << std::endl; // NOXLATE
}

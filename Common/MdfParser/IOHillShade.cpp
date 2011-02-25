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
#include "IOHillShade.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, HillShade);
ELEM_MAP_ENTRY(2, Band);
ELEM_MAP_ENTRY(3, Azimuth);
ELEM_MAP_ENTRY(4, Altitude);
ELEM_MAP_ENTRY(5, ScaleFactor);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOHillShade::IOHillShade(Version& version) : SAX2ElementHandler(version)
{
    this->m_colorStyle = NULL;
    this->m_hillShade = NULL;
}


IOHillShade::IOHillShade(GridColorStyle* colorStyle, Version& version) : SAX2ElementHandler(version)
{
    this->m_colorStyle = colorStyle;
    this->m_hillShade = NULL;
}


IOHillShade::~IOHillShade()
{
}


void IOHillShade::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eHillShade:
        this->m_startElemName = name;
        this->m_hillShade = new HillShade();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOHillShade::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eBand:
        this->m_hillShade->SetBand(ch);
        break;

    case eAzimuth:
        this->m_hillShade->SetAzimuth(wstrToDouble(ch));
        break;

    case eAltitude:
        this->m_hillShade->SetAltitude(wstrToDouble(ch));
        break;

    case eScaleFactor:
        this->m_hillShade->SetScaleFactor(wstrToDouble(ch));
        break;
    }
}


void IOHillShade::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_hillShade->SetUnknownXml(this->m_unknownXml);

        this->m_colorStyle->AdoptHillShade(this->m_hillShade);
        this->m_colorStyle = NULL;
        this->m_hillShade = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOHillShade::Write(MdfStream& fd, HillShade* hillShade, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sHillShade) << std::endl;
    tab.inctab();

    // Property: Band
    fd << tab.tab() << startStr(sBand);
    fd << EncodeString(hillShade->GetBand());
    fd << endStr(sBand) << std::endl;

    // Property : Azimuth
    fd << tab.tab() << startStr(sAzimuth);
    fd << hillShade->GetAzimuth();
    fd << endStr(sAzimuth) << std::endl;

    // Property : Altitude
    fd << tab.tab() << startStr(sAltitude);
    fd << hillShade->GetAltitude();
    fd << endStr(sAltitude) << std::endl;

    // Property : ScaleFactor (optional)
    if (hillShade->GetScaleFactor() != 1.0)
    {
        fd << tab.tab() << startStr(sScaleFactor);
        fd << hillShade->GetScaleFactor();
        fd << endStr(sScaleFactor) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, hillShade->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sHillShade) << std::endl;
}

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "IOWatermarkAppearance.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Appearance);
ELEM_MAP_ENTRY(2, AppearanceOverride);
ELEM_MAP_ENTRY(3, Transparency);
ELEM_MAP_ENTRY(4, Rotation);

IOWatermarkAppearance::IOWatermarkAppearance(Version& version)
: SAX2ElementHandler(version), m_appearance(NULL)
{
}


IOWatermarkAppearance::IOWatermarkAppearance(WatermarkAppearance* appearance, Version& version)
: SAX2ElementHandler(version), m_appearance(appearance)
{
}


IOWatermarkAppearance::~IOWatermarkAppearance()
{
    delete this->m_appearance;
}


void IOWatermarkAppearance::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eAppearance:
        this->m_startElemName = name;
        break;
    case eAppearanceOverride:
        this->m_startElemName = name;
        break;
    }
}


void IOWatermarkAppearance::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eTransparency:
        this->m_appearance->SetTransparency(wstrToDouble(ch));
        break;

    case eRotation:
        this->m_appearance->SetRotation(wstrToDouble(ch));
        break;
    }
}


void IOWatermarkAppearance::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_appearance = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOWatermarkAppearance::Write(MdfStream& fd, WatermarkAppearance* appearance, Version* version, const std::string& name)
{
    fd << tab() << startStr(name) << std::endl;
    inctab();
    fd << tab() << startStr(sTransparency);
    fd << DoubleToStr(appearance->GetTransparency());
    fd << endStr(sTransparency) << std::endl;
    fd << tab() << startStr(sRotation);
    fd << DoubleToStr(appearance->GetRotation());
    fd << endStr(sRotation) << std::endl;
    dectab();
    fd << tab() << endStr(name) << std::endl;
}

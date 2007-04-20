//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "IOElevationSettings.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ElevationSettings);
ELEM_MAP_ENTRY(2, ZOffset);
ELEM_MAP_ENTRY(3, ZExtrusion);
ELEM_MAP_ENTRY(4, Unit);
ELEM_MAP_ENTRY(5, ZOffsetType);

IOElevationSettings::IOElevationSettings()
{
    this->_elevationSettings = NULL;
    this->scaleRange = NULL;
}

IOElevationSettings::IOElevationSettings(VectorScaleRange *scaleRange)
{
    this->_elevationSettings = NULL;
    this->scaleRange = scaleRange;
}

IOElevationSettings::~IOElevationSettings()
{
}

void IOElevationSettings::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eElevationSettings:
        m_startElemName = name;
        this->_elevationSettings = new ElevationSettings();
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOElevationSettings::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == swZOffset)
    {
        (this->_elevationSettings)->SetZOffsetExpression(ch);
    }
    else if (m_currElemName == swZExtrusion)
    {
        (this->_elevationSettings)->SetZExtrusionExpression(ch);
    }
    else if (this->m_currElemName == swUnit)
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        (this->_elevationSettings)->SetUnit(unit);
    }
    else if (m_currElemName == swZOffsetType)
    {
        ElevationSettings::ElevationType elevType;
        if(::wcscmp(ch, L"Absolute") == 0) // NOXLATE
        {
            elevType = ElevationSettings::Absolute;
        }
        else
        {
            elevType = ElevationSettings::RelativeToGround;
        }
        (this->_elevationSettings)->SetElevationType(elevType);
    }
}

void IOElevationSettings::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_elevationSettings->SetUnknownXml(UnknownXml());

        this->scaleRange->AdoptElevationSettings(this->_elevationSettings);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_elevationSettings = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOElevationSettings::Write(MdfStream &fd, ElevationSettings *elevationSettings)
{
    fd << tab() << "<ElevationSettings>" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<ZOffset>"; // NOXLATE
    fd << EncodeString(elevationSettings->GetZOffsetExpression());
    fd << "</ZOffset>" << std::endl; // NOXLATE

    fd << tab() << "<ZExtrusion>"; // NOXLATE
    fd << EncodeString(elevationSettings->GetZExtrusionExpression());
    fd << "</ZExtrusion>" << std::endl; // NOXLATE

    fd << tab() << "<ZOffsetType>"; // NOXLATE
    ElevationSettings::ElevationType elevType = elevationSettings->GetElevationType();
    switch(elevType)
    {
    case ElevationSettings::Absolute:
        {
            fd << "Absolute"; // NOXLATE
            break;
        }
    case ElevationSettings::RelativeToGround:
    default:
        {
            fd << "RelativeToGround"; // NOXLATE
            break;
        }
    }
    fd << "</ZOffsetType>" << std::endl; // NOXLATE

    fd << tab() << "<Unit>"; // NOXLATE
    std::auto_ptr<MdfString> str(LengthConverter::UnitToEnglish(elevationSettings->GetUnit()));
    fd << EncodeString(*str);
    fd << "</Unit>" << std::endl; // NOXLATE

    // Write any previously found unknown XML
    if (!elevationSettings->GetUnknownXml().empty())
    {
        fd << toCString(elevationSettings->GetUnknownXml());
    }

    dectab();
    fd << tab() << "</ElevationSettings>" << std::endl; // NOXLATE
}

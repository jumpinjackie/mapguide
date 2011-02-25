//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ElevationSettings);
ELEM_MAP_ENTRY(2, ZOffset);
ELEM_MAP_ENTRY(3, ZExtrusion);
ELEM_MAP_ENTRY(4, ZOffsetType);
ELEM_MAP_ENTRY(5, Unit);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOElevationSettings::IOElevationSettings(Version& version) : SAX2ElementHandler(version)
{
    this->m_elevationSettings = NULL;
    this->m_scaleRange = NULL;
}


IOElevationSettings::IOElevationSettings(VectorScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
{
    this->m_elevationSettings = NULL;
    this->m_scaleRange = scaleRange;
}


IOElevationSettings::~IOElevationSettings()
{
}


void IOElevationSettings::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eElevationSettings:
        this->m_startElemName = name;
        this->m_elevationSettings = new ElevationSettings();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOElevationSettings::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eZOffset:
        this->m_elevationSettings->SetZOffsetExpression(ch);
        break;

    case eZExtrusion:
        this->m_elevationSettings->SetZExtrusionExpression(ch);
        break;

    case eZOffsetType:
    {
        if (::wcscmp(ch, L"Absolute") == 0) // NOXLATE
            this->m_elevationSettings->SetElevationType(ElevationSettings::Absolute);
        else if (::wcscmp(ch, L"RelativeToGround") == 0) // NOXLATE
            this->m_elevationSettings->SetElevationType(ElevationSettings::RelativeToGround);
    }

    case eUnit:
        this->m_elevationSettings->SetUnit(LengthConverter::EnglishToUnit(ch));
        break;
    }
}


void IOElevationSettings::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_elevationSettings->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->AdoptElevationSettings(this->m_elevationSettings);
        this->m_scaleRange = NULL;
        this->m_elevationSettings = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOElevationSettings::Write(MdfStream& fd, ElevationSettings* elevationSettings, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sElevationSettings) << std::endl;
    tab.inctab();

    // Property: ZOffset
    fd << tab.tab() << startStr(sZOffset);
    fd << EncodeString(elevationSettings->GetZOffsetExpression());
    fd << endStr(sZOffset) << std::endl;

    // Property: ZExtrusion
    fd << tab.tab() << startStr(sZExtrusion);
    fd << EncodeString(elevationSettings->GetZExtrusionExpression());
    fd << endStr(sZExtrusion) << std::endl;

    // Property: ZOffsetType
    fd << tab.tab() << startStr(sZOffsetType);
    if (elevationSettings->GetElevationType() == ElevationSettings::Absolute)
        fd << "Absolute"; // NOXLATE
    else
        fd << "RelativeToGround"; // NOXLATE
    fd << endStr(sZOffsetType) << std::endl;

    // Property: Unit
    fd << tab.tab() << startStr(sUnit);
    std::auto_ptr<MdfString> str(LengthConverter::UnitToEnglish(elevationSettings->GetUnit()));
    fd << EncodeString(*str);
    fd << endStr(sUnit) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, elevationSettings->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sElevationSettings) << std::endl;
}

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
#include "IOSupplementalSpatialContextInfo.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, SupplementalSpatialContextInfo);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, CoordinateSystem);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOSupplementalSpatialContextInfo::IOSupplementalSpatialContextInfo(Version& version) : SAX2ElementHandler(version)
{
    this->m_ssContextInfo = NULL;
    this->m_featureSource = NULL;
}


IOSupplementalSpatialContextInfo::IOSupplementalSpatialContextInfo(FeatureSource* featureSource, Version& version) : SAX2ElementHandler(version)
{
    this->m_ssContextInfo = NULL;
    this->m_featureSource = featureSource;
}


IOSupplementalSpatialContextInfo::~IOSupplementalSpatialContextInfo()
{
}


void IOSupplementalSpatialContextInfo::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eSupplementalSpatialContextInfo:
        if (this->m_featureSource)
        {
            this->m_startElemName = name;
            this->m_ssContextInfo = new SupplementalSpatialContextInfo();
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


void IOSupplementalSpatialContextInfo::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_ssContextInfo->SetName(ch);
        break;

    case eCoordinateSystem:
        this->m_ssContextInfo->SetCoordinateSystem(ch);
        break;
    }
}


void IOSupplementalSpatialContextInfo::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_ssContextInfo->SetUnknownXml(this->m_unknownXml);

        if (this->m_featureSource)
            this->m_featureSource->GetSupplementalSpatialContextInfo()->Adopt(this->m_ssContextInfo);

        this->m_ssContextInfo = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOSupplementalSpatialContextInfo::Write(MdfStream& fd, SupplementalSpatialContextInfo* ssContextInfo, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sSupplementalSpatialContextInfo) << std::endl;
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(ssContextInfo->GetName());
    fd << endStr(sName) << std::endl;

    // Property: CoordinateSystem
    fd << tab.tab() << startStr(sCoordinateSystem);
    fd << EncodeString(ssContextInfo->GetCoordinateSystem());
    fd << endStr(sCoordinateSystem) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, ssContextInfo->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sSupplementalSpatialContextInfo) << std::endl;
}

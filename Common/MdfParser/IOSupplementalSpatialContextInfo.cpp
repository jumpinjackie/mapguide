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
#include "IOSupplementalSpatialContextInfo.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, SupplementalSpatialContextInfo);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, CoordinateSystem);


IOSupplementalSpatialContextInfo::IOSupplementalSpatialContextInfo()
    : _ssContextInfo(NULL), featureSource(NULL)
{
}


IOSupplementalSpatialContextInfo::IOSupplementalSpatialContextInfo(FeatureSource* pFeatureSource)
    : _ssContextInfo(NULL), featureSource(pFeatureSource)
{
}


IOSupplementalSpatialContextInfo::~IOSupplementalSpatialContextInfo()
{
}


void IOSupplementalSpatialContextInfo::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eSupplementalSpatialContextInfo:
        if (NULL != featureSource)
        {
            m_startElemName = name;
            this->_ssContextInfo = new SupplementalSpatialContextInfo(L"", L"");
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOSupplementalSpatialContextInfo::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        this->_ssContextInfo->SetName(ch);
    else if (m_currElemName == L"CoordinateSystem") // NOXLATE
        this->_ssContextInfo->SetCoordinateSystem(ch);
}


void IOSupplementalSpatialContextInfo::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_ssContextInfo->SetUnknownXml(UnknownXml());

        if (NULL != this->featureSource)
            this->featureSource->GetSupplementalSpatialContextInfo()->Adopt(this->_ssContextInfo);

        handlerStack->pop();
        this->_ssContextInfo = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOSupplementalSpatialContextInfo::Write(MdfStream& fd, SupplementalSpatialContextInfo* ssContextInfo, Version* version)
{
    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(ssContextInfo->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    //Property: CoordinateSystem
    fd << tab() << "<CoordinateSystem>"; // NOXLATE
    fd << EncodeString(ssContextInfo->GetCoordinateSystem());
    fd << "</CoordinateSystem>" << std::endl; // NOXLATE

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, ssContextInfo->GetUnknownXml(), version);
}

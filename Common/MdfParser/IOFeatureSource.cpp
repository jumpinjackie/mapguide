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
#include "IOFeatureSource.h"
#include "IONameStringPair.h"
#include "IOExtension.h"
#include "IOSupplementalSpatialContextInfo.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, FeatureSource);
ELEM_MAP_ENTRY(2, Provider);
ELEM_MAP_ENTRY(3, Parameter);
ELEM_MAP_ENTRY(4, SupplementalSpatialContextInfo);
ELEM_MAP_ENTRY(5, ConfigurationDocument);
ELEM_MAP_ENTRY(6, LongTransaction);
ELEM_MAP_ENTRY(7, Extension);
ELEM_MAP_ENTRY(8, ExtendedData1);


IOFeatureSource::IOFeatureSource(Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = NULL;
}


IOFeatureSource::IOFeatureSource(FeatureSource* featureSource, Version& version) : SAX2ElementHandler(version)
{
    this->m_featureSource = featureSource;
}


IOFeatureSource::~IOFeatureSource()
{
}


void IOFeatureSource::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eFeatureSource:
        this->m_startElemName = name;
        break;

    case eParameter:
        {
            IONameStringPair* IO = new IONameStringPair(this->m_featureSource, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eSupplementalSpatialContextInfo:
        {
            IOSupplementalSpatialContextInfo* IO = new IOSupplementalSpatialContextInfo(this->m_featureSource, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtension:
        {
            IOExtension* IO = new IOExtension(this->m_featureSource, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
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


void IOFeatureSource::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eProvider:
        this->m_featureSource->SetProvider(ch);
        break;

    case eConfigurationDocument:
        this->m_featureSource->SetConfigurationDocument(ch);
        break;

    case eLongTransaction:
        this->m_featureSource->SetLongTransaction(ch);
        break;
    }
}


void IOFeatureSource::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_featureSource->SetUnknownXml(this->m_unknownXml);

        this->m_featureSource = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOFeatureSource::Write(MdfStream& fd, FeatureSource* featureSource, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    tab.inctab();

    // Property: Provider
    fd << tab.tab() << startStr(sProvider);
    fd << EncodeString(featureSource->GetProvider());
    fd << endStr(sProvider) << std::endl;

    // Property: Parameters
    for (int i=0; i<featureSource->GetParameters()->GetCount(); ++i)
        IONameStringPair::Write(fd, sParameter, featureSource->GetParameters()->GetAt(i), version, tab);

    // Property: SupplementalSpatialContextInfo
    for (int i=0; i<featureSource->GetSupplementalSpatialContextInfo()->GetCount(); ++i)
        IOSupplementalSpatialContextInfo::Write(fd, featureSource->GetSupplementalSpatialContextInfo()->GetAt(i), version, tab);

    // Property: ConfigurationDocument
    fd << tab.tab() << startStr(sConfigurationDocument);
    fd << EncodeString(featureSource->GetConfigurationDocument());
    fd << endStr(sConfigurationDocument) << std::endl;

    // Property: LongTransaction
    fd << tab.tab() << startStr(sLongTransaction);
    fd << EncodeString(featureSource->GetLongTransaction());
    fd << endStr(sLongTransaction) << std::endl;

    // Property: Extension
    for (int i=0; i<featureSource->GetExtensions()->GetCount(); ++i)
        IOExtension::Write(fd, featureSource->GetExtensions()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, featureSource->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</FeatureSource>" << std::endl; // NOXLATE
}

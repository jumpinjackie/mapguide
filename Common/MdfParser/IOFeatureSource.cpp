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
ELEM_MAP_ENTRY(2, Parameter);
ELEM_MAP_ENTRY(3, Extension);
ELEM_MAP_ENTRY(4, SupplementalSpatialContextInfo);
ELEM_MAP_ENTRY(5, Provider);
ELEM_MAP_ENTRY(6, ConfigurationDocument);
ELEM_MAP_ENTRY(7, LongTransaction);


IOFeatureSource::IOFeatureSource()
{
    this->m_featureSource = NULL;
}


IOFeatureSource::IOFeatureSource(FeatureSource* featureSource)
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
                IONameStringPair* IO = new IONameStringPair(this->m_featureSource);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eExtension:
            {
                IOExtension* IO = new IOExtension(this->m_featureSource);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eSupplementalSpatialContextInfo:
            {
                IOSupplementalSpatialContextInfo* IO = new IOSupplementalSpatialContextInfo(this->m_featureSource);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eUnknown:
            ParseUnknownXml(name, handlerStack);
            break;

        default:
            break;
    }
}


void IOFeatureSource::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Provider") // NOXLATE
        this->m_featureSource->SetProvider(ch);
    else if (this->m_currElemName == L"ConfigurationDocument") // NOXLATE
        this->m_featureSource->SetConfigurationDocument(ch);
    else if (this->m_currElemName == L"LongTransaction") // NOXLATE
        this->m_featureSource->SetLongTransaction(ch);
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
}


void IOFeatureSource::Write(MdfStream& fd, FeatureSource* featureSource, Version* version)
{
    fd << tab() << "<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"FeatureSource-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    inctab();

    // Property: Provider
    fd << tab() << "<Provider>"; // NOXLATE
    fd << EncodeString(featureSource->GetProvider());
    fd << "</Provider>" << std::endl; // NOXLATE

    // Property: Parameters
    for (int i=0; i<featureSource->GetParameters()->GetCount(); ++i)
    {
        fd << tab() << "<Parameter>" << std::endl; // NOXLATE
        inctab();
        IONameStringPair::Write(fd, featureSource->GetParameters()->GetAt(i), version);
        dectab();
        fd << tab() << "</Parameter>" << std::endl; // NOXLATE
    }

    // Property: SupplementalSpatialContextInfo
    for (int i=0; i<featureSource->GetSupplementalSpatialContextInfo()->GetCount(); ++i)
    {
        fd << tab() << "<SupplementalSpatialContextInfo>" << std::endl; // NOXLATE
        inctab();
        IOSupplementalSpatialContextInfo::Write(fd, featureSource->GetSupplementalSpatialContextInfo()->GetAt(i), version);
        dectab();
        fd << tab() << "</SupplementalSpatialContextInfo>" << std::endl; // NOXLATE
    }

    // Property: ConfigurationDocument
    fd << tab() << "<ConfigurationDocument>"; // NOXLATE
    fd << EncodeString(featureSource->GetConfigurationDocument());
    fd << "</ConfigurationDocument>" << std::endl; // NOXLATE

    // Property: LongTransaction
    fd << tab() << "<LongTransaction>"; // NOXLATE
    fd << EncodeString(featureSource->GetLongTransaction());
    fd << "</LongTransaction>" << std::endl; // NOXLATE

    // Property: Extension
    for (int i=0; i<featureSource->GetExtensions()->GetCount(); ++i)
        IOExtension::Write(fd, featureSource->GetExtensions()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, featureSource->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</FeatureSource>" << std::endl; // NOXLATE
}

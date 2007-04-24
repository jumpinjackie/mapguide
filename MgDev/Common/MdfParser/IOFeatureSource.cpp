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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

#include "IONameStringPair.h"
#include "IOExtension.h"
#include "IOSupplementalSpatialContextInfo.h"

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, FeatureSource);
ELEM_MAP_ENTRY(2, Parameter);
ELEM_MAP_ENTRY(3, Extension);
ELEM_MAP_ENTRY(4, SupplementalSpatialContextInfo);
ELEM_MAP_ENTRY(5, Provider);
ELEM_MAP_ENTRY(6, ConfigurationDocument);
ELEM_MAP_ENTRY(7, LongTransaction);


IOFeatureSource::IOFeatureSource()
    : m_pFeatureSource(NULL)
{
}

IOFeatureSource::IOFeatureSource(FeatureSource *pFeatureSource)
    : m_pFeatureSource(pFeatureSource)
{
}

IOFeatureSource::~IOFeatureSource()
{
}

void IOFeatureSource::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId) {
        case eFeatureSource:
            m_startElemName = name;
            break;

        case eParameter:
            {
                IONameStringPair *IO = new IONameStringPair(this->m_pFeatureSource);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eExtension:
            {
                IOExtension *IO = new IOExtension(this->m_pFeatureSource);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eSupplementalSpatialContextInfo:
            {
                IOSupplementalSpatialContextInfo *IO =
                    new IOSupplementalSpatialContextInfo(this->m_pFeatureSource);
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

void IOFeatureSource::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Provider") // NOXLATE
        (this->m_pFeatureSource)->SetProvider(ch);
    else if (m_currElemName == L"ConfigurationDocument") // NOXLATE
        (this->m_pFeatureSource)->SetConfigurationDocument(ch);
    else if (m_currElemName == L"LongTransaction") // NOXLATE
        (this->m_pFeatureSource)->SetLongTransaction(ch);
}

void IOFeatureSource::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->m_pFeatureSource->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->m_pFeatureSource = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOFeatureSource::Write(MdfStream &fd,  FeatureSource *pFeatureSource)
{
    fd << tab() << "<FeatureSource xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"MapFeatureSource-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    inctab();

    // Property: Provider
    fd << tab() << "<Provider>"; // NOXLATE
    fd << EncodeString(pFeatureSource->GetProvider());
    fd << "</Provider>" << std::endl; // NOXLATE

    // Property: Parameters
    for(int x = 0; x < pFeatureSource->GetParameters()->GetCount(); x++)
    {
        fd << tab() << "<Parameter>" << std::endl; // NOXLATE
        inctab();
        std::auto_ptr<IONameStringPair> spIO(new IONameStringPair());
        spIO->Write(fd, pFeatureSource->GetParameters()->GetAt(x));
        dectab();
        fd << tab() << "</Parameter>" << std::endl; // NOXLATE
    }

    // Property: SupplementalSpatialContextInfo
    for(int x = 0; x < pFeatureSource->GetSupplementalSpatialContextInfo()->GetCount(); x++)
    {
        fd << tab() << "<SupplementalSpatialContextInfo>" << std::endl; // NOXLATE
        inctab();
        std::auto_ptr<IOSupplementalSpatialContextInfo> spIO(new IOSupplementalSpatialContextInfo());
        spIO->Write(fd, pFeatureSource->GetSupplementalSpatialContextInfo()->GetAt(x));
        dectab();
        fd << tab() << "</SupplementalSpatialContextInfo>" << std::endl; // NOXLATE
    }

    // Property: ConfigurationDocument
    fd << tab() << "<ConfigurationDocument>"; // NOXLATE
    fd << EncodeString(pFeatureSource->GetConfigurationDocument());
    fd << "</ConfigurationDocument>" << std::endl; // NOXLATE

    // Property: LongTransaction
    fd << tab() << "<LongTransaction>"; // NOXLATE
    fd << EncodeString(pFeatureSource->GetLongTransaction());
    fd << "</LongTransaction>" << std::endl; // NOXLATE

    // Property: Extension
    for(int x = 0; x < pFeatureSource->GetExtensions()->GetCount(); x++)
    {
        std::auto_ptr<IOExtension> spIO(new IOExtension());
        spIO->Write(fd, pFeatureSource->GetExtensions()->GetAt(x));
    }

    // Write any previously found unknown XML
    if (!pFeatureSource->GetUnknownXml().empty())
        fd << tab() << toCString(pFeatureSource->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</FeatureSource>" << std::endl; // NOXLATE
}

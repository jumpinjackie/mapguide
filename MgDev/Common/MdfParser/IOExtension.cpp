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
#include "IOExtension.h"
#include "IOCalculatedProperty.h"
#include "IOAttributeRelate.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Extension);
ELEM_MAP_ENTRY(2, CalculatedProperty);
ELEM_MAP_ENTRY(3, AttributeRelate);
ELEM_MAP_ENTRY(4, Name);
ELEM_MAP_ENTRY(5, FeatureClass);


IOExtension::IOExtension()
    : m_pExtension(NULL), m_pFeatureSource(NULL)
{
}

IOExtension::IOExtension(FeatureSource* pFeatureSource)
    : m_pExtension(NULL), m_pFeatureSource(pFeatureSource)
{
}


IOExtension::~IOExtension()
{
}


void IOExtension::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
        case eExtension:
            m_startElemName = name;
            m_pExtension = new Extension();
            break;

        case eCalculatedProperty:
            {
                IOCalculatedProperty* IO = new IOCalculatedProperty(this->m_pExtension);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eAttributeRelate:
            {
                IOAttributeRelate* IO = new IOAttributeRelate(this->m_pExtension);
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


void IOExtension::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        this->m_pExtension->SetName(ch);
    else if (m_currElemName == L"FeatureClass") // NOXLATE
        this->m_pExtension->SetFeatureClass(ch);
}


void IOExtension::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_pExtension->SetUnknownXml(UnknownXml());

        m_pFeatureSource->GetExtensions()->Adopt(m_pExtension);
        handlerStack->pop();
        this->m_pExtension = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOExtension::Write(MdfStream& fd, Extension* pExtension, Version* version)
{
    fd << tab() << "<Extension>" << std::endl; // NOXLATE
    inctab();

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(pExtension->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: FeatureClass
    fd << tab() << "<FeatureClass>"; // NOXLATE
    fd << EncodeString(pExtension->GetFeatureClass());
    fd << "</FeatureClass>" << std::endl; // NOXLATE

    // Property: CalculatedProperties
    for (int i=0; i<pExtension->GetCalculatedProperties()->GetCount(); ++i)
        IOCalculatedProperty::Write(fd, pExtension->GetCalculatedProperties()->GetAt(i), version);

    // Property: AttributeRelate
    for (int i=0; i<pExtension->GetAttributeRelates()->GetCount(); ++i)
        IOAttributeRelate::Write(fd, pExtension->GetAttributeRelates()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pExtension->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Extension>" << std::endl; // NOXLATE
}

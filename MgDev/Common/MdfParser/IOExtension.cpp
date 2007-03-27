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

IOExtension::IOExtension(FeatureSource *pFeatureSource)
    : m_pExtension(NULL), m_pFeatureSource(pFeatureSource)
{
}

IOExtension::~IOExtension()
{
}

void IOExtension::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId) {
        case eExtension:
            m_startElemName = name;
            m_pExtension = new Extension();
            break;

        case eCalculatedProperty:
            {
                IOCalculatedProperty *IO = new IOCalculatedProperty(this->m_pExtension);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eAttributeRelate:
            {
                IOAttributeRelate *IO = new IOAttributeRelate(this->m_pExtension);
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

void IOExtension::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        (this->m_pExtension)->SetName(ch);
    else if (m_currElemName == L"FeatureClass") // NOXLATE
        (this->m_pExtension)->SetFeatureClass(ch);
}

void IOExtension::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            m_pExtension->SetUnknownXml(UnknownXml());

        m_pFeatureSource->GetExtensions()->Adopt(m_pExtension);
        handlerStack->pop();
        this->m_pExtension = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOExtension::Write(MdfStream &fd,  Extension *pExtension)
{
    fd << tab() << "<Extension>" << std::endl; // NOXLATE
    inctab();

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(pExtension->GetName());
    fd << "</Name>" << std::endl; // NOXLATE
    
    // Property: FeatureClass
    fd << tab() << "<FeatureClass>";  // NOXLATE
    fd << EncodeString(pExtension->GetFeatureClass());
    fd << "</FeatureClass>" << std::endl; // NOXLATE

    // Property: CalculatedProperties
    for(int x = 0; x < pExtension->GetCalculatedProperties()->GetCount(); x++)
    {
        std::auto_ptr<IOCalculatedProperty> spIO(new IOCalculatedProperty());
        spIO->Write(fd, pExtension->GetCalculatedProperties()->GetAt(x));
    }

    // Property: AttributeRelate
    for(int x = 0; x < pExtension->GetAttributeRelates()->GetCount(); x++)
    {
        std::auto_ptr<IOAttributeRelate> spIO(new IOAttributeRelate());
        spIO->Write(fd, pExtension->GetAttributeRelates()->GetAt(x));
    }

        // Write any previously found unknown XML
    if (!pExtension->GetUnknownXml().empty())
    {
        fd << toCString(pExtension->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</Extension>" << std::endl; // NOXLATE
}



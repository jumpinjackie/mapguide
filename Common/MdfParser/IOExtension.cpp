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
{
    this->m_extension = NULL;
    this->m_featureSource = NULL;
}

IOExtension::IOExtension(FeatureSource* featureSource)
{
    this->m_extension = NULL;
    this->m_featureSource = featureSource;
}


IOExtension::~IOExtension()
{
}


void IOExtension::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
        case eExtension:
            this->m_startElemName = name;
            this->m_extension = new Extension();
            break;

        case eCalculatedProperty:
            {
                IOCalculatedProperty* IO = new IOCalculatedProperty(this->m_extension);
                handlerStack->push(IO);
                IO->StartElement(name, handlerStack);
            }
            break;

        case eAttributeRelate:
            {
                IOAttributeRelate* IO = new IOAttributeRelate(this->m_extension);
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
    if (this->m_currElemName == L"Name") // NOXLATE
        this->m_extension->SetName(ch);
    else if (this->m_currElemName == L"FeatureClass") // NOXLATE
        this->m_extension->SetFeatureClass(ch);
}


void IOExtension::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_extension->SetUnknownXml(this->m_unknownXml);

        this->m_featureSource->GetExtensions()->Adopt(this->m_extension);
        this->m_extension = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOExtension::Write(MdfStream& fd, Extension* extension, Version* version)
{
    fd << tab() << "<Extension>" << std::endl; // NOXLATE
    inctab();

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(extension->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: FeatureClass
    fd << tab() << "<FeatureClass>"; // NOXLATE
    fd << EncodeString(extension->GetFeatureClass());
    fd << "</FeatureClass>" << std::endl; // NOXLATE

    // Property: CalculatedProperties
    for (int i=0; i<extension->GetCalculatedProperties()->GetCount(); ++i)
        IOCalculatedProperty::Write(fd, extension->GetCalculatedProperties()->GetAt(i), version);

    // Property: AttributeRelate
    for (int i=0; i<extension->GetAttributeRelates()->GetCount(); ++i)
        IOAttributeRelate::Write(fd, extension->GetAttributeRelates()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, extension->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Extension>" << std::endl; // NOXLATE
}

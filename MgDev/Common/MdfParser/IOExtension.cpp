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
ELEM_MAP_ENTRY(6, ExtendedData1);


IOExtension::IOExtension(Version& version) : SAX2ElementHandler(version)
{
    this->m_extension = NULL;
    this->m_featureSource = NULL;
}

IOExtension::IOExtension(FeatureSource* featureSource, Version& version) : SAX2ElementHandler(version)
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
            IOCalculatedProperty* IO = new IOCalculatedProperty(this->m_extension, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eAttributeRelate:
        {
            IOAttributeRelate* IO = new IOAttributeRelate(this->m_extension, this->m_version);
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


void IOExtension::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eName:
        this->m_extension->SetName(ch);
        break;

    case eFeatureClass:
        this->m_extension->SetFeatureClass(ch);
        break;
    }
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOExtension::Write(MdfStream& fd, Extension* extension, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sExtension) << std::endl;
    tab.inctab();

    // Property: CalculatedProperties
    for (int i=0; i<extension->GetCalculatedProperties()->GetCount(); ++i)
        IOCalculatedProperty::Write(fd, extension->GetCalculatedProperties()->GetAt(i), version, tab);

    // Property: AttributeRelate
    for (int i=0; i<extension->GetAttributeRelates()->GetCount(); ++i)
        IOAttributeRelate::Write(fd, extension->GetAttributeRelates()->GetAt(i), version, tab);

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(extension->GetName());
    fd << endStr(sName) << std::endl;

    // Property: FeatureClass
    fd << tab.tab() << startStr(sFeatureClass);
    fd << EncodeString(extension->GetFeatureClass());
    fd << endStr(sFeatureClass) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, extension->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sExtension) << std::endl;
}

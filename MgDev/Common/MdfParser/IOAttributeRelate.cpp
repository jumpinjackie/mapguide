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
#include "IOAttributeRelate.h"
#include "IORelateProperty.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, AttributeRelate);
ELEM_MAP_ENTRY(2, RelateProperty);
ELEM_MAP_ENTRY(3, AttributeClass);
ELEM_MAP_ENTRY(4, ResourceId);
ELEM_MAP_ENTRY(5, Name);
ELEM_MAP_ENTRY(6, AttributeNameDelimiter);
ELEM_MAP_ENTRY(7, RelateType);
ELEM_MAP_ENTRY(8, ForceOneToOne);


IOAttributeRelate::IOAttributeRelate()
{
    this->m_attributeRelate = NULL;
    this->m_extension = NULL;
}


IOAttributeRelate::IOAttributeRelate(Extension* extension)
{
    this->m_attributeRelate = NULL;
    this->m_extension = extension;
}


IOAttributeRelate::~IOAttributeRelate()
{
}


void IOAttributeRelate::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eAttributeRelate:
        this->m_startElemName = name;
        this->m_attributeRelate = new AttributeRelate();
        break;

    case eRelateProperty:
        {
            IORelateProperty* IO = new IORelateProperty(this->m_attributeRelate);
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


void IOAttributeRelate::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"AttributeClass") // NOXLATE
        this->m_attributeRelate->SetAttributeClass(ch);
    else if (this->m_currElemName == L"ResourceId") // NOXLATE
        this->m_attributeRelate->SetResourceId(ch);
    else if (this->m_currElemName == L"Name") // NOXLATE
        this->m_attributeRelate->SetName(ch);
    else if (this->m_currElemName == L"AttributeNameDelimiter") // NOXLATE
        this->m_attributeRelate->SetAttributeNameDelimiter(ch);
    else if (this->m_currElemName == L"RelateType") // NOXLATE
        this->m_attributeRelate->SetRelateType(ReadType(ch));
    else if (this->m_currElemName == L"ForceOneToOne") // NOXLATE
        this->m_attributeRelate->SetForceOneToOne(wstrToBool(ch));
}


void IOAttributeRelate::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_attributeRelate->SetUnknownXml(this->m_unknownXml);

        this->m_extension->GetAttributeRelates()->Adopt(this->m_attributeRelate);
        this->m_attributeRelate = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOAttributeRelate::WriteType(MdfStream& fd, AttributeRelate* attributeRelate)
{
    switch (attributeRelate->GetRelateType())
    {
        case AttributeRelate::LeftOuter:
        fd << "LeftOuter"; // NOXLATE
        break;

        case AttributeRelate::RightOuter:
        fd << "RightOuter"; // NOXLATE
        break;

        case AttributeRelate::Inner:
        fd << "Inner"; // NOXLATE
        break;

        case AttributeRelate::Association:
        fd << "Association"; // NOXLATE
        break;

        default:;
    }
}


AttributeRelate::RelateType IOAttributeRelate::ReadType(const wchar_t* strType)
{
    AttributeRelate::RelateType type = AttributeRelate::LeftOuter;

    if (::wcscmp(strType, L"LeftOuter") == 0) // NOXLATE
        type = AttributeRelate::LeftOuter;
    else if (::wcscmp(strType, L"RightOuter") == 0) // NOXLATE
        type = AttributeRelate::RightOuter;
    else if (::wcscmp(strType, L"Inner") == 0) // NOXLATE
        type = AttributeRelate::Inner;
    else if (::wcscmp(strType, L"Association") == 0) // NOXLATE
        type = AttributeRelate::Association;

    return type;
}


void IOAttributeRelate::Write(MdfStream& fd, AttributeRelate* attributeRelate, Version* version)
{
    fd << tab() << "<AttributeRelate>" << std::endl; // NOXLATE
    inctab();

    // Property: AttributeClass
    fd << tab() << "<AttributeClass>"; // NOXLATE
    fd << EncodeString(attributeRelate->GetAttributeClass());
    fd << "</AttributeClass>" << std::endl; // NOXLATE

    // Property: ResourceId
    fd << tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(attributeRelate->GetResourceId());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(attributeRelate->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: AttributeNameDelimiter
    fd << tab() << "<AttributeNameDelimiter>"; // NOXLATE
    fd << EncodeString(attributeRelate->GetAttributeNameDelimiter());
    fd << "</AttributeNameDelimiter>" << std::endl; // NOXLATE

    // Property: RelateType
    fd << tab() << "<RelateType>"; // NOXLATE
    IOAttributeRelate::WriteType(fd, attributeRelate);
    fd << "</RelateType>" << std::endl; // NOXLATE

    // Property: ForceOneToOne
    fd << tab() << "<ForceOneToOne>"; // NOXLATE
    fd << (attributeRelate->GetForceOneToOne()? "true" : "false"); // NOXLATE
    fd << "</ForceOneToOne>" << std::endl; // NOXLATE

    // Property: RelateProperties
    for (int i=0; i<attributeRelate->GetRelateProperties()->GetCount(); ++i)
        IORelateProperty::Write(fd, attributeRelate->GetRelateProperties()->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, attributeRelate->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</AttributeRelate>" << std::endl; // NOXLATE
}

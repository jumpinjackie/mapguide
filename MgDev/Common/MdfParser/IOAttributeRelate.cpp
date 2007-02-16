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
    : m_pAttributeRelate(NULL), m_pExtension(NULL)
{
}

IOAttributeRelate::IOAttributeRelate(Extension *pExtension)
    : m_pAttributeRelate(NULL), m_pExtension(pExtension)
{
}

IOAttributeRelate::~IOAttributeRelate()
{
}

void IOAttributeRelate::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eAttributeRelate:
        m_startElemName = name;
        m_pAttributeRelate = new AttributeRelate();
        break;

    case eRelateProperty:
        {
            IORelateProperty *IO = new IORelateProperty(this->m_pAttributeRelate);
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

void IOAttributeRelate::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"AttributeClass") // NOXLATE
        (this->m_pAttributeRelate)->SetAttributeClass(ch);
    else if (m_currElemName == L"ResourceId") // NOXLATE
        (this->m_pAttributeRelate)->SetResourceId(ch);
    else if (m_currElemName == L"Name") // NOXLATE
        (this->m_pAttributeRelate)->SetName(ch);
    else if (m_currElemName == L"AttributeNameDelimiter") // NOXLATE
        (this->m_pAttributeRelate)->SetAttributeNameDelimiter(ch);
	else if (m_currElemName == L"RelateType") // NOXLATE
        (this->m_pAttributeRelate)->SetRelateType(ReadType(ch));
	else if (m_currElemName == L"ForceOneToOne") // NOXLATE
        (this->m_pAttributeRelate)->SetForceOneToOne(wstrToBool(ch));
}

void IOAttributeRelate::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->m_pAttributeRelate->SetUnknownXml(UnknownXml());

        m_pExtension->GetAttributeRelates()->Adopt(m_pAttributeRelate);
        handlerStack->pop();
        this->m_pAttributeRelate = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOAttributeRelate::Write(MdfStream &fd,  AttributeRelate *pAttributeRelate)
{
    fd << tab() << "<AttributeRelate>" << std::endl; // NOXLATE
    inctab();

    // Property: AttributeClass
    fd << tab() << "<AttributeClass>"; // NOXLATE
    fd << EncodeString(pAttributeRelate->GetAttributeClass());
    fd << "</AttributeClass>" << std::endl; // NOXLATE
    
    // Property: ResourceId
    fd << tab() << "<ResourceId>";  // NOXLATE
    fd << EncodeString(pAttributeRelate->GetResourceId());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    // Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(pAttributeRelate->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: AttributeNameDelimiter
    fd << tab() << "<AttributeNameDelimiter>"; // NOXLATE
    fd << EncodeString(pAttributeRelate->GetAttributeNameDelimiter());
    fd << "</AttributeNameDelimiter>" << std::endl; // NOXLATE

    // Property: RelateType
    fd << tab() << "<RelateType>"; // NOXLATE
	this->WriteType(fd, pAttributeRelate);
    fd << "</RelateType>" << std::endl; // NOXLATE

    // Property: ForceOneToOne
    fd << tab() << "<ForceOneToOne>"; // NOXLATE
	fd << (BoolToStr(pAttributeRelate->GetForceOneToOne())) ? ("true") : ("false"); // NOXLATE
    fd << "</ForceOneToOne>" << std::endl; // NOXLATE

    // Property: RelateProperties
    for(int x = 0; x < pAttributeRelate->GetRelateProperties()->GetCount(); x++)
    {
        std::auto_ptr<IORelateProperty> spIO(new IORelateProperty());
        spIO->Write(fd, pAttributeRelate->GetRelateProperties()->GetAt(x));
    }

    // Write any previously found unknown XML
    if (!pAttributeRelate->GetUnknownXml().empty())
    {
        fd << toCString(pAttributeRelate->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</AttributeRelate>" << std::endl; // NOXLATE
}

void IOAttributeRelate::WriteType(MdfStream &fd, AttributeRelate *pAttributeRelate)
{
	switch (pAttributeRelate->GetRelateType())
	{
		case AttributeRelate::LeftOuter:
		fd << "LeftOuter";	//NOXLATE
		break;

		case AttributeRelate::RightOuter:
		fd << "RightOuter";	//NOXLATE
		break;

		case AttributeRelate::Inner:
		fd << "Inner";		//NOXLATE
		break;

		case AttributeRelate::Association:
		fd << "Association";	//NOXLATE
		break;

		default:;
	}
}

AttributeRelate::RelateType IOAttributeRelate::ReadType(const wchar_t *strType)
{
	AttributeRelate::RelateType type = AttributeRelate::LeftOuter;

    if (::wcscmp(strType, L"LeftOuter") == 0)           // NOXLATE
        type = AttributeRelate::LeftOuter;
    else if (::wcscmp(strType, L"RightOuter") == 0)     // NOXLATE 
        type = AttributeRelate::RightOuter;
    else if (::wcscmp(strType, L"Inner") == 0)          // NOXLATE
        type = AttributeRelate::Inner;
    else if (::wcscmp(strType, L"Association") == 0)    // NOXLATE
        type = AttributeRelate::Association;

	return type;
}

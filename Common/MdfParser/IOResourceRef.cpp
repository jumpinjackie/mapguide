//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include "IOResourceRef.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOResourceRef::IOResourceRef(std::wstring elementName)
{
    this->m_elementName = elementName;
    this->m_resourceId = L"";
    this->m_itemName = L"";
}

void IOResourceRef::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == this->m_elementName)
        this->m_startElemName = name;
}

void IOResourceRef::ElementChars(const wchar_t *ch)
{
    if (this->m_currElemName == L"ResourceId") // NOXLATE
        this->m_resourceId = ch;
    else if (this->m_currElemName == L"LibraryItemName") // NOXLATE
        this->m_itemName = ch;
}

void IOResourceRef::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (this->m_startElemName == name)
    {
        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

std::wstring IOResourceRef::GetResourceId()
{
    return this->m_resourceId;
}

std::wstring IOResourceRef::GetItemName()
{
    return this->m_itemName;
}

void IOResourceRef::Write(MdfStream &fd, std::string name, std::wstring resourceId, std::wstring itemName, bool mandatory)
{
    fd << tab() << "<" << name << ">" << std::endl; // NOXLATE
    inctab();

    //Property: ResourceId
    fd << tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(resourceId);
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: LibraryItemName
    if (itemName.length() > 0 || mandatory)
    {
        fd << tab() << "<LibraryItemName>"; // NOXLATE
        fd << EncodeString(itemName);
        fd << "</LibraryItemName>" << std::endl; // NOXLATE
    }

    dectab();
    fd << tab() << "</" << name << ">" <<std::endl; // NOXLATE
}

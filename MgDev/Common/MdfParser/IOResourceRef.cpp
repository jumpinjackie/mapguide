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
#include "IOResourceRef.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ResourceId);
ELEM_MAP_ENTRY(2, LibraryItemName);


IOResourceRef::IOResourceRef(std::wstring elementName, Version& version) : SAX2ElementHandler(version)
{
    this->m_elementName = elementName;
    this->m_resourceId = L"";
    this->m_itemName = L"";
}


void IOResourceRef::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    if (this->m_currElemName == this->m_elementName)
        this->m_startElemName = name;
}


void IOResourceRef::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eResourceId:
        this->m_resourceId = ch;
        break;

    case eLibraryItemName:
        this->m_itemName = ch;
        break;
    }
}


void IOResourceRef::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_startElemName = L"";
        handlerStack->pop();
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


void IOResourceRef::Write(MdfStream& fd, std::string name, std::wstring resourceId, std::wstring itemName, bool mandatory, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(resourceId);
    fd << endStr(sResourceId) << std::endl;

    // Property: LibraryItemName
    if (!itemName.empty() || mandatory)
    {
        fd << tab.tab() << startStr(sLibraryItemName);
        fd << EncodeString(itemName);
        fd << endStr(sLibraryItemName) << std::endl;
    }

    tab.dectab();
    fd << tab.tab() << endStr(name) <<std::endl;
}

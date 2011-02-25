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

#include "../stdafx.h"
#include "IOStringObjectCollection.h"

IOStringObjectCollection::IOStringObjectCollection(StringObjectCollection* strCol, Version& version, const std::string& colName, const std::string& strName) :
    SAX2ElementHandler(version),
    m_strCol(strCol)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_strCol);

    m_colName = toMdfString(colName.c_str());
    m_strName = toMdfString(strName.c_str());
}

IOStringObjectCollection::~IOStringObjectCollection()
{
}

void IOStringObjectCollection::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;

    if (m_currElemName == m_colName)
    {
        m_startElemName = name;
    }
}

void IOStringObjectCollection::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == m_strName)
    {
        m_strCol->Adopt(new StringObject(ch));
    }
}

void IOStringObjectCollection::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOStringObjectCollection::Write(MdfStream& fd, StringObjectCollection* strCol, Version* version, const std::string& colName, const std::string& strName, MgTab& tab)
{
    _ASSERT(NULL != strCol);

    fd << tab.tab() << startStr(colName) << std::endl;
    tab.inctab();

    for (int i = 0; i < strCol->GetCount(); ++i)
    {
        StringObject* strObj = dynamic_cast<StringObject*>(strCol->GetAt(i));
        _ASSERT(NULL != strObj);

        fd << tab.tab() << startStr(strName);
        fd << EncodeString(strObj->GetString());
        fd << endStr(strName) << std::endl;
    }

    tab.dectab();
    fd << tab.tab() << endStr(colName) << std::endl;
}

//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "IOValueList.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOValueList::IOValueList(ValueList* valueList)
{
    this->_valueList = valueList;
}

void IOValueList::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;

    if (m_currElemName == L"ValueList") // NOXLATE
    {
        m_startElemName = name;
    }
}

void IOValueList::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Value") // NOXLATE
    {
        MdfString* sValue = new MdfString(ch);
        this->_valueList->Adopt(sValue);
    }
}

void IOValueList::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        delete this;
    }
}

void IOValueList::Write(MdfStream &fd, ValueList* valueList)
{
    // don't output the list if it's empty - the schema requires this
    int numElements = valueList->GetCount();
    if (numElements == 0)
        return;

    fd << tab() << "<ValueList>" << std::endl; // NOXLATE
    inctab();

    for (int i=0; i<numElements; ++i)
    {
        MdfString* sValue = valueList->GetAt(i);
        fd << tab() << "<Value>" << EncodeString(*sValue) << "</Value>" << std::endl; // NOXLATE
    }

    dectab();
    fd << tab() << "</ValueList>" << std::endl; // NOXLATE
}

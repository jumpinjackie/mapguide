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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOUnknown::IOUnknown(std::wstring* xml)
{
    _xml = xml;
    m_startElemName.clear();
}

IOUnknown::~IOUnknown()
{
}

void IOUnknown::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{

    if (m_startElemName.empty())
    {
        m_startElemName = name;

        _xml->append(L"<"); // NOXLATE
        _xml->append(name);
        _xml->append(L">"); // NOXLATE
    }
    else
    {
        IOUnknown *IO = new IOUnknown(_xml);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOUnknown::ElementChars(const wchar_t *ch)
{
    _xml->append(ch);
}

void IOUnknown::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    _xml->append(L"</"); // NOXLATE
    _xml->append(name);
    _xml->append(L">"); // NOXLATE

    if (m_startElemName == name)
    {
        handlerStack->pop();
        delete this;
    }
}


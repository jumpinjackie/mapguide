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
#include "IOFill.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Fill);
ELEM_MAP_ENTRY(2, FillPattern);
ELEM_MAP_ENTRY(3, ForegroundColor);
ELEM_MAP_ENTRY(4, BackgroundColor);


IOFill::IOFill()
{
    this->_fill = NULL;
}


IOFill::IOFill(Fill* fill)
{
    this->_fill = fill;
}


IOFill::~IOFill()
{
}


void IOFill::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eFill:
        m_startElemName = name;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOFill::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == L"FillPattern") // NOXLATE
        (this->_fill)->SetFillPattern(ch);
    else if (m_currElemName == L"ForegroundColor") // NOXLATE
        (this->_fill)->SetForegroundColor(ch);
    else if (m_currElemName == L"BackgroundColor") // NOXLATE
        (this->_fill)->SetBackgroundColor(ch);
}


void IOFill::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_fill->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->_fill = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOFill::Write(MdfStream& fd, Fill* fill, Version* version)
{
    fd << tab() << "<Fill>" << std::endl; // NOXLATE
    inctab();

    //Property: FillPattern
    fd << tab() << "<FillPattern>"; // NOXLATE
    fd << EncodeString(fill->GetFillPattern());
    fd << "</FillPattern>" << std::endl; // NOXLATE

    //Property: ForegroundColor
    fd << tab() << "<ForegroundColor>"; // NOXLATE
    fd << EncodeString(fill->GetForegroundColor());
    fd << "</ForegroundColor>" << std::endl; // NOXLATE

    //Property: BackgroundColor
    fd << tab() << "<BackgroundColor>"; // NOXLATE
    fd << EncodeString(fill->GetBackgroundColor());
    fd << "</BackgroundColor>" << std::endl; // NOXLATE

    // Write any previously found unknown XML
    if (!fill->GetUnknownXml().empty())
        fd << tab() << toCString(fill->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</Fill>" << std::endl; // NOXLATE
}

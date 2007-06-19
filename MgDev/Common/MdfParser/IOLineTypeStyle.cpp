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
#include "IOLineTypeStyle.h"
#include "IOLineRule.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineTypeStyle);
ELEM_MAP_ENTRY(2, LineRule);


IOLineTypeStyle::IOLineTypeStyle()
{
    this->_lineTypeStyle = NULL;
    this->scaleRange = NULL;
}


IOLineTypeStyle::IOLineTypeStyle(VectorScaleRange* scaleRange)
{
    this->_lineTypeStyle = NULL;
    this->scaleRange = scaleRange;
}


IOLineTypeStyle::~IOLineTypeStyle()
{
}


void IOLineTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eLineTypeStyle:
        m_startElemName = name;
        this->_lineTypeStyle = new LineTypeStyle();
        break;

    case eLineRule:
        {
            IOLineRule* IO = new IOLineRule(this->_lineTypeStyle);
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


void IOLineTypeStyle::ElementChars(const wchar_t* ch)
{
}


void IOLineTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_lineTypeStyle->SetUnknownXml(UnknownXml());

        this->scaleRange->GetFeatureTypeStyles()->Adopt(this->_lineTypeStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_lineTypeStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOLineTypeStyle::Write(MdfStream& fd, LineTypeStyle* lineTypeStyle, Version* version)
{
    fd << tab() << "<LineTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: Rules
    for (int i=0; i<lineTypeStyle->GetRules()->GetCount(); ++i)
        IOLineRule::Write(fd, static_cast<LineRule*>(lineTypeStyle->GetRules()->GetAt(i)), version);

    // Write any previously found unknown XML
    if (!lineTypeStyle->GetUnknownXml().empty())
        fd << tab() << toCString(lineTypeStyle->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</LineTypeStyle>" << std::endl; // NOXLATE
}

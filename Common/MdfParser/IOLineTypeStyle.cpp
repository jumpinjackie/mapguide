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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineTypeStyle);
ELEM_MAP_ENTRY(2, LineRule);
ELEM_MAP_ENTRY(3, ExtendedData1);


IOLineTypeStyle::IOLineTypeStyle()
{
    this->m_lineTypeStyle = NULL;
    this->m_scaleRange = NULL;
}


IOLineTypeStyle::IOLineTypeStyle(VectorScaleRange* scaleRange)
{
    this->m_lineTypeStyle = NULL;
    this->m_scaleRange = scaleRange;
}


IOLineTypeStyle::~IOLineTypeStyle()
{
}


void IOLineTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eLineTypeStyle:
        this->m_startElemName = name;
        this->m_lineTypeStyle = new LineTypeStyle();
        break;

    case eLineRule:
        {
            IOLineRule* IO = new IOLineRule(this->m_lineTypeStyle);
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


void IOLineTypeStyle::ElementChars(const wchar_t* ch)
{
}


void IOLineTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_lineTypeStyle->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->GetFeatureTypeStyles()->Adopt(this->m_lineTypeStyle);
        this->m_scaleRange = NULL;
        this->m_lineTypeStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOLineTypeStyle::Write(MdfStream& fd, LineTypeStyle* lineTypeStyle, Version* version)
{
    fd << tab() << startStr(sLineTypeStyle) << std::endl;
    inctab();

    // Property: Rules
    for (int i=0; i<lineTypeStyle->GetRules()->GetCount(); ++i)
        IOLineRule::Write(fd, static_cast<LineRule*>(lineTypeStyle->GetRules()->GetAt(i)), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, lineTypeStyle->GetUnknownXml(), version);

    dectab();
    fd << tab() << endStr(sLineTypeStyle) << std::endl;
}

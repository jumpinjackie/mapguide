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
#include "IOPointTypeStyle.h"
#include "IOPointRule.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, PointTypeStyle);
ELEM_MAP_ENTRY(2, PointRule);
ELEM_MAP_ENTRY(3, DisplayAsText);
ELEM_MAP_ENTRY(4, AllowOverpost);


IOPointTypeStyle::IOPointTypeStyle()
{
    this->m_pointTypeStyle = NULL;
    this->m_scaleRange = NULL;
}


IOPointTypeStyle::IOPointTypeStyle(VectorScaleRange* scaleRange)
{
    this->m_pointTypeStyle = NULL;
    this->m_scaleRange = scaleRange;
}


IOPointTypeStyle::~IOPointTypeStyle()
{
}


void IOPointTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case ePointTypeStyle:
        this->m_startElemName = name;
        this->m_pointTypeStyle = new PointTypeStyle();
        break;

    case ePointRule:
        {
            IOPointRule* IO = new IOPointRule(this->m_pointTypeStyle);
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


void IOPointTypeStyle::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"DisplayAsText") // NOXLATE
        this->m_pointTypeStyle->SetDisplayAsText(wstrToBool(ch));
    else if (this->m_currElemName == L"AllowOverpost") // NOXLATE
        this->m_pointTypeStyle->SetAllowOverpost(wstrToBool(ch));
}


void IOPointTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_pointTypeStyle->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->GetFeatureTypeStyles()->Adopt(this->m_pointTypeStyle);
        this->m_scaleRange = NULL;
        this->m_pointTypeStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOPointTypeStyle::Write(MdfStream& fd, PointTypeStyle* pointTypeStyle, Version* version)
{
    fd << tab() << "<PointTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: DisplayAsText
    fd << tab() << "<DisplayAsText>"; // NOXLATE
    fd << (pointTypeStyle->IsDisplayAsText()? "true" : "false"); // NOXLATE
    fd << "</DisplayAsText>" << std::endl; // NOXLATE

    //Property: AllowOverpost
    fd << tab() << "<AllowOverpost>"; // NOXLATE
    fd << (pointTypeStyle->IsAllowOverpost()? "true" : "false"); // NOXLATE
    fd << "</AllowOverpost>" << std::endl; // NOXLATE

    //Property: Rules
    for (int i=0; i<pointTypeStyle->GetRules()->GetCount(); ++i)
        IOPointRule::Write(fd, static_cast<PointRule*>(pointTypeStyle->GetRules()->GetAt(i)), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pointTypeStyle->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</PointTypeStyle>" << std::endl; // NOXLATE
}

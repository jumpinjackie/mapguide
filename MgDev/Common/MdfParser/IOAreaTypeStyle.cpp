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
#include "IOAreaTypeStyle.h"
#include "IOAreaRule.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, AreaTypeStyle);
ELEM_MAP_ENTRY(2, AreaRule);


IOAreaTypeStyle::IOAreaTypeStyle()
{
    this->_areaTypeStyle = NULL;
    this->scaleRange = NULL;
}


IOAreaTypeStyle::IOAreaTypeStyle(VectorScaleRange* scaleRange)
{
    this->_areaTypeStyle = NULL;
    this->scaleRange = scaleRange;
}


IOAreaTypeStyle::~IOAreaTypeStyle()
{
}


void IOAreaTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eAreaTypeStyle:
        m_startElemName = name;
        this->_areaTypeStyle = new AreaTypeStyle();
        break;

    case eAreaRule:
        {
            IOAreaRule* IO = new IOAreaRule(this->_areaTypeStyle);
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


void IOAreaTypeStyle::ElementChars(const wchar_t* ch)
{
}


void IOAreaTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_areaTypeStyle->SetUnknownXml(UnknownXml());

        this->scaleRange->GetFeatureTypeStyles()->Adopt(this->_areaTypeStyle);
        handlerStack->pop();
        this->scaleRange = NULL;
        this->_areaTypeStyle = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOAreaTypeStyle::Write(MdfStream& fd, AreaTypeStyle* areaTypeStyle, Version* version)
{
    fd << tab() << "<AreaTypeStyle>" << std::endl; // NOXLATE
    inctab();

    //Property: Rules
    for (int i=0; i<areaTypeStyle->GetRules()->GetCount(); ++i)
        IOAreaRule::Write(fd, static_cast<AreaRule*>(areaTypeStyle->GetRules()->GetAt(i)), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, areaTypeStyle->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</AreaTypeStyle>" << std::endl; // NOXLATE
}

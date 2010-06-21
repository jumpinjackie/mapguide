//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
ELEM_MAP_ENTRY(3, ShowInLegend);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOAreaTypeStyle::IOAreaTypeStyle(Version& version) : SAX2ElementHandler(version)
{
    this->m_areaTypeStyle = NULL;
    this->m_scaleRange = NULL;
}


IOAreaTypeStyle::IOAreaTypeStyle(VectorScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
{
    this->m_areaTypeStyle = NULL;
    this->m_scaleRange = scaleRange;
}


IOAreaTypeStyle::~IOAreaTypeStyle()
{
}


void IOAreaTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eAreaTypeStyle:
        this->m_startElemName = name;
        this->m_areaTypeStyle = new AreaTypeStyle();
        break;

    case eAreaRule:
        {
            IOAreaRule* IO = new IOAreaRule(this->m_areaTypeStyle, this->m_version);
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


void IOAreaTypeStyle::ElementChars(const wchar_t* ch)
{
    if (eShowInLegend == this->m_currElemId)
        this->m_areaTypeStyle->SetShowInLegend(wstrToBool(ch));
}


void IOAreaTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_areaTypeStyle->SetUnknownXml(this->m_unknownXml);

        this->m_scaleRange->GetFeatureTypeStyles()->Adopt(this->m_areaTypeStyle);
        this->m_scaleRange = NULL;
        this->m_areaTypeStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOAreaTypeStyle::Write(MdfStream& fd, AreaTypeStyle* areaTypeStyle, Version* version)
{
    fd << tab() << startStr(sAreaTypeStyle) << std::endl;
    inctab();

    MdfStringStream fdExtData;

    // Property: Rules
    for (int i=0; i<areaTypeStyle->GetRules()->GetCount(); ++i)
        IOAreaRule::Write(fd, static_cast<AreaRule*>(areaTypeStyle->GetRules()->GetAt(i)), version);

    // Property: ShowInLegend
    if (!version || (*version >= Version(1, 3, 0)))
    {
        // version 1.3.0 has a ShowInLegend Property
        fd << tab() << startStr(sShowInLegend);
        fd << BoolToStr(areaTypeStyle->IsShowInLegend());
        fd << endStr(sShowInLegend) << std::endl;
    }
    else
    {
        inctab();
        // earlier version - save ShowInLegend to ExtendedData1
        fdExtData << tab() << startStr(sShowInLegend);
        fdExtData << BoolToStr(areaTypeStyle->IsShowInLegend());
        fdExtData << endStr(sShowInLegend) << std::endl;
        dectab();
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, areaTypeStyle->GetUnknownXml(), fdExtData.str(), version);

    dectab();
    fd << tab() << endStr(sAreaTypeStyle) << std::endl;
}

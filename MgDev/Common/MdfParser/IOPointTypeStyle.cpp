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
#include "IOPointTypeStyle.h"
#include "IOPointRule.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, PointTypeStyle);
ELEM_MAP_ENTRY(2, DisplayAsText);
ELEM_MAP_ENTRY(3, AllowOverpost);
ELEM_MAP_ENTRY(4, PointRule);
ELEM_MAP_ENTRY(5, ShowInLegend);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOPointTypeStyle::IOPointTypeStyle(Version& version) : SAX2ElementHandler(version)
{
    this->m_pointTypeStyle = NULL;
    this->m_scaleRange = NULL;
}


IOPointTypeStyle::IOPointTypeStyle(VectorScaleRange* scaleRange, Version& version) : SAX2ElementHandler(version)
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
            IOPointRule* IO = new IOPointRule(this->m_pointTypeStyle, this->m_version);
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


void IOPointTypeStyle::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eDisplayAsText:
        this->m_pointTypeStyle->SetDisplayAsText(wstrToBool(ch));
        break;

    case eAllowOverpost:
        this->m_pointTypeStyle->SetAllowOverpost(wstrToBool(ch));
        break;

    case eShowInLegend:
        this->m_pointTypeStyle->SetShowInLegend(wstrToBool(ch));
        break;
    }
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOPointTypeStyle::Write(MdfStream& fd, PointTypeStyle* pointTypeStyle, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sPointTypeStyle) << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    // Property: DisplayAsText
    fd << tab.tab() << startStr(sDisplayAsText);
    fd << BoolToStr(pointTypeStyle->IsDisplayAsText());
    fd << endStr(sDisplayAsText) << std::endl;

    // Property: AllowOverpost
    fd << tab.tab() << startStr(sAllowOverpost);
    fd << BoolToStr(pointTypeStyle->IsAllowOverpost());
    fd << endStr(sAllowOverpost) << std::endl;

    // Property: Rules
    for (int i=0; i<pointTypeStyle->GetRules()->GetCount(); ++i)
        IOPointRule::Write(fd, static_cast<PointRule*>(pointTypeStyle->GetRules()->GetAt(i)), version, tab);

    // Property: ShowInLegend
    if (!version || (*version >= Version(1, 3, 0)))
    {
        // version 1.3.0 has a ShowInLegend Property
        fd << tab.tab() << startStr(sShowInLegend);
        fd << BoolToStr(pointTypeStyle->IsShowInLegend());
        fd << endStr(sShowInLegend) << std::endl;
    }
    else if (*version >= Version(1, 0, 0))
    {
        // save ShowInLegend as extended data for LDF versions 1.0.0, 1.1.0, and 1.2.0
        tab.inctab();
        fdExtData << tab.tab() << startStr(sShowInLegend);
        fdExtData << BoolToStr(pointTypeStyle->IsShowInLegend());
        fdExtData << endStr(sShowInLegend) << std::endl;
        tab.dectab();
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pointTypeStyle->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sPointTypeStyle) << std::endl;
}

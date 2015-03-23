//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOCompositeTypeStyle.h"
#include "IOCompositeRule.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, CompositeTypeStyle);
ELEM_MAP_ENTRY(2, CompositeRule);
ELEM_MAP_ENTRY(3, ShowInLegend);
ELEM_MAP_ENTRY(4, ExtendedData1);

IOCompositeTypeStyle::IOCompositeTypeStyle(VectorScaleRange* vectorScaleRange, Version& version) : SAX2ElementHandler(version)
{
    this->m_vectorScaleRange = vectorScaleRange;
    this->m_compositeTypeStyle = NULL;
}


IOCompositeTypeStyle::~IOCompositeTypeStyle()
{
}


void IOCompositeTypeStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eCompositeTypeStyle:
        this->m_startElemName = name;
        this->m_compositeTypeStyle = new CompositeTypeStyle();
        break;

    case eCompositeRule:
        {
            IOCompositeRule* IO = new IOCompositeRule(this->m_compositeTypeStyle, this->m_version);
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


void IOCompositeTypeStyle::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eShowInLegend:
        this->m_compositeTypeStyle->SetShowInLegend(wstrToBool(ch));
        break;
    }
}


void IOCompositeTypeStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_compositeTypeStyle->SetUnknownXml(this->m_unknownXml);

        this->m_vectorScaleRange->GetFeatureTypeStyles()->Adopt(this->m_compositeTypeStyle);
        this->m_vectorScaleRange = NULL;
        this->m_compositeTypeStyle = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOCompositeTypeStyle::Write(MdfStream& fd, CompositeTypeStyle* compositeTypeStyle, Version* version, MgTab& tab)
{
    // the schema currently requires at least one rule
    RuleCollection* ruleCollection = compositeTypeStyle->GetRules();
    int numElements = ruleCollection->GetCount();
    _ASSERT(numElements > 0);

    fd << tab.tab() << startStr(sCompositeTypeStyle) << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    for (int i=0; i<numElements; ++i)
    {
        CompositeRule* compositeRule = dynamic_cast<CompositeRule*>(ruleCollection->GetAt(i));
        if (compositeRule)
            IOCompositeRule::Write(fd, compositeRule, version, tab);
    }

    // Property: ShowInLegend
    if (!version || (*version >= Version(1, 3, 0)))
    {
        // version 1.3.0 has a ShowInLegend Property
        fd << tab.tab() << startStr(sShowInLegend);
        fd << BoolToStr(compositeTypeStyle->IsShowInLegend());
        fd << endStr(sShowInLegend) << std::endl;
    }
    else if (*version >= Version(1, 0, 0))
    {
        // save ShowInLegend as extended data for LDF versions 1.0.0, 1.1.0, and 1.2.0
        tab.inctab();
        fdExtData << tab.tab() << startStr(sShowInLegend);
        fdExtData << BoolToStr(compositeTypeStyle->IsShowInLegend());
        fdExtData << endStr(sShowInLegend) << std::endl;
        tab.dectab();
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, compositeTypeStyle->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sCompositeTypeStyle) << std::endl;
}

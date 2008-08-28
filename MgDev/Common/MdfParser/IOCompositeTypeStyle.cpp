//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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
    if (this->m_currElemName == L"CompositeTypeStyle") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_compositeTypeStyle = new CompositeTypeStyle();
    }
    else if (this->m_currElemName == L"CompositeRule") // NOXLATE
    {
        IOCompositeRule* IO = new IOCompositeRule(this->m_compositeTypeStyle, this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOCompositeTypeStyle::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"ShowInLegend")
        this->m_compositeTypeStyle->SetShowInLegend(wstrToBool(ch));
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
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOCompositeTypeStyle::Write(MdfStream& fd, CompositeTypeStyle* compositeTypeStyle, Version* version)
{
    // the schema currently requires at least one rule
    RuleCollection* ruleCollection = compositeTypeStyle->GetRules();
    int numElements = ruleCollection->GetCount();
    _ASSERT(numElements > 0);

    fd << tab() << "<CompositeTypeStyle>" << std::endl; // NOXLATE
    inctab();

    MdfStringStream fdExtData;

    for (int i=0; i<numElements; ++i)
    {
        CompositeRule* compositeRule = dynamic_cast<CompositeRule*>(ruleCollection->GetAt(i));
        if (compositeRule)
            IOCompositeRule::Write(fd, compositeRule, version);
    }

    // Property: ShowInLegend
    if (!version || (*version >= Version(1, 3, 0)))
    {
        // version 1.3.0 has a ShowInLegend Property
        fd << tab() << "<ShowInLegend>";
        fd << BoolToStr(compositeTypeStyle->IsShowInLegend());
        fd << tab() << "</ShowInLegend>" << std::endl;
    }
    else
    {
        inctab();
        // earlier version - save ShowInLegend to ExtendedData1
        fdExtData << tab() << "<ShowInLegend>";
        fdExtData << BoolToStr(compositeTypeStyle->IsShowInLegend());
        fdExtData << tab() << "</ShowInLegend>" << std::endl;
        dectab();
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, compositeTypeStyle->GetUnknownXml(), fdExtData.str(), version);

    dectab();
    fd << tab() << "</CompositeTypeStyle>" << std::endl; // NOXLATE
}

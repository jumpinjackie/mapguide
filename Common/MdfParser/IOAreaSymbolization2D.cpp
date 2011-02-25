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
#include "IOAreaSymbolization2D.h"
#include "IOStroke.h"
#include "IOFill.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, AreaSymbolization2D);
ELEM_MAP_ENTRY(2, Fill);
ELEM_MAP_ENTRY(3, Stroke);
ELEM_MAP_ENTRY(4, ExtendedData1);


IOAreaSymbolization2D::IOAreaSymbolization2D(Version& version) : SAX2ElementHandler(version)
{
    this->m_areaSymbolization = NULL;
    this->m_areaRule = NULL;
}


IOAreaSymbolization2D::IOAreaSymbolization2D(AreaRule* areaRule, Version& version) : SAX2ElementHandler(version)
{
    this->m_areaSymbolization = NULL;
    this->m_areaRule = areaRule;
}


IOAreaSymbolization2D::~IOAreaSymbolization2D()
{
}


void IOAreaSymbolization2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eAreaSymbolization2D:
        this->m_startElemName = name;
        this->m_areaSymbolization = new AreaSymbolization2D();
        // delete the fill and edge that are created by default - recreate if present when parsing
        delete this->m_areaSymbolization->OrphanFill();
        delete this->m_areaSymbolization->OrphanEdge();
        break;

    case eFill:
        {
            this->m_areaSymbolization->AdoptFill(new Fill());
            IOFill* IO = new IOFill(this->m_areaSymbolization->GetFill(), this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eStroke:
        {
            this->m_areaSymbolization->AdoptEdge(new Stroke());
            IOStroke* IO = new IOStroke(this->m_areaSymbolization->GetEdge(), this->m_currElemName, this->m_version);
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


void IOAreaSymbolization2D::ElementChars(const wchar_t* ch)
{
}


void IOAreaSymbolization2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_areaSymbolization->SetUnknownXml(this->m_unknownXml);

        if (this->m_areaSymbolization)
            this->m_areaRule->AdoptSymbolization(this->m_areaSymbolization);
        this->m_areaRule = NULL;
        this->m_areaSymbolization = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOAreaSymbolization2D::Write(MdfStream& fd, AreaSymbolization2D* areaSymbolization, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sAreaSymbolization2D) << std::endl;
    tab.inctab();

    if (areaSymbolization)
    {
        // Property: Fill
        if (areaSymbolization->GetFill())
            IOFill::Write(fd, areaSymbolization->GetFill(), version, tab);

        // Property: Stroke
        if (areaSymbolization->GetEdge())
            IOStroke::Write(fd, areaSymbolization->GetEdge(), sStroke, version, tab);

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, areaSymbolization->GetUnknownXml(), version, tab);
    }

    tab.dectab();
    fd << tab.tab() << endStr(sAreaSymbolization2D) << std::endl;
}

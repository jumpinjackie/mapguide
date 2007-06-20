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


IOAreaSymbolization2D::IOAreaSymbolization2D()
{
    this->_areaSymbolization = NULL;
    this->areaRule = NULL;
}


IOAreaSymbolization2D::IOAreaSymbolization2D(AreaRule* areaRule)
{
    this->_areaSymbolization = NULL;
    this->areaRule = areaRule;
}


IOAreaSymbolization2D::~IOAreaSymbolization2D()
{
}


void IOAreaSymbolization2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eAreaSymbolization2D:
        m_startElemName = name;
        this->_areaSymbolization = new AreaSymbolization2D();
        // delete the fill and edge that are created by default - recreate if present when parsing
        delete this->_areaSymbolization->OrphanFill();
        delete this->_areaSymbolization->OrphanEdge();
        break;

    case eFill:
        {
            this->_areaSymbolization->AdoptFill(new Fill());
            IOFill* IO = new IOFill(this->_areaSymbolization->GetFill());
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eStroke:
        {
            this->_areaSymbolization->AdoptEdge(new Stroke());
            IOStroke* IO = new IOStroke(this->_areaSymbolization->GetEdge(), m_currElemName);
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


void IOAreaSymbolization2D::ElementChars(const wchar_t* ch)
{
}


void IOAreaSymbolization2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_areaSymbolization->SetUnknownXml(UnknownXml());

        if (this->_areaSymbolization != NULL)
            this->areaRule->AdoptSymbolization(this->_areaSymbolization);
        handlerStack->pop();
        this->areaRule = NULL;
        this->_areaSymbolization = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOAreaSymbolization2D::Write(MdfStream& fd, AreaSymbolization2D* areaSymbolization, Version* version)
{
    fd << tab() << "<AreaSymbolization2D>" << std::endl; // NOXLATE
    inctab();

    if (areaSymbolization != NULL)
    {
        //Property: Fill
        if (areaSymbolization->GetFill() != NULL)
            IOFill::Write(fd, areaSymbolization->GetFill(), version);

        //Property: Stroke
        if (areaSymbolization->GetEdge() != NULL)
            IOStroke::Write(fd, areaSymbolization->GetEdge(), "Stroke", version); // NOXLATE

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, areaSymbolization->GetUnknownXml(), version);
    }

    dectab();
    fd << tab() << "</AreaSymbolization2D>" << std::endl; // NOXLATE
}

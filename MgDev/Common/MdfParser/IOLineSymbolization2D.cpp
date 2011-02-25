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
#include "IOLineSymbolization2D.h"
#include "IOStroke.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineSymbolization2D);
ELEM_MAP_ENTRY(2, LineStyle);
ELEM_MAP_ENTRY(3, Thickness);
ELEM_MAP_ENTRY(4, Color);
ELEM_MAP_ENTRY(5, Unit);
ELEM_MAP_ENTRY(6, SizeContext);
ELEM_MAP_ENTRY(7, ExtendedData1);


IOLineSymbolization2D::IOLineSymbolization2D(Version& version) : SAX2ElementHandler(version)
{
    this->m_lineSymbolization = NULL;
    this->m_lineRule = NULL;
}


IOLineSymbolization2D::IOLineSymbolization2D(LineRule* lineRule, Version& version) : SAX2ElementHandler(version)
{
    this->m_lineSymbolization = NULL;
    this->m_lineRule = lineRule;
}


IOLineSymbolization2D::~IOLineSymbolization2D()
{
}


void IOLineSymbolization2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eLineSymbolization2D:
        this->m_startElemName = name;
        this->m_lineSymbolization = new LineSymbolization2D();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOLineSymbolization2D::ElementChars(const wchar_t* ch)
{
    Stroke* stroke = this->m_lineSymbolization->GetStroke();

    switch (this->m_currElemId)
    {
    case eLineStyle:
        stroke->SetLineStyle(ch);
        break;

    case eThickness:
        stroke->SetThickness(ch);
        break;

    case eColor:
        stroke->SetColor(ch);
        break;

    case eUnit:
        stroke->SetUnit(LengthConverter::EnglishToUnit(ch));
        break;

    case eSizeContext:
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            stroke->SetSizeContext(MdfModel::MappingUnits);
        else if (::wcscmp(ch, L"DeviceUnits") == 0) // NOXLATE
            stroke->SetSizeContext(MdfModel::DeviceUnits);
        break;
    }
}


void IOLineSymbolization2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_lineSymbolization->GetStroke()->SetUnknownXml(this->m_unknownXml);

        this->m_lineRule->GetSymbolizations()->Adopt(this->m_lineSymbolization);
        this->m_lineRule = NULL;
        this->m_lineSymbolization = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOLineSymbolization2D::Write(MdfStream& fd, LineSymbolization2D* lineSymbolization, Version* version, MgTab& tab)
{
    // a LineSymbolization2D is just a Stroke
    IOStroke::Write(fd, lineSymbolization->GetStroke(), sLineSymbolization2D, version, tab);
}

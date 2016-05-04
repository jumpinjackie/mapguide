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
#include "IOStroke.h"
#include "IOFill.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Stroke);
ELEM_MAP_ENTRY(2, LineStyle);
ELEM_MAP_ENTRY(3, Thickness);
ELEM_MAP_ENTRY(4, Color);
ELEM_MAP_ENTRY(5, Unit);
ELEM_MAP_ENTRY(6, SizeContext);
ELEM_MAP_ENTRY(7, ExtendedData1);


IOStroke::IOStroke(std::wstring elementName, Version& version) : SAX2ElementHandler(version)
{
    this->m_stroke = NULL;
    this->m_elementName = elementName;
}


IOStroke::IOStroke(Stroke* stroke, std::wstring elementName, Version& version) : SAX2ElementHandler(version)
{
    this->m_stroke = stroke;
    this->m_elementName = elementName;
}


IOStroke::~IOStroke()
{
}


void IOStroke::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // Note: m_elementName is not part of the Element Map, so the
    // eStroke value is substituted - but sStroke will not be the correct
    // string, and cannot not be used in place of m_elementName.

    this->m_currElemName = name;
    if (this->m_currElemName == this->m_elementName)
    {
        this->m_currElemId = eStroke;
        this->m_startElemName = name;
    }
    else
    {
        this->m_currElemId = _ElementIdFromName(name);
    }

    switch (this->m_currElemId)
    {
    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOStroke::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eLineStyle:
        this->m_stroke->SetLineStyle(ch);
        break;

    case eThickness:
        this->m_stroke->SetThickness(ch);
        break;

    case eColor:
        this->m_stroke->SetColor(ch);
        break;

    case eUnit:
        this->m_stroke->SetUnit(LengthConverter::EnglishToUnit(ch));
        break;

    case eSizeContext:
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            this->m_stroke->SetSizeContext(MdfModel::MappingUnits);
        else if (::wcscmp(ch, L"DeviceUnits") == 0) // NOXLATE
            this->m_stroke->SetSizeContext(MdfModel::DeviceUnits);
        break;
    }
}


void IOStroke::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_stroke->SetUnknownXml(this->m_unknownXml);

        this->m_stroke = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOStroke::Write(MdfStream& fd, Stroke* stroke, std::string name, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    MdfStringStream fdExtData;

    // Property: LineStyle
    fd << tab.tab() << startStr(sLineStyle);
    fd << EncodeString(stroke->GetLineStyle());
    fd << endStr(sLineStyle) << std::endl;

    // Property: Thickness
    fd << tab.tab() << startStr(sThickness);
    fd << EncodeString(stroke->GetThickness());
    fd << endStr(sThickness) << std::endl;

    // Property: ForegroundColor
    fd << tab.tab() << startStr(sColor);
    fd << EncodeString(stroke->GetColor());
    fd << endStr(sColor) << std::endl;

    // Property: Unit
    fd << tab.tab() << startStr(sUnit);
    std::auto_ptr<MdfString> str(LengthConverter::UnitToEnglish(stroke->GetUnit()));
    fd << EncodeString(*str);
    fd << endStr(sUnit) << std::endl;

    // Property: SizeContext
    if (!version || (*version >= Version(1, 1, 0)))
    {
        // only write SizeContext if the LDF version is 1.1.0 or greater
        fd << tab.tab() << startStr(sSizeContext);
        if (stroke->GetSizeContext() == MdfModel::MappingUnits)
            fd << "MappingUnits"; // NOXLATE
        else
            fd << "DeviceUnits"; // NOXLATE
        fd << endStr(sSizeContext) << std::endl;
    }
    else if (*version == Version(1, 0, 0))
    {
        // save SizeContext as extended data for LDF version 1.0.0
        tab.inctab();

        fdExtData << tab.tab() << startStr(sSizeContext);
        if (stroke->GetSizeContext() == MdfModel::MappingUnits)
            fdExtData << "MappingUnits"; // NOXLATE
        else
            fdExtData << "DeviceUnits"; // NOXLATE
        fdExtData << endStr(sSizeContext) << std::endl;

        tab.dectab();
    }

    // Write the unknown XML / extended data
    IOUnknown::Write(fd, stroke->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}

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
#include "IOStroke.h"
#include "IOExtra.h"
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


IOStroke::IOStroke(std::wstring elementName)
{
    this->_stroke = NULL;
    this->m_elementName = elementName;
}


IOStroke::IOStroke(Stroke* stroke, std::wstring elementName)
{
    this->_stroke = stroke;
    this->m_elementName = elementName;
}


IOStroke::~IOStroke()
{
}


void IOStroke::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // Note: this->m_elementName is not part of the Element Map, so the
    // eStroke value is substituted - but sStroke will not be the correct
    // string, and cannot not be used in place of m_elementName.

    m_currElemName = name;
    if (m_currElemName == this->m_elementName)
    {
        m_currElemId = eStroke;
        m_startElemName = name;
    }
    else
    {
        m_currElemId = _ElementIdFromName(name);
    }

    if (eUnknown == m_currElemId)
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOStroke::ElementChars(const wchar_t* ch)
{
    if (m_currElemName == swLineStyle)
        this->_stroke->SetLineStyle(ch);
    else if (m_currElemName == swThickness)
        this->_stroke->SetThickness(ch);
    else if (m_currElemName == swColor)
        this->_stroke->SetColor(ch);
    else if (m_currElemName == swUnit)
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        this->_stroke->SetUnit(unit);
    }
    else if (m_currElemName == swSizeContext)
    {
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            this->_stroke->SetSizeContext(MdfModel::MappingUnits);
        else // "DeviceUnits" & default
            this->_stroke->SetSizeContext(MdfModel::DeviceUnits);
    }
}


void IOStroke::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_stroke->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->_stroke = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOStroke::Write(MdfStream& fd, Stroke* stroke, std::string name, Version* version)
{
    fd << tab() << "<" << name << ">" << std::endl;
    inctab();

    MdfStringStream fdExtData;

    //Property: LineStyle
    fd << tab() << startStr(sLineStyle);
    fd << EncodeString(stroke->GetLineStyle());
    fd << endStr(sLineStyle) << std::endl;

    //Property: Thickness
    fd << tab() << startStr(sThickness);
    fd << EncodeString(stroke->GetThickness());
    fd << endStr(sThickness) << std::endl;

    //Property: ForegroundColor
    fd << tab() << startStr(sColor);
    fd << EncodeString(stroke->GetColor());
    fd << endStr(sColor) << std::endl;

    //Property: Unit
    fd << tab() << startStr(sUnit);
    std::auto_ptr<MdfString> str(LengthConverter::UnitToEnglish(stroke->GetUnit()));
    fd << EncodeString(*str);
    fd << endStr(sUnit) << std::endl;

    //Property: SizeContext
    if (!version || (*version >= Version(1, 1, 0)))
    {
        // only write SizeContext if the LDF version is 1.1.0 or greater
        fd << tab() << startStr(sSizeContext);
        if (stroke->GetSizeContext() == MdfModel::MappingUnits)
            fd << "MappingUnits"; // NOXLATE
        else
            fd << "DeviceUnits"; // NOXLATE
        fd << endStr(sSizeContext) << std::endl;
    }
    else if (*version == Version(1, 0, 0))
    {
        // save SizeContext as extended data for LDF version 1.0.0
        inctab();

        fdExtData << tab() << startStr(sSizeContext);
        if (stroke->GetSizeContext() == MdfModel::MappingUnits)
            fdExtData << "MappingUnits"; // NOXLATE
        else
            fdExtData << "DeviceUnits"; // NOXLATE
        fdExtData << endStr(sSizeContext) << std::endl;

        dectab();
    }

    // Add any unknown XML to the extended data
    if (!stroke->GetUnknownXml().empty())
    {
        inctab();
        fdExtData << tab() << toCString(stroke->GetUnknownXml());
        dectab();
    }

    // Write the unknown XML / extended data
    IOUnknown::WriteRaw(fd, fdExtData.str(), version);

    dectab();
    fd << tab() << "</" << name << ">" << std::endl;
}

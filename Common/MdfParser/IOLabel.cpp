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
#include "IOLabel.h"
#include "IOSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Label);
// Inherited Symbol Elements
ELEM_MAP_ENTRY(2, Unit);
ELEM_MAP_ENTRY(3, SizeContext);
ELEM_MAP_ENTRY(4, SizeX);
ELEM_MAP_ENTRY(5, SizeY);
ELEM_MAP_ENTRY(6, Rotation);
ELEM_MAP_ENTRY(7, MaintainAspect);
ELEM_MAP_ENTRY(8, InsertionPointX);
ELEM_MAP_ENTRY(9, InsertionPointY);
// Local Elements
ELEM_MAP_ENTRY(10, Text);
ELEM_MAP_ENTRY(11, FontName);
ELEM_MAP_ENTRY(12, ForegroundColor);
ELEM_MAP_ENTRY(13, BackgroundColor);
ELEM_MAP_ENTRY(14, BackgroundStyle);
ELEM_MAP_ENTRY(15, HorizontalAlignment);
ELEM_MAP_ENTRY(16, VerticalAlignment);
ELEM_MAP_ENTRY(17, Bold);
ELEM_MAP_ENTRY(18, Italic);
ELEM_MAP_ENTRY(19, Underlined);
ELEM_MAP_ENTRY(20, AdvancedPlacement);
ELEM_MAP_ENTRY(21, ScaleLimit);
ELEM_MAP_ENTRY(22, ExtendedData1);


IOLabel::IOLabel(Version& version) : SAX2ElementHandler(version)
{
    this->m_label = NULL;
    this->m_rule = NULL;
}


IOLabel::IOLabel(Rule* rule, Version& version) : SAX2ElementHandler(version)
{
    this->m_label = NULL;
    this->m_rule = rule;
}


IOLabel::~IOLabel()
{
}


void IOLabel::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eLabel:
        this->m_startElemName = name;
        this->m_label = new Label();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOLabel::ElementChars(const wchar_t* ch)
{
    if (this->m_label->GetSymbol() == NULL)
        this->m_label->AdoptSymbol(new TextSymbol());

    TextSymbol* symbol = this->m_label->GetSymbol();

    switch (this->m_currElemId)
    {
    case eText:
        symbol->SetText(ch);
        break;

    case eFontName:
        symbol->SetFontName(ch);
        break;

    case eForegroundColor:
        symbol->SetForegroundColor(ch);
        break;

    case eBackgroundColor:
        symbol->SetBackgroundColor(ch);
        break;

    case eBackgroundStyle:
        if (::wcscmp(ch, L"Transparent") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Transparent);
        else if (::wcscmp(ch, L"Opaque") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Opaque);
        else if (::wcscmp(ch, L"Ghosted") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Ghosted);
        break;

    case eHorizontalAlignment:
        if (this->m_version < Version(1, 3, 0))
        {
            // legacy layer definitions may have unquoted alignments
            // - add quotes if necessary
            if (::wcscmp(ch, L"Left") == 0) // NOXLATE
                symbol->SetHorizontalAlignment(L"'Left'");
            else if (::wcscmp(ch, L"Center") == 0) // NOXLATE
                symbol->SetHorizontalAlignment(L"'Center'");
            else if (::wcscmp(ch, L"Right") == 0) // NOXLATE
                symbol->SetHorizontalAlignment(L"'Right'");
            else
                symbol->SetHorizontalAlignment(ch);
        }
        else
            symbol->SetHorizontalAlignment(ch);
        break;

    case eVerticalAlignment:
        if (this->m_version < Version(1, 3, 0))
        {
            // legacy layer definitions may have unquoted alignments
            // - add quotes if necessary
            if (::wcscmp(ch, L"Bottom") == 0) // NOXLATE
                symbol->SetVerticalAlignment(L"'Bottom'");
            else if (::wcscmp(ch, L"Baseline") == 0) // NOXLATE
                symbol->SetVerticalAlignment(L"'Baseline'");
            else if (::wcscmp(ch, L"Halfline") == 0) // NOXLATE
                symbol->SetVerticalAlignment(L"'Halfline'");
            else if (::wcscmp(ch, L"Capline") == 0) // NOXLATE
                symbol->SetVerticalAlignment(L"'Capline'");
            else if (::wcscmp(ch, L"Top") == 0) // NOXLATE
                symbol->SetVerticalAlignment(L"'Top'");
            else
                symbol->SetVerticalAlignment(ch);
        }
        else
            symbol->SetVerticalAlignment(ch);
        break;

    case eBold:
        symbol->SetBold(ch);
        break;

    case eItalic:
        symbol->SetItalic(ch);
        break;

    case eUnderlined:
        symbol->SetUnderlined(ch);
        break;

    case eScaleLimit:
        symbol->SetScaleLimit(wstrToDouble(ch));
        break;

    case eUnit:
        symbol->SetUnit(LengthConverter::EnglishToUnit(ch));
        break;

    case eSizeContext:
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            symbol->SetSizeContext(MdfModel::MappingUnits);
        else if (::wcscmp(ch, L"DeviceUnits") == 0) // NOXLATE
            symbol->SetSizeContext(MdfModel::DeviceUnits);
        break;

    case eSizeX:
        symbol->SetSizeX(ch);
        break;

    case eSizeY:
        symbol->SetSizeY(ch);
        break;

    case eRotation:
        symbol->SetRotation(ch);
        break;

    case eMaintainAspect:
        symbol->SetMaintainAspect(wstrToBool(ch));
        break;

    case eInsertionPointX:
        symbol->SetInsertionPointX(ch);
        break;

    case eInsertionPointY:
        symbol->SetInsertionPointY(ch);
        break;
    }
}


void IOLabel::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_label->SetUnknownXml(this->m_unknownXml);

        this->m_rule->AdoptLabel(this->m_label);
        this->m_rule = NULL;
        this->m_label = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOLabel::Write(MdfStream& fd, Label* label, Version* version, MgTab& tab)
{
    TextSymbol* symbol = label->GetSymbol();

    if (symbol)
    {
        fd << tab.tab() << startStr(sLabel) << std::endl;
        tab.inctab();

        IOSymbol::Write(fd, symbol, version, tab);

        // Property: Text
        fd << tab.tab() << startStr(sText);
        fd << EncodeString(symbol->GetText());
        fd << endStr(sText) << std::endl;

        // Property: FontName
        fd << tab.tab() << startStr(sFontName);
        fd << EncodeString(symbol->GetFontName());
        fd << endStr(sFontName) << std::endl;

        // Property: ForegroundColor
        fd << tab.tab() << startStr(sForegroundColor);
        fd << EncodeString(symbol->GetForegroundColor());
        fd << endStr(sForegroundColor) << std::endl;

        // Property: BackgroundColor
        fd << tab.tab() << startStr(sBackgroundColor);
        fd << EncodeString(symbol->GetBackgroundColor());
        fd << endStr(sBackgroundColor) << std::endl;

        // Property: BackgroundStyle
        fd << tab.tab() << startStr(sBackgroundStyle);
        if (symbol->GetBackgroundStyle() == TextSymbol::Transparent)
            fd << "Transparent"; // NOXLATE
        else if (symbol->GetBackgroundStyle() == TextSymbol::Opaque)
            fd << "Opaque"; // NOXLATE
        else
            fd << "Ghosted"; // NOXLATE
        fd << endStr(sBackgroundStyle) << std::endl;

        // Property: HorizontalAlignment
        if (symbol->GetHorizontalAlignment() != L"'Center'") // NOXLATE
        {
            fd << tab.tab() << startStr(sHorizontalAlignment);
            fd << EncodeString(symbol->GetHorizontalAlignment());
            fd << endStr(sHorizontalAlignment) << std::endl;
        }

        // Property: VerticalAlignment
        if (symbol->GetVerticalAlignment() != L"'Baseline'") // NOXLATE
        {
            fd << tab.tab() << startStr(sVerticalAlignment);
            fd << EncodeString(symbol->GetVerticalAlignment());
            fd << endStr(sVerticalAlignment) << std::endl;
        }

        // Property: Bold
        if (wstrToBool(symbol->GetBold().c_str()))
        {
            fd << tab.tab() << startStr(sBold);
            fd << BoolToStr(true);
            fd << endStr(sBold) << std::endl;
        }

        // Property: Italic
        if (wstrToBool(symbol->GetItalic().c_str()))
        {
            fd << tab.tab() << startStr(sItalic);
            fd << BoolToStr(true);
            fd << endStr(sItalic) << std::endl;
        }

        // Property: Underlined
        if (wstrToBool(symbol->GetUnderlined().c_str()))
        {
            fd << tab.tab() << startStr(sUnderlined);
            fd << BoolToStr(true);
            fd << endStr(sUnderlined) << std::endl;
        }

        // Property: AdvancedPlacement
        fd << tab.tab() << startStr(sAdvancedPlacement) << std::endl;
        tab.inctab();
        fd << tab.tab() << startStr(sScaleLimit);
        fd << DoubleToStr(symbol->GetScaleLimit());
        fd << endStr(sScaleLimit) << std::endl;
        tab.dectab();
        fd << tab.tab() << endStr(sAdvancedPlacement) << std::endl;

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, label->GetUnknownXml(), version, tab);

        tab.dectab();
        fd << tab.tab() << endStr(sLabel) << std::endl;
    }
}

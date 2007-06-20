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
#include "IOLabel.h"
#include "IOSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Label);
ELEM_MAP_ENTRY(2, AdvancedPlacement);
ELEM_MAP_ENTRY(3, ScaleLimit);
ELEM_MAP_ENTRY(4, Text);
ELEM_MAP_ENTRY(5, FontName);
ELEM_MAP_ENTRY(6, ForegroundColor);
ELEM_MAP_ENTRY(7, BackgroundColor);
ELEM_MAP_ENTRY(8, BackgroundStyle);
ELEM_MAP_ENTRY(9, HorizontalAlignment);
ELEM_MAP_ENTRY(10, VerticalAlignment);
ELEM_MAP_ENTRY(11, Bold);
ELEM_MAP_ENTRY(12, Italic);
ELEM_MAP_ENTRY(13, Underlined);
ELEM_MAP_ENTRY(14, Unit);
ELEM_MAP_ENTRY(15, SizeContext);
ELEM_MAP_ENTRY(16, SizeX);
ELEM_MAP_ENTRY(17, SizeY);
ELEM_MAP_ENTRY(18, InsertionPointX);
ELEM_MAP_ENTRY(19, InsertionPointY);
ELEM_MAP_ENTRY(20, Rotation);
ELEM_MAP_ENTRY(21, MaintainAspect);


IOLabel::IOLabel()
{
    this->_label = NULL;
    this->rule = NULL;
    this->m_handlingPlacement = false;
}


IOLabel::IOLabel(Rule* rule)
{
    this->_label = NULL;
    this->rule = rule;
    this->m_handlingPlacement = false;
}


IOLabel::~IOLabel()
{
}


void IOLabel::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eLabel:
        m_startElemName = name;
        this->_label = new Label();
        break;

    case eAdvancedPlacement:
        if (this->_label)
        {
            this->_label->GetSymbol()->SetAdvancedPlacement(true);
            this->m_handlingPlacement = true;
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOLabel::ElementChars(const wchar_t* ch)
{
    if (this->_label->GetSymbol() == NULL)
        this->_label->AdoptSymbol(new TextSymbol());
    TextSymbol* symbol = this->_label->GetSymbol();

    if (this->m_handlingPlacement && m_currElemName == L"ScaleLimit") // NOXLATE
    {
        symbol->SetScaleLimit(wstrToDouble(ch));
        // we wind up here twice, the second time with a garbage ch string - avoid using it.
        this->m_handlingPlacement = false;
    }
    else if (m_currElemName == L"Text") // NOXLATE
        symbol->SetText(ch);
    else if (m_currElemName == L"FontName") // NOXLATE
        symbol->SetFontName(ch);
    else if (m_currElemName == L"ForegroundColor") // NOXLATE
        symbol->SetForegroundColor(ch);
    else if (m_currElemName == L"BackgroundColor") // NOXLATE
        symbol->SetBackgroundColor(ch);
    else if (m_currElemName == L"BackgroundStyle") // NOXLATE
    {
        if (::wcscmp(ch, L"Transparent") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Transparent);
        else if (::wcscmp(ch, L"Opaque") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Opaque);
        else if (::wcscmp(ch, L"Ghosted") == 0) // NOXLATE
            symbol->SetBackgroundStyle(TextSymbol::Ghosted);
    }
    else if (m_currElemName == L"HorizontalAlignment") // NOXLATE
        symbol->SetHorizontalAlignment(ch);
    else if (m_currElemName == L"VerticalAlignment") // NOXLATE
        symbol->SetVerticalAlignment(ch);
    else if (m_currElemName == L"Bold") // NOXLATE
        symbol->SetBold(ch);
    else if (m_currElemName == L"Italic") // NOXLATE
        symbol->SetItalic(ch);
    else if (m_currElemName == L"Underlined") // NOXLATE
        symbol->SetUnderlined(ch);

    else if (m_currElemName == L"Unit") // NOXLATE
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        symbol->SetUnit(unit);
    }
    else if (m_currElemName == L"SizeContext") // NOXLATE
    {
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            symbol->SetSizeContext(MdfModel::MappingUnits);
        else if (::wcscmp(ch, L"DeviceUnits") == 0) // NOXLATE
            symbol->SetSizeContext(MdfModel::DeviceUnits);
    }
    else if (m_currElemName == L"SizeX") // NOXLATE
        symbol->SetSizeX(ch);
    else if (m_currElemName == L"SizeY") // NOXLATE
        symbol->SetSizeY(ch);
    else if (m_currElemName == L"InsertionPointX") // NOXLATE
        symbol->SetInsertionPointX(ch);
    else if (m_currElemName == L"InsertionPointY") // NOXLATE
        symbol->SetInsertionPointY(ch);
    else if (m_currElemName == L"Rotation") // NOXLATE
        symbol->SetRotation(ch);
    else if (m_currElemName == L"MaintainAspect") // NOXLATE
        symbol->SetMaintainAspect(wstrToBool(ch));
}


void IOLabel::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_label->SetUnknownXml(UnknownXml());

        this->rule->AdoptLabel(this->_label);
        handlerStack->pop();
        this->rule = NULL;
        this->_label = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOLabel::Write(MdfStream& fd, Label* label, Version* version)
{
    TextSymbol* symbol = label->GetSymbol();

    if (symbol != NULL)
    {
        fd << tab() << "<Label>" << std::endl; // NOXLATE
        inctab();

        IOSymbol::Write(fd, symbol, version);

        //Property: Text
        fd << tab() << "<Text>"; // NOXLATE
        fd << EncodeString(symbol->GetText());
        fd << "</Text>" << std::endl; // NOXLATE

        //Property: FontName
        fd << tab() << "<FontName>"; // NOXLATE
        fd << EncodeString(symbol->GetFontName());
        fd << "</FontName>" << std::endl; // NOXLATE

        //Property: ForegroundColor
        fd << tab() << "<ForegroundColor>"; // NOXLATE
        fd << EncodeString(symbol->GetForegroundColor());
        fd << "</ForegroundColor>" << std::endl; // NOXLATE

        //Property: BackgroundColor
        fd << tab() << "<BackgroundColor>"; // NOXLATE
        fd << EncodeString(symbol->GetBackgroundColor());
        fd << "</BackgroundColor>" << std::endl; // NOXLATE

        //Property: BackgroundStyle
        fd << tab() << "<BackgroundStyle>"; // NOXLATE
        if (symbol->GetBackgroundStyle() == TextSymbol::Transparent)
            fd << "Transparent"; // NOXLATE
        else if (symbol->GetBackgroundStyle() == TextSymbol::Opaque)
            fd << "Opaque"; // NOXLATE
        else
            fd << "Ghosted"; // NOXLATE
        fd << "</BackgroundStyle>" << std::endl; // NOXLATE

        //Property: HorizontalAlignment
        if (symbol->GetHorizontalAlignment().c_str() != L"\'Center\'") // NOXLATE
        {
            fd << tab() << "<HorizontalAlignment>"; // NOXLATE
            fd << EncodeString(symbol->GetHorizontalAlignment());
            fd << "</HorizontalAlignment>" << std::endl; // NOXLATE
        }

        //Property: VerticalAlignment
        if (symbol->GetVerticalAlignment().c_str() != L"\'Baseline\'") // NOXLATE
        {
            fd << tab() << "<VerticalAlignment>"; // NOXLATE
            fd << EncodeString(symbol->GetVerticalAlignment());
            fd << "</VerticalAlignment>" << std::endl; // NOXLATE
        }

        //Property: Bold
        if (wstrToBool(symbol->GetBold().c_str()))
        {
            fd << tab() << "<Bold>"; // NOXLATE
            fd << "true";
            fd << "</Bold>" << std::endl; // NOXLATE
        }

        //Property: Italic
        if (wstrToBool(symbol->GetItalic().c_str()))
        {
            fd << tab() << "<Italic>"; // NOXLATE
            fd << "true";
            fd << "</Italic>" << std::endl; // NOXLATE
        }

        //Property: Underlined
        if (wstrToBool(symbol->GetUnderlined().c_str()))
        {
            fd << tab() << "<Underlined>"; // NOXLATE
            fd << "true";
            fd << "</Underlined>" << std::endl; // NOXLATE
        }

        //Property: AdvancePlacement
        if (symbol->IsAdvancedPlacement())
        {
            fd << tab() << "<AdvancedPlacement>" << std::endl; // NOXLATE
            if (symbol->GetScaleLimit() != 1.0)
            {
                inctab();
                fd << tab() << "<ScaleLimit>"; // NOXLATE
                fd << DoubleToStr(symbol->GetScaleLimit());
                fd << "</ScaleLimit>" << std::endl; // NOXLATE
                dectab();
            }
            fd << tab() << "</AdvancedPlacement>" << std::endl; // NOXLATE
        }

        // Write any unknown XML / extended data
        IOUnknown::Write(fd, label->GetUnknownXml(), version);

        dectab();
        fd << tab() << "</Label>" << std::endl; // NOXLATE
    }
}

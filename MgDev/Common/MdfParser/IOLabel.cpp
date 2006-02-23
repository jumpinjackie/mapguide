//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOLabel::IOLabel()
{
    this->m_handlingPlacement = false;
}

IOLabel::IOLabel(Rule * rule)
{
    this->rule = rule;
    this->m_handlingPlacement = false;
}

IOLabel::~IOLabel()
{
}

void IOLabel::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"Label") // NOXLATE
    {
        m_startElemName = name;
        this->_label = new Label();
    }
    else if (m_currElemName == L"AdvancedPlacement" && this->_label) // NOXLATE
    {
        this->_label->GetSymbol()->SetAdvancedPlacement(true);
        this->m_handlingPlacement = true;
    }
}

void IOLabel::ElementChars(const wchar_t *ch)
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
        (symbol)->SetHorizontalAlignment(ch);
    else if (m_currElemName == L"VerticalAlignment") // NOXLATE
        (symbol)->SetVerticalAlignment(ch);
    else if (m_currElemName == L"Bold") // NOXLATE
        (symbol)->SetBold(ch);
    else if (m_currElemName == L"Italic") // NOXLATE
        (symbol)->SetItalic(ch);
    else if (m_currElemName == L"Underlined") // NOXLATE
        (symbol)->SetUnderlined(ch);

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
        (symbol)->SetSizeX(ch);
    else if (m_currElemName == L"SizeY") // NOXLATE
        (symbol)->SetSizeY(ch);
    else if (m_currElemName == L"InsertionPointX") // NOXLATE
        (symbol)->SetInsertionPointX(ch);
    else if (m_currElemName == L"InsertionPointY") // NOXLATE
        (symbol)->SetInsertionPointY(ch);
    else if (m_currElemName == L"Rotation") // NOXLATE
        (symbol)->SetRotation(ch);
    else if (m_currElemName == L"MaintainAspect") // NOXLATE
        (symbol)->SetMaintainAspect(wstrToBool(ch));
}

void IOLabel::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->rule->AdoptLabel(this->_label);
        handlerStack->pop();
        this->rule= NULL;
        this->_label = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOLabel::Write(MdfStream &fd, Label *label)
{
    TextSymbol* symbol = label->GetSymbol();

    if (symbol != NULL)
    {
        fd << tab() << "<Label>" << std::endl; // NOXLATE
        inctab();

        IOSymbol::Write(fd, symbol);

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

        dectab();
        fd << tab() << "</Label>" << std::endl; // NOXLATE
    }
}

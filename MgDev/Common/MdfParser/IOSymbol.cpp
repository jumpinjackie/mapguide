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
#include "IOSymbol.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOSymbol::IOSymbol(Version& version) : SAX2ElementHandler(version)
{
    this->m_symbol = NULL;
}


void IOSymbol::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Unit") // NOXLATE
    {
        LengthUnit unit = LengthConverter::EnglishToUnit(ch);
        this->m_symbol->SetUnit(unit);
    }
    else if (this->m_currElemName == L"SizeContext") // NOXLATE
    {
        if (::wcscmp(ch, L"MappingUnits") == 0) // NOXLATE
            this->m_symbol->SetSizeContext(MdfModel::MappingUnits);
        else if (::wcscmp(ch, L"DeviceUnits") == 0) // NOXLATE
            this->m_symbol->SetSizeContext(MdfModel::DeviceUnits);
    }
    else if (this->m_currElemName == L"SizeX") // NOXLATE
        this->m_symbol->SetSizeX(ch);
    else if (this->m_currElemName == L"SizeY") // NOXLATE
        this->m_symbol->SetSizeY(ch);
    else if (this->m_currElemName == L"Rotation") // NOXLATE
        this->m_symbol->SetRotation(ch);
    else if (this->m_currElemName == L"MaintainAspect") // NOXLATE
        this->m_symbol->SetMaintainAspect(wstrToBool(ch));
    else if (this->m_currElemName == L"InsertionPointX") // NOXLATE
        this->m_symbol->SetInsertionPointX(ch);
    else if (this->m_currElemName == L"InsertionPointY") // NOXLATE
        this->m_symbol->SetInsertionPointY(ch);
}


Symbol* IOSymbol::GetSymbol()
{
    return this->m_symbol;
}


void IOSymbol::Write(MdfStream& fd, Symbol* symbol, Version* version, MgTab& tab)
{
    // Property: Unit
    fd << tab.tab() << "<Unit>"; // NOXLATE
    std::auto_ptr<MdfString> str(LengthConverter::UnitToEnglish(symbol->GetUnit()));
    fd << EncodeString(*str);
    fd << "</Unit>" << std::endl; // NOXLATE

    // Property: SizeContext
    fd << tab.tab() << "<SizeContext>"; // NOXLATE
    if (symbol->GetSizeContext() == MdfModel::MappingUnits)
        fd << "MappingUnits"; // NOXLATE
    else
        fd << "DeviceUnits"; // NOXLATE
    fd << "</SizeContext>" << std::endl; // NOXLATE

    // Property: SizeX
    fd << tab.tab() << "<SizeX>"; // NOXLATE
    fd << EncodeString(symbol->GetSizeX());
    fd << "</SizeX>" << std::endl; // NOXLATE

    // Property: SizeY
    fd << tab.tab() << "<SizeY>"; // NOXLATE
    fd << EncodeString(symbol->GetSizeY());
    fd << "</SizeY>" << std::endl; // NOXLATE

    // Property: Rotation (optional)
    fd << tab.tab() << "<Rotation>"; // NOXLATE
    fd << EncodeString(symbol->GetRotation());
    fd << "</Rotation>" << std::endl; // NOXLATE

    // Property: MaintainAspect (optional)
    if (symbol->GetMaintainAspect() == false)
    {
        fd << tab.tab() << "<MaintainAspect>"; // NOXLATE
        fd << BoolToStr(false);
        fd << "</MaintainAspect>" << std::endl; // NOXLATE
    }

    // Property: InsertionPointX (optional)
    if (wstrToDouble(symbol->GetInsertionPointX().c_str()) != 0.5)
    {
        fd << tab.tab() << "<InsertionPointX>"; // NOXLATE
        fd << EncodeString(symbol->GetInsertionPointX());
        fd << "</InsertionPointX>" << std::endl; // NOXLATE
    }

    // Property: InsertionPointY (optional)
    if (wstrToDouble(symbol->GetInsertionPointY().c_str()) != 0.5)
    {
        fd << tab.tab() << "<InsertionPointY>"; // NOXLATE
        fd << EncodeString(symbol->GetInsertionPointY());
        fd << "</InsertionPointY>" << std::endl; // NOXLATE
    }
}

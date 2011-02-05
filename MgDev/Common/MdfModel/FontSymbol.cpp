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

//-------------------------------------------------------------------------
// DESCRIPTION:
// FontSymbol class implementation. FontSymbol class is a concrete
// subclass of class FlatSymbol.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "FontSymbol.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the FontSymbol class.
//-------------------------------------------------------------------------
FontSymbol::FontSymbol() : m_character(0)
{
    // default values
    this->m_strFontName = L"Arial"; // NOXLATE
    this->m_strForeColor = L"ff000000"; // NOXLATE
    this->m_strBold = L"false"; // NOXLATE
    this->m_strItalic = L"false"; // NOXLATE
    this->m_strUnderlined = L"false"; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
FontSymbol::~FontSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FontName property in this FontSymbol.
//          The property represents the name of font to be used to display
//          the text.
// RETURNS: Font name.
//-------------------------------------------------------------------------
const MdfString& FontSymbol::GetFontName() const
{
    return this->m_strFontName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FontName property in this FontSymbol.
//          The property represents the name of font to be used to display
//          the text.
// PARAMETERS:
//      Input:
//          strFontName - Font name.
//-------------------------------------------------------------------------
void FontSymbol::SetFontName(const MdfString& strFontName)
{
    this->m_strFontName = strFontName;
}

//-------------------------------------------------------------------------
// PURPOSE: Access the character that is to be displayed in the given font.
// RETURNS: An unsigned int from 0 to 255
//-------------------------------------------------------------------------
const wchar_t FontSymbol::GetCharacter() const
{
    return this->m_character;
}

//-------------------------------------------------------------------------
// PURPOSE: Set the character that is to be displayed in the given font.
// PARAMETERS:
//      Input:
//       character - an unsigned int from 0 to 255.
//-------------------------------------------------------------------------
void FontSymbol::SetCharacter(const wchar_t character)
{
    this->m_character = character;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ForegroundColor property in this HatchFill.
//          ForegroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Hatch pattern.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& FontSymbol::GetForegroundColor() const
{
    return this->m_strForeColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the ForegroundColor property in this HatchFill.
//          ForegroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Hatch pattern. This method does not test
//          the argument to be a valid expression.
// PARAMETERS:
//      Input:
//         strForegroundColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void FontSymbol::SetForegroundColor(const MdfString& strForegroundColor)
{
    this->m_strForeColor = strForegroundColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Bold property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed bold.
// RETURNS: String representation of the Bold Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& FontSymbol::GetBold() const
{
    return this->m_strBold;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Bold property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed bold.
// PARAMETERS:
//      Input:
//      strBoldExpr - String representation of the Bold
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void FontSymbol::SetBold(const MdfString& strBoldExpr)
{
    this->m_strBold = strBoldExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Italic property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Italic.
// RETURNS: String representation of the Italic Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& FontSymbol::GetItalic() const
{
    return this->m_strItalic;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Italic property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Italic.
// PARAMETERS:
//      Input:
//      strItalicExpr - String representation of the Italic
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void FontSymbol::SetItalic(const MdfString& strItalicExpr)
{
    this->m_strItalic = strItalicExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Underlined property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Underlined.
// RETURNS: String representation of the Underlined Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& FontSymbol::GetUnderlined() const
{
    return this->m_strUnderlined;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Underlined property in this FontSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Underlined.
// PARAMETERS:
//      Input:
//      strUnderlinedExpr - String representation of the Underlined
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void FontSymbol::SetUnderlined(const MdfString& strUnderlinedExpr)
{
    this->m_strUnderlined = strUnderlinedExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                        that accept the final concrete type this FontSymbol
//                        represents as an argument.
//-------------------------------------------------------------------------
void FontSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitFontSymbol(*this);
}

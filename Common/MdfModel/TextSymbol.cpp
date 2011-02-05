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
// TextSymbol class implementation. TextSymbol class is a concrete
// subclass of class FlatSymbol.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "TextSymbol.h"
#include "Fill.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the TextSymbol class.
//-------------------------------------------------------------------------
TextSymbol::TextSymbol()
{
    // default values
//  this->m_strText = L"";
    this->m_strFontName = L"Arial"; // NOXLATE
    this->m_strForeColor = L"ff000000"; // NOXLATE
    this->m_strBackColor = L"ffffffff"; // NOXLATE
    this->m_eBackgroundStyle = TextSymbol::Ghosted;
    this->m_strHrzAlignment = L"'Center'"; // NOXLATE
    this->m_strVrtAlignment = L"'Baseline'"; // NOXLATE
    this->m_strBold = L"false"; // NOXLATE
    this->m_strItalic = L"false"; // NOXLATE
    this->m_strUnderlined = L"false"; // NOXLATE
#ifdef MG_ZERO_SCALELIMIT
    this->m_dScaleLimit = 0.0;
#else
    this->m_dScaleLimit = 0.5;
#endif
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
TextSymbol::~TextSymbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Text property in this TextSymbol.
//          The Text property is of type Expression:Text, which is an
//          expression that evaluates to a string for a given feature.
//          It represents the text to display in this symbol.
// RETURNS: String representation of the Text Expression:Text.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetText() const
{
    return this->m_strText;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Text property in this TextSymbol.
//          The Text property is of type Expression:Text, which is an
//          expression that evaluates to a string for a given feature.
//          It represents the text to display in this symbol.
// PARAMETERS:
//      Input:
//       strTextExpr - String representation of the Text Expression:Text.
//-------------------------------------------------------------------------
void TextSymbol::SetText(const MdfString& strTextExpr)
{
    this->m_strText = strTextExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the FontName property in this TextSymbol.
//          The property represents the name of font to be used to display
//          the text.
// RETURNS: Font name.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetFontName() const
{
    return this->m_strFontName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the FontName property in this TextSymbol.
//          The property represents the name of font to be used to display
//          the text.
// PARAMETERS:
//      Input:
//          strFontName - Font name.
//-------------------------------------------------------------------------
void TextSymbol::SetFontName(const MdfString& strFontName)
{
    this->m_strFontName = strFontName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ForegroundColor property in this HatchFill.
//          ForegroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Hatch pattern.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetForegroundColor() const
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
void TextSymbol::SetForegroundColor(const MdfString& strForegroundColor)
{
    this->m_strForeColor = strForegroundColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BackgroundColor property in this HatchFill.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          is applied to the background before the Hatch pattern is applied.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetBackgroundColor() const
{
    return this->m_strBackColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the BackgroundColor property in this HatchFill.
//          BackgroundColor is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          is applied to the background before the Hatch pattern is applied.
//          This method does not test for the argument being a valid expression.
// PARAMETERS:
//      Input:
//          strBackgroundColor - The string representation of the
//                               Expression:Color.
//-------------------------------------------------------------------------
void TextSymbol::SetBackgroundColor(const MdfString& strBackgroundColor)
{
    this->m_strBackColor = strBackgroundColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the BackgroundStyle property in this
//          TextSymbol. The property holds a BackgroundStyle enum that
//          specifies the display option for the text background
//          of the Symbol. It may be Ghosted, Opaque or Transparent.
// RETURNS: BackgroundStyle enum value. Default is Opaque.
//-------------------------------------------------------------------------
TextSymbol::BackgroundStyle TextSymbol::GetBackgroundStyle() const
{
    return this->m_eBackgroundStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the BackgroundStyle property in this
//          TextSymbol. The property holds a BackgroundStyle enum that
//          specifies the display option for the text background
//          of the Symbol. It may be Ghosted, Opaque or Transparent.
// PARAMETERS:
//      Input:
//         eBackgroundStyle - BackgroundStyle enum value. Default is Opaque.
//-------------------------------------------------------------------------
void TextSymbol::SetBackgroundStyle(TextSymbol::BackgroundStyle eBackgroundStyle)
{
    this->m_eBackgroundStyle = eBackgroundStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalAlignment property in this
//          TextSymbol. HorizontalAlignment is of type Expression:String,
//          which is an expression that evaluates to the text representation
//          of a member of the enum HorizontalAlignment.
// RETURNS: The string representation of the Expression:String. Default is
//          "Center".
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetHorizontalAlignment() const
{
    return this->m_strHrzAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalAlignment property in this
//          TextSymbol. HorizontalAlignment is of type Expression:String,
//          which is an expression that evaluates to the text representation
//          of a member of the enum HorizontalAlignment.
// PARAMETERS:
//      Input:
//          strHrzAlignment - The string representation of the
//                              Expression:String.
//-------------------------------------------------------------------------
void TextSymbol::SetHorizontalAlignment(const MdfString& strHrzAlignment)
{
    this->m_strHrzAlignment = strHrzAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalAlignment property in this
//          TextSymbol. VerticalAlignment is of type Expression:String,
//          which is an expression that evaluates to the text representation
//          of a member of the enum VerticalAlignment.
// RETURNS: The string representation of the Expression:String. Default is
//          "Baseline".
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetVerticalAlignment() const
{
    return this->m_strVrtAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalAlignment property in this
//          TextSymbol. VerticalAlignment is of type Expression:String,
//          which is an expression that evaluates to the text representation
//          of a member of the enum VerticalAlignment.
// PARAMETERS:
//          strHrzAlignment - The string representation of the
//                              Expression:String.
//-------------------------------------------------------------------------
void TextSymbol::SetVerticalAlignment(const MdfString& strVrtAlignment)
{
    this->m_strVrtAlignment = strVrtAlignment;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Bold property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed bold.
// RETURNS: String representation of the Bold Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetBold() const
{
    return this->m_strBold;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Bold property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed bold.
// PARAMETERS:
//      Input:
//      strBoldExpr - String representation of the Bold
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void TextSymbol::SetBold(const MdfString& strBoldExpr)
{
    this->m_strBold = strBoldExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Italic property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Italic.
// RETURNS: String representation of the Italic Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetItalic() const
{
    return this->m_strItalic;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Italic property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Italic.
// PARAMETERS:
//      Input:
//      strItalicExpr - String representation of the Italic
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void TextSymbol::SetItalic(const MdfString& strItalicExpr)
{
    this->m_strItalic = strItalicExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Underlined property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Underlined.
// RETURNS: String representation of the Underlined Expression:Boolean.
//-------------------------------------------------------------------------
const MdfString& TextSymbol::GetUnderlined() const
{
    return this->m_strUnderlined;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Underlined property in this TextSymbol.
//          The property is of type Expression:Boolean, which is an
//          expression that evaluates to a bool for a given feature.
//          It defines if the text is displayed Underlined.
// PARAMETERS:
//      Input:
//      strUnderlinedExpr - String representation of the Underlined
//                          Expression:Boolean.
//-------------------------------------------------------------------------
void TextSymbol::SetUnderlined(const MdfString& strUnderlinedExpr)
{
    this->m_strUnderlined = strUnderlinedExpr;
}

//-------------------------------------------------------------------------
// Returns true if this label is to be drawn using the advanced placement
// algorithm, false if the label is to be placed in the traditional way.
// Advanced placement makes labels follow the path of line features, only
// draws one label for a set of contiguous line features, and shrinks the
// label font height to fit the length of a feature.
//-------------------------------------------------------------------------
bool TextSymbol::IsAdvancedPlacement() const
{
    return this->m_dScaleLimit > 0.0;
}

//-------------------------------------------------------------------------
// Returns the smallest scale factor that can be used for this label
// when advanced label placement is in effect.  A value of 1.0 means that
// no downsizing is allowed.  Valid values range from > 0 to 1.0.
// If set to <= 0.0 then the advanced placement option is disabled.
//-------------------------------------------------------------------------
double TextSymbol::GetScaleLimit() const
{
    return this->m_dScaleLimit;
}

//-------------------------------------------------------------------------
// Sets the smallest scale factor that can be used for this label
// when advanced label placement is in effect.  A value of 1.0 means that
// no downsizing is allowed.  Valid values range from > 0 to 1.0.
// If set to <= 0.0 then the advanced placement option is disabled.
//-------------------------------------------------------------------------
void TextSymbol::SetScaleLimit(const double& dScaleLimit)
{
    this->m_dScaleLimit = dScaleLimit;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                        that accept the final concrete type this TextSymbol
//                        represents as an argument.
//-------------------------------------------------------------------------
void TextSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitTextSymbol(*this);
}

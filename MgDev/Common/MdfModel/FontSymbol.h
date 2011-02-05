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

#ifndef FONTSYMBOL_H_
#define FONTSYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The FontSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D.  In addition, to displaying alphanumeric
    // strings as labels, this class can be used to create a symbol from a single
    // character of a symbol font.
    //-------------------------------------------------------------------------
    class MDFMODEL_API FontSymbol : public Symbol
    {
    public:
        // Construction, destruction, initialization.
        FontSymbol();
        virtual ~FontSymbol();

        // Operations
        // Property : FontName
        const MdfString& GetFontName() const;
        void SetFontName(const MdfString& strFontName);

        // Property : Character
        const wchar_t GetCharacter() const;
        void SetCharacter(const wchar_t character);

        // Property : ForegroundColor
        const MdfString& GetForegroundColor() const;
        void SetForegroundColor(const MdfString& strForegroundColor);

        // Property : Bold  Type : Expression:Boolean
        const MdfString& GetBold() const;
        void SetBold(const MdfString& strBoldExpr);

        // Property : Italic  Type : Expression:Boolean
        const MdfString& GetItalic() const;
        void SetItalic(const MdfString& strItalicExpr);

        // Property : Underline  Type : Expression:Boolean
        const MdfString& GetUnderlined() const;
        void SetUnderlined(const MdfString& strUnderlinedExpr);

         // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        FontSymbol(const FontSymbol&);
        FontSymbol& operator=(const FontSymbol&);

        // Data members
        // The font to be used.
        MdfString m_strFontName;

        // The character to display in the specified font
        wchar_t m_character;

        // The Expression:Color string representation for the foreground color.
        MdfString m_strForeColor;

        // The Expression:Boolean string representation for Bold style.
        MdfString m_strBold;

        // The Expression:Boolean string representation for Italic style.
        MdfString m_strItalic;

        // The Expression:Boolean string representation for Underlined style.
        MdfString m_strUnderlined;
    };

END_NAMESPACE_MDFMODEL
#endif // FONTSYMBOL_H_

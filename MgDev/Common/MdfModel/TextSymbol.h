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

#ifndef TEXTSYMBOL_H_
#define TEXTSYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The TextSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D.  In addition, to displaying alphanumeric
    // strings as labels, this class can be used to create a symbol from a single
    // character of a symbol font.
    //-------------------------------------------------------------------------
    class MDFMODEL_API TextSymbol : public Symbol
    {
    public:

        // Defines the background of the Text.
        enum BackgroundStyle
        {
            Transparent,
            Opaque,
            Ghosted
        };

        // Defines the horizontal orientation of the text.
        enum HorizontalAlignment
        {
            Left,
            Center,
            Right
        };

        // Defines the vertical orientation of the text.
        enum VerticalAlignment
        {
            Baseline,
            Bottom,
            Capline,
            Halfline,
            Top
        };

    public:
        // Construction, destruction, initialization.
        TextSymbol();
        virtual ~TextSymbol();

        // Operations
        // Property : Text  Type : Expression:Text
        const MdfString& GetText() const;
        void SetText(const MdfString& strTextExpr);

        // Property : FontName
        const MdfString& GetFontName() const;
        void SetFontName(const MdfString& strFontName);

        // Property : ForegroundColor
        const MdfString& GetForegroundColor() const;
        void SetForegroundColor(const MdfString& strForegroundColor);

        // Property : BackgroundColor
        const MdfString& GetBackgroundColor() const;
        void SetBackgroundColor(const MdfString& strBackgroundColor);

        // Property : BackgroundStyle
        BackgroundStyle GetBackgroundStyle() const;
        void SetBackgroundStyle(BackgroundStyle eBackgroundStyle);

        // Property : HorizontalAlignment
        const MdfString& GetHorizontalAlignment() const;
        void SetHorizontalAlignment(const MdfString& strHrzAlignment);

        // Property : VerticalAlignment
        const MdfString& GetVerticalAlignment() const;
        void SetVerticalAlignment(const MdfString& strVrtAlignment);

        // Property : Bold  Type : Expression:Boolean
        const MdfString& GetBold() const;
        void SetBold(const MdfString& strBoldExpr);

        // Property : Italic  Type : Expression:Boolean
        const MdfString& GetItalic() const;
        void SetItalic(const MdfString& strItalicExpr);

        // Property : Underlined  Type : Expression:Boolean
        const MdfString& GetUnderlined() const;
        void SetUnderlined(const MdfString& strUnderlinedExpr);

        // Property : AdvancedPlacement
        // Determines if label is placed using the advanced algorithm or not.
        bool IsAdvancedPlacement() const;

        // Property : ScaleLimit
        double GetScaleLimit() const;
        void SetScaleLimit(const double& dScaleLimit);

         // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        TextSymbol(const TextSymbol&);
        TextSymbol& operator=(const TextSymbol&);

        // Data members
        // The string representation of the Text Expression:Text.
        MdfString m_strText;

        // The font name to be used.
        MdfString m_strFontName;

        // The Expression:Color string representation for the foreground color.
        MdfString m_strForeColor;

        // The Expression:Color string representation for the background color.
        MdfString m_strBackColor;

        // The enum to represent the text Background diplay option.
        BackgroundStyle m_eBackgroundStyle;

        // The Expression:enum string representation for the horizontal orientation.
        MdfString m_strHrzAlignment;

        // The Expression:enum string representation for the vertical orientation.
        MdfString m_strVrtAlignment;

        // The Expression:Boolean string representation for Bold style.
        MdfString m_strBold;

        // The Expression:Boolean string representation for Italic style.
        MdfString m_strItalic;

        // The Expression:Boolean string representation for Underlined style.
        MdfString m_strUnderlined;

        // The scale limit used with the advanced placement option.
        double m_dScaleLimit;
    };

END_NAMESPACE_MDFMODEL
#endif // TEXTSYMBOL_H_

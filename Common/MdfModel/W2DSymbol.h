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

#ifndef W2DSYMBOL_H_
#define W2DSYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The W2DSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D. It uses Mark shapes, which are 2D
    // polygons, along with specifications for coloring both the edges and the
    // inside of the shape for creating the symbol.
    //-------------------------------------------------------------------------
    class MDFMODEL_API W2DSymbol : public Symbol
    {
    public:
        // Construction, destruction, initialization.
        W2DSymbol(const MdfString& strSymbolLibrary, const MdfString& strSymbolName);
        virtual ~W2DSymbol();

        // Operations
        // Property : SymbolLibrary
        const MdfString& GetSymbolLibrary() const;
        void SetSymbolLibrary(const MdfString& strSymbolLibrary);

        // Property : SymbolName
        const MdfString& GetSymbolName() const;
        void SetSymbolName(const MdfString& strSymbolName);

        // Property : FillColor
        const MdfString& GetFillColor() const;
        void SetFillColor(const MdfString& strFillColor);

        // Property : LineColor
        const MdfString& GetLineColor() const;
        void SetLineColor(const MdfString& strLineColor);

        // Property : TextColor
        const MdfString& GetTextColor() const;
        void SetTextColor(const MdfString& strTextColor);

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        W2DSymbol(const W2DSymbol&);
        W2DSymbol& operator=(const W2DSymbol&);

        // Data members
        // The SymbolLibrary that is used by this W2DSymbol.
        MdfString m_strSymbolLibrary;

        // The SymbolName that is used by this W2DSymbol.
        MdfString m_strSymbolName;

        // The Expression:Color string representation for the fill color.
        MdfString m_strFillColor;

        // The Expression:Color string representation for the line color.
        MdfString m_strLineColor;

        // The Expression:Color string representation for the text color.
        MdfString m_strTextColor;
   };

END_NAMESPACE_MDFMODEL
#endif // W2DSYMBOL_H_

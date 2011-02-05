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

#ifndef BLOCKSYMBOL_H_
#define BLOCKSYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The BlockSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D. It uses Mark shapes, which are 2D
    // polygons, along with specifications for coloring both the edges and the
    // inside of the shape for creating the symbol.
    //-------------------------------------------------------------------------
    class MDFMODEL_API BlockSymbol : public Symbol
    {
    public:
        // Construction, destruction, initialization.
        BlockSymbol();
        virtual ~BlockSymbol();

        // Operations
        // Property : DrawingName
        const MdfString& GetDrawingName() const;
        void SetDrawingName(const MdfString& strDrawingName);

        // Property : BlockName
        const MdfString& GetBlockName() const;
        void SetBlockName(const MdfString& strBlockName);

        // Property : BlockColor
        const MdfString& GetBlockColor() const;
        void SetBlockColor(const MdfString& strBlockColor);

        // Property : LayerColor
        const MdfString& GetLayerColor() const;
        void SetLayerColor(const MdfString& strLayerColor);

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        BlockSymbol(const BlockSymbol&);
        BlockSymbol& operator=(const BlockSymbol&);

        // Data members
        // The SymbolLibrary that is used by this BlockSymbol.
        MdfString m_strDrawingName;

        // The SymbolName that is used by this BlockSymbol.
        MdfString m_strBlockName;

        // The Expression:Color string representation for the block color.
        MdfString m_strBlockColor;

        // The Expression:Color string representation for the layer color.
        MdfString m_strLayerColor;
   };

END_NAMESPACE_MDFMODEL
#endif // BLOCKSYMBOL_H_

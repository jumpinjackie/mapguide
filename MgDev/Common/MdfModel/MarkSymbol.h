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

#ifndef MARKSYMBOL_H_
#define MARKSYMBOL_H_

#include "MdfModel.h"
#include "Symbol.h"
#include "Fill.h"
#include "Stroke.h"

BEGIN_NAMESPACE_MDFMODEL

    // -------------------------------------------------------------------------
    // DESCRIPTION:
    // The MarkSymbol class is used to define a way to stylize features containing
    // point geometric primitives in 2D. It provides a set of 2D Shapes, and fill and
    // edge stylization options.
    //-------------------------------------------------------------------------
    class MDFMODEL_API MarkSymbol : public Symbol
    {
    public:
        // Defines the shape of a MarkSymbol.
        enum Shape
        {
            Square,
            Circle,
            Triangle,
            Star,
            Cross,
            X
        };

        // Construction, destruction, initialization.
        MarkSymbol(const Shape shape);
        virtual ~MarkSymbol();

        // Operations
        // Property : Shape
        const Shape GetShape() const;
        void SetShape(const Shape shape);

        // Property : Fill
        Fill* GetFill();
        void AdoptFill(Fill* pflAdoptedFill);
        Fill* OrphanFill();

        // Property : Edge
        Stroke* GetEdge();
        void AdoptEdge(Stroke* pstkAdoptedEdge);
        Stroke* OrphanEdge();

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        MarkSymbol(const MarkSymbol&);
        MarkSymbol& operator=(const MarkSymbol&);

        // Data members
        // defines the appearance of the filled center of the mark shape.
        Fill* m_pflFill;

        // defines the appearance of the edge of the mark shape.
        Stroke* m_pstkEdge;

        // The SymbolLibrary that is used by this MarkSymbol.
        Shape m_shape;
   };

END_NAMESPACE_MDFMODEL
#endif // MARKSYMBOL_H_

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

#ifndef AREASYMBOLIZATION2D_H_
#define AREASYMBOLIZATION2D_H_

#include "Symbolization.h"
#include "Fill.h"
#include "Stroke.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The AreaSymbolization2D defines a way to specify the stylization details
    // for features with polygon geometric primitives.
    //-------------------------------------------------------------------------
    class MDFMODEL_API AreaSymbolization2D : public Symbolization
    {
    public:
        // Construction, destruction, initialization
        AreaSymbolization2D();
        virtual ~AreaSymbolization2D();

        // Operations
        // Property : Fill
        Fill* GetFill();
        void AdoptFill(Fill* pflAdoptedFill);
        Fill* OrphanFill();

        // Property : Edge
        Stroke* GetEdge();
        void AdoptEdge(Stroke* pstkAdoptedEdge);
        Stroke* OrphanEdge();

        // Visitor Pattern method defined in Symbol.
        virtual void AcceptVisitor(ISymbolizationVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        AreaSymbolization2D(const AreaSymbolization2D&);
        AreaSymbolization2D& operator=(const AreaSymbolization2D&);

        // Data members
        // The Fill subclass that is applied by this AreaSymbolization2D
        // to polygon geometric primitives.
        Fill* m_pflFill;

        // The Stroke subclass that is applied by this AreaSymbolization2D
        // to polygon geometric primitives.
        Stroke* m_pstkEdge;
    };

END_NAMESPACE_MDFMODEL
#endif //AREASYMBOLIZATION2D_H_

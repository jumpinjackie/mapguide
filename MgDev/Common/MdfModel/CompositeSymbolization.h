//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#ifndef COMPOSITESYMBOLIZATION_H_
#define COMPOSITESYMBOLIZATION_H_

#include "MdfModel.h"
#include "Symbolization.h"
#include "SymbolInstance.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //-------------------------------------------------------------------------
    class MDFMODEL_API CompositeSymbolization : public Symbolization
    {
    public:
        // Construction, destruction, initialization
        CompositeSymbolization();
        virtual ~CompositeSymbolization();

        SymbolInstanceCollection* GetSymbolCollection();

        // Visitor Pattern method defined in Symbolization.
        virtual void AcceptVisitor(ISymbolizationVisitor& isymVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        CompositeSymbolization(const CompositeSymbolization&);
        CompositeSymbolization& operator=(CompositeSymbolization&);

        // Data members
        SymbolInstanceCollection m_collInstances;
    };

END_NAMESPACE_MDFMODEL
#endif // COMPOSITESYMBOLIZATION_H_

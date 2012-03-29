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

#ifndef COMPOSITERULE_H_
#define COMPOSITERULE_H_

#include "MdfModel.h"
#include "Rule.h"
#include "CompositeSymbolization.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // A composite Rule can be used for customized rendering of all 3 basic
    // types of geometry -- point, polylines, and polygons. It combines
    // a collection of basic Symbol building blocks.
    //-------------------------------------------------------------------------
    class MDFMODEL_API CompositeRule : public Rule
    {
    public:
        // Construction, destruction, initialization
        CompositeRule();
        virtual ~CompositeRule();

        // Operations
        // Property : Symbolization
        CompositeSymbolization* GetSymbolization();
        void AdoptSymbolization(CompositeSymbolization* compositeSymbolization);
        CompositeSymbolization* OrphanSymbolization();

    private:
        // Hidden copy constructor and assignment operator.
        CompositeRule(const CompositeRule&);
        CompositeRule& operator=(const CompositeRule&);

        // Data members
        CompositeSymbolization* m_pSymbolization;
    };

END_NAMESPACE_MDFMODEL
#endif // COMPOSITERULE_H_

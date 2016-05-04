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

#ifndef AREARULE_H_
#define AREARULE_H_

#include "Rule.h"
#include "AreaSymbolization2D.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // If this AreaRule's Filter evaluates to true for a given feature, its
    // AreaSymbolization2D is applied to that feature.
    //-------------------------------------------------------------------------
    class MDFMODEL_API AreaRule : public Rule
    {
    public:
        // Construction, destruction, initialization
        AreaRule();
        virtual ~AreaRule();

        // Operations
        // Property : Symbolization
        AreaSymbolization2D* GetSymbolization();
        void AdoptSymbolization(AreaSymbolization2D* pflAdoptedSymbolization);
        AreaSymbolization2D* OrphanSymbolization();

    private:
        // Hidden copy constructor and assignment operator.
        AreaRule(const AreaRule&);
        AreaRule& operator=(const AreaRule&);

        // Data members
        // this rule's area symbolization.
        AreaSymbolization2D* m_Symbolization;
    };

END_NAMESPACE_MDFMODEL
#endif // AREARULE_H_

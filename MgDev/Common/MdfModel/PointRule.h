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

#ifndef POINTRULE_H_
#define POINTRULE_H_

#include "MdfModel.h"
#include "Rule.h"
#include "PointSymbolization2D.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // PointRule is used by PointStyles to build up multivariate theming by
    // defining a PointSymbolization2D. If this PointRule's Filter evaluates to true
    // for a given feature, this PointSymbolization2D if applied to that feature.
    // Only those display attributes that are defined by this PointSymbolization2D
    // will be included in the feature's PointSymbolization2D. Any display attributes
    // that were assigned by previous PointRules will not be overwritten.
    //-------------------------------------------------------------------------
    class MDFMODEL_API PointRule : public Rule
    {
    public:
        // Construction, destruction, initialization
        PointRule();
        virtual ~PointRule();

        // Operations
        // Property : Symbolization
        PointSymbolization2D* GetSymbolization();
        void AdoptSymbolization(PointSymbolization2D* pflAdoptedSymbolization);
        PointSymbolization2D* OrphanSymbolization();

    private:
        // Hidden copy constructor and assignment operator.
        PointRule(const PointRule&);
        PointRule& operator=(const PointRule&);

        // Data members
        // this rule's point symbolization.
        PointSymbolization2D* m_Symbolization;
    };

END_NAMESPACE_MDFMODEL
#endif // POINTRULE_H_

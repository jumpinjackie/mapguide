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

#ifndef LINERULE_H_
#define LINERULE_H_

#include "MdfModel.h"
#include "Rule.h"
#include "LineSymbolization2D.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // LineRule is used by LineStyles to build up multivariate theming by
    // defining a LineSymbolization2D. If this LineRule's Filter evaluates to true
    // for a given feature, this LineSymbolization2D if applied to that feature. Only
    // those display attributes that are defined by this LineSymbolization2D will
    // be included in the feature's LineSymbolization2D. Any display attributes
    // that were assigned by previous LineRules will not be overwritten.
    //-------------------------------------------------------------------------
    class MDFMODEL_API LineRule : public Rule
    {
    public:
        // Construction, destruction, initialization
        LineRule();
        virtual ~LineRule();

        // Operations
        // Property : Symbolizations
        LineSymbolizationCollection* GetSymbolizations();

    private:
        // Hidden copy constructor and assignment operator.
        LineRule(const LineRule&);
        LineRule& operator=(const LineRule&);

        // Data members
        // Collection of line symbolizations.
        LineSymbolizationCollection m_collSymbolizations;
    };

END_NAMESPACE_MDFMODEL
#endif // LINERULE_H_

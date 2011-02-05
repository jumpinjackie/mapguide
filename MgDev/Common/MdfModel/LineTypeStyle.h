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

#ifndef LINETYPESTYLE_H_
#define LINETYPESTYLE_H_

#include "FeatureTypeStyle.h"
#include "LineRule.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The LineTypeStyle class is a FeatureTypeStyle for geometric line
    // primitives. It contains a collection of LineRules that are processed
    // in order. If the LineRule's Filter evaluates to True, the Rule can
    // contribute to the Symbolization that this LineTypeStyle defines.
    //-------------------------------------------------------------------------
    class MDFMODEL_API LineTypeStyle : public FeatureTypeStyle
    {
    public:
        // Construction, destruction, initialization
        LineTypeStyle();
        virtual ~LineTypeStyle();

        // Operations
        // Implementation of FeatureTypeStyle Visitor pattern method.
        virtual void AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor);

    private:
        // Hidden copy constructor and assignment operator.
        LineTypeStyle(const LineTypeStyle&);
        LineTypeStyle& operator=(const LineTypeStyle&);
    };

END_NAMESPACE_MDFMODEL
#endif // LINETYPESTYLE_H_

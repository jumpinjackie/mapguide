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

#ifndef POINTTYPESTYLE_H_
#define POINTTYPESTYLE_H_

#include "FeatureTypeStyle.h"
#include "PointRule.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The PointTypeStyle class is a FeatureTypeStyle for geometric point
    // primitives. It contains a collection of PointRules that are processed
    // in order. If the PointRule's Filter evaluates to True, the Rule can
    // contribute to the Symbolization that this PointTypeStyle defines.
    //-------------------------------------------------------------------------
    class MDFMODEL_API PointTypeStyle : public FeatureTypeStyle
    {
    public:
        // Construction, destruction, initialization.
        PointTypeStyle();
        virtual ~PointTypeStyle();

        // Operations
        // Implementation of FeatureTypeStyle Visitor pattern method.
        virtual void AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor);

        // Property : DisplayAsText
        bool IsDisplayAsText() const;
        void SetDisplayAsText(bool bDisplayAsText);

        // Property : AllowOverpost
        bool IsAllowOverpost() const;
        void SetAllowOverpost(bool bAllowOverpost);

    private:
        // Hidden copy constructor and assignment operator.
        PointTypeStyle(const PointTypeStyle&);
        PointTypeStyle& operator=(const PointTypeStyle&);

        // Data members
        // Determines if the features styled by this TypeStyle are displayed only as text labels.
        // Default is false.
        bool m_bDisplayAsText;

        // Determines if the features styled by this TypeStyle may be overposted.
        // Default is false.
        bool m_bAllowOverpost;
    };

END_NAMESPACE_MDFMODEL
#endif // POINTTYPESTYLE_H_

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

#ifndef FEATURETYPESTYLE_H_
#define FEATURETYPESTYLE_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "Rule.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    class IFeatureTypeStyleVisitor;

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // FeatureTypeStyle class is used by the ScaleRange to specify a symbolization
    // for a specific geometric primitive. The FeatureClassName property on this class
    // contains a path from the parent Style's FeatureClass down through any
    // intermediate FeatureClasses, to that geometric primitive type.
    // It is possible for FeatureTypeStyle's concrete classes to contain
    // only Labels.
    //-------------------------------------------------------------------------
    class MDFMODEL_API FeatureTypeStyle : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        FeatureTypeStyle();
        virtual ~FeatureTypeStyle();

        // Operations
        // Property : Rules
        RuleCollection* GetRules();

        // Visitor Pattern to be implemented by all subclasses
        virtual void AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor) = 0;

        // Property ShowInLegend
        bool IsShowInLegend() const;
        void SetShowInLegend(bool bShowInLegend);

    private:
        // Data members
        // Collection of Rules.
        RuleCollection m_collRules;

        // ShowInLegend flag
        bool m_showInLegend;
    };

    typedef MdfOwnerCollection<FeatureTypeStyle> FeatureTypeStyleCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<FeatureTypeStyle>;

END_NAMESPACE_MDFMODEL
#endif //FEATURETYPESTYLE_H_

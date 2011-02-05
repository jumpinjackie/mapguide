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

#ifndef RULE_H_
#define RULE_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "Label.h"

namespace MdfParser
{
    class IOAreaRule;
    class IOLineRule;
    class IOPointRule;
    class IOCompositeRule;
}

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // A Rule class is used by the FeatureTypeStyle subclasses to define
    // multivariate theming. A Rule's concrete subclasses apply their Symbolization
    // and Label to a feature when the Filter criteria is true for that feature.
    // Filtering criteria takes the form of a tree that uses logical operators
    // to combine attribute-based and geometry-based filters.
    //-------------------------------------------------------------------------
    class MDFMODEL_API Rule : public MdfRootObject
    {
        friend class MdfParser::IOAreaRule;
        friend class MdfParser::IOLineRule;
        friend class MdfParser::IOPointRule;
        friend class MdfParser::IOCompositeRule;

    public:
        // Destruction
        virtual ~Rule();

        // Operations
        // Property : LegendLabel
        const MdfString& GetLegendLabel() const;
        void SetLegendLabel(const MdfString& pstrLegendLabel);

        // Property : Filter
        const MdfString& GetFilter() const;
        void SetFilter(const MdfString& strFilter);

        // Property : Label
        Label* GetLabel();
        void AdoptLabel(Label* pLabel);
        Label* OrphanLabel();

    protected:
        // Construction, initialization
        // Default constructor is protected to make this class abstract.
        Rule();

    private:
        // Data members
        // Optional label for this theming element in the legend.
        MdfString m_strLegendLabel;

        // Optional bitmap to be used in the legend.
        MdfString m_strLegend;

        // Filter criteria.
        MdfString m_strFilter;

        // Label for multivariate theming.
        Label* m_pLabel;
    };

    typedef MdfOwnerCollection<Rule> RuleCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<Rule>;

END_NAMESPACE_MDFMODEL
#endif // RULE_H_

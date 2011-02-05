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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The Rule class implementation. Rule is an abstract class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Rule.h"
#include "Label.h"
#include "Base64.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the Rule class.
//-------------------------------------------------------------------------
Rule::Rule()
{
    this->m_pLabel = new Label();
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
Rule::~Rule()
{
    delete this->m_pLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the LegendLabel property defined in this Rule.
//          The LegendLabel is optional and is used for putting a label
//          for the Rule in the Legend.
// RETURNS:
//-------------------------------------------------------------------------
const MdfString& Rule::GetLegendLabel() const
{
    return this->m_strLegendLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor to the LegendLabel property defined in this Rule.
//          The LegendLabel is optional and is used for putting a label
//          for the Rule in the Legend.
// PARAMETERS:
//      Input:
//          pstrLegendLabel - the label for presenting in the Legend. It
//                           may be a pointer to an empty string.
//-------------------------------------------------------------------------
void Rule::SetLegendLabel(const MdfString& strLegendLabel)
{
    this->m_strLegendLabel = strLegendLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Filter Property defined in this Rule.
//          The Symbolization and Label defined in this Rule will be
//          applied to a feature when its Filter criteria is true for
//          that feature.
// RETURNS: The Filter criteria string.
//-------------------------------------------------------------------------
const MdfString& Rule::GetFilter() const
{
     return this->m_strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Filter Property defined in this Rule.
//          The Symbolization and Label defined in this Rule will be
//          applied to a feature when its Filter criteria is true for
//          that feature. This method does not test for the validity of
//          the Filter criteria. It is left up to the user to ensure its
//          validity.
// PARAMETERS:
//      Input:
//          strFilter - The Filter criteria string.
//-------------------------------------------------------------------------
void Rule::SetFilter(const MdfString& strFilter)
{
    this->m_strFilter = strFilter;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Label Property defined in this Rule.
//          The Label may be built up using multivariate theming in the
//          same way as Symbolizations. A Rule need not have a Symbolization
//          and a Label, but must at least have one or the other.
// RETURNS: Label pointer. May be NULL.
//-------------------------------------------------------------------------
Label* Rule::GetLabel()
{
    return this->m_pLabel;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt the Label object for the Label Property defined
//          in this Rule. The user is not responsible for the deletion of the
//          adopted Label. Labels may be built up using multivariate theming
//          in the same way as Symbolizations. A Rule need not have a
//          Symbolization and a Label, but must at least have one or the other.
// PARAMETERS:
//        Input:
//          pLabel - Adopted Label object that is created on the heap.
//                   It may be NULL.
//-------------------------------------------------------------------------
void Rule::AdoptLabel(Label* pLabel)
{
    if (this->m_pLabel != pLabel)
    {
        delete this->m_pLabel;
        this->m_pLabel = pLabel;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Label object representing the Label Property
//          defined in this Rule. The returned Label object is now the
//          responsibility of the user for its deletion.
// RETURNS: The pointer to the orphaned Label object. It may be NULL.
//-------------------------------------------------------------------------
Label* Rule::OrphanLabel()
{
    Label* ret = this->m_pLabel;
    this->m_pLabel = NULL;
    return ret;
}

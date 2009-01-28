//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

//////////////////////////////////////////////////////////////////
/// <summary>
/// Besides retrieval of data, sometimes users want to group similar
/// features and apply some funtions like AVG(), STDDEV() to certain
/// properties. This class allows to group data and apply
/// aggregate funtions published in provider capablilities document
/// based on a criteria.
/// </summary>

#include "PlatformBase.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureAggregateOptions)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgFeatureAggregateOptions::MgFeatureAggregateOptions()
{
    m_selectDistinct = false;
    m_groupByProperties = NULL;
    m_groupFilter = L"";
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgFeatureAggregateOptions::~MgFeatureAggregateOptions()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Apply Distinct operation on Select
/// SELECT DISTINCT B FROM ClassTest
/// </summary>
/// <param name="yes">
/// true - yes (apply distinct operation)
/// false - no ( this is default )
/// </param>
/// <returns>
/// Nothing
/// </returns>
void MgFeatureAggregateOptions::SelectDistinct(bool yes)
{
    m_selectDistinct = yes;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Apply additional grouping filter to select operation
/// This is like SELECT A, MIN(B) AS MIN_B FROM ClassTest WHERE (A > 0) AND
/// "ONLY FROM WITHIN THE SPECIFIED CIRLCE BASED ON GeometryPropertyName"
/// ORDER BY A DESC GROUP BY A HAVING MIN_B > 50
/// </summary>
/// <param name="groupByProperties">
/// Collection of properties to be used in Group By statement above
/// </param>
/// <param name="groupFilter">
/// This is similar to having clause in SQL SELECT which is applied
/// to a group
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException
void MgFeatureAggregateOptions::SetGroupingFilter(MgStringCollection* groupByProperties, CREFSTRING groupFilter)
{
    // Ptr template class will handle cleanup if m_groupByProperties is not NULL
    m_groupByProperties = SAFE_ADDREF(groupByProperties);
    m_groupFilter = groupFilter;
}


MgStringCollection* MgFeatureAggregateOptions::GetGroupingProperties()
{
    return SAFE_ADDREF((MgStringCollection*)m_groupByProperties);
}

STRING MgFeatureAggregateOptions::GetGroupFilter()
{
    return m_groupFilter;
}

void MgFeatureAggregateOptions::Serialize(MgStream* stream)
{
    // Call Parent method
    MgFeatureQueryOptions::Serialize(stream);

    stream->WriteBoolean(m_selectDistinct);
    stream->WriteObject(m_groupByProperties);
    stream->WriteString(m_groupFilter);
}

void MgFeatureAggregateOptions::Deserialize(MgStream* stream)
{
    // Call Parent method
    MgFeatureQueryOptions::Deserialize(stream);

    stream->GetBoolean(m_selectDistinct);
    m_groupByProperties = (MgStringCollection*)stream->GetObject();
    stream->GetString(m_groupFilter);
}

bool MgFeatureAggregateOptions::GetDistinct()
{
    return m_selectDistinct;
}

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

//////////////////////////////////////////////////////////////////
/// <summary>
/// Retrieval of features can be done by specifying several options
/// and filter criteria. Users may also want to generate computed
/// properties in terms of expressions and define them with an alias.
/// This class will be utilized to apply query options to a feature
/// class.
/// </summary>

#include "PlatformBase.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureQueryOptions)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgFeatureQueryOptions::MgFeatureQueryOptions()
{
    m_filterText = L"";
    m_classProperties = new MgStringCollection();
    m_computedProperties = new MgStringPropertyCollection();

    m_orderByProperties = (MgStringCollection*)NULL;
    m_orderOption = MgOrderingOption::Ascending;

    m_geometryProperty = L"";

    // TODO: What is default geometric operation
    m_operation = MgFeatureSpatialOperations::Within;

    // TODO: Convert this to use smart pointer
    m_geometry = NULL;

    m_binaryOp = true;

    m_fetchSize = 0;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Add class property for which data to be retrieved.
/// SELECT A, B, C FROM ClassTest{}
/// A, B, C are class properties. The names of these class
/// properties are retrieved from class definition.
/// Please refer to GetClassDefinition API in FeatureServices
/// User can call this method multiple times to add more
/// properties
/// </summary>
/// <param name="propertyName">
/// Name of the class property to be added
/// </param>
/// <returns>
/// Returns the integer indicating total properties added
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException - if property name is empty
INT32 MgFeatureQueryOptions::AddFeatureProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_classProperties, L"MgFeatureQueryOptions.AddFeatureProperty");
    this->ValidateEmptyArgument(propertyName);
    m_classProperties->Add(propertyName);
    return m_classProperties->GetCount();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Define computed property with alias name.
/// SELECT MIN(SUM(A*B/C)+AVG(A)) AS MIN_OF_A, MIN(SUM(A*B/C)+AVG(B)) AS MIN_OF_B FROM ClassTest{}
/// MIN_OF_A, MIN_OF_B are computed properties.
/// User can call this method multiple times to add more properties
/// </summary>
/// <param name="aliasName">
/// Name of the property for the expression (e.g. MIN_OF_A)
/// </param>
/// <param name="expression">
/// Expression to be associated with the alias name
/// </param>
/// <returns>
/// Returns the integer indicating total properties added
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException - if aliasName or expression is empty
INT32 MgFeatureQueryOptions::AddComputedProperty(CREFSTRING aliasName, CREFSTRING expression)
{
    CHECKNULL(m_computedProperties, L"MgFeatureQueryOptions.AddFeatureProperty");
    this->ValidateEmptyArgument(aliasName);
    this->ValidateEmptyArgument(expression);
    m_computedProperties->Add(aliasName, expression);
    return m_computedProperties->GetCount();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the filter for this select operation. This is something similar
/// to where clause in SELECT A,B,C FROM ClassTest WHERE A > 20
/// In this case filterText is "A > 20".
/// For more details on defining filter, please refer to Filter and Expression
/// documentation.
/// </summary>
/// <param name="filterText">
/// Filter text to be applied
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException
void MgFeatureQueryOptions::SetFilter(CREFSTRING filterText)
{
    m_filterText = filterText;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Apply additional spatial filter to select operation
/// This is like SELECT A,B,C FROM ClassTest WHERE (A > 0) AND
/// "ONLY FROM WITHIN THE SPECIFIED CIRLCE BASED ON GeometryPropertyName"
/// </summary>
/// <param name="geometryProperty">
/// Name of Geometry property on which this spatial filter to be applied
/// </param>
/// <param name="geometry">
/// Geometry object, could be circle, polygon etc. (enclosed geometry object)
/// e.g. it can not be point, line, polyline
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException
void MgFeatureQueryOptions::SetSpatialFilter(CREFSTRING geometryProperty,
                                             MgGeometry* geometry,
                                             INT32 spatialOperation)
{

    // Validate spatial operator
    MgFeatureSpatialOperations::ValidateRange(spatialOperation);
    // Validate other arguments
    CHECKARGUMENTEMPTYSTRING(geometryProperty, L"MgFeatureQueryOptions.SetSpatialFilter");
    CHECKARGUMENTNULL(geometry, L"MgFeatureQueryOptions.SetSpatialFilter");

    m_geometryProperty = geometryProperty;
    m_operation = spatialOperation;
    m_geometry = SAFE_ADDREF(geometry);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Apply additional ordering filter to select operation
/// This is like SELECT A,B,C FROM ClassTest WHERE (A > 0) AND
/// "ONLY FROM WITHIN THE SPECIFIED CIRLCE BASED ON GeometryPropertyName"
/// ORDER BY A,B DESC
/// </summary>
/// <param name="orderByProperties">
/// Collection of properties to be used in Order By statement above
/// </param>
/// <param name="orderOption">
/// ordering option, Ascending or Descending
/// Defaults to Ascending if nothing is specified
/// </param>
/// <returns>
/// Nothing
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException
void MgFeatureQueryOptions::SetOrderingFilter(MgStringCollection* orderByProperties, INT32 orderOption)
{
    // Validate Range for order option
    MgOrderingOption::ValidateRange(orderOption);

    // Ptr template class will handle cleanup if m_orderByProperties is not NULL
    m_orderByProperties = SAFE_ADDREF(orderByProperties);
    m_orderOption = orderOption;
}

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// For queries that return a large number of objects
/// some feature sources support improving performance by setting
/// fetch size in order to reduce the number of database server
/// round trips required to satisfy the selection criteria.
/// Providers that do not use a fetch size will ignore the fetch
/// size parameter.  This does not affect the actual results of
/// queries as it is a performance tuning parameter.
/// </summary>
/// <param name="fetchSize">
/// The fetch size of query. The query returns all of query
/// results if setting the fetch size to 0.
/// </param>
/// <returns>
/// Returns nothing.
/// </returns>
void MgFeatureQueryOptions::SetFetchSize(INT32 fetchSize)
{
    m_fetchSize = fetchSize;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes class property.
/// </summary>
/// <param name="propertyName">
/// Name of the class property to be removed
/// </param>
/// <returns>
/// Returns the integer indicating total properties left
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException - if property name is empty
void MgFeatureQueryOptions::RemoveFeatureProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_classProperties, L"MgFeatureQueryOptions.RemoveFeatureProperty");
    this->ValidateEmptyArgument(propertyName);
    m_classProperties->Remove(propertyName);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Removes computed property.
/// </summary>
/// <param name="propertyName">
/// Name of the computed property to be removed
/// </param>
/// <returns>
/// Returns the integer indicating total properties left
/// </returns>
/// EXCEPTIONS:
/// MgInvalidArgumentException - if property name is empty
void MgFeatureQueryOptions::RemoveComputedProperty(CREFSTRING aliasName)
{
    CHECKNULL(m_computedProperties, L"MgFeatureQueryOptions.RemoveComputedProperty");
    this->ValidateEmptyArgument(aliasName);
    m_computedProperties->Remove(aliasName);
}

MgFeatureQueryOptions::~MgFeatureQueryOptions()
{
    if (m_classProperties != NULL)
        m_classProperties->Clear();

    if (m_computedProperties != NULL)
        m_computedProperties->Clear();
}

MgStringCollection* MgFeatureQueryOptions::GetClassProperties()
{
    CHECKNULL(m_classProperties, L"MgFeatureQueryOptions.GetClassProperties");

    return SAFE_ADDREF((MgStringCollection*)m_classProperties);
}

MgStringPropertyCollection* MgFeatureQueryOptions::GetComputedProperties()
{
    CHECKNULL((MgStringPropertyCollection*)m_computedProperties, L"MgFeatureQueryOptions.GetComputedProperties");

    return SAFE_ADDREF((MgStringPropertyCollection*)m_computedProperties);
}

MgStringCollection* MgFeatureQueryOptions::GetOrderingProperties()
{
    return SAFE_ADDREF((MgStringCollection*)m_orderByProperties);
}

STRING MgFeatureQueryOptions::GetFilter()
{
    return m_filterText;
}

STRING MgFeatureQueryOptions::GetGeometryProperty()
{
    return m_geometryProperty;
}

MgGeometry* MgFeatureQueryOptions::GetGeometry()
{
    return SAFE_ADDREF((MgGeometry*)m_geometry);
}

INT32 MgFeatureQueryOptions::GetOrderOption()
{
    return m_orderOption;
}

INT32 MgFeatureQueryOptions::GetSpatialOperation()
{
    return m_operation;
}

INT32 MgFeatureQueryOptions::GetFetchSize()
{
    return m_fetchSize;
}

void MgFeatureQueryOptions::Serialize(MgStream* stream)
{
    stream->WriteString(m_filterText);

    stream->WriteObject(m_classProperties);
    stream->WriteObject(m_computedProperties);
    stream->WriteObject(m_orderByProperties);

    stream->WriteInt32((INT32)m_orderOption);
    stream->WriteString(m_geometryProperty);
    stream->WriteInt32((INT32)m_operation);

    stream->WriteObject(m_geometry);
    stream->WriteBoolean(m_binaryOp);

    stream->WriteInt32(m_fetchSize);
}

void MgFeatureQueryOptions::Deserialize(MgStream* stream)
{
    stream->GetString(m_filterText);

    m_classProperties = (MgStringCollection*)stream->GetObject();
    m_computedProperties = (MgStringPropertyCollection*)stream->GetObject();
    m_orderByProperties = (MgStringCollection*)stream->GetObject();

    INT32 value = 0;
    stream->GetInt32(value);
    m_orderOption = value;

    stream->GetString(m_geometryProperty);

    value = 0;
    stream->GetInt32(value);
    m_operation = value;

    m_geometry = (MgGeometry*)stream->GetObject();

    stream->GetBoolean(m_binaryOp);
    stream->GetInt32(m_fetchSize);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// User can apply regular filter in conjunction with spatial filter
/// There are two possiblities of join between these two filters (AND, OR).
///
/// The binary operator is only applied when both filters are set, otherwise
/// it will be ignored.
///
/// Default operator is AND if nothing is specified.
/// </summary>
/// <param name="andOr">
/// AND - TRUE
/// OR  - FALSE
/// </param>
/// <returns>
/// Nothing
/// </returns>
void MgFeatureQueryOptions::SetBinaryOperator(bool andOr)
{
    m_binaryOp = andOr;
}

bool MgFeatureQueryOptions::GetBinaryOperator()
{
    return m_binaryOp;
}

STRING MgFeatureQueryOptions::GetLogString()
{

    STRING tmp = L"{";

    Ptr<MgStringCollection> classProp = GetClassProperties();
    STRING value = classProp->GetLogString();
    if (!value.empty())
    {
        tmp += L"{ClassProps=";
        tmp += value;
        tmp += L"}";
    }

    Ptr<MgStringPropertyCollection> computed = GetComputedProperties();
    value = computed->GetLogString();
    if (!value.empty())
    {
        tmp += L"{ComputedProps=";
        tmp += value;
        tmp += L"}";
    }

    Ptr<MgStringCollection> orderingProp = GetOrderingProperties();
    value = orderingProp.p != NULL ? orderingProp->GetLogString() : L"";
    if (!value.empty())
    {
        tmp += L"{OrderingProps=";
        tmp += value;
        tmp += L"}{OrderOption=";
        STRING intval;
        MgUtil::Int32ToString(GetOrderOption(),intval);
        tmp += intval;
        tmp += L"}";
    }

    tmp += L"{Operator=";
    tmp += GetBinaryOperator() ? L"1}" : L"0}";

    value = GetFilter();
    if (!value.empty())
    {
        tmp += L"{Filter=";
        tmp += value;
        tmp += L"}";
    }

    STRING geomProp = GetGeometryProperty();
    if (!geomProp.empty())
    {
        tmp += L"{GeomProp=";
        tmp += geomProp;
        tmp += L"}{GeomOp=";
        STRING geomOp = MgFeatureSpatialOperations::ToString(GetSpatialOperation());
        tmp += geomOp;
        tmp += L"}";
        Ptr<MgGeometry> geom = GetGeometry();
        if (geom.p != NULL)
        {
            tmp += L"{Geometry=";
            tmp += geom->ToAwkt(false);
            tmp += L"}";
        }
    }

    tmp += L"{FetchSize=";
    STRING intval;
    MgUtil::Int32ToString(m_fetchSize ,intval);
    tmp += intval;
    tmp += L"}";

    return tmp;
}

void MgFeatureQueryOptions::ValidateEmptyArgument(CREFSTRING value)
{
    if( value.empty() )
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgFeatureQueryOptions.ValidateEmptyArgument",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
}

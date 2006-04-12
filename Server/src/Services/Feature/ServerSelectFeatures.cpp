//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "FeatureService.h"
#include "ServerSelectFeatures.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureReaderIdentifier.h"
#include "ServerGetFeatures.h"
#include "ServerFeatureReader.h"
#include "ServerFeatureUtil.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "FeatureDistribution.h"

#include "ServerGwsFeatureReader.h"
#include "GwsConnectionPool.h"


MgServerSelectFeatures::MgServerSelectFeatures()
{
    m_command = NULL;
    m_options = NULL;
    m_customPropertyFound = false;
    m_customFunction = NULL;
    m_customPropertyName = L"";
}

MgServerSelectFeatures::~MgServerSelectFeatures()
{
    GIS_SAFE_RELEASE(m_customFunction);
}

// Executes the describe schema command and serializes the schema to XML
MgReader* MgServerSelectFeatures::SelectFeatures(MgResourceIdentifier* resource,
                                                 CREFSTRING className,
                                                 MgFeatureQueryOptions* options,
                                                 bool executeSelectAggregate)
{
    Ptr<MgReader> mgReader;
    bool isSelectAggregate = executeSelectAggregate;

    MG_FEATURE_SERVICE_TRY()

    // Validate parameters
    ValidateParam(resource,className);

    // Check if a feature join is to be performed by inspecting the resource for join properties
    if (!isSelectAggregate && FindFeatureJoinProperties(resource, className))
    {
        // Get the FdoFilter from the options
        // Create Command
        CreateCommand(resource, isSelectAggregate);
        // Set the FeatureClass Name
        m_command->SetFeatureClassName((GisString*)className.c_str());
        // Set options (NULL is a valid value)
        m_options = SAFE_ADDREF(options);
        // Apply options to FDO command
        ApplyQueryOptions(isSelectAggregate);

        GisPtr<FdoFilter> filter = m_command->GetFilter();

        // Perform feature join
        mgReader = JoinFeatures(resource, className, filter);
    }
    else
    {
        // Custom function specified from Select command is not allowed
        if (!isSelectAggregate && ContainsCustomFunction(options))
        {
            STRING message = MgServerFeatureUtil::GetMessage(L"MgCustomFunctionNotSupported");

            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"MgServerSelectFeatures.SelectFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
        }

        // Custom function specified using SelectAggregate,
        // we execute as standard Select command on single property
        // then apply function on top of it.
        if (isSelectAggregate && ContainsCustomFunction(options))
        {
            isSelectAggregate = false;
        }

        Ptr<MgReader> reader;

        // Create Command
        CreateCommand(resource, isSelectAggregate);

        // Set the FeatureClass Name
        m_command->SetFeatureClassName((GisString*)className.c_str());

        // Set options (NULL is a valid value)
        m_options = SAFE_ADDREF(options);

        // Apply options to FDO command
        ApplyQueryOptions(isSelectAggregate);

        // Check if the specified className is an extension (join) in the feature source
        if (FindFeatureJoinProperties(resource, className))
        {
            // Perform feature join to obtain the joined properties
            Ptr<MgServerGwsFeatureReader> gwsFeatureReader = JoinFeatures(resource, className, NULL);

            // Get the requested property name from the MgFeatureServiceCommandObject.  This property name may be
            // prefixed by the relation name if it is from a secondary resource.
            GisPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
            int nFicCnt = fic->GetCount();
            if (nFicCnt <= 0)
            {
                // throw invalid argument exception because the properties the m_command is empty
                MgStringCollection arguments;
                arguments.Add(L"1");
                arguments.Add(L"MgFeatureServiceCommand");

                throw new MgInvalidArgumentException(L"MgServerSelectFeatures::SelectFeatures()",
                    __LINE__, __WFILE__, &arguments, L"MgCollectionEmpty", NULL);

            }
            GisPtr<FdoIdentifier> fi = fic->GetItem(0);
            STRING propName = fi->GetName();

            // Parse the relation name from the property name by determining which source the property is from
            STRING relationName;
            STRING secClassName;
            STRING parsedPropertyName;
            IGWSFeatureIterator* gwsFeatureIter = NULL;
            gwsFeatureReader->ReadNext();
            gwsFeatureReader->DeterminePropertyFeatureSource(propName, &gwsFeatureIter, relationName, secClassName, parsedPropertyName);

            Ptr<MgResourceIdentifier> resId;

            // Get the resource id associated with the relation name
            if (relationName.empty())
            {
                // this is the primary resource
                resId = SAFE_ADDREF(resource);
            }
            else
            {
                // this is a secondary resource
                resId = GetSecondaryResourceIdentifier(resource, className, relationName);
            }

            // Create new command to execute against the desired resource in defined by the join
            m_customPropertyFound = false;
            CreateCommand(resId, isSelectAggregate);
            // Set feature class name (either primary or secondary class)
            m_command->SetFeatureClassName((GisString*)secClassName.c_str());
            // Set options
            m_options = SAFE_ADDREF(options);

            // Update the value of the computed property.  Since the joined property may be prefixed by the relation name
            // i.e. Join1PropA, this needs to be parsed into it individual relation and property components.
            Ptr<MgStringPropertyCollection> computedProperties = m_options->GetComputedProperties();

            // Can only have one computed property for custom functions
            assert(computedProperties->GetCount() == 1);
            STRING aliasName = computedProperties->GetName(0);
            STRING expression = computedProperties->GetValue(0);

            STRING::size_type nPropLength = propName.length();
            STRING::size_type nPropStart = expression.rfind(propName);
            STRING::size_type nPropEnd = expression.find(L")", nPropStart + 1);

            STRING newExpression = expression.substr(0, nPropStart);
            newExpression += parsedPropertyName;
            newExpression += expression.substr(nPropStart + nPropLength);
            computedProperties->SetValue(aliasName, newExpression);

            // Apply options to FDO command
            ApplyQueryOptions(isSelectAggregate);
        }

        // If custom function is found, then validate that no more than one function/property is specified
        ValidateConstraintsOnCustomFunctions();

        // Execute the command
        reader = m_command->Execute();

        CHECKNULL((MgReader*)reader, L"MgServerSelectFeatures.SelectFeatures");

        if (executeSelectAggregate && m_customPropertyFound)
        {
            mgReader = this->GetCustomReader(reader);
        }
        else
        {
            // Increment the refcount as it will be decremented when we go out of  try...catch
            mgReader = SAFE_ADDREF((MgReader*)reader);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSelectFeatures.SelectFeatures")

    return mgReader.Detach();
}

void MgServerSelectFeatures::ApplyQueryOptions(bool isSelectAggregate)
{
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyQueryOptions");

    if (m_options != NULL)
    {
        ApplyClassProperties();
        ApplyComputedProperties();
        ApplyFilter();
        // ApplySpatialFilter();
        ApplyOrderingOptions();
        ApplyAggregateOptions(isSelectAggregate);
    }
}

// ClassProperties
void MgServerSelectFeatures::ApplyClassProperties()
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyClassProperties");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyClassProperties");

    Ptr<MgStringCollection> properties = m_options->GetClassProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    GisPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyClassProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        GisPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((GisString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyClassProperties");

        fic->Add(fdoIden);
    }
}


// Computed properties
void MgServerSelectFeatures::ApplyComputedProperties()
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyComputedProperties");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyComputedProperties");

    Ptr<MgStringPropertyCollection> properties = m_options->GetComputedProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // TODO: Add support for custom functions

    for (INT32 i=0; i < cnt; i++)
    {
        STRING aliasName = properties->GetName(i);
        STRING expression = properties->GetValue(i);

        GisString* str = (GisString*)expression.c_str();
        if (str != NULL)
        {
            GisPtr<FdoExpression> expression = FdoExpression::Parse(str);
            CHECKNULL((FdoExpression*)expression, L"MgServerSelectFeatures.ApplyComputedProperties");

            bool udf = ContainsUdf(expression);
            if (!udf)
            {
                AddFdoComputedProperty(aliasName, expression);
            }
            else
            {
                AddCustomComputedProperty(aliasName, expression);
            }
        }
    }
}

//// Filter text
//void MgServerSelectFeatures::ApplyFilter()
//{
//    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyFilter");
//    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyFilter");
//
//    STRING filterText = m_options->GetFilter();
//    if (filterText.empty()) { return; } // Nothing to do
//
//    m_command->SetFilter(filterText.c_str());
//}

// Spatial Filter
void MgServerSelectFeatures::ApplyFilter()
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyFilter");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyFilter");

    GisPtr<FdoFilter> regularFilter;
    GisPtr<FdoSpatialCondition> spatialFilter;
    GisPtr<FdoFilter> combineFilter;
    FdoBinaryLogicalOperations bOper = FdoBinaryLogicalOperations_And;

    // Build regular filter
    STRING filterText = m_options->GetFilter();
    if (!filterText.empty())
    {
        regularFilter = FdoFilter::Parse(filterText.c_str());
    }

    // Build spatial filter
    bool isAnd = m_options->GetBinaryOperator();
    Ptr<MgGeometry> geom = m_options->GetGeometry();
    INT32 spatialOp = m_options->GetSpatialOperation();

    STRING geomProp = m_options->GetGeometryProperty();

    if (!isAnd) // Apply OR operator
    {
        bOper = FdoBinaryLogicalOperations_Or;
    }

    // Check whether we have valid geometric filter
    if (!geomProp.empty() && (geom != NULL))
    {
        MgAgfReaderWriter agfWriter;

        Ptr<MgByteReader> reader = agfWriter.Write(geom);
        Ptr<MgByteSink> sink = new MgByteSink(reader);
        Ptr<MgByte> bytes = sink->ToBuffer();

        GisByte* gisBytes = bytes->Bytes();
        INT32 len = bytes->GetLength();

        GisPtr<GisByteArray> byteArray = GisByteArray::Create(gisBytes, (GisInt32)len);

        GisPtr<FdoGeometryValue> geomValue = FdoGeometryValue::Create(byteArray);
        if (geomValue != NULL)
        {
            FdoSpatialOperations fdoSpatialOp = MgServerFeatureUtil::GetFdoSpatialOperation(spatialOp);
            spatialFilter = FdoSpatialCondition::Create((GisString*)geomProp.c_str(), fdoSpatialOp, (FdoExpression*)geomValue);
        }
    }

    // Determine which one to apply
    if ((regularFilter != NULL) && (spatialFilter != NULL))
    {
        // Both filter exists, combine them
        combineFilter = FdoFilter::Combine(regularFilter, bOper, spatialFilter);
    }
    else
    {
        if (regularFilter != NULL)
        {
            // Only regular filter exists
            combineFilter = GIS_SAFE_ADDREF((FdoFilter*)regularFilter);
        }
        else if (spatialFilter != NULL)
        {
            // Only spatial filter exists
            combineFilter = GIS_SAFE_ADDREF((FdoFilter*)spatialFilter);
        }
    }

    // Apply the filter
    if (combineFilter != NULL)
    {
        m_command->SetFilter(combineFilter);
    }
}

// Ordering options
void MgServerSelectFeatures::ApplyOrderingOptions()
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyOrderingOptions");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyOrderingOptions");

    Ptr<MgStringCollection> properties = m_options->GetOrderingProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // Ordering options are supplied but provider does not support it
    if (!m_command->SupportsSelectOrdering())
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgOrderingOptionNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerSelectFeatures.ApplyOrderingOptions", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    GisPtr<FdoIdentifierCollection> fic = m_command->GetOrdering();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyOrderingOptions");

    // Order option Asc or Desc (default is Asc)
    FdoOrderingOption option = MgServerFeatureUtil::GetFdoOrderingOption(m_options->GetOrderOption());
    m_command->SetOrderingOption(option);

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        if (!propertyName.empty())
        {
            GisPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((GisString*)propertyName.c_str());
            CHECKNULL((FdoIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyOrderingOptions");

            fic->Add(fdoIden);
        }
    }
}


bool MgServerSelectFeatures::ContainsUdf(FdoExpression* expression)
{
    bool isUdf = false;
    bool fdoSupported = false;

    // Downcast to FdoFunction
    FdoFunction* function = dynamic_cast<FdoFunction*>(expression);

    // If we are unable to downcast, it means it is not a function, it is just
    // an expression. We do not do anything with this. We just pass it to FDO
    if (function != NULL)
    {
        if (m_command != NULL)
        {
            // Check if FDO supports this function, if so, let FDO handle it
            fdoSupported = m_command->IsSupportedFunction(function);
        }

        if (!fdoSupported)
        {
            // If function is not supported, then check if it is a custom function.
            isUdf = IsCustomFunction(function);
        }
    }

    return isUdf;
}


bool MgServerSelectFeatures::IsCustomFunction(FdoFunction* fdoFunc)
{
    bool customFunc = false;

    GisString* funcNameAllowed = fdoFunc->GetName();
    if (funcNameAllowed != NULL)
    {
        INT32 funcIndex = -1;
        customFunc = MgServerFeatureUtil::FindCustomFunction(STRING(funcNameAllowed),funcIndex);
    }

    return customFunc;
}

void MgServerSelectFeatures::AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    GisPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.AddFdoComputedProperty");

    GisString* expName = aliasName.c_str();
    if (expName != NULL)
    {
        GisPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expression);
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"MgServerSelectFeatures.AddFdoComputedProperty");

        fic->Add(fdoIden);
    }
}

void MgServerSelectFeatures::AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    CHECKNULL((FdoExpression*)expression, L"MgServerSelectFeatures.AddCustomComputedProperty");

    GisPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.AddCustomComputedProperty");

    // If property is already found, two custom properties are not supported and therefore throw exception
    if (m_customPropertyFound)
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerSelectFeatures.AddCustomComputedProperty", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Downcast to FdoFunction
    FdoFunction* function = dynamic_cast<FdoFunction*>(expression);

    if (function != NULL)
    {
        GisString* expName = aliasName.c_str();
        if (expName != NULL)
        {
            GisPtr<FdoExpressionCollection> exprCol = function->GetArguments();
            GisInt32 cnt = exprCol->GetCount();
            GisPtr<FdoExpression> expr = NULL;
            if (cnt > 0)
            {
                expr = exprCol->GetItem(0);   // Property Name
            }

            // Just pass in the property name
            // GisPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expr);

            // NOTE: Each provider has its own rule for supporting computed properties, select and select aggregate
            // functionality. Therefore we just work with simple select command, fetch the property and do the needed
            // calculations. Therefore, we are not adding them as computed properties.

            FdoIdentifier* propName = dynamic_cast<FdoIdentifier*>(expr.p);

            if (propName != NULL)
                fic->Add(propName);

            m_customPropertyName = aliasName;
            m_customPropertyFound = true;
            m_customFunction = GIS_SAFE_ADDREF(function);
        }
    }
}

void MgServerSelectFeatures::ValidateConstraintsOnCustomFunctions()
{
    // Custom function should only be allowed stand alone. This means, no other property along withit
    // will be supported. Therefore
    // 1. ClassProperties should be zero.
    // 2. Computed properties (custom) should not be more than one
    // 3. No GroupBy clause allowed
    if (m_options != NULL)
    {
        Ptr<MgStringCollection> strCol = m_options->GetClassProperties();
        INT32 classPropCnt = strCol->GetCount();

        if (m_customPropertyFound)
        {
            if (classPropCnt > 0)
            {
                STRING message = MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(L"MgServerSelectFeatures.ValidateConstraintsOnCustomFunctions",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            MgServerFeatureUtil::ValidateCustomConstraints(m_customFunction);
        }
    }
}


void MgServerSelectFeatures::CreateCommand(MgResourceIdentifier* resource, bool isSelectAggregate)
{
    if (!isSelectAggregate)
    {
        m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select);
    }
    else
    {
        m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_SelectAggregates);
    }
    CHECKNULL((MgFeatureServiceCommand*)m_command, L"MgServerSelectFeatures.CreateCommand");
}

void MgServerSelectFeatures::ValidateParam(MgResourceIdentifier* resource, CREFSTRING className)
{
    // className and resource identifier can not be NULL
    if (resource == NULL)
    {
        throw new MgNullArgumentException(L"MgServerSelectFeatures::ValidateParam()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (className.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerSelectFeatures::ValidateParam()",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
}

void MgServerSelectFeatures::ApplyAggregateOptions(bool isSelectAggregate)
{
    // If not select aggregate, just return
    if (!isSelectAggregate)
        return;

    // Downcast to see if it is an instance of MgFeatureAggregateOptions.
    MgFeatureAggregateOptions* options = dynamic_cast<MgFeatureAggregateOptions*>((MgFeatureQueryOptions*)m_options);

    // If it is not MgFeatureAggregateOptions, we simply return
    if (options == NULL)
        return;

    STRING groupFilter = options->GetGroupFilter();
    Ptr<MgStringCollection> strCol = options->GetGroupingProperties();
    bool distinct = options->GetDistinct();

    // Provider may throw exception if distinct operation is not supported.
    // We delegate to provider instead of determining and throwing exception
    // of our own
    if (distinct)
    {
        // Set distinct requirements
        ((MgFeatureServiceCommand*)m_command)->SetDistinct(distinct);
    }

    // Set all properties for grouping
    ApplyFdoGroupingProperties(strCol);
    // Set Grouping Filter
    GisPtr<FdoFilter> filter = (FdoFilter*)NULL;
    if (!groupFilter.empty())
    {
        filter = FdoFilter::Parse((GisString*)groupFilter.c_str());
        if (filter != NULL)
        {
            ((MgFeatureServiceCommand*)m_command)->SetGroupingFilter(filter);
        }
    }
}

void MgServerSelectFeatures::ApplyFdoGroupingProperties(MgStringCollection* propertyNames)
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyFdoGroupingProperties");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyFdoGroupingProperties");

    Ptr<MgStringCollection> properties = SAFE_ADDREF(propertyNames);

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // Grouping options are supplied but provider does not support it
    if (!m_command->SupportsSelectGrouping())
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgGroupingNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerSelectFeatures.ApplyFdoGroupingProperties", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    GisPtr<FdoIdentifierCollection> fic = ((MgFeatureServiceCommand*)m_command)->GetGrouping();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyFdoGroupingProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        GisPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((GisString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyFdoGroupingProperties");

        fic->Add(fdoIden);
    }
}

// Check whether user is requesting custom operations
bool MgServerSelectFeatures::ContainsCustomFunction(MgFeatureQueryOptions* options)
{
    bool hasCustomFunction = false;

    if (options == NULL)
        return false;

    Ptr<MgStringPropertyCollection> properties = options->GetComputedProperties();

    if (properties == NULL)
        return false;   // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return false;   // Nothing to do

    for (INT32 i=0; i < cnt; i++)
    {
        STRING aliasName = properties->GetName(i);
        STRING expression = properties->GetValue(i);

        GisString* str = (GisString*)expression.c_str();
        if (str != NULL)
        {
            GisPtr<FdoExpression> expression = FdoExpression::Parse(str);
            CHECKNULL((FdoExpression*)expression, L"MgServerSelectFeatures.ContainsCustomFunction");

            hasCustomFunction = ContainsUdf(expression);
        }
    }

    if (hasCustomFunction && (cnt != 1))
    {
        // Only one custom function with no property is allowed
        STRING message = MgServerFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerSelectFeatures.ContainsCustomFunction",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return hasCustomFunction;
}

// Convert reader into a custom MgDataReader
MgReader* MgServerSelectFeatures::GetCustomReader(MgReader* reader)
{
    Ptr<MgReader> distReader;
    if (m_customPropertyFound)
    {
        Ptr<MgFeatureDistribution> featureDist =
            MgFeatureDistribution::CreateDistributionFunction(reader, m_customFunction, m_customPropertyName);

        distReader = featureDist->Execute();
    }

    return distReader.Detach();
}

// Look for extension (feature join) properties in the feature source document
bool MgServerSelectFeatures::FindFeatureJoinProperties(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bJoinPropertiesExists = false;

    //// Parse the resource identifier to get the join parameters
    // Retrieve XML from repository
    string featureSourceXmlContent;
    RetrieveFeatureSource(resourceId, featureSourceXmlContent);

    // Needed to parse XML and get properties
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(featureSourceXmlContent.c_str());

    DOMElement* rootNode = xmlUtil.GetRootNode();
    DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );

    if (NULL != extensionNodeList)
    {
        int extensionNodes = (int)extensionNodeList->getLength();

        for (int i = 0; i < extensionNodes; i++)
        {
            DOMNode* extensionNode = extensionNodeList->item(i);
            CHECKNULL(extensionNode, L"MgServerSelectFeatures.FindFeatureJoinProperties");

            DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
            int nNameNodes = (int)nameNodeList->getLength();

            // get the extension name node
            DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

            // get the extension name value
            STRING name;
            xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, name);

            STRING parsedSchemaName = L"";
            STRING parsedExtensionName = L"";
            ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

            if (parsedExtensionName != name)
            {
                continue;
            }
            else
            {
                bJoinPropertiesExists = true;
            }
        }
    }

    return bJoinPropertiesExists;
}

MgServerGwsFeatureReader* MgServerSelectFeatures::JoinFeatures(MgResourceIdentifier* featureSourceIdentifier, CREFSTRING extensionName, FdoFilter* filter)
{
    Ptr<MgServerGwsFeatureReader> gwsFeatureReader;

    MG_FEATURE_SERVICE_TRY()

    GisPtr<IGWSQueryDefinition> qd;
    GisPtr<MgGwsConnectionPool> pool = MgGwsConnectionPool::Create();

    //// Parse the resource identifier to get the join parameters
    // Retrieve XML from repository
    string featureSourceXmlContent;
    RetrieveFeatureSource(featureSourceIdentifier, featureSourceXmlContent);

    // Needed to parse XML and get properties
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(featureSourceXmlContent.c_str());

    DOMElement* rootNode = xmlUtil.GetRootNode();
    DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );
    CHECKNULL(extensionNodeList, L"MgServerSelectFeatures.JoinFeatures");

    int extensionNodes = (int)extensionNodeList->getLength();

    for (int i = 0; i < extensionNodes; i++)
    {
        DOMNode* extensionNode = extensionNodeList->item(i);
        CHECKNULL(extensionNode, L"MgServerSelectFeatures.JoinFeatures");

        DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
        int nNameNodes = (int)nameNodeList->getLength();

        // get the extension name node
        DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

        // get the extension name value
        STRING name;
        xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, name);

        STRING parsedSchemaName = L"";
        STRING parsedExtensionName = L"";
        ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            // Establish connection to provider for primary feature source
            STRING primaryConnectionName;
            MgUtil::GenerateUuid(primaryConnectionName);
            MgServerFeatureConnection msfcLeft(featureSourceIdentifier);
            if ( msfcLeft.IsConnectionOpen() )
            {
                GisPtr<FdoIConnection> connLeft = msfcLeft.GetConnection();
                pool->AddConnection(primaryConnectionName.c_str(), connLeft);
            }
            else
            {
                throw new MgConnectionFailedException(L"MgServerSelectFeatures.JoinFeatures",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Retrieve the primary feature class
            wstring szFeatureClass;
            xmlUtil.GetElementValue(extensionNode, "FeatureClass", szFeatureClass);

            // Parse the qualified classname
            STRING primaryFsSchema = L"";
            STRING primaryFsClassName = L"";
            ParseQualifiedClassName(szFeatureClass, primaryFsSchema, primaryFsClassName);

            // Create primary query definition
            GisPtr<GisStringCollection> lsellist;
            GisPtr<FdoFilter> lfilter = GIS_SAFE_ADDREF(filter);

            GisPtr<IGWSQueryDefinition> lqd = IGWSFeatureQueryDefinition::Create(
                   lsellist,
                   GWSQualifiedName(primaryConnectionName.c_str(), primaryFsSchema.c_str(), primaryFsClassName.c_str()),
                   lfilter);
            CHECKNULL(lqd, L"MgServerSelectFeatures.JoinFeatures");
            qd = lqd;

            IGWSJoinQueryDefinition* jqd = NULL;

            // Determine the number of secondary sources (AttributeRelate nodes)
            DOMNodeList* attributeRelateNodeList = xmlUtil.GetNodeList(extensionNode, "AttributeRelate");
            int nAttributeRelateNodes = (int)attributeRelateNodeList->getLength();

            // For each join to a secondary source need to do the following
            for (int attributeRelateIndex = 0; attributeRelateIndex < nAttributeRelateNodes; attributeRelateIndex++)
            {
                DOMNode* attributeRelateNode = attributeRelateNodeList->item(attributeRelateIndex);

                // Get the secondary resource id
                wstring szSecondaryResourceId;
                xmlUtil.GetElementValue(attributeRelateNode, "ResourceId", szSecondaryResourceId);

                // Get the name for the join relationship
                wstring szAttributeRelateName;
                xmlUtil.GetElementValue(attributeRelateNode, "Name", szAttributeRelateName);
                STRING secondaryConnectionName = szAttributeRelateName;

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(szSecondaryResourceId);

                if (NULL != secondaryFeatureSource)
                {
                    MgServerFeatureConnection msfcRight(secondaryFeatureSource);
                    if ( msfcRight.IsConnectionOpen() )
                    {
                        GisPtr<FdoIConnection> connRight = msfcRight.GetConnection();
                        pool->AddConnection(secondaryConnectionName.c_str(), connRight);
                    }
                    else
                    {
                        throw new MgConnectionFailedException(L"MgServerSelectFeatures.JoinFeatures",
                            __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }

                // Get the secondary feature className (qualified className)
                wstring szSecondaryClassName;
                xmlUtil.GetElementValue(attributeRelateNode, "AttributeClass", szSecondaryClassName);

                // Parse the qualifed classname
                STRING secondaryFsSchema = L"";
                STRING secondaryFsClassName = L"";
                ParseQualifiedClassName(szSecondaryClassName, secondaryFsSchema, secondaryFsClassName);

                // Create secondary query definition
                GisPtr<GisStringCollection> rsellist;
                GisPtr<FdoFilter> rfilter;

                GisPtr<IGWSQueryDefinition> rqd = IGWSFeatureQueryDefinition::Create(
                       rsellist,
                       GWSQualifiedName(secondaryConnectionName.c_str(), secondaryFsSchema.c_str(), secondaryFsClassName.c_str()),
                       rfilter);
                CHECKNULL(rqd, L"MgServerSelectFeatures.JoinFeatures");

                // Get Join Attributes
                GisPtr<GisStringCollection> lattrs = GisStringCollection::Create();
                GisPtr<GisStringCollection> rattrs = GisStringCollection::Create();

                // Determine the number of ReleateProperties (attributes)
                DOMNodeList* relatePropertyNodeList = xmlUtil.GetNodeList(attributeRelateNode, "RelateProperty");
                int nRelatePropertyNodes = relatePropertyNodeList->getLength();

                // For each RelatePropery need to do the following
                for (int relatePropertyNodeIndex = 0; relatePropertyNodeIndex < nRelatePropertyNodes; relatePropertyNodeIndex++)
                {
                    DOMNode* relatePropertyNode = relatePropertyNodeList->item(relatePropertyNodeIndex);

                    // Get the FeatureClassProperty (primary attribute)
                    wstring szPrimaryAttribute;
                    xmlUtil.GetElementValue(relatePropertyNode, "FeatureClassProperty", szPrimaryAttribute);

                    // Add to the primary attribute String collection
                    lattrs->Add(szPrimaryAttribute.c_str());

                    // Get the AttributeClassProperty (secondary attribute)
                    wstring szSecondaryAttribute;
                    xmlUtil.GetElementValue(relatePropertyNode, "AttributeClassProperty", szSecondaryAttribute);

                    // Add to the secondary attribute String collection
                    rattrs->Add(szSecondaryAttribute.c_str());
                }

                // Create the QueryDefinition
                if (NULL != rqd)
                {
                    jqd = IGWSLeftJoinQueryDefinition::Create(lqd, rqd, lattrs, rattrs);
                    lqd = jqd;
                }

            } // Repeat for each secondary source
            qd = lqd;

            //Pass the QueryDefinition to the JoinEngine
            // Create query object by passing connection pool object and query definition
            GisPtr<IGWSQuery> query = IGWSQuery::Create(pool, qd, NULL);
            GisPtr<IGWSFeatureIterator> iter;
            GisPtr<IGWSFeatureIterator> iterCopy;

            // Prepare and Execute Query
            query->Prepare();
            query->Execute(&iter);
            query->Execute(&iterCopy);

            GisPtr<GisStringCollection> fsNames = qd->FeatureSourceNames();

            gwsFeatureReader = new MgServerGwsFeatureReader(iter);
            gwsFeatureReader->PrepareGwsGetFeatures(parsedExtensionName, fsNames);
            gwsFeatureReader->SetGwsIteratorCopy(iterCopy);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSelectFeatures.JoinFeatures")

    return gwsFeatureReader.Detach();
}

void MgServerSelectFeatures::RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent)
{
    CHECKNULL(resource, L"MgServerSelectFeatures.RetrieveFeatureSource");

    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgResourceService> resourceService = dynamic_cast<MgResourceService*>(
        serviceMan->RequestService(MgServiceType::ResourceService));
    assert(resourceService != NULL);

    // Get the feature source contents
    Ptr<MgByteReader> byteReader = resourceService->GetResourceContent(resource, MgResourcePreProcessingType::Substitution);

    Ptr<MgByteSink> byteSink = new MgByteSink((MgByteReader*)byteReader);
    byteSink->ToStringUtf8(resourceContent);

    ValidateFeatureSource(resourceContent);
}


void MgServerSelectFeatures::ParseQualifiedClassName(CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className)
{
    STRING::size_type nIndex = qualifiedClassName.rfind(CLASSNAME_QUALIFIER);

    schemaName = qualifiedClassName.substr(0, nIndex);
    className = qualifiedClassName.substr(nIndex+1);
}

void MgServerSelectFeatures::ValidateFeatureSource(string& featureSourceXmlContent)
{
    bool isValidFeatureSource = true;

    // TODO: Should we add XML validation here to ensure the integrity of feature source
    if (featureSourceXmlContent.empty())
    {
        isValidFeatureSource = false;
    }
    else
    {
        int index = (int)featureSourceXmlContent.find("<FeatureSource");
        if (index == -1)
        {
            isValidFeatureSource = false;
        }
    }

    // if invalid FeatureSource, throw exception saying invalid provider specified
    if (!isValidFeatureSource)
    {
        STRING message = MgUtil::GetResourceMessage(MgResources::FeatureService, L"MgInvalidFdoProvider");

        Ptr<MgStringCollection> strCol = (MgStringCollection*)NULL;
        if (!message.empty())
        {
            strCol = new MgStringCollection();
            strCol->Add(message);
        }

        throw new MgInvalidFeatureSourceException(L"MgServerSelectFeatures.ValidateFeatureSource",
            __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
    }
}

MgResourceIdentifier* MgServerSelectFeatures::GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName)
{
    Ptr<MgResourceIdentifier> secResId;

    // look in the xml to find the resource for the specified relation
    string featureSourceXmlContent;
    RetrieveFeatureSource(primResId, featureSourceXmlContent);

    // Needed to parse XML and get properties
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(featureSourceXmlContent.c_str());

    DOMElement* rootNode = xmlUtil.GetRootNode();
    DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );

    if (NULL != extensionNodeList)
    {
        int extensionNodes = (int)extensionNodeList->getLength();

        for (int i = 0; i < extensionNodes; i++)
        {
            DOMNode* extensionNode = extensionNodeList->item(i);
            CHECKNULL(extensionNode, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");

            DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
            int nNameNodes = (int)nameNodeList->getLength();

            // get the extension name node
            DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

            // get the extension name value
            STRING name;
            xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, name);

            STRING parsedSchemaName = L"";
            STRING parsedExtensionName = L"";
            ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

            if (parsedExtensionName != name)
            {
                continue;
            }
            else
            {
                // Determine the number of secondary sources (AttributeRelate nodes)
                DOMNodeList* attributeRelateNodeList = xmlUtil.GetNodeList(extensionNode, "AttributeRelate");
                int nAttributeRelateNodes = (int)attributeRelateNodeList->getLength();

                // Find the the specified relation name
                for (int attributeRelateIndex = 0; attributeRelateIndex < nAttributeRelateNodes; attributeRelateIndex++)
                {
                    DOMNode* attributeRelateNode = attributeRelateNodeList->item(attributeRelateIndex);

                    // Get the name for the join relationship
                    wstring szAttributeRelateName;
                    xmlUtil.GetElementValue(attributeRelateNode, "Name", szAttributeRelateName);

                    if ( szAttributeRelateName != relationName )
                    {
                        continue;
                    }
                    else
                    {
                        // Get the secondary resource id
                        wstring szSecondaryResourceId;
                        xmlUtil.GetElementValue(attributeRelateNode, "ResourceId", szSecondaryResourceId);
                        secResId = new MgResourceIdentifier(szSecondaryResourceId);
                        break;
                    }
                }
            }
        }
    }

    return secResId.Detach();
}

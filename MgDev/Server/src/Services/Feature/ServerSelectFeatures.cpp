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

#include "ServerFeatureServiceDefs.h"
#include "MapGuideCommon.h"
#include "Services/FeatureService.h"
#include "ServerSelectFeatures.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureReader.h"
#include "ServerFeatureUtil.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "FeatureDistribution.h"
#include "ServerGwsFeatureReader.h"
#include "GwsConnectionPool.h"
#include "GwsCommonImp.h"
#include "GwsMutableFeature.h"
#include "GwsFdoCommand.h"
#include "GwsQuery.h"
#include "FdoExpressionEngineCopyFilter.h"
#include "CacheManager.h"

//Uncomment for extra console chatter to debug FDO joins
//#define DEBUG_FDO_JOIN

MgServerSelectFeatures::MgServerSelectFeatures()
{
    m_command = NULL;
    m_options = NULL;
    m_customPropertyFound = false;
    m_customFunction = NULL;
    m_customPropertyName = L"";
    m_featureSourceCacheItem = NULL;

    // Set a default join query batch size
    m_nJoinQueryBatchSize = MgConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize;
    m_bUseFdoJoinOptimization = MgConfigProperties::DefaultFeatureServicePropertyUseFdoJoinOptimization;

    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config)
    {
        // Get the join batch size
        config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                            MgConfigProperties::FeatureServicePropertyJoinQueryBatchSize,
                            m_nJoinQueryBatchSize,
                            MgConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize);
        // Get data cache size
        config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                            MgConfigProperties::FeatureServicePropertyDataCacheSize,
                            m_nDataCacheSize,
                            MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);
        // Get FDO Join use flag
        config->GetBoolValue(MgConfigProperties::FeatureServicePropertiesSection,
                             MgConfigProperties::FeatureServicePropertyUseFdoJoinOptimization,
                             m_bUseFdoJoinOptimization,
                             MgConfigProperties::DefaultFeatureServicePropertyUseFdoJoinOptimization);
    }
}

MgServerSelectFeatures::~MgServerSelectFeatures()
{
}

// Executes the select features command and serializes the reader
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

    // Retrieve the feature source
    if (NULL == m_featureSourceCacheItem.p)
    {
        MgCacheManager* cacheManager = MgCacheManager::GetInstance();
        m_featureSourceCacheItem = cacheManager->GetFeatureSourceCacheItem(resource);
    }

    // Set options (NULL is a valid value)
    m_options = SAFE_ADDREF(options);
    // Check if a feature join is to be performed by inspecting the resource for join properties
    bool bFeatureJoinProperties = FindFeatureJoinProperties(resource, className);
    // Check if a feature join is only a calculation
    bool bFeatureCalculation = FindFeatureCalculation(resource, className);
    //Test for the FDO join optimization, because performance is **substantially** better
    bool bSupportsFdoJoin = SupportsFdoJoin(resource, className, isSelectAggregate);

    // HACK: Sorry, we have to do this for now. Only SQLite provider satisifes the original
    // FDO join optimization in both regular select and select aggregate modes
    //
    // SQL Server has problems in aggregate mode. So we'll say the FDO join optimization is 
    // not supported when this is in aggregate mode. This means the join is handled by the
    // GWS query engine. SQL Server on SQL Server join will use the best algorithm (sort merge)
    // as both sides are sortable so we won't suffer too much of a performance penalty.
    if (isSelectAggregate)
    {
        MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
        const MdfModel::MdfString& providerName = featureSource->GetProvider();
        if (providerName.find(L"OSGeo.SQLite") == MdfString::npos) //NOXLATE
            bSupportsFdoJoin = false;
    }

#ifdef DEBUG_FDO_JOIN
    STRING fsIdStr = resource->ToString();
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) Testing Feature Source (%W, %W) for FDO join optimization"), fsIdStr.c_str(), className.c_str()));
#endif  
    if (!isSelectAggregate && bFeatureJoinProperties)
    {
        if (bSupportsFdoJoin)
        {
#ifdef DEBUG_FDO_JOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) Feature Source (%W) supports FDO join optimization"), fsIdStr.c_str()));
#endif
            m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select, m_options);
            mgReader = SelectFdoJoin(resource, className, false);
        }
        else 
        {
#ifdef DEBUG_FDO_JOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) Feature Source (%W) does not support the FDO join optimization. Using GwsQueryEngine"), fsIdStr.c_str()));
#endif
            // Get the FdoFilter from the options
            // Create Command
            CreateCommand(resource, isSelectAggregate);

            // Apply options to FDO command
            ApplyQueryOptions(isSelectAggregate);
            FdoPtr<FdoFilter> filter = m_command->GetFilter();
            // Perform feature join
            mgReader = JoinFeatures(resource, className, filter);
        }
    }
    else if (isSelectAggregate && bFeatureJoinProperties && bSupportsFdoJoin)
    {
        // NOTE: If this is FDO join capable, but contains functions over prefixed secondary properties
        // We have to go through the GWS Query Engine because we haven't implemented reverse mapping
        // of prefixed secondary class properties. Fortunately, the common case for this is distict value
        // queries (eg. Generating themeable values), and GWS Query Engine is surprisingly performant in this case
#ifdef DEBUG_FDO_JOIN
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) Feature Source (%W) supports aggregate FDO join optimization"), fsIdStr.c_str()));
#endif
        // Perform the same select query as above, but route this through the FDO expression engine
        // Slow maybe, but anything is faster than going via the GWS query engine.
        m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select, m_options);
        mgReader = SelectFdoJoin(resource, className, true);
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
        bool useClassName = true;
        if (isSelectAggregate && ContainsCustomFunction(options))
        {
            isSelectAggregate = false;
            useClassName = false;
        }
        else if (bFeatureJoinProperties)
            useClassName = false;

        Ptr<MgReader> reader;

        // Create Command
        CreateCommand(resource, isSelectAggregate);

        // Set the FeatureClass Name
        if (useClassName && !bFeatureCalculation)
        {
            m_command->SetFeatureClassName((FdoString*)className.c_str());
        }

        // Apply options to FDO command
        ApplyQueryOptions(isSelectAggregate);

        // Check if the specified className is an extension (join) in the feature source
        if (bFeatureJoinProperties && !bSupportsFdoJoin)
        {
            // Perform feature join to obtain the joined properties
            // Note: this gwsFeatureReader is just for temporary use. it will not be returned
            // to the web-tier. Therefore this object must be closed to avoid connection leak
            // I put the following code in a try-catch block just to make sure the gwsFeaturereader
            // gets chance to be closed in case that an exception is thrown from underneath
            Ptr<MgServerGwsFeatureReader> gwsFeatureReader = JoinFeatures(resource, className, NULL);
            try
            {
                // Get the requested property name from the MgFeatureServiceCommandObject.  This property name may be
                // prefixed by the relation name if it is from a secondary resource.
                FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
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

                FdoPtr<FdoIdentifier> fi = fic->GetItem(0);
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
                m_command->SetFeatureClassName((FdoString*)secClassName.c_str());
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
                if(nPropStart != STRING::npos)
                {
                    STRING::size_type nPropEnd = expression.find(L")", nPropStart + 1);

                    STRING newExpression = expression.substr(0, nPropStart);
                    newExpression += parsedPropertyName;
                    newExpression += expression.substr(nPropStart + nPropLength);
                    computedProperties->SetValue(aliasName, newExpression);
                }

                // Apply options to FDO command
                ApplyQueryOptions(isSelectAggregate);
                if (bFeatureCalculation)
                    UpdateCommandOnJoinCalculation(resource, className);
            }
            catch(...)
            {
                // Close the reader to avoid connection leak
                gwsFeatureReader->Close();
                throw;
            }

            // gwsFeatureReader will not be returned to the webtier. Therefore, we have to close it here since
            // it is no longer neened. If we don't do this, the connection it owns will leak.
            gwsFeatureReader->Close();
        }
        else if (bFeatureCalculation && !bFeatureJoinProperties)
            UpdateCommandOnCalculation(resource, className);
        else
            m_command->SetFeatureClassName((FdoString*)className.c_str());

        // If custom function is found, then validate that no more than one function/property is specified
        ValidateConstraintsOnCustomFunctions();

        // Execute the command
        reader = m_command->Execute();

        CHECKNULL((MgReader*)reader, L"MgServerSelectFeatures.SelectFeatures");

        if (executeSelectAggregate && m_customPropertyFound)
        {
            mgReader = this->GetCustomReader(reader);
            // Ticket 1051 MapGuide exhausts FDO connections during Fusion selection
            // We are done with "reader" but Close() is not called during the release
            // Close it here to avoid the connection leak
            reader->Close();
        }
        else
        {
            // Increment the refcount as it will be decremented when we go out of  try...catch
            mgReader = SAFE_ADDREF((MgReader*)reader);
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerSelectFeatures.SelectFeatures")

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
        ApplyFetchSize();
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

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyClassProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyClassProperties");

        fic->Add(fdoIden);
    }
}

void MgServerSelectFeatures::ApplyClassPropertiesForFdoJoin(CREFSTRING primaryAlias, CREFSTRING secondaryAlias, CREFSTRING secondaryPrefix)
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyClassPropertiesForFdoJoin");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyClassPropertiesForFdoJoin");

    Ptr<MgStringCollection> properties = m_options->GetClassProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyClassPropertiesForFdoJoin");

    //If we're given an explicit property list, it will be whatever is presented by the Extended Feature Class
    //So we have to "re-shape" this property list into a aliased qualified property list like the standard FDO
    //join query. Basically, were doing a reverse property mapping.

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        //Check if this name starts with prefix
        //
        // If it does, it's a secondary join property and should be re-aliased as [secondaryAlias].[propertyNameWithoutPrefix]
        // Otherwise, it should be re-aliased as [primaryAlias].[propertyNameAsIs]
        STRING reAliasedPropName;
        if (propertyName.compare(0, secondaryPrefix.length(), secondaryPrefix) == 0)
        {
            reAliasedPropName = secondaryAlias;
            reAliasedPropName += L".";
            reAliasedPropName += propertyName.substr(secondaryPrefix.length());
        }
        else
        {
            reAliasedPropName = primaryAlias;
            reAliasedPropName += L".";
            reAliasedPropName += propertyName;
        }

        //This will now be [alias].[reAliasedPropertyName] AS [propertyName]
        FdoPtr<FdoExpression> expr = FdoExpression::Parse((FdoString*)reAliasedPropName.c_str());
        FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create((FdoString*)propertyName.c_str(), expr);
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyClassPropertiesForFdoJoin");

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

    //TODO: Need to check if FDO join optimization is supported, and whether this contains prefixed
    //secondary properties in any expressions

    // TODO: Add support for custom functions

    for (INT32 i=0; i < cnt; i++)
    {
        STRING aliasName = properties->GetName(i);
        STRING expression = properties->GetValue(i);

        FdoString* str = (FdoString*)expression.c_str();
        if (str != NULL)
        {
            FdoPtr<FdoExpression> expression = FdoExpression::Parse(str);
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

// Fetch size
void MgServerSelectFeatures::ApplyFetchSize()
{
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyFetchSize");
    if(m_options)
        m_command->SetFetchSize(m_options->GetFetchSize());
    else
        m_command->SetFetchSize(m_nDataCacheSize);
}

// Spatial Filter
void MgServerSelectFeatures::ApplyFilter()
{
    CHECKNULL(m_options, L"MgServerSelectFeatures.ApplyFilter");
    CHECKNULL(m_command, L"MgServerSelectFeatures.ApplyFilter");

    FdoPtr<FdoFilter> regularFilter;
    FdoPtr<FdoSpatialCondition> spatialFilter;
    FdoPtr<FdoFilter> combineFilter;
    FdoBinaryLogicalOperations bOper = FdoBinaryLogicalOperations_And;

    // Build regular filter
    STRING filterText = m_options->GetFilter();
    if (!filterText.empty())
    {
        regularFilter = FdoFilter::Parse(filterText.c_str());
        #if defined(_DEBUG) || defined(DEBUG_FDO_JOIN)
        ACE_DEBUG((LM_ERROR, ACE_TEXT("FILTER(size=%d):\n%W\n\n"), filterText.length(), filterText.c_str()));
        #endif
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

        FdoByte* gisBytes = bytes->Bytes();
        INT32 len = bytes->GetLength();

        FdoPtr<FdoByteArray> byteArray = FdoByteArray::Create(gisBytes, (FdoInt32)len);

        #ifdef _DEBUG
        // Get the spatial filter geometry text
        FdoPtr<FdoFgfGeometryFactory> geometryFactory = FdoFgfGeometryFactory::GetInstance();
        if(geometryFactory)
        {
            FdoPtr<FdoIGeometry> geometry = geometryFactory->CreateGeometryFromFgf(byteArray);
            STRING geomText = geometry->GetText();
            ACE_DEBUG((LM_INFO, ACE_TEXT("(%t) SPATIAL FILTER:\n%W\n\n"), geomText.c_str()));
        }
        #endif

        FdoPtr<FdoGeometryValue> geomValue = FdoGeometryValue::Create(byteArray);
        if (geomValue != NULL)
        {
            FdoSpatialOperations fdoSpatialOp = MgServerFeatureUtil::GetFdoSpatialOperation(spatialOp);
            spatialFilter = FdoSpatialCondition::Create((FdoString*)geomProp.c_str(), fdoSpatialOp, (FdoExpression*)geomValue);
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
            combineFilter = FDO_SAFE_ADDREF((FdoFilter*)regularFilter);
        }
        else if (spatialFilter != NULL)
        {
            // Only spatial filter exists
            combineFilter = FDO_SAFE_ADDREF((FdoFilter*)spatialFilter);
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

    bool bSupportsOrdering = m_command->SupportsSelectOrdering();
    bool bExtended = m_command->IsExtended();
    // Not supported? We can still support if it supports extended select. This may look like a bit of hack
    // but the known implementations of FdoIExtendedSelect (SDF/SHP) will honor the ordering option if used
    if (!bSupportsOrdering)
    {
        bSupportsOrdering = (cnt == 1) && bExtended;
    }

    // Ordering options are supplied but provider does not support it
    if (!bSupportsOrdering)
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgOrderingOptionNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgServerSelectFeatures.ApplyOrderingOptions", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if ((cnt == 1) && bExtended)
    {
        // Order option Asc or Desc (default is Asc)
        FdoOrderingOption option = MgServerFeatureUtil::GetFdoOrderingOption(m_options->GetOrderOption());
        STRING propertyName = properties->GetItem(0);

        FdoPtr<FdoIdentifierCollection> fic = m_command->GetOrdering();
        CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyOrderingOptions");

        // Order option Asc or Desc (default is Asc)
        //
        // With extended select, we still have to make sure the base ordering option is set
        m_command->SetOrderingOption(option);
        FdoPtr<FdoIdentifier> ident = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        fic->Add(ident);

        m_command->SetExtendedOrderingOption((FdoString*)propertyName.c_str(), option);
    }
    else
    {
        FdoPtr<FdoIdentifierCollection> fic = m_command->GetOrdering();
        CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyOrderingOptions");

        // Order option Asc or Desc (default is Asc)
        FdoOrderingOption option = MgServerFeatureUtil::GetFdoOrderingOption(m_options->GetOrderOption());
        m_command->SetOrderingOption(option);

        for (INT32 i = 0; i < cnt; i++)
        {
            STRING propertyName = properties->GetItem(i);

            if (!propertyName.empty())
            {
                FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
                CHECKNULL((FdoIdentifier*)fdoIden, L"MgServerSelectFeatures.ApplyOrderingOptions");

                fic->Add(fdoIden);
            }
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

    FdoString* funcNameAllowed = fdoFunc->GetName();
    if (funcNameAllowed != NULL)
    {
        INT32 funcIndex = -1;
        customFunc = MgServerFeatureUtil::FindCustomFunction(STRING(funcNameAllowed),funcIndex);
    }

    return customFunc;
}

void MgServerSelectFeatures::AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.AddFdoComputedProperty");

    FdoString* expName = aliasName.c_str();
    if (expName != NULL)
    {
        FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expression);
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"MgServerSelectFeatures.AddFdoComputedProperty");

        fic->Add(fdoIden);
    }
}

void MgServerSelectFeatures::AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    CHECKARGUMENTNULL((FdoExpression*)expression, L"MgServerSelectFeatures.AddCustomComputedProperty");

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
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
        FdoString* expName = aliasName.c_str();
        if (expName != NULL)
        {
            FdoPtr<FdoExpressionCollection> exprCol = function->GetArguments();
            FdoInt32 cnt = exprCol->GetCount();
            FdoPtr<FdoExpression> expr;
            if (cnt > 0)
            {
                expr = exprCol->GetItem(0);   // Property Name
            }

            // Just pass in the property name
            // FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expr);

            // NOTE: Each provider has its own rule for supporting computed properties, select and select aggregate
            // functionality. Therefore we just work with simple select command, fetch the property and do the needed
            // calculations. Therefore, we are not adding them as computed properties.

            FdoIdentifier* propName = dynamic_cast<FdoIdentifier*>(expr.p);

            if (propName != NULL)
                fic->Add(propName);

            m_customPropertyName = aliasName;
            m_customPropertyFound = true;
            m_customFunction = FDO_SAFE_ADDREF(function);
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
        m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select, m_options);
    }
    else
    {
        m_command = MgFeatureServiceCommand::CreateCommand(resource, FdoCommandType_SelectAggregates, m_options);
    }
    CHECKNULL((MgFeatureServiceCommand*)m_command, L"MgServerSelectFeatures.CreateCommand");
}

void MgServerSelectFeatures::ValidateParam(MgResourceIdentifier* resource, CREFSTRING className)
{
    // className and resource identifier can not be NULL
    CHECKARGUMENTNULL(resource, L"MgServerSelectFeatures::ValidateParam");
    CHECKARGUMENTEMPTYSTRING(className, L"MgServerSelectFeatures::ValidateParam");
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
    FdoPtr<FdoFilter> filter;
    if (!groupFilter.empty())
    {
        filter = FdoFilter::Parse((FdoString*)groupFilter.c_str());
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

    FdoPtr<FdoIdentifierCollection> fic = ((MgFeatureServiceCommand*)m_command)->GetGrouping();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgServerSelectFeatures.ApplyFdoGroupingProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
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

        FdoString* str = (FdoString*)expression.c_str();
        if (str != NULL)
        {
            FdoPtr<FdoExpression> expression = FdoExpression::Parse(str);
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

// Look for extension which have calculations but no joins
bool MgServerSelectFeatures::FindFeatureCalculation(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bCalculationExists = false;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.FindFeatureCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.FindFeatureCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.FindFeatureCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            // we don't have joins but we have calculations
            bCalculationExists = (calcProps != NULL && calcProps->GetCount() != 0);
            break;
        }
    }

    return bCalculationExists;
}

// Look for extension (feature join) properties in the feature source document
bool MgServerSelectFeatures::FindFeatureJoinProperties(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bJoinPropertiesExists = false;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.FindFeatureJoinProperties");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.FindFeatureJoinProperties");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.FindFeatureJoinProperties");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            AttributeRelateCollection* relates = extension->GetAttributeRelates();
            bJoinPropertiesExists = (relates != NULL && relates->GetCount() != 0);
            break;
        }
    }

    return bJoinPropertiesExists;
}

void MgServerSelectFeatures::UpdateCommandOnJoinCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.UpdateCommandOnJoinCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.UpdateCommandOnJoinCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.UpdateCommandOnJoinCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        ParseQualifiedClassNameForCalculation(extension, extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps == NULL || calcProps->GetCount() == 0)
                break;
            FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                idList->Add(idfCalc);
            }
            FdoPtr<FdoFilter> filter = m_command->GetFilter();
            if (filter != NULL)
            {
                FdoPtr<FdoFilter> newFilter = FdoExpressionEngineCopyFilter::Copy(filter, idList);
                m_command->SetFilter(newFilter);
            }

            FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
            if (fic->GetCount() != 0)
            {
                // replace calculated properties provided as identifiers to computed identifiers
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoIdentifier> idf = fic->FindItem(calcProp->GetName().c_str());
                    if (idf != NULL)
                    {
                        FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(idf->GetName()));
                        FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                        FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                        int idfIndex = fic->IndexOf(idf);
                        FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(idf->GetName(), expandedExpression);
                        fic->SetItem(idfIndex, newIdf);
                    }
                }
            }
            break;
        }
    }
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgServerSelectFeatures.UpdateCommandOnJoinCalculation")
}

void MgServerSelectFeatures::UpdateCommandOnCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.UpdateCommandOnCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.UpdateCommandOnCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.UpdateCommandOnCalculation");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        ParseQualifiedClassNameForCalculation(extension, extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            m_command->SetFeatureClassName(extension->GetFeatureClass().c_str());
            CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps == NULL || calcProps->GetCount() == 0)
                break;
            FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                idList->Add(idfCalc);
            }
            FdoPtr<FdoFilter> filter = m_command->GetFilter();
            if (filter != NULL)
            {
                FdoPtr<FdoFilter> newFilter = FdoExpressionEngineCopyFilter::Copy(filter, idList);
                m_command->SetFilter(newFilter);
            }

            FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
            bool addAllProps = (fic->GetCount() == 0);
            for (int idx = 0; idx < calcProps->GetCount(); idx++)
            {
                CalculatedProperty* calcProp = calcProps->GetAt(idx);
                FdoPtr<FdoIdentifier> idf = fic->FindItem(calcProp->GetName().c_str());
                if (idf != NULL)
                {
                    // replace calculated properties provided as identifiers to computed identifiers
                    FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(idf->GetName()));
                    FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                    FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                    int idfIndex = fic->IndexOf(idf);
                    FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(idf->GetName(), expandedExpression);
                    fic->SetItem(idfIndex, newIdf);
                }
                else
                {
                    if (addAllProps)
                    {
                        FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(calcProp->GetName().c_str()));
                        FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                        FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                        FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expandedExpression);
                        fic->Add(newIdf);
                    }
                }
                if (addAllProps)
                {
                    Ptr<MgServerFeatureConnection> fcConnection = new MgServerFeatureConnection(featureSourceId);
                    if ((NULL != fcConnection.p) && ( fcConnection->IsConnectionOpen() ))
                    {
                        // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
                        // otherwise it leaves the FDO connection marked as still in use.
                        FdoPtr<FdoIConnection> conn = fcConnection->GetConnection();
                        FdoPtr<FdoIDescribeSchema>  descSchema = (FdoIDescribeSchema *) conn->CreateCommand (FdoCommandType_DescribeSchema);

                        STRING fullClassName = extension->GetFeatureClass();
                        STRING schemaName, className;
                        MgUtil::ParseQualifiedClassName(fullClassName, schemaName, className);

                        if (!parsedSchemaName.empty())
                        {
                            descSchema->SetSchemaName(parsedSchemaName.c_str());
                        }

                        if (!className.empty())
                        {
                            FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();

                            classNames->Add(className.c_str());
                            descSchema->SetClassNames(classNames.p);
                        }

                        FdoPtr <FdoFeatureSchemaCollection> schemas = (FdoFeatureSchemaCollection *) descSchema->Execute ();
                        FdoPtr<FdoFeatureSchema> schema = (FdoFeatureSchema *)schemas->GetItem (parsedSchemaName.c_str());
                        FdoPtr<FdoClassCollection> classes = schema->GetClasses();

                        FdoPtr<FdoClassDefinition> activeClass = classes->GetItem(className.c_str());
                        FdoPtr<FdoPropertyDefinitionCollection> properties = activeClass->GetProperties();
                        for(int i = 0; i < properties->GetCount(); i++)
                        {
                            FdoPtr<FdoPropertyDefinition> activeProperty = properties->GetItem(i);
                            FdoPtr<FdoIdentifier> idf = fic->FindItem(activeProperty->GetName());
                            if (idf == NULL)
                            {
                                idf = FdoIdentifier::Create(activeProperty->GetName());
                                fic->Add(idf);
                            }
                        }
                        FdoPtr<FdoReadOnlyPropertyDefinitionCollection> baseProps = activeClass->GetBaseProperties();
                        if (baseProps != NULL)
                        {
                            for(int i = 0; i < baseProps->GetCount(); i++)
                            {
                                FdoPtr<FdoPropertyDefinition>prop = baseProps->GetItem(i);
                                if( prop->GetIsSystem() )
                                    continue;
                                fic->Add( FdoPtr<FdoIdentifier>(FdoIdentifier::Create( prop->GetName() ) ) );
                            }
                        }
                    }
                }
            }
            break;
        }
    }
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgServerSelectFeatures.UpdateCommandOnCalculation")
}

MgServerGwsFeatureReader* MgServerSelectFeatures::JoinFeatures(MgResourceIdentifier* featureSourceIdentifier, CREFSTRING extensionName, FdoFilter* filter)
{
    Ptr<MgServerGwsFeatureReader> gwsFeatureReader;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<IGWSQueryDefinition> qd;
    FdoPtr<MgGwsConnectionPool> pool = MgGwsConnectionPool::Create();

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.JoinFeatures");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.JoinFeatures");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.JoinFeatures");
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            // Establish connection to provider for primary feature source
            STRING primaryConnectionName;
            MgUtil::GenerateUuid(primaryConnectionName);
            Ptr<MgServerFeatureConnection> msfcLeft = new MgServerFeatureConnection(featureSourceIdentifier);
            if ((NULL != msfcLeft.p) && ( msfcLeft->IsConnectionOpen() ))
            {
                pool->AddConnection(primaryConnectionName.c_str(), msfcLeft);
            }
            else
            {
                throw new MgConnectionFailedException(L"MgServerSelectFeatures.JoinFeatures",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

            // Retrieve the primary feature class
            STRING featureClass = (STRING)extension->GetFeatureClass();

            // Parse the qualifed classname
            STRING primaryFsSchema, primaryFsClassName;
            MgUtil::ParseQualifiedClassName(featureClass, primaryFsSchema, primaryFsClassName);

            // Create primary query definition
            FdoPtr<FdoIdentifierCollection> lsellist;
            FdoPtr<FdoFilter> lfilter;

            // in case we have calculations we must add all properties + computed properties to the selection list
            MdfModel::CalculatedPropertyCollection* calcProps = extension->GetCalculatedProperties();
            if (calcProps != NULL && calcProps->GetCount() != 0)
            {
                FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                    FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                    idList->Add(idfCalc);
                }
                lsellist = FdoIdentifierCollection::Create();
                for (int idx = 0; idx < calcProps->GetCount(); idx++)
                {
                    CalculatedProperty* calcProp = calcProps->GetAt(idx);
                    FdoPtr<FdoComputedIdentifier> idfCalc = static_cast<FdoComputedIdentifier*>(idList->GetItem(calcProp->GetName().c_str()));
                    FdoPtr<FdoExpression> expressionCalc = idfCalc->GetExpression();
                    FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expressionCalc, idList);

                    FdoPtr<FdoComputedIdentifier> newIdf = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expandedExpression);
                    lsellist->Add(newIdf);
                }
                FdoPtr<FdoIConnection> conn = msfcLeft->GetConnection();
                FdoPtr<FdoIDescribeSchema>  descSchema = (FdoIDescribeSchema *) conn->CreateCommand (FdoCommandType_DescribeSchema);

                STRING fullClassName = extension->GetFeatureClass();
                STRING schemaName, className;
                MgUtil::ParseQualifiedClassName(fullClassName, schemaName, className);

                if (!parsedSchemaName.empty())
                {
                    descSchema->SetSchemaName(parsedSchemaName.c_str());
                }

                if (!className.empty())
                {
                    FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();

                    classNames->Add(className.c_str());
                    descSchema->SetClassNames(classNames.p);
                }

                FdoPtr <FdoFeatureSchemaCollection> schemas = (FdoFeatureSchemaCollection *) descSchema->Execute ();
                FdoPtr<FdoFeatureSchema> schema = (FdoFeatureSchema *)schemas->GetItem (parsedSchemaName.c_str());
                FdoPtr<FdoClassCollection> classes = schema->GetClasses();

                FdoPtr<FdoClassDefinition> activeClass = classes->GetItem(className.c_str());
                FdoPtr<FdoPropertyDefinitionCollection> properties = activeClass->GetProperties();
                for(int i = 0; i < properties->GetCount(); i++)
                {
                    FdoPtr<FdoPropertyDefinition> activeProperty = properties->GetItem(i);
                    FdoPtr<FdoIdentifier> idf = FdoIdentifier::Create(activeProperty->GetName());
                    lsellist->Add(idf);
                }
                FdoPtr<FdoReadOnlyPropertyDefinitionCollection> baseProps = activeClass->GetBaseProperties();
                if (baseProps != NULL)
                {
                    for(int i = 0; i < baseProps->GetCount(); i++)
                    {
                        FdoPtr<FdoPropertyDefinition>prop = baseProps->GetItem(i);
                        if( prop->GetIsSystem() )
                            continue;
                        lsellist->Add( FdoPtr<FdoIdentifier>(FdoIdentifier::Create( prop->GetName() ) ) );
                    }
                }
            }

            FdoPtr<IGWSQueryDefinition> lqd = IGWSFeatureQueryDefinition::Create(
                lsellist,
                GWSQualifiedName(primaryConnectionName.c_str(), primaryFsSchema.c_str(), primaryFsClassName.c_str()),
                lfilter);
            CHECKNULL(lqd, L"MgServerSelectFeatures.JoinFeatures");
            qd = lqd;

            IGWSJoinQueryDefinition* jqd = NULL;

            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgServerSelectFeatures.JoinFeatures");

            bool bForceOneToOne = true;
            Ptr<MgStringCollection> attributeNameDelimiters = new MgStringCollection();

            //Store all the secondary's name, and if the filter property doesn't contain secondary's name,
            //it applies only to the primary.
            Ptr<MgStringCollection> secondaryNames = new MgStringCollection();

            // For each join (attributeRelate) to a secondary source need to do the following
            for (int attributeRelateIndex = 0; attributeRelateIndex < attributeRelates->GetCount(); attributeRelateIndex++)
            {
                MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                CHECKNULL(attributeRelate, L"MgServerSelectFeatures.JoinFeatures");

                // Get the secondary resource id
                STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                // Get the name for the join relationship
                STRING attributeRelateName = (STRING)attributeRelate->GetName();
                STRING secondaryConnectionName = attributeRelateName;

                if(!secondaryNames->Contains(attributeRelateName))
                    secondaryNames->Add(attributeRelateName);

                // Get the RelateType (join type).  Default is Left Outer join.
                MdfModel::AttributeRelate::RelateType relateType = attributeRelate->GetRelateType();

                // Get the ForceOneToOne field, which specifies if multiple matching secondary features
                // are retrieved via a 1-to-1 or 1-to-many relationship.  Default is 1-to-1 relationship.
                bool forceOneToOne = attributeRelate->GetForceOneToOne();
                // If there is at least one relation is defined as one-to-many, then the one-to-many result will apply to all join results.
                if (!forceOneToOne)
                {
                    bForceOneToOne = false;
                }

                // Get the AttributeNameDelimiter field, which specifies the delimiter between the JoinName (attribute relate name)
                // and the property name for an extended property.  Default delimiter is "" (blank).
                STRING attributeNameDelimiter = (STRING)attributeRelate->GetAttributeNameDelimiter();
                attributeNameDelimiters->Add(attributeNameDelimiter);

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);

                if (NULL != secondaryFeatureSource)
                {
                    Ptr<MgServerFeatureConnection> msfcRight = new MgServerFeatureConnection(secondaryFeatureSource);
                    if ((NULL != msfcRight.p) && ( msfcRight->IsConnectionOpen() ))
                    {
                        pool->AddConnection(secondaryConnectionName.c_str(), msfcRight);
                    }
                    else
                    {
                        throw new MgConnectionFailedException(L"MgServerSelectFeatures.JoinFeatures",
                            __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }

                // Get the secondary featureClassName (qualified className)
                STRING secondaryClassName = (STRING)attributeRelate->GetAttributeClass();

                // Parse the qualified classname
                STRING secondaryFsSchema, secondaryFsClassName;
                MgUtil::ParseQualifiedClassName(secondaryClassName, secondaryFsSchema, secondaryFsClassName);

                // Create secondary query definition
                FdoPtr<FdoIdentifierCollection> rsellist;
                FdoPtr<FdoFilter> rfilter;

                FdoPtr<IGWSQueryDefinition> rqd  = IGWSFeatureQueryDefinition::Create(
                    rsellist,
                    GWSQualifiedName(secondaryConnectionName.c_str(), secondaryFsSchema.c_str(), secondaryFsClassName.c_str()),
                    rfilter);
                CHECKNULL(rqd, L"MgServerSelectFeatures.JoinFeatures");

                // Get Join Attributes
                FdoPtr<FdoStringCollection> lattrs = FdoStringCollection::Create();
                FdoPtr<FdoStringCollection> rattrs = FdoStringCollection::Create();

                // Determine the number of RelateProperties (attributes)
                MdfModel::RelatePropertyCollection* relateProperties = attributeRelate->GetRelateProperties();
                CHECKNULL(relateProperties, L"MgServerSelectFeatures.JoinFeatures");
                int nRelatePropertyCount = relateProperties->GetCount();

                // For each RelateProperty need to do the following
                for (int relatePropertyIndex = 0; relatePropertyIndex < nRelatePropertyCount; relatePropertyIndex++)
                {
                    MdfModel::RelateProperty* relateProperty = relateProperties->GetAt(relatePropertyIndex);
                    CHECKNULL(relateProperty, L"MgServerSelectFeatures.JoinFeatures");

                    // Get the FeatureClassProperty (primary attribute)
                    STRING primaryAttribute = (STRING)relateProperty->GetFeatureClassProperty();

                    // Add to the primary attribute String collection
                    lattrs->Add(primaryAttribute.c_str());

                    // Get the AttributeClassProperty (secondary attribute)
                    STRING secondaryAttribute = (STRING)relateProperty->GetAttributeClassProperty();

                    // Add to the secondary attribute String collection
                    rattrs->Add(secondaryAttribute.c_str());
                }

                // Create the QueryDefinition
                if (NULL != rqd)
                {
                    FdoString* joinName = attributeRelateName.c_str();
                    FdoString* joinDelimiter = L".";
                    if (MdfModel::AttributeRelate::Inner == relateType)
                    {
                        jqd = IGWSEqualJoinQueryDefinition::Create(joinName, joinDelimiter, bForceOneToOne, lqd, rqd, lattrs, rattrs);
                    }
                    else  // if (RelateType::LeftOuter == relateType)
                    {
                        jqd = IGWSLeftJoinQueryDefinition::Create(joinName, joinDelimiter, bForceOneToOne, lqd, rqd, lattrs, rattrs);
                    }
                    lqd = jqd;
                }

            }  // Repeat for each secondary source
            qd = lqd;

            FdoPtr<IGWSQuery> query = IGWSQuery::Create(pool, qd, NULL);
            FdoPtr<IGWSFeatureIterator> iter;
            FdoPtr<IGWSFeatureIterator> iterCopy;

            // Set batch size as it may be needed
            CGwsBatchSortedBlockJoinQueryResults::sm_nBatchSize = m_nJoinQueryBatchSize;

            // Prepare and Execute Query
            query->Prepare();

            // Search the filter to see if it contains the extension name
            // If the extension name is not found it means that the filter involves attribute(s) only from the primary
            if(NULL != filter)
            {
                FdoString* filterText = filter->ToString();
                if(NULL != filterText)
                {
                    bool match = false;
                    for(int i = 0; i < secondaryNames->GetCount(); i ++)
                    {
                        STRING secondaryName = secondaryNames->GetItem(i);
                        if(NULL != wcsstr(filterText, secondaryName.c_str()))
                        {
                            match = true;
                            break;
                        }
                    }
                    if(!match)
                    {
                        // Add the filter to the query because it only applies to the primary
                        query->SetFilter(filter);
                    }
                }
            }

            // Execute the query
            query->Execute(&iter, true);
            query->Execute(&iterCopy, true);

            FdoPtr<FdoStringCollection> fsNames = qd->FeatureSourceNames();

            gwsFeatureReader = new MgServerGwsFeatureReader(pool, iter, iterCopy, parsedExtensionName, fsNames, bForceOneToOne, attributeNameDelimiters);
            gwsFeatureReader->SetFilter(filter);
            break;
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceIdentifier, L"MgServerSelectFeatures.JoinFeatures")

    // Now that the reader has been created we will need to mark all of the connections it uses as OwnReader() because the GWS reader will be
    // taking ownership of the connections. We have to do it this late in the code in case an exception is thrown somewhere before this.
    // We want to avoid a deadlock of the connection :)
    gwsFeatureReader->OwnsConnections();

    return gwsFeatureReader.Detach();
}

void MgServerSelectFeatures::ParseQualifiedClassNameForCalculation(MdfModel::Extension* extension, CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className)
{
    CHECKARGUMENTNULL(extension, L"MgServerSelectFeatures.ParseQualifiedClassNameForCalculation");

    MgUtil::ParseQualifiedClassName(qualifiedClassName, schemaName, className);

    if (schemaName.empty())
    {
        STRING dummyStr;

        MgUtil::ParseQualifiedClassName(extension->GetFeatureClass(), schemaName, dummyStr);
    }
}

MgResourceIdentifier* MgServerSelectFeatures::GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName)
{
    Ptr<MgResourceIdentifier> secResId;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");

        // Get the extension name
        STRING name = (STRING)extension->GetName();

        STRING parsedSchemaName, parsedExtensionName;
        MgUtil::ParseQualifiedClassName(extensionName, parsedSchemaName, parsedExtensionName);

        if (parsedExtensionName != name)
        {
            continue;
        }
        else
        {
            // Determine the number of secondary sources (AttributeRelates)
            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");
            int nAttributeRelateCount = attributeRelates->GetCount();

            // Find the specified relation name
            {
                for (int attributeRelateIndex = 0; attributeRelateIndex < nAttributeRelateCount; attributeRelateIndex++)
                {
                    MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                    CHECKNULL(attributeRelate, L"MgServerSelectFeatures.GetSecondaryResourceIdentifier");

                    // Get the name for the join relationship
                    STRING attributeRelateName = (STRING)attributeRelate->GetName();

                    if (attributeRelateName != relationName)
                    {
                        continue;
                    }
                    else
                    {
                        // Get the secondary resource id
                        STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();
                        secResId = new MgResourceIdentifier(secondaryResourceId);
                        break;
                    }
                }
            }
        }
    }

    return secResId.Detach();
}

bool MgServerSelectFeatures::SupportsFdoJoin(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, bool isAggregate)
{
    bool bSupported = false;

    //If disabled on a global level, don't even bother continuing
    if (!m_bUseFdoJoinOptimization)
        return false;

    MG_FEATURE_SERVICE_TRY()

    //This could be qualified, so parse it to be sure
    STRING schemaName;
    STRING extName;
    MgUtil::ParseQualifiedClassName(extensionName, schemaName, extName);

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.SupportsFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.SupportsFdoJoin");

    MdfModel::Extension* extension = NULL;
    for (INT32 i = 0; i < extensions->GetCount(); i++) 
    {
        MdfModel::Extension* ext = extensions->GetAt(i);
        if (ext->GetName() == extName)
        {
            extension = ext;
            break;
        }
    }

    if (NULL == extension) //Extension in question not found
    {
#ifdef DEBUG_FDO_JOIN
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] Could not find extension named: %W"), extensionName.c_str()));
#endif
        return false;
    }

    Ptr<MgServerFeatureConnection> conn = new MgServerFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgConnectionFailedException(L"MgServerSelectFeatures.SupportsFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();
        FdoPtr<FdoIConnectionCapabilities> connCaps = fdoConn->GetConnectionCapabilities();
        
        MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();

        //Get the easy checks out of the way
        if (!connCaps->SupportsJoins())
        {
#ifdef DEBUG_FDO_JOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The provider does not support the FDO join APIs")));
#endif
            return false;
        }

        FdoJoinType jtypes = (FdoJoinType)connCaps->GetJoinTypes();

        //Now ascertain if all participating feature classes originate from the same feature source
        
        //We've yet to figure out chained joins. TODO: Revisit later
        if (relates->GetCount() > 1)
        {
#ifdef DEBUG_FDO_JOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] Chained/Multiple FDO Joins not supported yet")));
#endif
            return false;
        }

        //Need to check the filter here to see if it involves secondary class properties and/or
        //expressions involving such properties. We don't support this yet
        if (m_options != NULL && relates->GetCount())
        {
            MdfModel::AttributeRelate* relate = relates->GetAt(0);
            STRING filterText = m_options->GetFilter();
            STRING qualifiedName = relate->GetAttributeClass();
            STRING schemaName;
            STRING clsName;
            MgUtil::ParseQualifiedClassName(qualifiedName, schemaName, clsName);
            if (FilterContainsSecondaryProperties(featureSourceId, filterText, schemaName, clsName, relate->GetName()))
            {
    #ifdef DEBUG_FDO_JOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] Filter contains secondary class properties")));
    #endif
                return false;
            }
        }

        for (INT32 i = 0; i < relates->GetCount(); i++)
        {
            MdfModel::AttributeRelate* relate = relates->GetAt(i);
            const MdfModel::MdfString& fsId = relate->GetResourceId();

            //Different feature sources
            if (featureSourceId->ToString() != fsId)
            {
#ifdef DEBUG_FDO_JOIN
                ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The extension does not join with another class from the same data store")));
#endif
                return false;
            }

            //TODO: Review when we lift the one join limit
            if (NULL != (MgFeatureQueryOptions*)m_options)
            {
                //Is this costly? Should we cache it?
                //FdoPtr<FdoFunctionDefinitionCollection> functions = FdoExpressionEngine::GetStandardFunctions();
                Ptr<MgStringPropertyCollection> expressions = m_options->GetComputedProperties();
                for (INT32 i = 0; i < expressions->GetCount(); i++)
                {
                    Ptr<MgStringProperty> compProp = expressions->GetItem(i);
                    STRING exprText = compProp->GetValue();
                    FdoPtr<FdoExpression> fdoExpr = FdoExpression::Parse(exprText.c_str());

                    if (fdoExpr->GetExpressionType() == FdoExpressionItemType_Function)
                    {
                        FdoFunction* func = static_cast<FdoFunction*>(fdoExpr.p);
                        FdoString* funcName = func->GetName();

                        // We don't support functions on prefixed secondary properties (yet)
                        // so we have to check these functions to ensure they are only operating on the 
                        // primary side properties
                        STRING primarySchemaName;
                        STRING primaryClassName;
                        MgUtil::ParseQualifiedClassName(extension->GetFeatureClass(), primarySchemaName, primaryClassName);
                        bool bValidFunction = IsFunctionOnPrimaryProperty(func, fdoConn, primarySchemaName, primaryClassName);
                        if (!bValidFunction)
                        {
#ifdef DEBUG_FDO_JOIN
                            ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] FDO Function %W contains a non-primary or unrecognised identifier"), funcName));
#endif
                            return false;
                        }
                    }
                }
            }

            MdfModel::RelatePropertyCollection* relProps = relate->GetRelateProperties();

            //Check if the join type is supported. Given FDO exposes more join types than
            //the ones specified here, the chances are real good that we'll have a match
            MdfModel::AttributeRelate::RelateType rtype = relate->GetRelateType();
            switch(rtype)
            {
            case MdfModel::AttributeRelate::Inner:
                if ((jtypes & FdoJoinType_Inner) != FdoJoinType_Inner)
                {
#ifdef DEBUG_FDO_JOIN
                    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The provider does not support Inner Join")));
#endif
                    return false;
                }
                break;
            case MdfModel::AttributeRelate::LeftOuter:
                if ((jtypes & FdoJoinType_LeftOuter) != FdoJoinType_LeftOuter)
                {
#ifdef DEBUG_FDO_JOIN
                    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The provider does not support Left Outer Join")));
#endif
                    return false;
                }
                break;
            case MdfModel::AttributeRelate::RightOuter:
                if ((jtypes & FdoJoinType_RightOuter) != FdoJoinType_RightOuter)
                {
#ifdef DEBUG_FDO_JOIN
                    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The provider does not support Right Outer Join")));
#endif
                    return false;
                }
                break;
            default:
                {
#ifdef DEBUG_FDO_JOIN
                    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The join type is not recognised")));
#endif
                    return false;
                }
            }
        }

        //Still here? You pass the test
        bSupported = true;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSelectFeatures.SupportsFdoJoin")

    return bSupported;
}

bool MgServerSelectFeatures::IsFunctionOnPrimaryProperty(FdoFunction* function, FdoIConnection* fdoConn, CREFSTRING schemaName, CREFSTRING className)
{
    FdoPtr<FdoIdentifierCollection> identifiers = MgServerFeatureUtil::ExtractIdentifiers(function);
    if (identifiers->GetCount() == 0)
        return true; //Inconsequential

    FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
    CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgServerSelectFeatures.SelectFdoJoin");

    if (!schemaName.empty())
    {
        descSchema->SetSchemaName(schemaName.c_str());
    }
    if (!className.empty())
    {
        FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();
        classNames->Add(className.c_str());
        descSchema->SetClassNames(classNames);
    }

    FdoPtr<FdoClassDefinition> classDef;
    FdoPtr<FdoFeatureSchemaCollection> schemas = descSchema->Execute();
    for (FdoInt32 i = 0; i < schemas->GetCount(); i++)
    {
        FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
        if (wcscmp(schema->GetName(), schemaName.c_str()) == 0) 
        {
            FdoPtr<FdoClassCollection> classes = schema->GetClasses();
            for (FdoInt32 j = 0; j < classes->GetCount(); j++) 
            {
                FdoPtr<FdoClassDefinition> klassDef = classes->GetItem(j);
                if (wcscmp(klassDef->GetName(), className.c_str()) == 0)
                {
                    classDef = SAFE_ADDREF(klassDef.p);
                    break;
                }
            }
        }
    }

    if (NULL == (FdoClassDefinition*)classDef)
    {
        //TODO: Refine message if available
        throw new MgClassNotFoundException(L"MgServerSelectFeatures.IsFunctionOnPrimaryProperty", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoPtr<FdoPropertyDefinitionCollection> properties = classDef->GetProperties();
    for (FdoInt32 i = 0; i < identifiers->GetCount(); i++)
    {
        FdoPtr<FdoIdentifier> identifier = identifiers->GetItem(i);
        FdoString* name = identifier->GetName();
        if (properties->IndexOf(name) < 0) //Not in primary class or not recognised
        {
#ifdef DEBUG_FDO_JOIN
            FdoString* funcName = function->GetName();
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FDO Join Test] The aggregate function %W contains an unknown or non-primary identifier: %W"), funcName, name));
#endif
            return false;
        }
    }

    return true;
}

bool MgServerSelectFeatures::FilterContainsSecondaryProperties(MgResourceIdentifier* featureSourceId, CREFSTRING filter, STRING secondarySchema, STRING secondaryClassName, STRING secondaryPrefix)
{
    if (filter.empty())
        return false;

    //TODO: There's probably a more efficient way to do this without needing to fetch the secondary
    //class definition. But we're aiming for functionality and simplicity first.
    Ptr<MgServerFeatureConnection> conn = new MgServerFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgConnectionFailedException(L"MgServerSelectFeatures.SupportsFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();

        FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
        CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgServerSelectFeatures.SelectFdoJoin");

        if (!secondarySchema.empty())
        {
            descSchema->SetSchemaName(secondarySchema.c_str());
        }
        if (!secondaryClassName.empty())
        {
            FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();
            classNames->Add(secondaryClassName.c_str());
            descSchema->SetClassNames(classNames);
        }

        FdoPtr<FdoClassDefinition> classDef;
        FdoPtr<FdoFeatureSchemaCollection> schemas = descSchema->Execute();
        for (FdoInt32 i = 0; i < schemas->GetCount(); i++)
        {
            FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
            if (wcscmp(schema->GetName(), secondarySchema.c_str()) == 0) 
            {
                FdoPtr<FdoClassCollection> classes = schema->GetClasses();
                for (FdoInt32 j = 0; j < classes->GetCount(); j++) 
                {
                    FdoPtr<FdoClassDefinition> klassDef = classes->GetItem(j);
                    if (wcscmp(klassDef->GetName(), secondaryClassName.c_str()) == 0)
                    {
                        classDef = SAFE_ADDREF(klassDef.p);
                        break;
                    }
                }
            }
        }

        if (NULL == (FdoClassDefinition*)classDef)
        {
            //TODO: Refine message if available
            throw new MgClassNotFoundException(L"MgServerSelectFeatures.FilterContainsSecondaryProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoPropertyDefinitionCollection> propDefs = classDef->GetProperties();
        for (INT32 i = 0; i < propDefs->GetCount(); i++)
        {
            FdoPtr<FdoPropertyDefinition> propDef = propDefs->GetItem(i);
            STRING findStr = secondaryPrefix;
            findStr += propDef->GetName();

            if (filter.find(findStr) != STRING::npos)
                return true; //The filter string contains this extended feature class property
        }
    }
    return false;
}

MgReader* MgServerSelectFeatures::SelectFdoJoin(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, bool isAggregate)
{
    // TODO: This does not handle filters on the secondary side (yet)
    // Can GwsQueryEngine do this?

    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

    //This could be qualified, so parse it to be sure
    STRING schemaName;
    STRING extName;
    MgUtil::ParseQualifiedClassName(extensionName, schemaName, extName);

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.SelectFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.SelectFdoJoin");

    MdfModel::Extension* extension = NULL;
    for (INT32 i = 0; i < extensions->GetCount(); i++) 
    {
        MdfModel::Extension* ext = extensions->GetAt(i);
        if (ext->GetName() == extName)
        {
            extension = ext;
            break;
        }
    }

    CHECKNULL(extension, L"MgServerSelectFeatures.SelectFdoJoin");
    FdoString* clsName = extension->GetFeatureClass().c_str();
    m_command->SetFeatureClassName(clsName);
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FdoISelect] Set primary feature class: %W"), clsName));
#endif
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();
    CHECKNULL(relates, L"MgServerSelectFeatures.SelectFdoJoin");
    MdfModel::AttributeRelate* relate = relates->GetAt(0);

    const MdfModel::MdfString& prefix = relate->GetName();

    STRING primaryAlias = PRIMARY_ALIAS;
    STRING secondaryAlias = SECONDARY_ALIAS;

    FdoPtr<FdoJoinCriteriaCollection> joinCriteria;
    if (isAggregate)
    {
        MgSelectAggregateCommand* cmd = static_cast<MgSelectAggregateCommand*>(m_command.p);
        cmd->SetAlias(primaryAlias.c_str());
        joinCriteria = cmd->GetJoinCriteria();
    }
    else
    {
        MgSelectCommand* cmd = static_cast<MgSelectCommand*>(m_command.p);
        cmd->SetAlias(primaryAlias.c_str());
        joinCriteria = cmd->GetJoinCriteria();
    }

    Ptr<MgStringCollection> idPropNames = new MgStringCollection();
    Ptr<MgServerFeatureConnection> conn = new MgServerFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgConnectionFailedException(L"MgServerSelectFeatures.SelectFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CHECKNULL(m_command, L"MgServerSelectFeatures.SelectFdoJoin");
        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();

        bool bAppliedProperties = false;
        if (m_options != NULL)
        {
            ApplyComputedProperties();
            // TODO: We need to find out if there are any filters involving the secondary side
            ApplyFilter();
            ApplyOrderingOptions();
            // We don't apply aggregate options here because these go through the FDO Expression Engine
            ApplyAggregateOptions(isAggregate);
            ApplyFetchSize();

            //If an explicit list is specified, we assume the caller knows about prefixed
            //extended feature class properties.
            Ptr<MgStringCollection> props = m_options->GetClassProperties();
            if (props->GetCount() > 0)
            {
                ApplyClassPropertiesForFdoJoin(primaryAlias, secondaryAlias, prefix);
                bAppliedProperties = true;
            }
        }

        //We need to fetch full class definitions of primary and secondary classes
        if (!bAppliedProperties)
        {
            //Add primary class properties
            STRING primaryClass = extension->GetFeatureClass();
            STRING primarySchemaName;
            STRING primaryClassName;
            MgUtil::ParseQualifiedClassName(primaryClass, primarySchemaName, primaryClassName);

            ApplyClassProperties(fdoConn, primarySchemaName, primaryClassName, idPropNames, primaryAlias);

            if (!isAggregate)
            {
                //Add secondary class properties
                STRING joinClass = relate->GetAttributeClass();
                STRING joinSchemaName;
                STRING joinClassName;
                MgUtil::ParseQualifiedClassName(joinClass, joinSchemaName, joinClassName);

                ApplyClassProperties(fdoConn, joinSchemaName, joinClassName, NULL, secondaryAlias, prefix); 
            }
        }
    }

    //Set join type
    FdoJoinType jtype = FdoJoinType_None;
    switch(relate->GetRelateType())
    {
    case MdfModel::AttributeRelate::Inner:
        jtype = FdoJoinType_Inner;
        break;
    case MdfModel::AttributeRelate::LeftOuter:
        jtype = FdoJoinType_LeftOuter;
        break;
    case MdfModel::AttributeRelate::RightOuter:
        jtype = FdoJoinType_RightOuter;
        break;
    }

    bool bForceOneToOne = relate->GetForceOneToOne();
    //Set filter for this join
    STRING secondaryClass = relate->GetAttributeClass();
    STRING filterText;
    MdfModel::RelatePropertyCollection* relProps = relate->GetRelateProperties();
    for (INT32 i = 0; i < relProps->GetCount(); i++)
    {
        MdfModel::RelateProperty* prop = relProps->GetAt(i);
        if (!filterText.empty())
        {
            filterText += L" AND ";
        }

        //[primaryAlias].[PropertyName] = [secondaryAlias].[PropertyName]
        filterText += primaryAlias;
        filterText += L".";
        filterText += prop->GetFeatureClassProperty();
        filterText += L" = ";
        filterText += secondaryAlias;
        filterText += L".";
        filterText += prop->GetAttributeClassProperty();
    }

    FdoPtr<FdoJoinCriteria> criteria;
    FdoPtr<FdoIdentifier> idJoinClass = FdoIdentifier::Create(secondaryClass.c_str());
    FdoPtr<FdoFilter> filter = FdoFilter::Parse(filterText.c_str());
    if (prefix.empty())
        criteria = FdoJoinCriteria::Create(idJoinClass, jtype, filter);
    else
        criteria = FdoJoinCriteria::Create(secondaryAlias.c_str(), idJoinClass, jtype, filter);

    joinCriteria->Add(criteria);

    if (isAggregate)
        ret = ((MgSelectAggregateCommand*)m_command.p)->ExecuteJoined(idPropNames, bForceOneToOne);
    else
        ret = ((MgSelectCommand*)m_command.p)->ExecuteJoined(idPropNames, bForceOneToOne);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSelectFeatures.SelectFdoJoin")

    return ret.Detach();
}

void MgServerSelectFeatures::ApplyAggregateCommandJoinFilterAndCriteria(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) Applying FDO join criteria and filter to aggregate command")));
#endif

     //This could be qualified, so parse it to be sure
    STRING schemaName;
    STRING extName;
    MgUtil::ParseQualifiedClassName(extensionName, schemaName, extName);

    CHECKNULL(m_featureSourceCacheItem.p, L"MgServerSelectFeatures.SupportsFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgServerSelectFeatures.SupportsFdoJoin");

    MdfModel::Extension* extension = NULL;
    for (INT32 i = 0; i < extensions->GetCount(); i++) 
    {
        MdfModel::Extension* ext = extensions->GetAt(i);
        if (ext->GetName() == extName)
        {
            extension = ext;
            break;
        }
    }

    CHECKNULL(extension, L"MgServerSelectFeatures.SelectFdoJoin");
    m_command->SetFeatureClassName(extension->GetFeatureClass().c_str());
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();
    CHECKNULL(relates, L"MgServerSelectFeatures.SelectFdoJoin");
    MdfModel::AttributeRelate* relate = relates->GetAt(0);

    const MdfModel::MdfString& prefix = relate->GetName();

    STRING primaryAlias = PRIMARY_ALIAS;
    STRING secondaryAlias = SECONDARY_ALIAS;

    MgSelectAggregateCommand* extSelect = static_cast<MgSelectAggregateCommand*>(m_command.p);
    extSelect->SetAlias(primaryAlias.c_str());

    FdoPtr<FdoJoinCriteriaCollection> joinCriteria = extSelect->GetJoinCriteria();

    //Set join type
    FdoJoinType jtype = FdoJoinType_None;
    switch(relate->GetRelateType())
    {
    case MdfModel::AttributeRelate::Inner:
        jtype = FdoJoinType_Inner;
        break;
    case MdfModel::AttributeRelate::LeftOuter:
        jtype = FdoJoinType_LeftOuter;
        break;
    case MdfModel::AttributeRelate::RightOuter:
        jtype = FdoJoinType_RightOuter;
        break;
    }

    bool bForceOneToOne = relate->GetForceOneToOne();
    //Set filter for this join
    STRING secondaryClass = relate->GetAttributeClass();
    STRING filterText;
    MdfModel::RelatePropertyCollection* relProps = relate->GetRelateProperties();
    for (INT32 i = 0; i < relProps->GetCount(); i++)
    {
        MdfModel::RelateProperty* prop = relProps->GetAt(i);
        if (!filterText.empty())
        {
            filterText += L" AND ";
        }

        //[primaryAlias].[PropertyName] = [secondaryAlias].[PropertyName]
        filterText += primaryAlias;
        filterText += L".";
        filterText += prop->GetFeatureClassProperty();
        filterText += L" = ";
        filterText += secondaryAlias;
        filterText += L".";
        filterText += prop->GetAttributeClassProperty();
    }

    FdoPtr<FdoJoinCriteria> criteria;
    FdoPtr<FdoIdentifier> idJoinClass = FdoIdentifier::Create(secondaryClass.c_str());
    FdoPtr<FdoFilter> filter = FdoFilter::Parse(filterText.c_str());
    if (prefix.empty())
        criteria = FdoJoinCriteria::Create(idJoinClass, jtype, filter);
    else
        criteria = FdoJoinCriteria::Create(secondaryAlias.c_str(), idJoinClass, jtype, filter);

    joinCriteria->Add(criteria);
}

void MgServerSelectFeatures::ApplyClassProperties(FdoIConnection* fdoConn, CREFSTRING schemaName, CREFSTRING className, MgStringCollection* idPropNames, CREFSTRING alias, CREFSTRING prefix)
{
    FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
    CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgServerSelectFeatures.SelectFdoJoin");

    if (!schemaName.empty())
    {
        descSchema->SetSchemaName(schemaName.c_str());
    }
    if (!className.empty())
    {
        FdoPtr<FdoStringCollection> classNames = FdoStringCollection::Create();
        classNames->Add(className.c_str());
        descSchema->SetClassNames(classNames);
    }

    FdoPtr<FdoClassDefinition> classDef;
    FdoPtr<FdoFeatureSchemaCollection> schemas = descSchema->Execute();
    for (FdoInt32 i = 0; i < schemas->GetCount(); i++)
    {
        FdoPtr<FdoFeatureSchema> schema = schemas->GetItem(i);
        if (wcscmp(schema->GetName(), schemaName.c_str()) == 0) 
        {
            FdoPtr<FdoClassCollection> classes = schema->GetClasses();
            for (FdoInt32 j = 0; j < classes->GetCount(); j++) 
            {
                FdoPtr<FdoClassDefinition> klassDef = classes->GetItem(j);
                if (wcscmp(klassDef->GetName(), className.c_str()) == 0)
                {
                    classDef = SAFE_ADDREF(klassDef.p);
                    break;
                }
            }
        }
    }

    if (NULL == (FdoClassDefinition*)classDef)
    {
        //TODO: Refine message if available
        throw new MgClassNotFoundException(L"MgServerSelectFeatures.ApplyClassProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoPtr<FdoIdentifierCollection> propNames = m_command->GetPropertyNames();

    //Add primary class properties
    FdoPtr<FdoPropertyDefinitionCollection> propDefs = classDef->GetProperties();
    for (FdoInt32 i = 0; i < propDefs->GetCount(); i++) 
    {
        FdoPtr<FdoPropertyDefinition> propDef = propDefs->GetItem(i);

        //Skip ones that aren't data/geometry
        if (propDef->GetPropertyType() != FdoPropertyType_DataProperty &&
            propDef->GetPropertyType() != FdoPropertyType_GeometricProperty)
            continue;

        STRING exprText = alias + L".";
        exprText += propDef->GetName();

        FdoPtr<FdoExpression> expr = FdoExpression::Parse(exprText.c_str());
        STRING idName = prefix + propDef->GetName();
        //[alias].[propertyName] AS [prefix][propertyName]
        FdoPtr<FdoComputedIdentifier> compId = FdoComputedIdentifier::Create(idName.c_str(), expr);
        propNames->Add(compId);
    }

    if (NULL != idPropNames)
    {
        FdoPtr<FdoDataPropertyDefinitionCollection> idPropDefs = classDef->GetIdentityProperties();
        for (FdoInt32 i = 0; i < idPropDefs->GetCount(); i++)
        {
            FdoPtr<FdoDataPropertyDefinition> dp = idPropDefs->GetItem(i);
            STRING propName = L"";
            propName += dp->GetName();
            idPropNames->Add(propName);
        }
    }
}

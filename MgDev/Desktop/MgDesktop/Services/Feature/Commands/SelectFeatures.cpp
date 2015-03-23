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

#include "Services/Feature/FeatureDefs.h"
#include "MgDesktop.h"
#include "Services/FeatureService.h"
#include "SelectFeatures.h"
#include "Services/Feature/FeatureConnection.h"
#include "Services/FeatureReader.h"
#include "Services/Feature/FeatureUtil.h"
#include "FeatureServiceCommand.h"
#include "SelectCommand.h"
#include "SelectAggregateCommand.h"
#include "Services/Feature/FeatureDistribution.h"
#include "Services/Feature/GwsFeatureReader.h"
#include "Services/Feature/GwsConnectionPool.h"
#include "GwsCommonImp.h"
#include "GwsMutableFeature.h"
#include "GwsQuery.h"
#include "FdoExpressionEngineCopyFilter.h"
#include "Services/Feature/FeatureServiceCache.h"

//Uncomment for extra console chatter to debug FDO joins
//#define DEBUG_FDO_JOIN

MgdSelectFeatures::MgdSelectFeatures()
{
    m_command = NULL;
    m_options = NULL;
    m_customPropertyFound = false;
    m_customFunction = NULL;
    m_customPropertyName = L"";
    m_featureSourceCacheItem = NULL;

    // Set a default join query batch size
    m_nJoinQueryBatchSize = MgdConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize;
    m_bUseFdoJoinOptimization = MgdConfigProperties::DefaultFeatureServicePropertyUseFdoJoinOptimization; 

    MgConfiguration* config = MgConfiguration::GetInstance();
    if(config)
    {
        // Get the join batch size
        config->GetIntValue(MgdConfigProperties::FeatureServicePropertiesSection,
                            MgdConfigProperties::FeatureServicePropertyJoinQueryBatchSize,
                            m_nJoinQueryBatchSize,
                            MgdConfigProperties::DefaultFeatureServicePropertyJoinQueryBatchSize);
        // Get data cache size
        config->GetIntValue(MgdConfigProperties::FeatureServicePropertiesSection,
                            MgdConfigProperties::FeatureServicePropertyDataCacheSize,
                            m_nDataCacheSize,
                            MgdConfigProperties::DefaultFeatureServicePropertyDataCacheSize);
        // Get FDO Join use flag 
 	    config->GetBoolValue(MgdConfigProperties::FeatureServicePropertiesSection, 
                             MgdConfigProperties::FeatureServicePropertyUseFdoJoinOptimization, 
                             m_bUseFdoJoinOptimization, 
                             MgdConfigProperties::DefaultFeatureServicePropertyUseFdoJoinOptimization); 
    }
}

MgdSelectFeatures::~MgdSelectFeatures()
{
}

// Executes the select features command and serializes the reader
MgReader* MgdSelectFeatures::SelectFeatures(MgResourceIdentifier* resource,
                                           CREFSTRING className,
                                           MgFeatureQueryOptions* options,
                                           bool executeSelectAggregate,
                                           bool isExtended,
                                           bool withLock)
{
    Ptr<MgReader> mgReader;
    bool isSelectAggregate = executeSelectAggregate;
    assert(!(isSelectAggregate && isExtended));
    assert(!(withLock && isExtended));

    MG_FEATURE_SERVICE_TRY()

    // Validate parameters
    ValidateParam(resource,className);

    // Retrieve the feature source
    if (NULL == m_featureSourceCacheItem.p)
    {
        MgdFeatureServiceCache* cacheManager = MgdFeatureServiceCache::GetInstance();
        m_featureSourceCacheItem = cacheManager->GetFeatureSource(resource);
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

    if (isExtended)
    {
        if (bFeatureJoinProperties)
        {
            throw new MgInvalidOperationException(L"MgdSelectFeatures.SelectFeatures", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        m_command = MgdFeatureServiceCommand::CreateCommand(resource, FdoCommandType_ExtendedSelect);
        m_command->SetFeatureClassName((FdoString*)className.c_str());
        mgReader = SelectExtended();
    }
    else if (!isSelectAggregate && bFeatureJoinProperties)
    {
        if (bSupportsFdoJoin)
        {
#ifdef DEBUG_FDO_JOIN
            ACE_DEBUG((LM_INFO, ACE_TEXT("\n(%t) Feature Source (%W) supports FDO join optimization"), fsIdStr.c_str()));
#endif
            m_command = MgdFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select);
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
        m_command = MgdFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select);
        mgReader = SelectFdoJoin(resource, className, true);
    }
    else
    {
        // Custom function specified from Select command is not allowed
        if (!isSelectAggregate && ContainsCustomFunction(options))
        {
            STRING message = MgdFeatureUtil::GetMessage(L"MgCustomFunctionNotSupported");

            MgStringCollection arguments;
            arguments.Add(message);
            throw new MgFeatureServiceException(L"MgdSelectFeatures.SelectFeatures", __LINE__, __WFILE__, &arguments, L"", NULL);
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
            Ptr<MgdGwsFeatureReader> gwsFeatureReader = JoinFeatures(resource, className, NULL);
            try
            {
                // Get the requested property name from the MgdFeatureServiceCommandObject.  This property name may be
                // prefixed by the relation name if it is from a secondary resource.
                FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
                int nFicCnt = fic->GetCount();
                if (nFicCnt <= 0)
                {
                    // throw invalid argument exception because the properties the m_command is empty
                    MgStringCollection arguments;
                    arguments.Add(L"1");
                    arguments.Add(L"MgdFeatureServiceCommand");

                    throw new MgInvalidArgumentException(L"MgdSelectFeatures::SelectFeatures()",
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
        if (withLock)
            reader = m_command->ExecuteWithLock();
        else
            reader = m_command->Execute();

        CHECKNULL((MgReader*)reader, L"MgdSelectFeatures.SelectFeatures");

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

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgdSelectFeatures.SelectFeatures")

    return mgReader.Detach();
}


void MgdSelectFeatures::ApplyQueryOptions(bool isSelectAggregate)
{
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyQueryOptions");

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
void MgdSelectFeatures::ApplyClassProperties()
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyClassProperties");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyClassProperties");

    Ptr<MgStringCollection> properties = m_options->GetClassProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    //TODO: Need to check if FDO join optimization is supported, and whether this contains prefixed
    //secondary properties.

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.ApplyClassProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"MgdSelectFeatures.ApplyClassProperties");

        fic->Add(fdoIden);
    }
}

void MgdSelectFeatures::ApplyClassPropertiesForFdoJoin(CREFSTRING primaryAlias, CREFSTRING secondaryAlias, CREFSTRING secondaryPrefix)
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyClassPropertiesForFdoJoin");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyClassPropertiesForFdoJoin");

    Ptr<MgStringCollection> properties = m_options->GetClassProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.ApplyClassPropertiesForFdoJoin");

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
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"MgdSelectFeatures.ApplyClassPropertiesForFdoJoin");

        fic->Add(fdoIden);
    }
}

// Computed properties
void MgdSelectFeatures::ApplyComputedProperties()
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyComputedProperties");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyComputedProperties");

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
            CHECKNULL((FdoExpression*)expression, L"MgdSelectFeatures.ApplyComputedProperties");

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
//void MgdSelectFeatures::ApplyFilter()
//{
//    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyFilter");
//    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyFilter");
//
//    STRING filterText = m_options->GetFilter();
//    if (filterText.empty()) { return; } // Nothing to do
//
//    m_command->SetFilter(filterText.c_str());
//}

// Fetch size
void MgdSelectFeatures::ApplyFetchSize()
{
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyFetchSize");
    if(m_options)
        m_command->SetFetchSize(m_options->GetFetchSize());
    else
        m_command->SetFetchSize(m_nDataCacheSize);
}

// Spatial Filter
void MgdSelectFeatures::ApplyFilter()
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyFilter");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyFilter");

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
            FdoSpatialOperations fdoSpatialOp = MgdFeatureUtil::GetFdoSpatialOperation(spatialOp);
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
void MgdSelectFeatures::ApplyOrderingOptions()
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyOrderingOptions");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyOrderingOptions");

    Ptr<MgStringCollection> properties = m_options->GetOrderingProperties();

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // Ordering options are supplied but provider does not support it
    if (!m_command->SupportsSelectOrdering())
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgOrderingOptionNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdSelectFeatures.ApplyOrderingOptions", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetOrdering();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.ApplyOrderingOptions");

    // Order option Asc or Desc (default is Asc)
    FdoOrderingOption option = MgdFeatureUtil::GetFdoOrderingOption(m_options->GetOrderOption());
    m_command->SetOrderingOption(option);

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        if (!propertyName.empty())
        {
            FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
            CHECKNULL((FdoIdentifier*)fdoIden, L"MgdSelectFeatures.ApplyOrderingOptions");

            fic->Add(fdoIden);
        }
    }
}


bool MgdSelectFeatures::ContainsUdf(FdoExpression* expression)
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


bool MgdSelectFeatures::IsCustomFunction(FdoFunction* fdoFunc)
{
    bool customFunc = false;

    FdoString* funcNameAllowed = fdoFunc->GetName();
    if (funcNameAllowed != NULL)
    {
        INT32 funcIndex = -1;
        customFunc = MgdFeatureUtil::FindCustomFunction(STRING(funcNameAllowed),funcIndex);
    }

    return customFunc;
}

void MgdSelectFeatures::AddFdoComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.AddFdoComputedProperty");

    FdoString* expName = aliasName.c_str();
    if (expName != NULL)
    {
        FdoPtr<FdoComputedIdentifier> fdoIden = FdoComputedIdentifier::Create(expName, expression);
        CHECKNULL((FdoComputedIdentifier*)fdoIden, L"MgdSelectFeatures.AddFdoComputedProperty");

        fic->Add(fdoIden);
    }
}

void MgdSelectFeatures::AddCustomComputedProperty(CREFSTRING aliasName, FdoExpression* expression)
{
    CHECKNULL((FdoExpression*)expression, L"MgdSelectFeatures.AddCustomComputedProperty");

    FdoPtr<FdoIdentifierCollection> fic = m_command->GetPropertyNames();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.AddCustomComputedProperty");

    // If property is already found, two custom properties are not supported and therefore throw exception
    if (m_customPropertyFound)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdSelectFeatures.AddCustomComputedProperty", __LINE__, __WFILE__, &arguments, L"", NULL);
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

void MgdSelectFeatures::ValidateConstraintsOnCustomFunctions()
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
                STRING message = MgdFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(L"MgdSelectFeatures.ValidateConstraintsOnCustomFunctions",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
            MgdFeatureUtil::ValidateCustomConstraints(m_customFunction);
        }
    }
}


void MgdSelectFeatures::CreateCommand(MgResourceIdentifier* resource, bool isSelectAggregate)
{
    if (!isSelectAggregate)
    {
        m_command = MgdFeatureServiceCommand::CreateCommand(resource, FdoCommandType_Select);
    }
    else
    {
        m_command = MgdFeatureServiceCommand::CreateCommand(resource, FdoCommandType_SelectAggregates);
    }
    CHECKNULL((MgdFeatureServiceCommand*)m_command, L"MgdSelectFeatures.CreateCommand");
}

void MgdSelectFeatures::ValidateParam(MgResourceIdentifier* resource, CREFSTRING className)
{
    CHECK_FEATURE_SOURCE_ARGUMENT(resource, L"MgdSelectFeatures::ValidateParam");

    if (className.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgdSelectFeatures::ValidateParam()",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }
}

void MgdSelectFeatures::ApplyAggregateOptions(bool isSelectAggregate)
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
        ((MgdFeatureServiceCommand*)m_command)->SetDistinct(distinct);
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
            ((MgdFeatureServiceCommand*)m_command)->SetGroupingFilter(filter);
        }
    }
}

void MgdSelectFeatures::ApplyFdoGroupingProperties(MgStringCollection* propertyNames)
{
    CHECKNULL(m_options, L"MgdSelectFeatures.ApplyFdoGroupingProperties");
    CHECKNULL(m_command, L"MgdSelectFeatures.ApplyFdoGroupingProperties");

    Ptr<MgStringCollection> properties = SAFE_ADDREF(propertyNames);

    if (properties == NULL)
        return; // Nothing to do

    INT32 cnt = properties->GetCount();
    if (cnt <= 0)
        return; // Nothing to do

    // Grouping options are supplied but provider does not support it
    if (!m_command->SupportsSelectGrouping())
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgGroupingNotSupported");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdSelectFeatures.ApplyFdoGroupingProperties", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    FdoPtr<FdoIdentifierCollection> fic = ((MgdFeatureServiceCommand*)m_command)->GetGrouping();
    CHECKNULL((FdoIdentifierCollection*)fic, L"MgdSelectFeatures.ApplyFdoGroupingProperties");

    for (INT32 i=0; i < cnt; i++)
    {
        STRING propertyName = properties->GetItem(i);

        FdoPtr<FdoIdentifier> fdoIden = FdoIdentifier::Create((FdoString*)propertyName.c_str());
        CHECKNULL((FdoIdentifier*)fdoIden, L"MgdSelectFeatures.ApplyFdoGroupingProperties");

        fic->Add(fdoIden);
    }
}

// Check whether user is requesting custom operations
bool MgdSelectFeatures::ContainsCustomFunction(MgFeatureQueryOptions* options)
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
            CHECKNULL((FdoExpression*)expression, L"MgdSelectFeatures.ContainsCustomFunction");

            hasCustomFunction = ContainsUdf(expression);
        }
    }

    if (hasCustomFunction && (cnt != 1))
    {
        // Only one custom function with no property is allowed
        STRING message = MgdFeatureUtil::GetMessage(L"MgOnlyOnePropertyAllowed");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdSelectFeatures.ContainsCustomFunction",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return hasCustomFunction;
}

// Convert reader into a custom MgDataReader
MgReader* MgdSelectFeatures::GetCustomReader(MgReader* reader)
{
    Ptr<MgReader> distReader;
    if (m_customPropertyFound)
    {
        Ptr<MgdFeatureDistribution> featureDist =
            MgdFeatureDistribution::CreateDistributionFunction(reader, m_customFunction, m_customPropertyName);

        distReader = featureDist->Execute();
    }

    return distReader.Detach();
}

// Look for extension which have calculations but no joins
bool MgdSelectFeatures::FindFeatureCalculation(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bCalculationExists = false;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.FindFeatureCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.FindFeatureCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.FindFeatureCalculation");
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
bool MgdSelectFeatures::FindFeatureJoinProperties(MgResourceIdentifier* resourceId, CREFSTRING extensionName)
{
    bool bJoinPropertiesExists = false;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.FindFeatureJoinProperties");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.FindFeatureJoinProperties");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.FindFeatureJoinProperties");
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

void MgdSelectFeatures::UpdateCommandOnJoinCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.UpdateCommandOnJoinCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.UpdateCommandOnJoinCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.UpdateCommandOnJoinCalculation");
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
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgdSelectFeatures.UpdateCommandOnJoinCalculation")
}

void MgdSelectFeatures::UpdateCommandOnCalculation(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.UpdateCommandOnCalculation");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.UpdateCommandOnCalculation");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.UpdateCommandOnCalculation");
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
                    Ptr<MgdFeatureConnection> fcConnection = new MgdFeatureConnection(featureSourceId);
                    if ((NULL != fcConnection.p) && ( fcConnection->IsConnectionOpen() ))
                    {
                        // The reference to the FDO connection from the MgdFeatureConnection object must be cleaned up before the parent object
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
    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceId, L"MgdSelectFeatures.UpdateCommandOnCalculation")
}

MgdGwsFeatureReader* MgdSelectFeatures::JoinFeatures(MgResourceIdentifier* featureSourceIdentifier, CREFSTRING extensionName, FdoFilter* filter)
{
    Ptr<MgdGwsFeatureReader> gwsFeatureReader;

    MG_FEATURE_SERVICE_TRY()

    FdoPtr<IGWSQueryDefinition> qd;
    FdoPtr<MgdGwsConnectionPool> pool = MgdGwsConnectionPool::Create();

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.JoinFeatures");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.JoinFeatures");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.JoinFeatures");
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
            Ptr<MgdFeatureConnection> msfcLeft = new MgdFeatureConnection(featureSourceIdentifier);
            if ((NULL != msfcLeft.p) && ( msfcLeft->IsConnectionOpen() ))
            {
                pool->AddConnection(primaryConnectionName.c_str(), msfcLeft);
            }
            else
            {
                throw new MgdConnectionFailedException(L"MgdSelectFeatures.JoinFeatures",
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
            CHECKNULL(lqd, L"MgdSelectFeatures.JoinFeatures");
            qd = lqd;

            IGWSJoinQueryDefinition* jqd = NULL;

            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgdSelectFeatures.JoinFeatures");

            bool bForceOneToOne = true;
            Ptr<MgStringCollection> attributeNameDelimiters = new MgStringCollection();

            //Store all the secondary's name, and if the filter property doesn't contain secondary's name,
            //it applies only to the primary.
            Ptr<MgStringCollection> secondaryNames = new MgStringCollection();

            // For each join (attributeRelate) to a secondary source need to do the following
            for (int attributeRelateIndex = 0; attributeRelateIndex < attributeRelates->GetCount(); attributeRelateIndex++)
            {
                MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                CHECKNULL(attributeRelate, L"MgdSelectFeatures.JoinFeatures");

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
                    Ptr<MgdFeatureConnection> msfcRight = new MgdFeatureConnection(secondaryFeatureSource);
                    if ((NULL != msfcRight.p) && ( msfcRight->IsConnectionOpen() ))
                    {
                        pool->AddConnection(secondaryConnectionName.c_str(), msfcRight);
                    }
                    else
                    {
                        throw new MgdConnectionFailedException(L"MgdSelectFeatures.JoinFeatures",
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
                CHECKNULL(rqd, L"MgdSelectFeatures.JoinFeatures");

                // Get Join Attributes
                FdoPtr<FdoStringCollection> lattrs = FdoStringCollection::Create();
                FdoPtr<FdoStringCollection> rattrs = FdoStringCollection::Create();

                // Determine the number of RelateProperties (attributes)
                MdfModel::RelatePropertyCollection* relateProperties = attributeRelate->GetRelateProperties();
                CHECKNULL(relateProperties, L"MgdSelectFeatures.JoinFeatures");
                int nRelatePropertyCount = relateProperties->GetCount();

                // For each RelateProperty need to do the following
                for (int relatePropertyIndex = 0; relatePropertyIndex < nRelatePropertyCount; relatePropertyIndex++)
                {
                    MdfModel::RelateProperty* relateProperty = relateProperties->GetAt(relatePropertyIndex);
                    CHECKNULL(relateProperty, L"MgdSelectFeatures.JoinFeatures");

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

            gwsFeatureReader = new MgdGwsFeatureReader(pool, iter, iterCopy, parsedExtensionName, fsNames, bForceOneToOne, attributeNameDelimiters);
            gwsFeatureReader->SetFilter(filter);
            break;
        }
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(featureSourceIdentifier, L"MgdSelectFeatures.JoinFeatures")

    // Now that the reader has been created we will need to mark all of the connections it uses as OwnReader() because the GWS reader will be
    // taking ownership of the connections. We have to do it this late in the code in case an exception is thrown somewhere before this.
    // We want to avoid a deadlock of the connection :)
    gwsFeatureReader->OwnsConnections();

    return gwsFeatureReader.Detach();
}

void MgdSelectFeatures::ParseQualifiedClassNameForCalculation(MdfModel::Extension* extension, CREFSTRING qualifiedClassName, STRING& schemaName, STRING& className)
{
    CHECKNULL(extension, L"MgdSelectFeatures.ParseQualifiedClassNameForCalculation");

    MgUtil::ParseQualifiedClassName(qualifiedClassName, schemaName, className);

    if (schemaName.empty())
    {
        STRING dummyStr;

        MgUtil::ParseQualifiedClassName(extension->GetFeatureClass(), schemaName, dummyStr);
    }
}

MgResourceIdentifier* MgdSelectFeatures::GetSecondaryResourceIdentifier(MgResourceIdentifier* primResId, CREFSTRING extensionName, CREFSTRING relationName)
{
    Ptr<MgResourceIdentifier> secResId;

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.GetSecondaryResourceIdentifier");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.GetSecondaryResourceIdentifier");

    for (int i = 0; i < extensions->GetCount(); i++)
    {
        MdfModel::Extension* extension = extensions->GetAt(i);
        CHECKNULL(extension, L"MgdSelectFeatures.GetSecondaryResourceIdentifier");

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
            CHECKNULL(attributeRelates, L"MgdSelectFeatures.GetSecondaryResourceIdentifier");
            int nAttributeRelateCount = attributeRelates->GetCount();

            // Find the specified relation name
            {
                for (int attributeRelateIndex = 0; attributeRelateIndex < nAttributeRelateCount; attributeRelateIndex++)
                {
                    MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(attributeRelateIndex);
                    CHECKNULL(attributeRelate, L"MgdSelectFeatures.GetSecondaryResourceIdentifier");

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

bool MgdSelectFeatures::SupportsFdoJoin(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, bool isAggregate)
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

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.SupportsFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.SupportsFdoJoin");

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

    Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgdConnectionFailedException(L"MgdSelectFeatures.SupportsFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSelectFeatures.SupportsFdoJoin")

    return bSupported;
}

bool MgdSelectFeatures::IsFunctionOnPrimaryProperty(FdoFunction* function, FdoIConnection* fdoConn, CREFSTRING schemaName, CREFSTRING className)
{
    FdoPtr<FdoIdentifierCollection> identifiers = MgdFeatureUtil::ExtractIdentifiers(function);
    if (identifiers->GetCount() == 0)
        return true; //Inconsequential

    FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
    CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgdSelectFeatures.SelectFdoJoin");

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
        throw new MgClassNotFoundException(L"MgdSelectFeatures.IsFunctionOnPrimaryProperty", __LINE__, __WFILE__, NULL, L"", NULL);
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

bool MgdSelectFeatures::FilterContainsSecondaryProperties(MgResourceIdentifier* featureSourceId, CREFSTRING filter, STRING secondarySchema, STRING secondaryClassName, STRING secondaryPrefix)
{
    if (filter.empty())
        return false;

    //TODO: There's probably a more efficient way to do this without needing to fetch the secondary
    //class definition. But we're aiming for functionality and simplicity first.
    Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgdConnectionFailedException(L"MgdSelectFeatures.SupportsFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        FdoPtr<FdoIConnection> fdoConn = conn->GetConnection();

        FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
        CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgdSelectFeatures.SelectFdoJoin");

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
            throw new MgClassNotFoundException(L"MgdSelectFeatures.FilterContainsSecondaryProperties", __LINE__, __WFILE__, NULL, L"", NULL);
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

MgReader* MgdSelectFeatures::SelectFdoJoin(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName, bool isAggregate)
{
    // TODO: This does not handle filters on the secondary side (yet)
    // Can GwsQueryEngine do this?

    Ptr<MgReader> ret;

    MG_FEATURE_SERVICE_TRY()

    //This could be qualified, so parse it to be sure
    STRING schemaName;
    STRING extName;
    MgUtil::ParseQualifiedClassName(extensionName, schemaName, extName);

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.SelectFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.SelectFdoJoin");

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

    CHECKNULL(extension, L"MgdSelectFeatures.SelectFdoJoin");
    FdoString* clsName = extension->GetFeatureClass().c_str();
    m_command->SetFeatureClassName(clsName);
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) [FdoISelect] Set primary feature class: %W"), clsName));
#endif
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();
    CHECKNULL(relates, L"MgdSelectFeatures.SelectFdoJoin");
    MdfModel::AttributeRelate* relate = relates->GetAt(0);

    const MdfModel::MdfString& prefix = relate->GetName();

    STRING primaryAlias = PRIMARY_ALIAS;
    STRING secondaryAlias = SECONDARY_ALIAS;

    FdoPtr<FdoJoinCriteriaCollection> joinCriteria;
    if (isAggregate)
    {
        MgdSelectAggregateCommand* cmd = static_cast<MgdSelectAggregateCommand*>(m_command.p);
        cmd->SetAlias(primaryAlias.c_str());
        joinCriteria = cmd->GetJoinCriteria();
    }
    else
    {
        MgdSelectCommand* cmd = static_cast<MgdSelectCommand*>(m_command.p);
        cmd->SetAlias(primaryAlias.c_str());
        joinCriteria = cmd->GetJoinCriteria();
    }

    Ptr<MgStringCollection> idPropNames = new MgStringCollection();
    Ptr<MgdFeatureConnection> conn = new MgdFeatureConnection(featureSourceId);
    {
        if (!conn->IsConnectionOpen())
        {
            throw new MgdConnectionFailedException(L"MgdSelectFeatures.SelectFdoJoin", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CHECKNULL(m_command, L"MgdSelectFeatures.SelectFdoJoin");
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
        ret = ((MgdSelectAggregateCommand*)m_command.p)->ExecuteJoined(idPropNames, bForceOneToOne);
    else
        ret = ((MgdSelectCommand*)m_command.p)->ExecuteJoined(idPropNames, bForceOneToOne);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgdSelectFeatures.SelectFdoJoin")

    return ret.Detach();
}

void MgdSelectFeatures::ApplyAggregateCommandJoinFilterAndCriteria(MgResourceIdentifier* featureSourceId, CREFSTRING extensionName)
{
#ifdef DEBUG_FDO_JOIN
    ACE_DEBUG((LM_INFO, ACE_TEXT("\n\t(%t) Applying FDO join criteria and filter to aggregate command")));
#endif

     //This could be qualified, so parse it to be sure
    STRING schemaName;
    STRING extName;
    MgUtil::ParseQualifiedClassName(extensionName, schemaName, extName);

    CHECKNULL(m_featureSourceCacheItem.p, L"MgdSelectFeatures.SupportsFdoJoin");
    MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
    MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
    CHECKNULL(extensions, L"MgdSelectFeatures.SupportsFdoJoin");

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

    CHECKNULL(extension, L"MgdSelectFeatures.SelectFdoJoin");
    m_command->SetFeatureClassName(extension->GetFeatureClass().c_str());
    MdfModel::AttributeRelateCollection* relates = extension->GetAttributeRelates();
    CHECKNULL(relates, L"MgdSelectFeatures.SelectFdoJoin");
    MdfModel::AttributeRelate* relate = relates->GetAt(0);

    const MdfModel::MdfString& prefix = relate->GetName();

    STRING primaryAlias = PRIMARY_ALIAS;
    STRING secondaryAlias = SECONDARY_ALIAS;

    MgdSelectAggregateCommand* extSelect = static_cast<MgdSelectAggregateCommand*>(m_command.p);
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

void MgdSelectFeatures::ApplyClassProperties(FdoIConnection* fdoConn, CREFSTRING schemaName, CREFSTRING className, MgStringCollection* idPropNames, CREFSTRING alias, CREFSTRING prefix)
{
    FdoPtr<FdoIDescribeSchema> descSchema = dynamic_cast<FdoIDescribeSchema*>(fdoConn->CreateCommand(FdoCommandType_DescribeSchema));
    CHECKNULL((FdoIDescribeSchema*)descSchema, L"MgdSelectFeatures.SelectFdoJoin");

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
        throw new MgClassNotFoundException(L"MgdSelectFeatures.ApplyClassProperties", __LINE__, __WFILE__, NULL, L"", NULL);
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

MgdScrollableFeatureReader* MgdSelectFeatures::SelectExtended()
{
    Ptr<MgdScrollableFeatureReader> scrollReader;
    ApplyQueryOptions(false);
    scrollReader = dynamic_cast<MgdScrollableFeatureReader*>(m_command->Execute());
    CHECKNULL(scrollReader.p, L"MgdSelectFeatures::SelectExtended");
    return scrollReader.Detach();
}
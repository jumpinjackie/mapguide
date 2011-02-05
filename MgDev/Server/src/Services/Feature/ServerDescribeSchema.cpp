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
#include "ServerDescribeSchema.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureReader.h"
#include "ServerFeatureUtil.h"
#include "FdoExpressionEngine.h"
#include "FdoExpressionEngineCopyFilter.h"
#include "CacheManager.h"

//////////////////////////////////////////////////////////////////
MgServerDescribeSchema::MgServerDescribeSchema()
{
    m_cacheManager = MgCacheManager::GetInstance();
    m_featureServiceCache = m_cacheManager->GetFeatureServiceCache();
}

//////////////////////////////////////////////////////////////////
MgServerDescribeSchema::~MgServerDescribeSchema()
{
}

///////////////////////////////////////////////////////////////////////////////
FdoFeatureSchemaCollection* MgServerDescribeSchema::DescribeFdoSchema(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames, bool& classNameHintUsed)
{
    // IMPORTANT INFORMATION
    // FDO objects cannot be cached as they are not thread safe.
    // This is not an issue because we create a MapGuide wrapper of this information that is thread safe.
    FdoPtr<FdoFeatureSchemaCollection> ffsc;
    ffsc = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Connect to provider
    Ptr<MgServerFeatureConnection> connection = new MgServerFeatureConnection(resource);

    if ((NULL != connection.p) && (connection->IsConnectionOpen()))
    {
        // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
        // otherwise it leaves the FDO connection marked as still in use.
        FdoPtr<FdoIConnection> fdoConn = connection->GetConnection();
        FdoPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)fdoConn->CreateCommand(FdoCommandType_DescribeSchema);

        classNameHintUsed = IsClassNameHintUsed(fdoCommand);

 /*       if (!schemaName.empty())
        {
            fdoCommand->SetSchemaName(schemaName.c_str());
        }

        FdoPtr<FdoStringCollection> fdoClassNames = MgServerFeatureUtil::MgToFdoStringCollection(classNames, false);

        if (NULL != fdoClassNames.p && fdoClassNames->GetCount() > 0)
        {
            fdoCommand->SetClassNames(fdoClassNames.p);
        }*/

        // Execute the command
        ffsc = fdoCommand->Execute();
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.DescribeFdoSchema");

        // Finished with primary feature source, so now cycle through any secondary sources
        if (NULL == m_featureSourceCacheItem.p)
        {
            m_featureSourceCacheItem = m_cacheManager->GetFeatureSourceCacheItem(resource);
        }

        MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
        CHECKNULL(featureSource, L"MgServerDescribeSchema.DescribeFdoSchema");
        MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
        CHECKNULL(extensions, L"MgServerDescribeSchema.DescribeFdoSchema");
        int extensionCount = extensions->GetCount();

        for (int i = 0; i < extensionCount; i++)
        {
            MdfModel::Extension* extension = extensions->GetAt(i);
            CHECKNULL(extension, L"MgServerDescribeSchema.DescribeFdoSchema");

            // Get the extension name
            STRING extensionName = (STRING)extension->GetName();

            // Determine the number of secondary sources (AttributeRelates)
            MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
            CHECKNULL(attributeRelates, L"MgServerDescribeSchema.DescribeFdoSchema");
            int nAttributeRelates = attributeRelates->GetCount();

            for (int arIndex = 0; arIndex < nAttributeRelates; arIndex++)
            {
                MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(arIndex);
                CHECKNULL(attributeRelate, L"MgServerDescribeSchema.DescribeFdoSchema");

                // get the resource id of the secondary feature source
                STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                // get the name for the join relationship (attribute relate name)
                STRING attributeRelateName = (STRING)attributeRelate->GetName();

                // Get the secondary feature class (AttributeClass)
                STRING attributeClass = (STRING)attributeRelate->GetAttributeClass();

                // Parse the schema name from the classname;
                STRING secSchemaName, secClassName;
                MgUtil::ParseQualifiedClassName(attributeClass, secSchemaName, secClassName);

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);

                if (NULL != secondaryFeatureSource)
                {
                    FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                    Ptr<MgServerFeatureConnection> connection2 = new MgServerFeatureConnection(secondaryFeatureSource);

                    if ((NULL != connection2.p) && ( connection2->IsConnectionOpen() ))
                    {
                        // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
                        // otherwise it leaves the FDO connection marked as still in use.
                        FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                        // Check whether this command is supported by the provider
                        FdoPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                        CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeFdoSchema");

                        //if (!secSchemaName.empty())
                        //{
                        //    fdoCommand2->SetSchemaName(secSchemaName.c_str());
                        //}

                        //if (!secClassName.empty())
                        //{
                        //    FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                        //    fdoClassNames2->Add(secClassName.c_str());
                        //    fdoCommand2->SetClassNames(fdoClassNames2.p);
                        //}

                        // Execute the command
                        ffsc2 = fdoCommand2->Execute();
                        CHECKNULL((FdoFeatureSchemaCollection*)ffsc2, L"MgServerDescribeSchema.DescribeFdoSchema");

                        //if (!secSchemaName.empty())
                        //{
                        //    fdoCommand2->SetSchemaName(secSchemaName.c_str());
                        //}

                        //if (!secClassName.empty())
                        //{
                        //    FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                        //    fdoClassNames2->Add(secClassName.c_str());
                        //    fdoCommand2->SetClassNames(fdoClassNames2.p);
                        //}

                        // Extract the schemas from the secondary collection and add them to the main collection
                        // Get schema count
                        FdoInt32 cnt = ffsc2->GetCount();
                        for (FdoInt32 i = 0; i < cnt; i++)
                        {
                            FdoPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(i);
                            STRING fdoSchemaName = (wchar_t*)ffs->GetName();

                            if (fdoSchemaName != secSchemaName)
                            {
                                continue;
                            }

                            // Prefix the schema name with the extension and attribute relate names
                            STRING modifiedSchemaName;
                            modifiedSchemaName = L"[" + extensionName + L"]";
                            modifiedSchemaName += L"[" + attributeRelateName + L"]";
                            modifiedSchemaName += fdoSchemaName;
                            FdoString* msn = modifiedSchemaName.c_str();
                            ffs->SetName(msn);

                            // Add this schema to the collection if it isn't already there
                            if (!ffsc->Contains(ffs))
                            {
                                ffsc->Add(ffs);
                            }
                        }
                    }
                    else
                    {
                        throw new MgConnectionFailedException(L"MgServerDescribeSchema.DescribeFdoSchema", __LINE__, __WFILE__, NULL, L"", NULL);
                    }
                }

            }  // End of the for-loop that iterates thru the secondary sources

        }  // End of for loop that iterates thru the extensions in the feature source
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerDescribeSchema.DescribeFdoSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.DescribeFdoSchema")

    return ffsc.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerDescribeSchema::DescribeSchema(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames, bool serialize)
{
    Ptr<MgFeatureSchemaCollection> fsCollection;

    MG_FEATURE_SERVICE_TRY()

    fsCollection = m_featureServiceCache->GetSchemas(resource, schemaName, classNames, serialize);

    if (NULL == fsCollection.p)
    {
        fsCollection = new MgFeatureSchemaCollection();

        bool classNameHintUsed = true;
        FdoPtr<FdoFeatureSchemaCollection> ffsc =
            DescribeFdoSchema(resource, schemaName, classNames, classNameHintUsed);
        CHECKNULL(ffsc.p, L"MgServerDescribeSchema.DescribeSchema");

        // Get schema count
        FdoInt32 schemaCount = ffsc->GetCount();

        //
        // A new MgFeatureSchema needs to be created for each primary schema in FDO schemas.
        //

        Ptr<MgFeatureSchema> schema;
        Ptr<MgClassDefinitionCollection> classCol;

        for (int nSchemaIndex = 0; nSchemaIndex < schemaCount; nSchemaIndex++)
        {
            // Retrieve schema from the collection
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nSchemaIndex);
            STRING currSchemaName = (wchar_t*)ffs->GetName();

            // Check if this schema is from secondary source which will be prefixed with [ExtensionName][RelationName],
            // ie [ExtensionName][RelationName]SchemaName
            if (currSchemaName.find(L"[") == 0)
            {
                // Found a schema for secondary source, so just skip over it for now
                continue;
            }

            schema = new MgFeatureSchema();
            classCol = schema->GetClasses();

            // Set the schema name and description
            FdoStringP fSchemaName = ffs->GetName();
            schema->SetName(STRING(fSchemaName));

            FdoStringP fSchemaDesc = ffs->GetDescription();
            schema->SetDescription(STRING(fSchemaDesc));

            // Get all classes for the schema
            FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
            FdoInt32 classCount = fcc->GetCount();

            // Add the primary class definitions to the MgClassDefinitionCollection
            for (FdoInt32 nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
            {
                FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                FdoStringP qname = fc->GetQualifiedName();
                FdoStringP name = fc->GetName();

                if (name != NULL && qname != NULL)
                {
                    Ptr<MgClassDefinition> classDefinition = MgServerFeatureUtil::GetMgClassDefinition(fc, serialize);
                    classCol->Add(classDefinition);
                }
            }

            //
            // A new MgClassDefinition needs to be created for each extension and added to the MgClassCollection
            //

            if (NULL == m_featureSourceCacheItem.p)
            {
                m_featureSourceCacheItem = m_cacheManager->GetFeatureSourceCacheItem(resource);
            }

            MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
            CHECKNULL(featureSource, L"MgServerDescribeSchema.DescribeSchema");
            MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
            CHECKNULL(extensions, L"MgServerDescribeSchema.DescribeSchema");
            int extensionCount = extensions->GetCount();

            for (int i = 0; i < extensionCount; i++)
            {
                Ptr<MgClassDefinition> extClassDefinition;
                FdoPtr<FdoClassDefinition> originalClassDef;

                MdfModel::Extension* extension = extensions->GetAt(i);
                CHECKNULL(extension, L"MgServerDescribeSchema.DescribeSchema");

                // Get the extension name
                STRING extensionName = (STRING)extension->GetName();

                // get FeatureClass element - this tells us which class is extended (SchemaName:ClassName)
                STRING featureClass = (STRING)extension->GetFeatureClass();

                // Parse the schemaName from the className
                STRING primSchemaName, primClassName;
                MgUtil::ParseQualifiedClassName(featureClass, primSchemaName, primClassName);

                if (currSchemaName != primSchemaName)
                {
                    continue;
                }

                // Cycle thru FDO schemas for schemaName.
                for (int nIndex = 0; nIndex < schemaCount; nIndex++)
                {
                    FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nIndex);
                    STRING currSchemaName = (wchar_t*)ffs->GetName();

                    // Check if this schema is from secondary source
                    if (currSchemaName.find(L"[") == 0)
                    {
                        // Found a schema for secondary source, so just skip over it
                        continue;
                    }

                    // get the class collection for this schema
                    FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
                    FdoInt32 classCount = fcc->GetCount();

                    for (int nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
                    {
                        originalClassDef = fcc->GetItem(nClassIndex);

                        STRING className = (wchar_t*)originalClassDef->GetName();
                        if (className == primClassName)
                        {
                            // get the class definition
                            extClassDefinition = MgServerFeatureUtil::GetMgClassDefinition(originalClassDef, serialize);
                            break;
                        }
                    }

                    break;
                }

                if (NULL == extClassDefinition)
                {
                    continue;
                }

                CalculatedPropertyCollection* calcPropsColl = extension->GetCalculatedProperties();
                if (calcPropsColl != NULL && calcPropsColl->GetCount() != 0)
                {
                    FdoPtr<FdoIdentifierCollection> idList = FdoIdentifierCollection::Create();
                    for (int idx = 0; idx < calcPropsColl->GetCount(); idx++)
                    {
                        CalculatedProperty* calcProp = calcPropsColl->GetAt(idx);
                        FdoPtr<FdoExpression> expressionCalc = FdoExpression::Parse(calcProp->GetExpression().c_str());
                        FdoPtr<FdoComputedIdentifier> idfCalc = FdoComputedIdentifier::Create(calcProp->GetName().c_str(), expressionCalc);
                        idList->Add(idfCalc);
                    }

                    Ptr<MgPropertyDefinitionCollection> mpdcLocal = extClassDefinition->GetProperties();
                    for(int idx = 0; idx < calcPropsColl->GetCount(); idx++)
                    {
                        CalculatedProperty* calcProp = calcPropsColl->GetAt(idx);
                        if (calcProp != NULL)
                        {
                            MdfString nameExpr = calcProp->GetName();
                            MdfString valueExpr = calcProp->GetExpression();
                            if (nameExpr.size() != 0 && valueExpr.size() != 0)
                            {
                                FdoPropertyType retPropType = FdoPropertyType_DataProperty;
                                FdoDataType retDataType = FdoDataType_Double;
                                FdoPtr<FdoExpression> expr = FdoExpression::Parse(valueExpr.c_str());
                                FdoPtr<FdoExpression> expandedExpression = FdoExpressionEngineCopyFilter::Copy(expr, idList);
                                FdoExpressionEngine::GetExpressionType(originalClassDef, expandedExpression, retPropType, retDataType);
                                if (retPropType == FdoPropertyType_DataProperty)
                                {
                                    STRING namePropStr = STRING(nameExpr.c_str());
                                    Ptr<MgDataPropertyDefinition> propDefExpr = new MgDataPropertyDefinition(namePropStr);
                                    propDefExpr->SetDataType(MgServerFeatureUtil::GetMgPropertyType(retDataType));
                                    propDefExpr->SetNullable(true);
                                    propDefExpr->SetReadOnly(true);
                                    propDefExpr->SetAutoGeneration(false);
                                    mpdcLocal->Add(propDefExpr);
                                }
                            }
                        }
                    }
                }
                //
                // Finished adding primary class properties to the extension class definition
                // Now add the secondary class properties
                //

                // Determine the number of secondary sources (AttributeRelates)
                MdfModel::AttributeRelateCollection* attributeRelates = extension->GetAttributeRelates();
                CHECKNULL(attributeRelates, L"MgDescribeSchema.DescribeSchema");
                int nAttributeRelateCount = attributeRelates->GetCount();

                for (int arIndex = 0; arIndex < nAttributeRelateCount; arIndex++)
                {
                    // get the attribute relate
                    MdfModel::AttributeRelate* attributeRelate = attributeRelates->GetAt(arIndex);
                    CHECKNULL(attributeRelate, L"MgDescribeSchema.DescribeSchema");

                    // Get the name of the secondary feature class (AttributeClass)
                    STRING attributeClass = (STRING)attributeRelate->GetAttributeClass();

                    // Parse the schema name from the class name;
                    STRING secSchemaName, secClassName;
                    MgUtil::ParseQualifiedClassName(attributeClass, secSchemaName, secClassName);

                    // Get the relation name
                    STRING relationName = (STRING)attributeRelate->GetName();

                    // Get the attributeName delimiter ( if none specified, default will be "" (blank) )
                    STRING attributeNameDelimiter = (STRING)attributeRelate->GetAttributeNameDelimiter();

                    // Get the resource id of the secondary feature source
                    STRING secondaryResourceId = (STRING)attributeRelate->GetResourceId();

                    // Establish connection to provider for secondary feature source
                    Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);
                    if (NULL != secondaryFeatureSource)
                    {
                        FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                        Ptr<MgServerFeatureConnection> connection2 = new MgServerFeatureConnection(secondaryFeatureSource);

                        if ((NULL != connection2.p) && ( connection2->IsConnectionOpen() ))
                        {
                            // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up before the parent object
                            // otherwise it leaves the FDO connection marked as still in use.
                            FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                            // Get the schema collection for the secondary resource
                            FdoPtr<FdoIDescribeSchema> fdoCommand2  = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                            CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeSchema");

                            //if (!secSchemaName.empty())
                            //{
                            //    fdoCommand2->SetSchemaName(secSchemaName.c_str());
                            //}

                            //if (!secClassName.empty())
                            //{
                            //    FdoPtr<FdoStringCollection> fdoClassNames2 = FdoStringCollection::Create();

                            //    fdoClassNames2->Add(secClassName.c_str());
                            //    fdoCommand2->SetClassNames(fdoClassNames2.p);
                            //}

                            // Execute the command
                            ffsc2 = fdoCommand2->Execute();
                            CHECKNULL((FdoFeatureSchemaCollection*)ffsc2, L"MgServerDescribeSchema.DescribeSchema");

                            int nSecSchemaCnt = (int)ffsc2->GetCount();

                            // cycle thru FdoFeatureSchemaCollection for secSchemaName
                            for (int nSecSchemaIndex = 0; nSecSchemaIndex < nSecSchemaCnt; nSecSchemaIndex++)
                            {
                                // retrieve the schema
                                FdoPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(nSecSchemaIndex);
                                STRING fdoSchemaName = (wchar_t*)ffs->GetName();

                                if (fdoSchemaName != secSchemaName)
                                {
                                    continue;
                                }

                                // get the class collection for schema
                                FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
                                FdoInt32 classCount = fcc->GetCount();

                                // cycle thru class collection for secClassName
                                for (int nClassIndex = 0; nClassIndex < classCount; nClassIndex++)
                                {
                                    Ptr<MgPropertyDefinitionCollection> mpdc = extClassDefinition->GetProperties();

                                    FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                                    STRING qualifiedName = (const wchar_t*)fc->GetQualifiedName();
                                    STRING className = (wchar_t*)fc->GetName();

                                    if (className != secClassName)
                                    {
                                        continue;
                                    }

                                    // Set the parent name for the secondary class definition
                                    FdoPtr<FdoSchemaElement> parent = fc->GetParent();
                                    if (!secSchemaName.empty())
                                    {
                                        parent->SetName(secSchemaName.c_str());
                                    }

                                    // get the secondary class definition
                                    Ptr<MgClassDefinition> classDefinition = MgServerFeatureUtil::GetMgClassDefinition(fc, serialize);

                                    // retrieve the secondary properties and prefix them with the relation name
                                    Ptr<MgPropertyDefinitionCollection> mpdc2 = classDefinition->GetProperties();
                                    INT32 mpdc2Count = mpdc2->GetCount();

                                    // Prefix the secondary properties with relationName and add to the extClassDefinition
                                    for (INT32 secPropIndex = 0; secPropIndex < mpdc2Count; secPropIndex++)
                                    {
                                        Ptr<MgPropertyDefinition> propDef = mpdc2->GetItem(secPropIndex);
                                        if ( MgFeaturePropertyType::GeometricProperty != propDef->GetPropertyType()
                                            && MgFeaturePropertyType::RasterProperty != propDef->GetPropertyType() )
                                        {
                                            STRING secPropName = propDef->GetName();
                                            secPropName = relationName + attributeNameDelimiter + secPropName;
                                            propDef->SetName(secPropName);
                                            mpdc->Add(propDef);
                                        }
                                    }

                                    break;

                                }  // end loop thru secondary class collection

                                break;

                            }  // end loop thru secondary schemas
                        }
                        else
                        {
                            throw new MgConnectionFailedException(L"MgServerDescribeSchema.DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
                        }

                    }  // end if (NULL != secFeatureSource)

                }  // end loop thru all attribute relates (joins)

                if (!extensionName.empty())
                {
                    extClassDefinition->SetName(extensionName);
                }

                // Add the extension class definition to the MgClassDefinitionCollection
                classCol->Add(extClassDefinition);

            }  // Repeat for all extensions

            // Add the schema to the MgFeatureSchemaCollection
            fsCollection->Add(schema);

        }  // End loop thru all schemas

        m_featureServiceCache->SetSchemas(resource, schemaName, classNames, serialize, fsCollection.p);
    }
    else
    {
        m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.DescribeSchema")

    return fsCollection.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// Executes the describe schema command and serializes the schema to XML
///
STRING MgServerDescribeSchema::DescribeSchemaAsXml(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames)
{
    STRING schemaXml;

    MG_FEATURE_SERVICE_TRY()

    schemaXml = m_featureServiceCache->GetSchemaXml(resource, schemaName, classNames);

    if (schemaXml.empty())
    {
        MgStringCollection* classNameCol = NULL;

        // Since the FDO provider does not know about the join information,
        // get the full schema if the feature source has joins.
        if (CheckExtendedFeatureClasses(resource, classNames))
        {
            schemaXml = m_featureServiceCache->GetSchemaXml(resource, schemaName, NULL);
        }
        else
        {
            classNameCol = classNames;
        }

        if (schemaXml.empty())
        {
            // The schema XML can be retrieved from either the serialized
            // schemas or the unserialized ones. So, try to get the serialized
            // schemas from the cache first then the unserialized ones later.
            Ptr<MgFeatureSchemaCollection> schemas = m_featureServiceCache->GetSchemas(
                resource, schemaName, classNameCol, true);

            if (NULL == schemas.p)
            {
                schemas = DescribeSchema(resource, schemaName, classNameCol, false);
            }
            else
            {
                m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
            }

            // Get the schema XML.
            schemaXml = SchemaToXml(schemas);

            m_featureServiceCache->SetSchemaXml(resource, schemaName, classNameCol, schemaXml);
        }
        else
        {
            m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
        }
    }
    else
    {
        m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.DescribeSchemaAsXml")

    return schemaXml;
}


//////////////////////////////////////////////////////////////////
// Converts MgFeatureSchemaCollection to XML
STRING MgServerDescribeSchema::SchemaToXml(MgFeatureSchemaCollection* schema)
{
    STRING xmlSchema;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == schema)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.SchemaToXml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CHECKNULL((MgFeatureSchemaCollection*)schema, L"MgServerDescribeSchema.SchemaToXml");

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = MgServerFeatureUtil::GetFdoFeatureSchemaCollection(schema);

    xmlSchema = GetSerializedXml(fdoSchemaCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.SchemaToXml")

    return xmlSchema;
}

//////////////////////////////////////////////////////////////////
// Converts MgFeatureSchemaCollection to XML with specified namespacePrefix and namespaceURL (Just used for OGC WFS certification)
STRING MgServerDescribeSchema::SchemaToXml(MgFeatureSchemaCollection* schema, CREFSTRING namespacePrefix, CREFSTRING namespaceUrl)
{
    STRING xmlSchema;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == schema)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.SchemaToXml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    CHECKNULL((MgFeatureSchemaCollection*)schema, L"MgServerDescribeSchema.SchemaToXml");

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = MgServerFeatureUtil::GetFdoFeatureSchemaCollection(schema);

    FdoPtr<FdoXmlFlags> flags = FdoXmlFlags::Create();

    if(!namespacePrefix.empty() && !namespaceUrl.empty())
    {
        FdoPtr<FdoPhysicalSchemaMappingCollection> fdoPhysicalSchemaMappingCol = FdoPhysicalSchemaMappingCollection::Create();
        for(int i = 0; i<fdoSchemaCol->GetCount(); i++)
        {
            FdoPtr<FdoFeatureSchema> fdoSchema = fdoSchemaCol->GetItem(i);
            FdoPtr<FdoXmlSchemaMapping> fdoSchemaMapping = FdoXmlSchemaMapping::Create(fdoSchema->GetName());
            fdoSchemaMapping->SetTargetNamespacePrefix(namespacePrefix.c_str());
            fdoSchemaMapping->SetTargetNamespace(namespaceUrl.c_str());
            fdoPhysicalSchemaMappingCol->Add(fdoSchemaMapping);
        }
        flags->SetSchemaMappings(fdoPhysicalSchemaMappingCol);
    }


    xmlSchema = GetSerializedXml(fdoSchemaCol,flags);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.SchemaToXml")

    return xmlSchema;
}

//////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerDescribeSchema::XmlToSchema(CREFSTRING xml)
{
    Ptr<MgFeatureSchemaCollection> mgSchemaCol;

    MG_FEATURE_SERVICE_TRY()
    mgSchemaCol = new MgFeatureSchemaCollection();

    string mbString = MgUtil::WideCharToMultiByte(xml);

    size_t len = mbString.length();

    FdoByte* gisBytes = (FdoByte*) mbString.c_str();

    FdoPtr<FdoIoMemoryStream> stream = FdoIoMemoryStream::Create();
    stream->Write(gisBytes, (FdoSize)len);

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);
    stream->Reset();
    fdoSchemaCol->ReadXml(stream);

    // Get schema count
    FdoInt32 cnt = fdoSchemaCol->GetCount();
    for (FdoInt32 i = 0; i < cnt; i++)
    {
        FdoPtr<FdoFeatureSchema> fdoSchema = fdoSchemaCol->GetItem(i);
        FdoStringP name = fdoSchema->GetName();
        CHECKNULL(name, L"MgServerDescribeSchema.XmlToSchema");

        FdoStringP description = fdoSchema->GetDescription();

        STRING tmpName(name);
        STRING tmpDesc(description);

        Ptr<MgFeatureSchema> mgSchema = new MgFeatureSchema(tmpName, tmpDesc);
        Ptr<MgClassDefinitionCollection> classCol = mgSchema->GetClasses();

        // Get all classes for a schema
        FdoPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
        FdoInt32 classCount = fdoClassCol->GetCount();

        for (FdoInt32 j = 0; j < classCount; j++)
        {
            FdoPtr<FdoClassDefinition> fdoClassDef = fdoClassCol->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            FdoStringP qname = fdoClassDef->GetQualifiedName();
            FdoStringP name = fdoClassDef->GetName();

            if (name != NULL && qname != NULL)
            {
                Ptr<MgClassDefinition> classDefinition = MgServerFeatureUtil::GetMgClassDefinition(fdoClassDef, true);
                classCol->Add(classDefinition);
            }
        }
        mgSchemaCol->Add(mgSchema);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.XmlToSchema")

    return mgSchemaCol.Detach();
}


//////////////////////////////////////////////////////////////////
STRING MgServerDescribeSchema::GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol)
{
    STRING serializedXml;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(fdoSchemaCol, L"MgServerDescribeSchema.GetSerializedXml");

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    CHECKNULL((FdoIoMemoryStream*)fmis, L"MgServerDescribeSchema.GetSerializedXml");

    // Write to memory stream
    fdoSchemaCol->WriteXml(fmis);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerDescribeSchema.GetSerializedXml");

    fmis->Read(bytes, (FdoSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    string out = MgUtil::GetTextFromReader(byteReader);
    serializedXml = MgUtil::MultiByteToWideChar(out);

    delete [] bytes;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetSerializedXml")

    return serializedXml;
}

//////////////////////////////////////////////////////////////////
STRING MgServerDescribeSchema::GetSerializedXml(FdoFeatureSchemaCollection* fdoSchemaCol, FdoXmlFlags* flags)
{
    STRING serializedXml;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(fdoSchemaCol, L"MgServerDescribeSchema.GetSerializedXml");

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    CHECKNULL((FdoIoMemoryStream*)fmis, L"MgServerDescribeSchema.GetSerializedXml");

    // Write to memory stream
    fdoSchemaCol->WriteXml(fmis,flags);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerDescribeSchema.GetSerializedXml");

    fmis->Read(bytes, (FdoSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    string out = MgUtil::GetTextFromReader(byteReader);
    serializedXml = MgUtil::MultiByteToWideChar(out);

    delete [] bytes;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetSerializedXml")

    return serializedXml;
}

///////////////////////////////////////////////////////////////////////////////
MgStringCollection* MgServerDescribeSchema::GetSchemas(MgResourceIdentifier* resource)
{
    Ptr<MgStringCollection> schemaNames;

    MG_FEATURE_SERVICE_TRY()

    schemaNames = m_featureServiceCache->GetSchemaNames(resource);

    if (NULL == schemaNames.p)
    {
        // Connect to provider.
        Ptr<MgServerFeatureConnection> connection = new MgServerFeatureConnection(resource);

        if ((NULL != connection.p) && (connection->IsConnectionOpen()))
        {
            if (connection->SupportsCommand((INT32)FdoCommandType_GetSchemaNames))
            {
                m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);

                // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up
                // before the parent object, otherwise it leaves the FDO connection marked as still in use.
                FdoPtr<FdoIConnection> fdoConn = connection->GetConnection();
                FdoPtr<FdoIGetSchemaNames> fdoCommand = (FdoIGetSchemaNames*)fdoConn->CreateCommand(FdoCommandType_GetSchemaNames);
                CHECKNULL(fdoCommand.p, L"MgServerDescribeSchema.GetSchemas");

                // Execute the command.
                FdoPtr<FdoStringCollection> schemas = fdoCommand->Execute();
                CHECKNULL(schemas.p, L"MgServerDescribeSchema.GetSchemas");

                // Get the schema names.
                schemaNames = MgServerFeatureUtil::FdoToMgStringCollection(schemas.p, false);
            }
            else // Fall back on using the DescribeSchema API.
            {
                // Release the connection so that it can be reused via the DescribeSchema API.
                connection = NULL;

                // The schema names can be retrieved from either the serialized
                // schemas or the unserialized ones. So, try to get the serialized
                // schemas from the cache first then the unserialized ones later.
                Ptr<MgFeatureSchemaCollection> schemas = m_featureServiceCache->GetSchemas(
                    resource, L"", NULL, true);

                if (NULL == schemas.p)
                {
                    schemas = DescribeSchema(resource, L"", NULL, false);
                }
                else
                {
                    m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
                }

                // Get the schema names.
                schemaNames = GetSchemaNames(schemas.p);
            }
        }
        else
        {
            throw new MgConnectionFailedException(L"MgServerDescribeSchema.GetSchemas",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        m_featureServiceCache->SetSchemaNames(resource, schemaNames.p);
    }
    else
    {
        m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.GetSchemas")

    return schemaNames.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgStringCollection* MgServerDescribeSchema::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    Ptr<MgStringCollection> classNames;

    MG_FEATURE_SERVICE_TRY()

    classNames = m_featureServiceCache->GetClassNames(resource, schemaName);

    if (NULL == classNames.p)
    {
        // Connect to provider.
        Ptr<MgServerFeatureConnection> connection = new MgServerFeatureConnection(resource);

        if ((NULL != connection.p) && (connection->IsConnectionOpen()))
        {
            // Fall back on using the DescribeSchema API to retrieve the class names
            // if the FDO provider does not support the GetClassNames command
            // or the feature source has joins.
            bool useSchema = true;

            if (connection->SupportsCommand((INT32)FdoCommandType_GetClassNames))
            {
                if (NULL == m_featureSourceCacheItem.p)
                {
                    m_featureSourceCacheItem = m_cacheManager->GetFeatureSourceCacheItem(resource);
                }

                MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
                CHECKNULL(featureSource, L"MgServerDescribeSchema.GetClasses");
                MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
                CHECKNULL(extensions, L"MgServerDescribeSchema.GetClasses");

                useSchema = (extensions->GetCount() > 0);
            }

            // TODO: The performance could be improved by getting the
            //       class names from the primary and secondary feature
            //       sources separately, then combining the results together.
            if (useSchema)
            {
                // Release the connection so that it can be reused via the DescribeSchema API.
                connection = NULL;

                // The class names can be retrieved from either the serialized
                // schemas or the unserialized ones. So, try to get the serialized
                // schemas from the cache first then the unserialized ones later.
                Ptr<MgFeatureSchemaCollection> schemas = m_featureServiceCache->GetSchemas(
                    resource, schemaName, NULL, true);

                if (NULL == schemas.p)
                {
                    schemas = DescribeSchema(resource, schemaName, NULL, false);
                }
                else
                {
                    m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
                }

                // Get the class names.
                classNames = GetClassNames(schemas.p, schemaName);
            }
            else
            {
                m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);

                // The reference to the FDO connection from the MgServerFeatureConnection object must be cleaned up
                // before the parent object, otherwise it leaves the FDO connection marked as still in use.
                FdoPtr<FdoIConnection> fdoConn = connection->GetConnection();
                FdoPtr<FdoIGetClassNames> fdoCommand = (FdoIGetClassNames*)fdoConn->CreateCommand(FdoCommandType_GetClassNames);
                CHECKNULL(fdoCommand.p, L"MgServerDescribeSchema.GetClasses");

                if (!schemaName.empty())
                {
                    fdoCommand->SetSchemaName(schemaName.c_str());
                }

                // Execute the command.
                FdoPtr<FdoStringCollection> classes = fdoCommand->Execute();
                CHECKNULL(classes.p, L"MgServerDescribeSchema.GetClasses");

                // Get the class names.
                classNames = MgServerFeatureUtil::FdoToMgStringCollection(classes.p, false);
            }
        }
        else
        {
            throw new MgConnectionFailedException(L"MgServerDescribeSchema.GetClasses",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }

        m_featureServiceCache->SetClassNames(resource, schemaName, classNames.p);
    }
    else
    {
        m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CHECK_CONNECTION_CATCH_AND_THROW(resource, L"MgServerDescribeSchema.GetClasses")

    return classNames.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerDescribeSchema::GetClassDefinition(  MgResourceIdentifier* resource,
                                                                CREFSTRING schemaName,
                                                                CREFSTRING className,
                                                                bool serialize)
{
    Ptr<MgClassDefinition> classDefinition;

    MG_FEATURE_SERVICE_TRY()

    if (className.empty())
    {
        throw new MgClassNotFoundException(
            L"MgServerDescribeSchema.GetClassDefinition",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    classDefinition = m_featureServiceCache->GetClassDefinition(resource, schemaName, className);

    if (NULL == classDefinition.p)
    {
        Ptr<MgStringCollection> classNames;

        // Since the FDO provider does not know about the join information,
        // use the full schema to retrieve the class definition if the feature source has joins.
        if (!CheckExtendedFeatureClass(resource, className))
        {
            classNames = new MgStringCollection();
            classNames->Add(className);
        }

        Ptr<MgFeatureSchemaCollection> schemas = DescribeSchema(
            resource, schemaName, classNames, serialize);

        // Get the class definition.
        classDefinition = GetClassDefinition(schemas.p, schemaName, className);

        if (NULL == classDefinition.p)
        {
            throw new MgClassNotFoundException(
                L"MgServerDescribeSchema.GetClassDefinition",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            m_featureServiceCache->SetClassDefinition(resource, schemaName, className, classDefinition.p);
        }
    }
    else
    {
        m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassDefinition")

    return classDefinition.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// Returns the collection of identity properties for the specified class.
/// If the schemaName is empty, then the className needs to be fully qualified.
///
MgClassDefinitionCollection* MgServerDescribeSchema::GetIdentityProperties(
    MgResourceIdentifier* resource, CREFSTRING schemaName, MgStringCollection* classNames)
{
    Ptr<MgClassDefinitionCollection> classDefs = new MgClassDefinitionCollection();

    MG_FEATURE_SERVICE_TRY()

    if (NULL == classNames || classNames->GetCount() == 0)
    {
        throw new MgClassNotFoundException(
            L"MgServerDescribeSchema.GetIdentityProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgStringCollection> uncachedClasses = new MgStringCollection();

    // Try pulling identity properties directly from the MapGuide cache first
    for (int i = 0; i < classNames->GetCount(); i++)
    {
        STRING className = classNames->GetItem(i);
        Ptr<MgPropertyDefinitionCollection> idProps = m_featureServiceCache->GetClassIdentityProperties(resource, schemaName, className);
        if (NULL != idProps.p)
        {
            // Identity properties found.  Add class to collection
            Ptr<MgClassDefinition> classDef = new MgClassDefinition();
            classDef->SetName(className);
            Ptr<MgPropertyDefinitionCollection> propDefs = classDef->GetIdentityProperties();
            for (int j = 0; j < idProps->GetCount(); j++)
            {
                Ptr<MgPropertyDefinition> idProp = idProps->GetItem(j);
                propDefs->Add(idProp);
            }
            classDefs->Add(classDef);
        }
        else
        {
            uncachedClasses->Add(className);
        }
    }

    if (uncachedClasses->GetCount() > 0)
    {
        // We did not find all the classes in our cache.  Query Fdo for them.

        // Since FDO does not know about joined feature sources,
        // use full schema to get the class identity properties
        // if any class is extended.
        bool classNameHintUsed = true;
        Ptr<MgStringCollection> emptyNames = new MgStringCollection();
        MgStringCollection* getNames = uncachedClasses;
        for (int i = 0; i < uncachedClasses->GetCount(); i++)
        {
            if (CheckExtendedFeatureClass(resource, uncachedClasses->GetItem(i)))
            {
                getNames = emptyNames;
                classNameHintUsed = false;
                break;
            }
        }

        // IMPORTANT INFORMATION
        // FDO objects cannot be cached as they are not thread safe.
        // This is not an issue because we create a MapGuide wrapper of this information that is thread safe.
        FdoPtr<FdoFeatureSchemaCollection> fdoSchemas;
        fdoSchemas = NULL;

        // The class identity properties can be retrieved from either the serialized
        // schemas or the unserialized ones. So, try to get the serialized
        // schemas from the cache first then the unserialized ones later.
        Ptr<MgFeatureSchemaCollection> mgSchemas = m_featureServiceCache->GetSchemas(
            resource, schemaName, getNames, true);

        if (NULL == mgSchemas.p)
        {
            mgSchemas = m_featureServiceCache->GetSchemas(
                resource, schemaName, getNames, false);
        }

        if (NULL == mgSchemas.p)
        {
            fdoSchemas = DescribeFdoSchema(resource, schemaName, getNames, classNameHintUsed);
        }
        else
        {
            m_cacheManager->CheckPermission(resource, MgResourcePermission::ReadOnly);
            fdoSchemas = MgServerFeatureUtil::GetFdoFeatureSchemaCollection(mgSchemas.p);
        }

        for (int j = 0; j < uncachedClasses->GetCount(); j++)
        {
            STRING className = uncachedClasses->GetItem(j);

            // Get the class identity properties.
            Ptr<MgPropertyDefinitionCollection> idProps = GetIdentityProperties(fdoSchemas.p, resource, schemaName, className);
            if (NULL != idProps.p && idProps->GetCount() > 0)
            {
                m_featureServiceCache->SetClassIdentityProperties(resource, schemaName, className, idProps.p);

                // Add class to collection
                Ptr<MgClassDefinition> classDef = new MgClassDefinition();
                classDef->SetName(className);
                Ptr<MgPropertyDefinitionCollection> propDefs = classDef->GetIdentityProperties();
                for (int k = 0; k < idProps->GetCount(); k++)
                {
                    Ptr<MgPropertyDefinition> idProp = idProps->GetItem(k);
                    propDefs->Add(idProp);
                }
                classDefs->Add(classDef);
            }
        }
    }


    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetIdentityProperties")

    return classDefs.Detach();
}

///////////////////////////////////////////////////////////////////////////////
bool MgServerDescribeSchema::GetIdentityProperties(CREFSTRING className,
    FdoClassCollection* classCol, MgPropertyDefinitionCollection* idProps)
{
    bool hasIdProps = false;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == classCol || NULL == idProps)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.GetIdentityProperties",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    FdoInt32 classCount = classCol->GetCount();

    for (FdoInt32 i = 0; i < classCount; ++i)
    {
        FdoPtr<FdoClassDefinition> classDef = classCol->GetItem(i);
        FdoStringP qname = classDef->GetQualifiedName();
        FdoStringP name = classDef->GetName();

        // Class name can be either fully qualified or non-qualified name.
        int idx1 = ::wcscmp(className.c_str(), qname);
        int idx2 = ::wcscmp(className.c_str(), name);

        if (0 == idx1 || 0 == idx2)
        {
            // Get identity properties for only the primary source (ie extensionName is blank).
            STRING qualifiedName = (const wchar_t*)qname;
            STRING::size_type nExtensionStart = qualifiedName.find(L"[");

            if (STRING::npos == nExtensionStart)
            {
                // Retrieve identity properties from FDO.
                FdoPtr<FdoDataPropertyDefinitionCollection> propDefCol =
                    classDef->GetIdentityProperties();

                MgServerFeatureUtil::GetClassProperties(idProps, propDefCol);
                hasIdProps = true;
                break;
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetIdentityProperties")

    return hasIdProps;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Determine whether or not the provider supports the Class Name hint for the
/// Describe Schema command.
///
bool MgServerDescribeSchema::IsClassNameHintUsed(FdoIDescribeSchema* fdoCommand)
{
    CHECKNULL(fdoCommand, L"MgServerDescribeSchema.IsClassNameHintUsed");

    FdoPtr<FdoStringCollection> classNames = fdoCommand->GetClassNames();
    bool classNameHintUsed = (NULL != classNames.p);

    return classNameHintUsed;
}

///////////////////////////////////////////////////////////////////////////////
MgStringCollection* MgServerDescribeSchema::GetSchemaNames(MgFeatureSchemaCollection* schemas)
{
    CHECKNULL(schemas, L"MgServerDescribeSchema.GetSchemaNames");

    Ptr<MgStringCollection> schemaNames = new MgStringCollection();
    INT32 schemaCount = schemas->GetCount();

    for (INT32 i = 0; i < schemaCount; ++i)
    {
        Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
        STRING currSchemaName = currSchema->GetName();

        if (!currSchemaName.empty())
        {
            schemaNames->Add(currSchemaName);
        }
    }

    return schemaNames.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgStringCollection* MgServerDescribeSchema::GetClassNames(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName)
{
    CHECKNULL(schemas, L"MgServerDescribeSchema.GetClassNames");

    Ptr<MgStringCollection> classNames = new MgStringCollection();
    INT32 schemaCount = schemas->GetCount();

    for (INT32 i = 0; i < schemaCount; ++i)
    {
        Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
        STRING currSchemaName = currSchema->GetName();
        bool schemaNameFound = false;

        if (schemaName.empty() || (schemaNameFound = (schemaName == currSchemaName)))
        {
            Ptr<MgClassDefinitionCollection> currClasses = currSchema->GetClasses();
            INT32 classCount = currClasses->GetCount();

            for (INT32 j = 0; j < classCount; ++j)
            {
                Ptr<MgClassDefinition> currClass = currClasses->GetItem(j);
                STRING currClassName = currClass->GetName();
                STRING currQualifiedName;

                MgUtil::FormatQualifiedClassName(currSchemaName, currClassName, currQualifiedName);
                classNames->Add(currQualifiedName);
            }
        }

        if (schemaNameFound)
        {
            break;
        }
    }

    return classNames.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerDescribeSchema::GetClassDefinition(MgFeatureSchemaCollection* schemas, CREFSTRING schemaName, CREFSTRING className)
{
    CHECKNULL(schemas, L"MgServerDescribeSchema.GetClassDefinition");

    Ptr<MgClassDefinition> classDef;
    INT32 schemaCount = schemas->GetCount();

    for (INT32 i = 0; i < schemaCount; ++i)
    {
        Ptr<MgFeatureSchema> currSchema = schemas->GetItem(i);
        STRING currSchemaName = currSchema->GetName();
        bool schemaNameFound = false;

        if (schemaName.empty() || (schemaNameFound = (schemaName == currSchemaName)))
        {
            Ptr<MgClassDefinitionCollection> currClasses = currSchema->GetClasses();
            INT32 classCount = currClasses->GetCount();

            for (INT32 j = 0; j < classCount; ++j)
            {
                Ptr<MgClassDefinition> currClass = currClasses->GetItem(j);
                STRING currClassName = currClass->GetName();

                if (className == currClassName)
                {
                    classDef = currClass;
                    break;
                }
                else
                {
                    STRING parsedSchemaName, parsedClassName;
                    MgUtil::ParseQualifiedClassName(className, parsedSchemaName, parsedClassName);

                    if (parsedClassName == currClassName)
                    {
                        classDef = currClass;
                        break;
                    }
                }
            }
        }

        if (schemaNameFound || NULL != classDef.p)
        {
            break;
        }
    }

    return classDef.Detach();
}

///////////////////////////////////////////////////////////////////////////////
MgPropertyDefinitionCollection* MgServerDescribeSchema::GetIdentityProperties(
    FdoFeatureSchemaCollection* schemas, MgResourceIdentifier* resource,
    CREFSTRING schemaName, CREFSTRING className)
{
    CHECKNULL(schemas, L"MgServerDescribeSchema.GetIdentityProperties");

    Ptr<MgPropertyDefinitionCollection> idProps = new MgPropertyDefinitionCollection();
    INT32 schemaCount = schemas->GetCount();

    // There should be at least one schema for the primary feature source.
    for (INT32 schemaIndex = 0; schemaIndex < schemaCount; ++schemaIndex)
    {
        // Retrieve the schema for the primary feature source.
        FdoPtr<FdoFeatureSchema> currSchema = schemas->GetItem(schemaIndex);
        STRING currSchemaName = currSchema->GetName();
        bool schemaNameFound = false;

        if (schemaName.empty() || (schemaNameFound = (schemaName == currSchemaName)))
        {
            // Get all classes for a schema.
            FdoPtr<FdoClassCollection> currClass = currSchema->GetClasses();
            INT32 classCount = currClass->GetCount();

            // Find identity properties for the specified class.
            if (classCount > 0 &&
                !GetIdentityProperties(className, currClass.p, idProps.p))
            {
                // If cannot find matching class in the class collection,
                // it must be an extension class. So get the identity properties
                // from the primary feature class source.
                if (NULL == m_featureSourceCacheItem.p)
                {
                    m_featureSourceCacheItem = m_cacheManager->GetFeatureSourceCacheItem(resource);
                }

                // Get the class name for the primary source that is being extended.
                MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
                CHECKNULL(featureSource, L"MgServerDescribeSchema.GetIdentityProperties");
                MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
                CHECKNULL(extensions, L"MgServerDescribeSchema.GetIdentityProperties");
                STRING extensionFeatureClass;
                int extensionCount = extensions->GetCount();

                // Find identity properties for the extension class.
                for (int i = 0; i < extensionCount; ++i)
                {
                    MdfModel::Extension* extension = extensions->GetAt(i);
                    CHECKNULL(extension, L"MgServerDescribeSchema.GetIdentityProperties");

                    // Get the extension name.
                    STRING extensionName = (STRING)extension->GetName();

                    if (className == extensionName)
                    {
                        extensionFeatureClass = (STRING)extension->GetFeatureClass();
                        GetIdentityProperties(extensionFeatureClass, currClass.p, idProps.p);
                    }
                }
            }
        }

        if (schemaNameFound)
        {
            break;
        }
    }

    return idProps.Detach();
}

///////////////////////////////////////////////////////////////////////////////
bool MgServerDescribeSchema::CheckExtendedFeatureClass(MgResourceIdentifier* resource,
    CREFSTRING className)
{
    bool extended = false;

    if (!className.empty())
    {
        STRING parsedSchemaName, parsedClassName;
        MgUtil::ParseQualifiedClassName(className, parsedSchemaName, parsedClassName);

        if (NULL == m_featureSourceCacheItem.p)
        {
            m_featureSourceCacheItem = m_cacheManager->GetFeatureSourceCacheItem(resource);
        }

        MdfModel::FeatureSource* featureSource = m_featureSourceCacheItem->Get();
        CHECKNULL(featureSource, L"MgServerDescribeSchema.CheckExtendedFeatureClass");
        MdfModel::ExtensionCollection* extensions = featureSource->GetExtensions();
        CHECKNULL(extensions, L"MgServerDescribeSchema.CheckExtendedFeatureClass");
        int extensionCount = extensions->GetCount();

        for (int i = 0; i < extensionCount; ++i)
        {
            MdfModel::Extension* extension = extensions->GetAt(i);
            CHECKNULL(extension, L"MgServerDescribeSchema.CheckExtendedFeatureClass");
            STRING extensionName = (STRING)extension->GetName();

            STRING currSchemaName, currClassName;
            MgUtil::ParseQualifiedClassName(extensionName, currSchemaName, currClassName);

            if (currClassName == parsedClassName)
            {
                extended = true;
                break;
            }
        }
    }

    return extended;
}

///////////////////////////////////////////////////////////////////////////////
bool MgServerDescribeSchema::CheckExtendedFeatureClasses(MgResourceIdentifier* resource,
    MgStringCollection* classNames)
{
    bool extended = false;

    if (NULL != classNames)
    {
        INT32 classCount = classNames->GetCount();

        for (INT32 i = 0; i < classCount; ++i)
        {
            STRING currClassName = classNames->GetItem(i);

            if (CheckExtendedFeatureClass(resource, currClassName))
            {
                extended = true;
                break;
            }
        }
    }

    return extended;
}

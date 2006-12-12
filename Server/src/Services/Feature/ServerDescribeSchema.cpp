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
#include "ServerDescribeSchema.h"
#include "ServerFeatureConnection.h"
#include "ServerGetFeatures.h"
#include "ServerFeatureUtil.h"

#define CHECKOPEN(methodName) \
    if (!IsConnectionOpen())  \
        throw new MgConnectionNotOpenException(methodName, __LINE__, __WFILE__, NULL, L"", NULL); \


//////////////////////////////////////////////////////////////////
MgServerDescribeSchema::MgServerDescribeSchema()
{
}


//////////////////////////////////////////////////////////////////
MgServerDescribeSchema::~MgServerDescribeSchema()
{
}


//////////////////////////////////////////////////////////////////
FdoFeatureSchemaCollection* MgServerDescribeSchema::ExecuteDescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    FdoPtr<FdoFeatureSchemaCollection> ffsc;

    MG_FEATURE_SERVICE_TRY()

    // Connect to provider
    Ptr<MgServerFeatureConnection> connection = new MgServerFeatureConnection(resource);
    if (!(connection->IsConnectionOpen()))
    {
        throw new MgConnectionFailedException(L"MgServerDescribeSchema::ExecuteDescribeSchema()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // TODO: Check whether this command is supported by the provider
    FdoPtr<FdoIConnection> fdoConn = connection->GetConnection();
    FdoPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)fdoConn->CreateCommand(FdoCommandType_DescribeSchema);
    CHECKNULL((FdoIDescribeSchema*)fdoCommand, L"MgServerDescribeSchema.ExecuteDescribeSchema");

    if (!schemaName.empty())
    {
        fdoCommand->SetSchemaName(schemaName.c_str());
    }

    // Execute the command
    ffsc = fdoCommand->Execute();
    CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.ExecuteDescribeSchema");

    // Finished with primary feature source, so now cycle through any secondary sources
    // Retrieve XML from repository
    string featureSourceXmlContent;
    RetrieveFeatureSource(resource, featureSourceXmlContent);

    // Need to parse XML and get properties
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(featureSourceXmlContent.c_str());

    DOMElement* rootNode = xmlUtil.GetRootNode();
    DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );
    CHECKNULL(extensionNodeList, L"MgServerDescribeSchema.ExecuteDescribeSchema()");

    int extensionNodes = (int)extensionNodeList->getLength();

    for (int i = 0; i < extensionNodes; i++)
    {
        DOMNode* extensionNode = extensionNodeList->item(i);
        CHECKNULL(extensionNode, L"MgServerDescribeSchema.ExecuteDescribeSchema");

        DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
        int nNameNodes = (int)nameNodeList->getLength();

        // get the extension name node
        DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

        // get the extension name value
        STRING extensionName;
        xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, extensionName);

        // Determine the number of secondary sources (AttributeRelate nodes)
        DOMNodeList* attributeRelateNodeList = xmlUtil.GetNodeList(extensionNode, "AttributeRelate");
        int nAttributeRelateNodes = (int)attributeRelateNodeList->getLength();

        for (int arNodesIndex = 0; arNodesIndex < nAttributeRelateNodes; arNodesIndex++)
        {
            // get the atribute relate node
            DOMNode* attributeRelateNode = attributeRelateNodeList->item(arNodesIndex);

            // get the resource id of the secondary feature source
            STRING secondaryResourceId;
            xmlUtil.GetElementValue(attributeRelateNode, "ResourceId", secondaryResourceId);

            // Get the name for the join relationship (attribute relate name)
            STRING attributeRelateName;
            xmlUtil.GetElementValue(attributeRelateNode, "Name", attributeRelateName);

            // Get the secondary feature class (AttributeClass)
            STRING attributeClass;
            xmlUtil.GetElementValue(attributeRelateNode, "AttributeClass", attributeClass);

            // Parse the schema name form the classname;
            STRING::size_type nDelimiter = attributeClass.find(L":");
            STRING secSchemaName;
            STRING secClassName;
            secSchemaName = attributeClass.substr(0, nDelimiter);
            secClassName = attributeClass.substr(nDelimiter + 1);

            // Establish connection to provider for secondary feature source
            Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);

            if (NULL != secondaryFeatureSource)
            {
                FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                STRING providerName2;

                Ptr<MgServerFeatureConnection> connection2 = new MgServerFeatureConnection(secondaryFeatureSource);
                if ( connection2->IsConnectionOpen() )
                {
                    providerName2 = connection2->GetProviderName();
                }
                else
                {
                    throw new MgConnectionFailedException(L"MgServerDescribeSchema::ExecuteDescribeSchema()", __LINE__, __WFILE__, NULL, L"", NULL);
                }

                // Check whether this command is supported by the provider
                FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                FdoPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.ExecuteDescribeSchema");

                // Execute the command
                ffsc2 = fdoCommand2->Execute();

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
                    modifiedSchemaName =  L"[" + extensionName + L"]";
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

        }  // End of for-loop that iterates thru the secondary sources

    }  // End of for loop that iteratates thru the extensions in the feature source

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.ExecuteDescribeSchema")

    return ffsc.Detach();
}


//////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerDescribeSchema::DescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCollection> fsCollection;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    fsCollection = featureServiceCache->ContainsFeatureSchema(resource, schemaName);
    if(NULL == fsCollection)
    {
        fsCollection = new MgFeatureSchemaCollection();

        FdoPtr<FdoFeatureSchemaCollection> ffsc;
        ffsc = ExecuteDescribeSchema(resource, schemaName);
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.DescribeSchema");

        // Get schema count
        FdoInt32 schemaCnt = ffsc->GetCount();

        //
        // A new MgFeatureSchema needs to be created for each primary schema in m_ffsc
        //

        Ptr<MgFeatureSchema> schema;
        Ptr<MgClassDefinitionCollection> classCol;

        for (int nSchemaIndex = 0; nSchemaIndex < schemaCnt; nSchemaIndex++)
        {
            // Retrieve schema from the collection
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nSchemaIndex);
            STRING schemaName = (wchar_t*)ffs->GetName();

            // Check if this schema is from secondary source which will be prefixed with [ExtensionName][RelationName],
            // ie [ExtensionName][RelationName]SchemaName
            if (schemaName.find(L"[") == 0)
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
            FdoInt32 classCnt = fcc->GetCount();

            // Add the primary class definitions to the MgClassDefinitionCollection
            for (FdoInt32 nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
            {
                FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                FdoStringP qname = fc->GetQualifiedName();
                FdoStringP name = fc->GetName();

                if (name != NULL && qname != NULL)
                {
                    MgServerGetFeatures msgf;
                    Ptr<MgClassDefinition> classDefinition = msgf.GetMgClassDefinition(fc, true);
                    classCol->Add(classDefinition);
                }
            }

            //
            // A new MgClassDefinition needs to be created for each extension and added to the classCollection
            //

            // Retrieve XML from repository
            string featureSourceXmlContent;
            RetrieveFeatureSource(resource, featureSourceXmlContent);

            // Parse the XML and get properties
            MgXmlUtil xmlUtil;
            xmlUtil.ParseString(featureSourceXmlContent.c_str());

            // Determine how may extension nodes
            DOMElement* rootNode = xmlUtil.GetRootNode();
            DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );
            CHECKNULL(extensionNodeList, L"MgServerDescribeSchema.DescribeSchema()");
            int nExtensionNodes = (int)extensionNodeList->getLength();

            // For each extension node
            for (int nExtNodeIndex = 0; nExtNodeIndex < nExtensionNodes; nExtNodeIndex++)
            {
                Ptr<MgClassDefinition> extClassDefinition;

                DOMNode* extensionNode = extensionNodeList->item(nExtNodeIndex);
                CHECKNULL(extensionNode, L"MgServerDescribeSchema.DescribeSchema");

                DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
                int nNameNodes = (int)nameNodeList->getLength();

                // get the extension name node
                DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

                // get the extension name value
                STRING extensionName;
                xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, extensionName);

                // get FeatureClass element - this tells us which class is extended (SchemaName:ClassName)
                STRING featureClass;
                xmlUtil.GetElementValue(extensionNode, "FeatureClass", featureClass);

                // Parse the schemaname from the classname
                STRING::size_type nDelimiter = featureClass.find(L":");
                STRING primSchemaName;
                STRING primClassName;

                if(STRING::npos == nDelimiter)
                {
                    primSchemaName = L"";
                    primClassName = featureClass;
                }
                else
                {
                    primSchemaName = featureClass.substr(0, nDelimiter);
                    primClassName = featureClass.substr(nDelimiter + 1);
                }

                if (schemaName != primSchemaName)
                {
                    continue;
                }

                // Cycle thru m_ffsc for schemaName
                for (int nIndex = 0; nIndex < schemaCnt; nIndex++)
                {
                    FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(nIndex);
                    STRING schemaName = (wchar_t*)ffs->GetName();

                    // Check if this schema is from secondary source
                    if (schemaName.find(L"[") == 0)
                    {
                        // Found a schema for secondary source, so just skip over it
                        continue;
                    }

                    // get the class collection for for this schema
                    FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
                    FdoInt32 classCnt = fcc->GetCount();

                    for (int nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
                    {
                        FdoPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                        STRING className = (wchar_t*)fc->GetName();
                        if (className == primClassName)
                        {
                            // get the class definition
                            MgServerGetFeatures msgf;
                            extClassDefinition = msgf.GetMgClassDefinition(fc, true);
                        }
                    }

                    break;
                }

                if (NULL == extClassDefinition)
                {
                    continue;
                }

                //
                // Finished adding primary class properties to the extension class definition
                // Now add the secondary class properties.
                //

                // Determine the number of secondary sources (AttributeRelate nodes)
                DOMNodeList* attributeRelateNodeList = xmlUtil.GetNodeList(extensionNode, "AttributeRelate");
                int nAttributeRelateNodes = (int)attributeRelateNodeList->getLength();

                for (int arNodesIndex = 0; arNodesIndex < nAttributeRelateNodes; arNodesIndex++)
                {
                    // get the attribute relate node
                    DOMNode* attributeRelateNode = attributeRelateNodeList->item(arNodesIndex);

                    // Get the name of the secondary feature class (AttributeClass)
                    STRING attributeClass;
                    xmlUtil.GetElementValue(attributeRelateNode, "AttributeClass", attributeClass);

                    // Parse the schema name from the class name;
                    STRING::size_type nDelimiter = attributeClass.find(L":");
                    STRING secSchemaName;
                    STRING secClassName;

                    if(STRING::npos == nDelimiter)
                    {
                        secSchemaName = L"";
                        secClassName = attributeClass;
                    }
                    else
                    {
                        secSchemaName = attributeClass.substr(0, nDelimiter);
                        secClassName = attributeClass.substr(nDelimiter + 1);
                    }

                    // Get the relation name
                    STRING relationName;
                    xmlUtil.GetElementValue(attributeRelateNode, "Name", relationName);

                    // Get the attribute name delimiter ( if none specified, default will be "" (blank) )
                    STRING attributeNameDelimiter;
                    xmlUtil.GetElementValue(attributeRelateNode, "AttributeNameDelimiter", attributeNameDelimiter, false);

                    // get the resource id of the secondary feature source
                    STRING secondaryResourceId;
                    xmlUtil.GetElementValue(attributeRelateNode, "ResourceId", secondaryResourceId);

                    // Establish connection to provider for secondary feature source
                    Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);
                    if (NULL != secondaryFeatureSource)
                    {
                        FdoPtr<FdoFeatureSchemaCollection> ffsc2;
                        STRING providerName2;

                        Ptr<MgServerFeatureConnection> connection2 = new MgServerFeatureConnection(secondaryFeatureSource);
                        if ( connection2->IsConnectionOpen() )
                        {
                            providerName2 = connection2->GetProviderName();
                        }
                        else
                        {
                            throw new MgConnectionFailedException(L"MgServerDescribeSchema::DescribeSchema()", __LINE__, __WFILE__, NULL, L"", NULL);
                        }

                        // Get the schema collection for the secondary resource
                        FdoPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                        FdoPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                        CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeSchema");
                        ffsc2 = fdoCommand2->Execute();

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
                            FdoInt32 classCnt = fcc->GetCount();

                            // cycle thru class collection for secClassName
                            for (int nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
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
                                MgServerGetFeatures msgf;
                                Ptr<MgClassDefinition> classDefinition = msgf.GetMgClassDefinition(fc, true);

                                // retrieve the secondary properties and prefix them with the relation name
                                Ptr<MgPropertyDefinitionCollection> mpdc2 = classDefinition->GetProperties();
                                INT32 mpdc2Count = mpdc2->GetCount();

                                // Prefix the secondary properties with relationName and add to the extClassDefinition
                                for (INT32 secPropIndex = 0; secPropIndex < mpdc2Count; secPropIndex++)
                                {
                                    Ptr<MgPropertyDefinition> propDef = mpdc2->GetItem(secPropIndex);
                                    STRING secPropName = propDef->GetName();
                                    secPropName = relationName + attributeNameDelimiter + secPropName;
                                    propDef->SetName(secPropName);
                                    mpdc->Add(propDef);
                                }

                                break;

                            }  // end loop thru secondary class collection

                            break;

                        } // end loop thru secondary schemas

                    } // end if (NULL != secfeatureSource)

                }  // end loop thru all attribute relates (joins)

                if (!extensionName.empty())
                {
                    extClassDefinition->SetName(extensionName);
                }

                // Add the extension class definition to theMgClassDefinitionCollection
                classCol->Add(extClassDefinition);

            }  // Repeat for all extensions

            // Add the schema to the MgFeatureSchemaCollection
            fsCollection->Add(schema);

        }  // End loop thru all schemas

        featureServiceCache->AddFeatureSchema(resource, schemaName, fsCollection);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.DescribeSchema")

    return fsCollection.Detach();
}


//////////////////////////////////////////////////////////////////
// Executes the describe schema command and serializes the schema to XML
STRING MgServerDescribeSchema::DescribeSchemaAsXml(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    STRING schema;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.DescribeSchemaAsXml", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    schema = featureServiceCache->ContainsFeatureSchemaXml(resource, schemaName);
    if(0 == schema.size())
    {
        Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(resource, schemaName);
        schema = SchemaToXml(fsc);

        featureServiceCache->AddFeatureSchemaXml(resource, schemaName, schema);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.DescribeSchema")

    return schema;
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

    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol = GetFdoFeatureSchemaCollection(schema);

    xmlSchema = GetSerializedXml(fdoSchemaCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.SchemaToXml")

    return xmlSchema;
}


//////////////////////////////////////////////////////////////////
//TODO: I will take this out as I do not know how to instantiate FdoFeatureSchemaCollection
FdoFeatureSchemaCollection* MgServerDescribeSchema::GetFdoFeatureSchemaCollection(MgFeatureSchemaCollection* mgSchemaCol)
{
    FdoPtr<FdoFeatureSchemaCollection> fdoSchemaCol;

    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(mgSchemaCol, L"MgServerDescribeSchema.GetFdoFeatureSchemaCollection");
    fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);

    INT32 i = 0;
    INT32 count = mgSchemaCol->GetCount();

    for (i=0; i<count; i++)
    {
        Ptr<MgFeatureSchema> mgSchema = mgSchemaCol->GetItem(i);
        FdoPtr<FdoFeatureSchema> fdoSchema = GetFdoFeatureSchema(mgSchema);
        if (fdoSchemaCol->Contains(fdoSchema))
        {
            throw new MgDuplicateObjectException(L"MgServerDescribeSchema.GetFdoFeatureSchemaCollection", __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            fdoSchemaCol->Add(fdoSchema);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoFeatureSchemaCollection")

    return fdoSchemaCol.Detach();
}


//////////////////////////////////////////////////////////////////
FdoFeatureSchema* MgServerDescribeSchema::GetFdoFeatureSchema(MgFeatureSchema* mgSchema)
{
    FdoPtr<FdoFeatureSchema> fdoSchema;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgSchema, L"MgServerDescribeSchema.GetFdoFeatureSchema");

    fdoSchema = FdoFeatureSchema::Create();
    CHECKNULL(fdoSchema, L"MgServerDescribeSchema.GetFdoFeatureSchema");

    STRING name = mgSchema->GetName();
    if (!name.empty())
    {
        fdoSchema->SetName((FdoString*) name.c_str());
    }

    STRING description = mgSchema->GetDescription();
    if (!description.empty())
    {
        fdoSchema->SetDescription((FdoString*) description.c_str());
    }

    FdoPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
    Ptr<MgClassDefinitionCollection> awClassCol = mgSchema->GetClasses();

    GetFdoClassCollection(fdoClassCol, awClassCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoFeatureSchema")

    return fdoSchema.Detach();
}


//////////////////////////////////////////////////////////////////
void MgServerDescribeSchema::GetFdoClassCollection(FdoClassCollection* fdoClassCol, MgClassDefinitionCollection* mgClassDefCol)
{
    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgFeatureSchema*)mgClassDefCol, L"MgServerDescribeSchema.GetFdoFeatureSchema");

    INT32 count = mgClassDefCol->GetCount();
    INT32 i = 0;

    for (i=0; i<count; i++)
    {
        Ptr<MgClassDefinition> mgClassDef = mgClassDefCol->GetItem(i);
        FdoPtr<FdoClassDefinition> fdoClassDef = GetFdoClassDefinition(mgClassDef, fdoClassCol);
        CHECKNULL(fdoClassDef, L"MgServerDescribeSchema.GetFdoClassCollection")

        FdoStringP name = fdoClassDef->GetName();
        if (!FdoClassExist(name, fdoClassCol))
            fdoClassCol->Add(fdoClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoClassCollection")
}


//////////////////////////////////////////////////////////////////
FdoClassDefinition* MgServerDescribeSchema::GetFdoClassDefinition(MgClassDefinition* mgClassDef, FdoClassCollection* fdoClassCol)
{
    FdoPtr<FdoClassDefinition> fdoClassDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgClassDef, L"MgServerDescribeSchema.GetFdoClassDefinition");
    CHECKNULL(fdoClassCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    STRING name = mgClassDef->GetName();
    assert(!name.empty());

    fdoClassDef = fdoClassCol->FindItem(name.c_str());

    if (fdoClassDef != NULL) // Class is already available and therefore return from here.
        return fdoClassDef.Detach();

    //Create FdoClassDefinition
    STRING geomName = mgClassDef->GetDefaultGeometryPropertyName();

    //If we have geometry create feature class
    if (!geomName.empty())
    {
        fdoClassDef = FdoFeatureClass::Create();
    }
    else
    {
        fdoClassDef = FdoClass::Create();
    }

    CHECKNULL(fdoClassDef, L"MgServerDescribeSchema.GetFdoClassDefinition");

    FdoPtr<FdoPropertyDefinitionCollection> fdoPropDefCol = fdoClassDef->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*) fdoPropDefCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    FdoPtr<FdoDataPropertyDefinitionCollection> fdoIdentityPropDefCol = (FdoDataPropertyDefinitionCollection*)fdoClassDef->GetIdentityProperties();
    CHECKNULL((FdoDataPropertyDefinitionCollection*)fdoIdentityPropDefCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    //Set description
    STRING desc = mgClassDef->GetDescription();
    if (!desc.empty())
    {
        fdoClassDef->SetDescription((FdoString*) desc.c_str());
    }

    if (!name.empty())
    {
        fdoClassDef->SetName((FdoString*) name.c_str());
    }

    bool isComputed = mgClassDef->IsComputed();
    if (isComputed)
    {
        fdoClassDef->SetIsComputed(isComputed);
    }

    bool isAbstract = mgClassDef->IsAbstract();
    if (isAbstract)
    {
        fdoClassDef->SetIsAbstract(isAbstract);
    }

    // Retrieve Class properties
    Ptr<MgPropertyDefinitionCollection> mgPropDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*) mgPropDefCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    //Retrieve identity properties
    Ptr<MgPropertyDefinitionCollection> awDataPropDefCol = mgClassDef->GetIdentityProperties();

    //Add properties
    GetClassProperties(fdoPropDefCol, mgPropDefCol, fdoClassCol);

    //Add identity properties
    GetClassProperties(fdoIdentityPropDefCol, awDataPropDefCol);

    Ptr<MgClassDefinition> awBaseDef = mgClassDef->GetBaseClassDefinition();
    if (awBaseDef != NULL)
    {
        FdoPtr<FdoClassDefinition> fdoBaseDef;
        STRING bname = awBaseDef->GetName();
        assert(!bname.empty());
        if (!bname.empty()) // Empty name is an error
        {
            fdoBaseDef = fdoClassCol->FindItem(bname.c_str());
            if (fdoBaseDef == NULL) // Not found
            {
                fdoBaseDef = GetFdoClassDefinition(awBaseDef, fdoClassCol); // Create a new one
                if (fdoBaseDef != NULL)
                {
                    FdoStringP nameBase = fdoBaseDef->GetName();
                    if (!FdoClassExist(nameBase, fdoClassCol))
                        fdoClassCol->Add(fdoBaseDef);
                }
            }
            fdoClassDef->SetBaseClass(fdoBaseDef);
        }
    }

    if (!geomName.empty())
    {
        FdoPtr<FdoGeometricPropertyDefinition> defaultGeom = (FdoGeometricPropertyDefinition*)fdoPropDefCol->GetItem(geomName.c_str());
        FdoPtr<FdoFeatureClass> ffClass = FDO_SAFE_ADDREF((FdoFeatureClass*)((FdoClassDefinition*)fdoClassDef));
        ffClass->SetGeometryProperty(defaultGeom);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoClassCollection")

    return fdoClassDef.Detach();
}


//////////////////////////////////////////////////////////////////
void MgServerDescribeSchema::GetClassProperties(FdoPropertyDefinitionCollection* fdoPropDefCol,
                                                MgPropertyDefinitionCollection* propDefCol,
                                                FdoClassCollection* fdoClassCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == propDefCol)
        return;

    INT32 cnt = propDefCol->GetCount();
    INT32 i=0;
    for (i=0; i<cnt; i++)
    {
        // Get Mg Property
        Ptr<MgPropertyDefinition> awpd = propDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*)awpd, L"MgServerDescribeSchema.GetClassProperties");

        // Create Fdo Property
        FdoPtr<FdoPropertyDefinition> fdoProp = GetFdoPropertyDefinition(awpd, fdoClassCol);
        // Add it to class definition
        if (fdoProp != NULL)
        {
            fdoPropDefCol->Add(fdoProp);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassProperties")
}


//////////////////////////////////////////////////////////////////
// TODO: Not sure why FdoDataPropertyDefinitionCollection derives from SchemaCollection??
void MgServerDescribeSchema::GetClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol,
                                                MgPropertyDefinitionCollection* mgPropDefCol)
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == mgPropDefCol)
        return;

    INT32 cnt = mgPropDefCol->GetCount();
    INT32 i=0;
    for (i=0; i<cnt; i++)
    {
        //Get property
        Ptr<MgPropertyDefinition> mgPropDef = mgPropDefCol->GetItem(i);
        CHECKNULL((MgPropertyDefinition*) mgPropDef, L"MgServerDescribeSchema.GetClassProperties");

        //Create Fdo property
        FdoPtr<FdoDataPropertyDefinition> fdoPropDef = (FdoDataPropertyDefinition*)GetFdoPropertyDefinition(mgPropDef, NULL);
        //Add it to class definition
        if (fdoPropDef != NULL)
        {
            fdoPropDefCol->Add(fdoPropDef);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassProperties")
}


//////////////////////////////////////////////////////////////////
FdoPropertyDefinition* MgServerDescribeSchema::GetFdoPropertyDefinition(MgPropertyDefinition* mgPropDef, FdoClassCollection* fdoClassCol)
{
    CHECKNULL((MgPropertyDefinition*)mgPropDef, L"MgServerDescribeSchema.GetFdoPropertyDefinition");

    FdoPtr<FdoPropertyDefinition> fdoPropDef;
    MG_FEATURE_SERVICE_TRY()

    INT16 fpt = mgPropDef->GetPropertyType();

    switch (fpt)
    {
        // Represents a Data Property type.
        case MgFeaturePropertyType::DataProperty:
        {
            fdoPropDef = GetDataPropertyDefinition((MgDataPropertyDefinition*)mgPropDef);
            break;
        }
        // Represents an Object Property type.
        case MgFeaturePropertyType::ObjectProperty:
        {
            fdoPropDef = GetObjectPropertyDefinition((MgObjectPropertyDefinition*)mgPropDef, fdoClassCol);
            break;
        }

        // Represents a Geometric Property type.
        case MgFeaturePropertyType::GeometricProperty:
        {
            fdoPropDef = GetGeometricPropertyDefinition((MgGeometricPropertyDefinition*)mgPropDef);
            break;
        }
        // Represents an Association Property type.
        case MgFeaturePropertyType::AssociationProperty:
        {
            // TODO:
            break;
        }

        // Represents a Raster (image) Property type.
        case MgFeaturePropertyType::RasterProperty:
        {
            fdoPropDef = GetRasterPropertyDefinition((MgRasterPropertyDefinition*)mgPropDef);
            break;
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoDataPropertyDefinition* MgServerDescribeSchema::GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoDataPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()

    // Null can be a valid values in some cases and therefore if NULL is encountered return NULL.
    if (mgPropDef == NULL)
    {
        return NULL;
    }

    fdoPropDef = FdoDataPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((FdoString*)name.c_str());

    //Get data members
    STRING defaultVal = mgPropDef->GetDefaultValue();
    INT32 length = mgPropDef->GetLength();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING desc = mgPropDef->GetDescription();
    INT32 precision = mgPropDef->GetPrecision();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    INT32 scale = mgPropDef->GetScale();
    bool isAutoGenerated = mgPropDef->IsAutoGenerated();

    //Set it for Fdo
    FdoDataType dataType = GetFdoDataType(mgPropDef->GetDataType());
    fdoPropDef->SetDataType(dataType);

    if (!defaultVal.empty())
    {
        fdoPropDef->SetDefaultValue((FdoString*) defaultVal.c_str());
    }

    fdoPropDef->SetLength((FdoInt32)length);
    fdoPropDef->SetReadOnly(isReadOnly);

    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*)desc.c_str());
    }

    fdoPropDef->SetPrecision((FdoInt32)precision);
    fdoPropDef->SetNullable(isNullable);

    fdoPropDef->SetScale((FdoInt32)scale);
    fdoPropDef->SetIsAutoGenerated(isAutoGenerated);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoClassCollection")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoObjectPropertyDefinition* MgServerDescribeSchema::GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef, FdoClassCollection* fdoClassCol)
{
    FdoPtr<FdoObjectPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgObjectPropertyDefinition*)objPropDef, L"MgServerDescribeSchema.GetObjectPropertyDefinition");

    fdoPropDef = FdoObjectPropertyDefinition::Create();
    // Retrieve data from MgObjectProperty
    STRING name = objPropDef->GetName();
    STRING desc = objPropDef->GetDescription();
    INT32 objType = objPropDef->GetObjectType();
    INT32 orderType = objPropDef->GetOrderType();
    Ptr<MgClassDefinition> clsDef = objPropDef->GetClassDefinition();
    Ptr<MgDataPropertyDefinition> idProp = objPropDef->GetIdentityProperty();
    // Convert MgObjectProperty data members to Fdo data members
    FdoPtr<FdoClassDefinition> fdoClsDef = GetFdoClassDefinition(clsDef, fdoClassCol);
    CHECKNULL(fdoClsDef, L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    FdoPtr<FdoDataPropertyDefinition> fdoDataPropDef = GetDataPropertyDefinition(idProp);
    FdoObjectType fdoObjectType = MgServerFeatureUtil::MgObjectPropertyTypeToFdoObjectType(objType);
    FdoOrderType fdoOrderType = MgServerFeatureUtil::MgOrderingOptionToFdoOrderType(orderType);
    // Set them to Fdo object property
    fdoPropDef->SetName(name.c_str());
    fdoPropDef->SetDescription(desc.c_str());
    fdoPropDef->SetObjectType(fdoObjectType);
    fdoPropDef->SetOrderType(fdoOrderType);
    fdoPropDef->SetClass(fdoClsDef);
    fdoPropDef->SetIdentityProperty(fdoDataPropDef);

    if (fdoClassCol != NULL)
    {
        FdoStringP clsName = fdoClsDef->GetName();
        if (!FdoClassExist(clsName, fdoClassCol))
            fdoClassCol->Add(fdoClsDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoGeometricPropertyDefinition* MgServerDescribeSchema::GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoGeometricPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgGeometricPropertyDefinition*)mgPropDef, L"MgServerDescribeSchema.GetGeometricPropertyDefinition");

    STRING name = mgPropDef->GetName();
    fdoPropDef = FdoGeometricPropertyDefinition::Create();
    fdoPropDef->SetName((FdoString*) name.c_str());

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 geomTypes = mgPropDef->GetGeometryTypes();
    bool hasElev = mgPropDef->GetHasElevation();
    bool hasMeasure = mgPropDef->GetHasMeasure();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();
    STRING spatialContextName = mgPropDef->GetSpatialContextAssociation();

    //Set it for Fdo
    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*) desc.c_str());
    }

    fdoPropDef->SetGeometryTypes((FdoInt32)geomTypes);
    fdoPropDef->SetHasElevation(hasElev);
    fdoPropDef->SetHasMeasure(hasMeasure);

    fdoPropDef->SetReadOnly(isReadOnly);
    if(spatialContextName.empty())
    {
        fdoPropDef->SetSpatialContextAssociation((FdoString*)spatialContextName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetGeometricPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoRasterPropertyDefinition* MgServerDescribeSchema::GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef)
{
    FdoPtr<FdoRasterPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgRasterPropertyDefinition*)mgPropDef, L"MgServerDescribeSchema.GetRasterPropertyDefinition");

    fdoPropDef = FdoRasterPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((FdoString*) name.c_str());

    //Get data members
    STRING desc = mgPropDef->GetDescription();
    INT32 xsize = mgPropDef->GetDefaultImageXSize();
    INT32 ysize = mgPropDef->GetDefaultImageYSize();
    bool isNullable = mgPropDef->GetNullable();
    STRING qname = mgPropDef->GetQualifiedName();
    bool isReadOnly = mgPropDef->GetReadOnly();

    //Set it for Fdo
    if (!desc.empty())
    {
        fdoPropDef->SetDescription((FdoString*) desc.c_str());
    }

    fdoPropDef->SetDefaultImageXSize((FdoInt32)xsize);
    fdoPropDef->SetDefaultImageYSize((FdoInt32)ysize);
    fdoPropDef->SetNullable(isNullable);

    //Cannot set qualified name in fdo
    fdoPropDef->SetReadOnly(isReadOnly);
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetRasterPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoDataType MgServerDescribeSchema::GetFdoDataType(INT32 awPropType)
{
    FdoDataType fdoDataType;
    switch(awPropType)
    {
        // Represents a Boolean value of true or false.
        case MgPropertyType::Boolean:
        {
            fdoDataType = FdoDataType_Boolean;
            break;
        }
        // Represents unsigned 8-bit integers with values between 0 and 255.
        case MgPropertyType::Byte:
        {
            fdoDataType = FdoDataType_Byte;
            break;
        }
        // Represents a date and time value.
        case MgPropertyType::DateTime:
        {
            fdoDataType = FdoDataType_DateTime;
            break;
        }
        // Represents values ranging from 1.0 x 10^-28 to approximately 7.9 x 10^28
        // with 28-29 significant digits.
        //TODO: KNN - What do we do with decimal?
        //case FdoDataType_Decimal:
        //{
            // TODO: Fdo has some inconsistency with Decimal, therefore MapGuide
            // TODO: does not support it. Making this type as invalid argument
            // TODO: until we determine what to do with it???
            //throw new MgInvalidArgumentException(L"MgServerGetFeatures.GetMgPropertyType", __LINE__, __WFILE__, NULL, L"", NULL);
            // propDef = new MgPropertyDefinition(name, ptDecimal);
            // break;
        //}
        // Represents a floating point value ranging from approximately 5.0 x
        // 10^-324 to 1.7 x 10^308 with a precision of 15-16 digits.
        case MgPropertyType::Double:
        {
            fdoDataType = FdoDataType_Double;
            break;
        }
        // Represents signed 16-bit integers with values between -32768 and 32767.
        case MgPropertyType::Int16:
        {
            fdoDataType = FdoDataType_Int16;
            break;
        }
        // Represents signed 32-bit integers with values between -2147483648 and
        // 2147483647.
        case MgPropertyType::Int32:
        {
            fdoDataType = FdoDataType_Int32;
            break;
        }
        // Represents signed 64-bit integers with values between
        // -9223372036854775808 and 9223372036854775807.
        case MgPropertyType::Int64:
        {
            fdoDataType = FdoDataType_Int64;
            break;
        }
        // Represents floating point values ranging from approximately 1.5 x 10^-45
        // to 3.4 x 10^38: with a precision of 7 digits.
        case MgPropertyType::Single:
        {
            fdoDataType = FdoDataType_Single;
            break;
        }
        // Represents a Unicode character strings.
        case MgPropertyType::String:
        {
            fdoDataType = FdoDataType_String;
            break;
        }
        // Represents a binary large object stored as a collection of bytes.
        case MgPropertyType::Blob:
        {
            fdoDataType = FdoDataType_BLOB;
            break;
        }
        // Represents a character large object stored as a collection of
        // characters.
        case MgPropertyType::Clob:
        {
            fdoDataType = FdoDataType_CLOB;
            break;
        }
        default:
        {
            // We will probably never hit this code
            STRING buffer;
            MgUtil::Int32ToString(awPropType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgServerDescribeSchema.GetFdoDataType",
                __LINE__, __WFILE__, &arguments, L"MgInvalidPropertyType", NULL);
        }
    }

    return fdoDataType;
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
        FdoInt32 classCnt = fdoClassCol->GetCount();

        for (FdoInt32 j = 0; j < classCnt; j++)
        {
            FdoPtr<FdoClassDefinition> fdoClassDef = fdoClassCol->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            FdoStringP qname = fdoClassDef->GetQualifiedName();
            FdoStringP name = fdoClassDef->GetName();

            if (name != NULL && qname != NULL)
            {
                // TODO: Separate the utility methods from MgServerGetFeatures
                MgServerGetFeatures msgf;
                Ptr<MgClassDefinition> classDefinition = msgf.GetMgClassDefinition(fdoClassDef, true);
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
MgStringCollection* MgServerDescribeSchema::GetSchemas(MgResourceIdentifier* resource)
{
    Ptr<MgStringCollection> strCol;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.GetSchemas", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    strCol = featureServiceCache->ContainsSchemas(resource);
    if(NULL == strCol)
    {
        FdoPtr<FdoFeatureSchemaCollection> ffsc;
        ffsc = ExecuteDescribeSchema(resource,L""); // No schema name
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.GetSchemas");

        strCol = new MgStringCollection();
        FdoInt32 cnt = ffsc->GetCount();

        for (FdoInt32 i = 0; i < cnt; i++)
        {
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(i);
            // TODO: Should we return qualified or non-qualified name
            FdoStringP pname = ffs->GetQualifiedName();
            if (pname != NULL)
            {
                int len = (int)wcslen(pname);
                if (len > 0)
                {
                    strCol->Add(STRING(pname));
                }
            }
        }

        featureServiceCache->AddSchemas(resource, strCol);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetSchemas")

    return strCol.Detach();
}

//////////////////////////////////////////////////////////////////
MgStringCollection* MgServerDescribeSchema::GetClasses(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    Ptr<MgStringCollection> strCol;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.GetClasses", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    strCol = featureServiceCache->ContainsClasses(resource, schemaName);
    if(NULL == strCol)
    {
        FdoPtr<FdoFeatureSchemaCollection> ffsc;
        ffsc = ExecuteDescribeSchema(resource, schemaName); // No schema name
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.GetClasses");

        strCol = new MgStringCollection();
        // Get schema count
        FdoInt32 cnt = ffsc->GetCount();
        for (FdoInt32 i = 0; i < cnt; i++)
        {
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(i);
            // Get all classes for a schema
            FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
            FdoInt32 classCnt = fcc->GetCount();

            for (FdoInt32 j = 0; j < classCnt; j++)
            {
                FdoPtr<FdoClassDefinition> fc = fcc->GetItem(j);
                // TODO: Should we return qualified or non-qualified name
                FdoStringP pname = fc->GetQualifiedName();
                if (pname != NULL)
                {
                    strCol->Add(STRING(pname));
                }
            }
        }

        featureServiceCache->AddClasses(resource, schemaName, strCol);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClasses")

    return strCol.Detach();
}

//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerDescribeSchema::GetClassDefinition(  MgResourceIdentifier* resource,
                                                                CREFSTRING schemaName,
                                                                CREFSTRING className    )
{
    Ptr<MgClassDefinition> classDefinition;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(
            L"MgServerDescribeSchema.GetClassDefinition",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (className.empty())
    {
        throw new MgClassNotFoundException(
            L"MgServerDescribeSchema.GetClassDefinition",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    classDefinition = featureServiceCache->ContainsClassDefinition(resource, schemaName, className);

    if (NULL == classDefinition)
    {
        Ptr<MgFeatureSchemaCollection> schemaCollection = DescribeSchema(resource, schemaName);
        INT32 count = schemaCollection->GetCount();

        for (INT32 featureSchemaIndex = 0; featureSchemaIndex < count; featureSchemaIndex++)
        {
            Ptr<MgFeatureSchema> schema = schemaCollection->GetItem(featureSchemaIndex);
            //Get all classes for a schema
            Ptr<MgClassDefinitionCollection> classesCollection = schema->GetClasses();
            INT32 classesCount = classesCollection->GetCount();

            for (INT32 classesIndex = 0; classesIndex < classesCount; classesIndex++)
            {
                Ptr<MgClassDefinition> classDef = classesCollection->GetItem(classesIndex);
                STRING name = classDef->GetName();
             
                if (name != className)
                {
                    continue;
                }
                else
                {
                    classDefinition = classDef;
                }
            }
        }

        if (NULL == classDefinition)
        {
            throw new MgClassNotFoundException(
                L"MgServerDescribeSchema.GetClassDefinition",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            featureServiceCache->AddClassDefinition(resource, schemaName, className, classDefinition);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassDefinition")

    return classDefinition.Detach();
}

//////////////////////////////////////////////////////////////////
bool MgServerDescribeSchema::FdoClassExist(const wchar_t* name, FdoClassCollection* clsCol)
{
    if (name == NULL) // Empty name is an error
        return false;

    return (clsCol->IndexOf(name) >= 0);
}


//////////////////////////////////////////////////////////////////
// Returns the collection of identity properties for the specified class.
// If the schemaName is empty, then the className needs to be fully
// qualified.
MgPropertyDefinitionCollection* MgServerDescribeSchema::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                              CREFSTRING schemaName,
                                                                              CREFSTRING className)
{
    Ptr<MgPropertyDefinitionCollection> idProps;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.GetIdentityProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (className.empty())
    {
        throw new MgClassNotFoundException(L"MgServerDescribeSchema.GetIdentityProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgFeatureServiceCache* featureServiceCache = MgFeatureServiceCache::GetInstance();
    idProps = featureServiceCache->ContainsIdentityProperties(resource, schemaName, className);
    if(NULL == idProps)
    {
        idProps = new MgPropertyDefinitionCollection();

        FdoPtr<FdoFeatureSchemaCollection> ffsc;
        ffsc = ExecuteDescribeSchema(resource, schemaName);
        CHECKNULL((FdoFeatureSchemaCollection*)ffsc, L"MgServerDescribeSchema.GetIdentityProperties");

        // There should be at least one schema for the primary feature source
        FdoInt32 cnt = ffsc->GetCount();
        if (cnt > 0)
        {
            // Retrieve schema for primary feature source
            FdoPtr<FdoFeatureSchema> ffs = ffsc->GetItem(0);

            // Get all classes for a schema
            FdoPtr<FdoClassCollection> fcc = ffs->GetClasses();
            FdoInt32 classCnt = fcc->GetCount();

            bool bHaveIdProps = false;

            for (FdoInt32 j = 0; j < classCnt; j++)
            {
                FdoPtr<FdoClassDefinition> fc = fcc->GetItem(j);

                FdoStringP qname = fc->GetQualifiedName();
                FdoStringP name = fc->GetName();

                int idx1 = wcscmp(className.c_str(), qname);
                int idx2 = wcscmp(className.c_str(), name);
                // classname can be either fully qualified or non-qualified name
                if (idx1 == 0 || idx2 == 0)
                {
                    STRING qualifiedName = (const wchar_t*)qname;

                    // Get identity properties for only the primary source (ie extensionName is blank)
                    STRING::size_type nExtensionStart = qualifiedName.find(L"[");
                    if (STRING::npos == nExtensionStart)
                    {
                        // retrieve identity properties from FDO
                        FdoPtr<FdoDataPropertyDefinitionCollection> fdpdc = fc->GetIdentityProperties();

                        MgServerGetFeatures msgf;
                        msgf.GetClassProperties(idProps, fdpdc);
                        bHaveIdProps = true;
                        break;
                    }
                }
            }  // End loop thru the class collection

            // If cannot find matching class in the class collection, it must be an extension class
            // So get the identity properties from the primary feature class source
            if (!bHaveIdProps && classCnt > 0)
            {
                // Get the className for the primary source that is being extended
                // Retrieve XML from repository
                string featureSourceXmlContent;
                RetrieveFeatureSource(resource, featureSourceXmlContent);

                // Need to parse XML and get properties
                MgXmlUtil xmlUtil;
                xmlUtil.ParseString(featureSourceXmlContent.c_str());

                DOMElement* rootNode = xmlUtil.GetRootNode();
                DOMNodeList* extensionNodeList = xmlUtil.GetNodeList(rootNode, "Extension" /* NOXLATE */ );
                CHECKNULL(extensionNodeList, L"MgServerDescribeSchema.GetIdentityProperties");

                int extensionNodes = (int)extensionNodeList->getLength();
                STRING extensionFeatureClass;

                for (int i = 0; i < extensionNodes; i++)
                {
                    DOMNode* extensionNode = extensionNodeList->item(i);
                    CHECKNULL(extensionNode, L"MgServerDescribeSchema.GetIdentityProperties");

                    DOMNodeList* nameNodeList = xmlUtil.GetNodeList(extensionNode, "Name");
                    int nNameNodes = (int)nameNodeList->getLength();

                    // get the extension name node
                    DOMNode* extensionNameNode = nameNodeList->item(nNameNodes - 1);

                    // get the extension name value
                    STRING extensionName;
                    xmlUtil.GetTextFromElement((DOMElement*)extensionNameNode, extensionName);

                    if (className == extensionName)
                    {
                        DOMNodeList* featureClassNodeList = xmlUtil.GetNodeList(extensionNode, "FeatureClass");
                        int nFeatureClassNodes = (int)featureClassNodeList->getLength();

                        // get the extension feature class node
                        DOMNode* extensionFeatureClassNode = featureClassNodeList->item(nFeatureClassNodes - 1);

                        // get the extension feature class value
                        xmlUtil.GetTextFromElement((DOMElement*)extensionFeatureClassNode, extensionFeatureClass);

                        // Loop thru the class collection
                        for (int classIndex = 0; classIndex < classCnt; classIndex++)
                        {
                            FdoPtr<FdoClassDefinition> fc = fcc->GetItem(classIndex);

                            FdoStringP qname = fc->GetQualifiedName();
                            FdoStringP name = fc->GetName();

                            // classname can be either fully qualified or non-qualified name
                            int idx1 = wcscmp(extensionFeatureClass.c_str(), qname);
                            int idx2 = wcscmp(extensionFeatureClass.c_str(), name);

                            if (idx1 == 0 || idx2 == 0)
                            {
                                STRING qualifiedName = (const wchar_t*)qname;

                                // Get identity properties for only the primary source (ie extensionName is blank)
                                STRING::size_type nExtensionStart = qualifiedName.find(L"[");
                                if (STRING::npos == nExtensionStart)
                                {
                                    // retrieve identity properties from FDO
                                    FdoPtr<FdoDataPropertyDefinitionCollection> fdpdc = fc->GetIdentityProperties();

                                    MgServerGetFeatures msgf;
                                    msgf.GetClassProperties(idProps, fdpdc);
                                    break;
                                }
                            }  // end if (qualifiedName == extensionFeatureClass)
                        }  // end loop thru class collection
                    }  // end if (className == extensionName)
                }  // end loop thru extension nodes
            }  // end find identity properties for extension class
        }

        featureServiceCache->AddIdentityProperties(resource, schemaName, className, idProps);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetIdentityProperties")

    return idProps.Detach();
}

void MgServerDescribeSchema::RetrieveFeatureSource(MgResourceIdentifier* resource, string& resourceContent)
{
    CHECKNULL(resource, L"MgServerDescribeSchema.RetrieveFeatureSource");

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

void MgServerDescribeSchema::ValidateFeatureSource(string& featureSourceXmlContent)
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

        throw new MgInvalidFeatureSourceException(L"MgServerDescribeSchema.ValidateFeatureSource",
            __LINE__, __WFILE__, (MgStringCollection*)strCol, L"", NULL);
    }
}

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
void MgServerDescribeSchema::ExecuteDescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    MG_FEATURE_SERVICE_TRY()

    // Connect to provider
    m_connection = new MgServerFeatureConnection(resource);
    if ( m_connection->IsConnectionOpen() )
    {
        m_providerName = m_connection->GetProviderName();
    }
    else
    {
        throw new MgConnectionFailedException(L"MgServerDescribeSchema::ExecuteDescribeSchema()", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // TODO: Check whether this command is supported by the provider
    GisPtr<FdoIConnection> fdoConn = m_connection->GetConnection();
    GisPtr<FdoIDescribeSchema> fdoCommand = (FdoIDescribeSchema*)fdoConn->CreateCommand(FdoCommandType_DescribeSchema);
    CHECKNULL((FdoIDescribeSchema*)fdoCommand, L"MgServerDescribeSchema.ExecuteDescribeSchema");

    if (!schemaName.empty())
    {
        fdoCommand->SetSchemaName(schemaName.c_str());
    }

    // Execute the command
    m_ffsc = fdoCommand->Execute();
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.ExecuteDescribeSchema");

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
                GisPtr<FdoFeatureSchemaCollection> ffsc2;
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
                GisPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                GisPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.ExecuteDescribeSchema");

                // Execute the command
                ffsc2 = fdoCommand2->Execute();

                // Extract the schemas from the secondary collection and add them to the main collection
                // Get schema count
                GisInt32 cnt = ffsc2->GetCount();
                for (GisInt32 i = 0; i < cnt; i++)
                {
                    GisPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(i);
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
                    GisString* msn = modifiedSchemaName.c_str();
                    ffs->SetName(msn);

                    // Add this schema to the collection if it isn't already there
                    if (!m_ffsc->Contains(ffs))
                    {
                        m_ffsc->Add(ffs);
                    }
                }
            }

        }  // End of for-loop that iterates thru the secondary sources

    }  // End of for loop that iteratates thru the extensions in the feature source

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.ExecuteDescribeSchema")
}


//////////////////////////////////////////////////////////////////
MgFeatureSchemaCollection* MgServerDescribeSchema::DescribeSchema(MgResourceIdentifier* resource, CREFSTRING schemaName)
{
    Ptr<MgFeatureSchemaCollection> fsCollection = new MgFeatureSchemaCollection();

    MG_FEATURE_SERVICE_TRY()

    if (NULL == resource)
    {
        throw new MgNullArgumentException(L"MgServerDescribeSchema.DescribeSchema", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    this->ExecuteDescribeSchema(resource, schemaName);
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.DescribeSchema");

    // Get schema count
    GisInt32 schemaCnt = m_ffsc->GetCount();

    //
    // A new MgFeatureSchema needs to be created for each primary schema in m_ffsc
    //

    Ptr<MgFeatureSchema> schema;
    Ptr<MgClassDefinitionCollection> classCol;

    for (int nSchemaIndex = 0; nSchemaIndex < schemaCnt; nSchemaIndex++)
    {
        // Retrieve schema from the collection
        GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(nSchemaIndex);
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
        GisStringP fSchemaName = ffs->GetName();
        schema->SetName(STRING(fSchemaName));

        GisStringP fSchemaDesc = ffs->GetDescription();
        schema->SetDescription(STRING(fSchemaDesc));

        // Get all classes for the schema
        GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
        GisInt32 classCnt = fcc->GetCount();

        // Add the primary class definitions to the MgClassDefinitionCollection
        for (GisInt32 nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
        {
            GisPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

            GisStringP qname = fc->GetQualifiedName();
            GisStringP name = fc->GetName();

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
            primSchemaName = featureClass.substr(0, nDelimiter);
            primClassName = featureClass.substr(nDelimiter + 1);

            if (schemaName != primSchemaName)
            {
                continue;
            }

            // Cycle thru m_ffsc for schemaName
            for (int nIndex = 0; nIndex < schemaCnt; nIndex++)
            {
                GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(nIndex);
                STRING schemaName = (wchar_t*)ffs->GetName();

                // Check if this schema is from secondary source
                if (schemaName.find(L"[") == 0)
                {
                    // Found a schema for secondary source, so just skip over it
                    continue;
                }

                // get the class collection for for this schema
                GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
                GisInt32 classCnt = fcc->GetCount();

                for (int nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
                {
                    GisPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

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
                secSchemaName = attributeClass.substr(0, nDelimiter);
                secClassName = attributeClass.substr(nDelimiter + 1);

                // Get the relation name
                STRING relationName;
                xmlUtil.GetElementValue(attributeRelateNode, "Name", relationName);

                // get the resource id of the secondary feature source
                STRING secondaryResourceId;
                xmlUtil.GetElementValue(attributeRelateNode, "ResourceId", secondaryResourceId);

                // Establish connection to provider for secondary feature source
                Ptr<MgResourceIdentifier> secondaryFeatureSource = new MgResourceIdentifier(secondaryResourceId);
                if (NULL != secondaryFeatureSource)
                {
                    GisPtr<FdoFeatureSchemaCollection> ffsc2;
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
                    GisPtr<FdoIConnection> fdoConn2 = connection2->GetConnection();
                    GisPtr<FdoIDescribeSchema> fdoCommand2 = (FdoIDescribeSchema*)fdoConn2->CreateCommand(FdoCommandType_DescribeSchema);
                    CHECKNULL((FdoIDescribeSchema*)fdoCommand2, L"MgDescribeSchema.DescribeSchema");
                    ffsc2 = fdoCommand2->Execute();

                    int nSecSchemaCnt = (int)ffsc2->GetCount();

                    // cycle thru FdoFeatureSchemaCollection for secSchemaName
                    for (int nSecSchemaIndex = 0; nSecSchemaIndex < nSecSchemaCnt; nSecSchemaIndex++)
                    {
                        // retrieve the schema
                        GisPtr<FdoFeatureSchema> ffs = ffsc2->GetItem(nSecSchemaIndex);
                        STRING fdoSchemaName = (wchar_t*)ffs->GetName();

                        if (fdoSchemaName != secSchemaName)
                        {
                            continue;
                        }

                        // get the class collection for schema
                        GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
                        GisInt32 classCnt = fcc->GetCount();

                        // cycle thru class collection for secClassName
                        for (int nClassIndex = 0; nClassIndex < classCnt; nClassIndex++)
                        {
                            Ptr<MgPropertyDefinitionCollection> mpdc = extClassDefinition->GetProperties();

                            GisPtr<FdoClassDefinition> fc = fcc->GetItem(nClassIndex);

                            STRING qualifiedName = (const wchar_t*)fc->GetQualifiedName();
                            STRING className = (wchar_t*)fc->GetName();

                            if (className != secClassName)
                            {
                                continue;
                            }

                            // Set the parent name for the secondary class definition
                            GisPtr<FdoSchemaElement> parent = fc->GetParent();
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
                                secPropName = relationName + secPropName;
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

    Ptr<MgFeatureSchemaCollection> fsc = DescribeSchema(resource, schemaName);
    schema = SchemaToXml(fsc);

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

    GisPtr<FdoFeatureSchemaCollection> fdoSchemaCol = this->GetFdoFeatureSchemaCollection(schema);

    xmlSchema = this->GetSerializedXml(fdoSchemaCol);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.SchemaToXml")

    return xmlSchema;
}


//////////////////////////////////////////////////////////////////
//TODO: I will take this out as I do not know how to instantiate FdoFeatureSchemaCollection
FdoFeatureSchemaCollection* MgServerDescribeSchema::GetFdoFeatureSchemaCollection(MgFeatureSchemaCollection* mgSchemaCol)
{
    GisPtr<FdoFeatureSchemaCollection> fdoSchemaCol;

    MG_FEATURE_SERVICE_TRY()

    CHECKNULL(mgSchemaCol, L"MgServerDescribeSchema.GetFdoFeatureSchemaCollection");
    fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);

    INT32 i = 0;
    INT32 count = mgSchemaCol->GetCount();

    for (i=0; i<count; i++)
    {
        Ptr<MgFeatureSchema> mgSchema = mgSchemaCol->GetItem(i);
        GisPtr<FdoFeatureSchema> fdoSchema = this->GetFdoFeatureSchema(mgSchema);
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
    GisPtr<FdoFeatureSchema> fdoSchema;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgSchema, L"MgServerDescribeSchema.GetFdoFeatureSchema");

    fdoSchema = FdoFeatureSchema::Create();
    CHECKNULL(fdoSchema, L"MgServerDescribeSchema.GetFdoFeatureSchema");

    STRING name = mgSchema->GetName();
    if (!name.empty())
    {
        fdoSchema->SetName((GisString*) name.c_str());
    }

    STRING description = mgSchema->GetDescription();
    if (!description.empty())
    {
        fdoSchema->SetDescription((GisString*) description.c_str());
    }

    GisPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
    Ptr<MgClassDefinitionCollection> awClassCol = mgSchema->GetClasses();

    this->GetFdoClassCollection(fdoClassCol, awClassCol);

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
        GisPtr<FdoClassDefinition> fdoClassDef = this->GetFdoClassDefinition(mgClassDef, fdoClassCol);
        CHECKNULL(fdoClassDef, L"MgServerDescribeSchema.GetFdoClassCollection")

        GisStringP name = fdoClassDef->GetName();
        if (!FdoClassExist(name, fdoClassCol))
            fdoClassCol->Add(fdoClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoClassCollection")
}


//////////////////////////////////////////////////////////////////
FdoClassDefinition* MgServerDescribeSchema::GetFdoClassDefinition(MgClassDefinition* mgClassDef, FdoClassCollection* fdoClassCol)
{
    GisPtr<FdoClassDefinition> fdoClassDef;

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

    GisPtr<FdoPropertyDefinitionCollection> fdoPropDefCol = fdoClassDef->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*) fdoPropDefCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    GisPtr<FdoDataPropertyDefinitionCollection> fdoIdentityPropDefCol = (FdoDataPropertyDefinitionCollection*)fdoClassDef->GetIdentityProperties();
    CHECKNULL((FdoDataPropertyDefinitionCollection*)fdoIdentityPropDefCol, L"MgServerDescribeSchema.GetFdoClassDefinition");

    //Set description
    STRING desc = mgClassDef->GetDescription();
    if (!desc.empty())
    {
        fdoClassDef->SetDescription((GisString*) desc.c_str());
    }

    if (!name.empty())
    {
        fdoClassDef->SetName((GisString*) name.c_str());
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
    this->GetClassProperties(fdoPropDefCol, mgPropDefCol, fdoClassCol);

    //Add identity properties
    this->GetClassProperties(fdoIdentityPropDefCol, awDataPropDefCol);

    Ptr<MgClassDefinition> awBaseDef = mgClassDef->GetBaseClassDefinition();
    if (awBaseDef != NULL)
    {
        GisPtr<FdoClassDefinition> fdoBaseDef;
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
                    GisStringP nameBase = fdoBaseDef->GetName();
                    if (!FdoClassExist(nameBase, fdoClassCol))
                        fdoClassCol->Add(fdoBaseDef);
                }
            }
            fdoClassDef->SetBaseClass(fdoBaseDef);
        }
    }

    if (!geomName.empty())
    {
        GisPtr<FdoGeometricPropertyDefinition> defaultGeom = (FdoGeometricPropertyDefinition*)fdoPropDefCol->GetItem(geomName.c_str());
        GisPtr<FdoFeatureClass> ffClass = GIS_SAFE_ADDREF((FdoFeatureClass*)((FdoClassDefinition*)fdoClassDef));
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
        GisPtr<FdoPropertyDefinition> fdoProp = this->GetFdoPropertyDefinition(awpd, fdoClassCol);
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
        GisPtr<FdoDataPropertyDefinition> fdoPropDef = (FdoDataPropertyDefinition*)this->GetFdoPropertyDefinition(mgPropDef, NULL);
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

    GisPtr<FdoPropertyDefinition> fdoPropDef;
    MG_FEATURE_SERVICE_TRY()

    INT16 fpt = mgPropDef->GetPropertyType();

    switch (fpt)
    {
        // Represents a Data Property type.
        case MgFeaturePropertyType::DataProperty:
        {
            fdoPropDef = this->GetDataPropertyDefinition((MgDataPropertyDefinition*)mgPropDef);
            break;
        }
        // Represents an Object Property type.
        case MgFeaturePropertyType::ObjectProperty:
        {
            fdoPropDef = this->GetObjectPropertyDefinition((MgObjectPropertyDefinition*)mgPropDef, fdoClassCol);
            break;
        }

        // Represents a Geometric Property type.
        case MgFeaturePropertyType::GeometricProperty:
        {
            fdoPropDef = this->GetGeometricPropertyDefinition((MgGeometricPropertyDefinition*)mgPropDef);
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
            fdoPropDef = this->GetRasterPropertyDefinition((MgRasterPropertyDefinition*)mgPropDef);
            break;
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoDataPropertyDefinition* MgServerDescribeSchema::GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef)
{
    GisPtr<FdoDataPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()

    // Null can be a valid values in some cases and therefore if NULL is encountered return NULL.
    if (mgPropDef == NULL)
    {
        return NULL;
    }

    fdoPropDef = FdoDataPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((GisString*)name.c_str());

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
    FdoDataType dataType = this->GetFdoDataType(mgPropDef->GetDataType());
    fdoPropDef->SetDataType(dataType);

    if (!defaultVal.empty())
    {
        fdoPropDef->SetDefaultValue((GisString*) defaultVal.c_str());
    }

    fdoPropDef->SetLength((GisInt32)length);
    fdoPropDef->SetReadOnly(isReadOnly);

    if (!desc.empty())
    {
        fdoPropDef->SetDescription((GisString*)desc.c_str());
    }

    fdoPropDef->SetPrecision((GisInt32)precision);
    fdoPropDef->SetNullable(isNullable);

    fdoPropDef->SetScale((GisInt32)scale);
    fdoPropDef->SetIsAutoGenerated(isAutoGenerated);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetFdoClassCollection")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoObjectPropertyDefinition* MgServerDescribeSchema::GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef, FdoClassCollection* fdoClassCol)
{
    GisPtr<FdoObjectPropertyDefinition> fdoPropDef;

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
    GisPtr<FdoClassDefinition> fdoClsDef = this->GetFdoClassDefinition(clsDef, fdoClassCol);
    CHECKNULL(fdoClsDef, L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    GisPtr<FdoDataPropertyDefinition> fdoDataPropDef = this->GetDataPropertyDefinition(idProp);
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
        GisStringP clsName = fdoClsDef->GetName();
        if (!FdoClassExist(clsName, fdoClassCol))
            fdoClassCol->Add(fdoClsDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoGeometricPropertyDefinition* MgServerDescribeSchema::GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef)
{
    GisPtr<FdoGeometricPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgGeometricPropertyDefinition*)mgPropDef, L"MgServerDescribeSchema.GetGeometricPropertyDefinition");

    STRING name = mgPropDef->GetName();
    fdoPropDef = FdoGeometricPropertyDefinition::Create();
    fdoPropDef->SetName((GisString*) name.c_str());

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
        fdoPropDef->SetDescription((GisString*) desc.c_str());
    }

    fdoPropDef->SetGeometryTypes((GisInt32)geomTypes);
    fdoPropDef->SetHasElevation(hasElev);
    fdoPropDef->SetHasMeasure(hasMeasure);

    fdoPropDef->SetReadOnly(isReadOnly);
    if(spatialContextName.empty())
    {
        fdoPropDef->SetSpatialContextAssociation((GisString*)spatialContextName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetGeometricPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoRasterPropertyDefinition* MgServerDescribeSchema::GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef)
{
    GisPtr<FdoRasterPropertyDefinition> fdoPropDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL((MgRasterPropertyDefinition*)mgPropDef, L"MgServerDescribeSchema.GetRasterPropertyDefinition");

    fdoPropDef = FdoRasterPropertyDefinition::Create();
    STRING name = mgPropDef->GetName();
    fdoPropDef->SetName((GisString*) name.c_str());

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
        fdoPropDef->SetDescription((GisString*) desc.c_str());
    }

    fdoPropDef->SetDefaultImageXSize((GisInt32)xsize);
    fdoPropDef->SetDefaultImageYSize((GisInt32)ysize);
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

    GisByte* gisBytes = (GisByte*) mbString.c_str();

    GisPtr<GisIoMemoryStream> stream = GisIoMemoryStream::Create();
    stream->Write(gisBytes, (GisSize)len);

    GisPtr<FdoFeatureSchemaCollection> fdoSchemaCol = FdoFeatureSchemaCollection::Create((FdoSchemaElement*)NULL);
    stream->Reset();
    fdoSchemaCol->ReadXml(stream);

    // Get schema count
    GisInt32 cnt = fdoSchemaCol->GetCount();
    for (GisInt32 i = 0; i < cnt; i++)
    {
        GisPtr<FdoFeatureSchema> fdoSchema = fdoSchemaCol->GetItem(i);
        GisStringP name = fdoSchema->GetName();
        CHECKNULL(name, L"MgServerDescribeSchema.XmlToSchema");

        GisStringP description = fdoSchema->GetDescription();

        STRING tmpName(name);
        STRING tmpDesc(description);

        Ptr<MgFeatureSchema> mgSchema = new MgFeatureSchema(tmpName, tmpDesc);
        Ptr<MgClassDefinitionCollection> classCol = mgSchema->GetClasses();

        // Get all classes for a schema
        GisPtr<FdoClassCollection> fdoClassCol = fdoSchema->GetClasses();
        GisInt32 classCnt = fdoClassCol->GetCount();

        for (GisInt32 j = 0; j < classCnt; j++)
        {
            GisPtr<FdoClassDefinition> fdoClassDef = fdoClassCol->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            GisStringP qname = fdoClassDef->GetQualifiedName();
            GisStringP name = fdoClassDef->GetName();

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

    GisIoMemoryStreamP fmis = GisIoMemoryStream::Create();
    CHECKNULL((GisIoMemoryStream*)fmis, L"MgServerDescribeSchema.DescribeSchema");

    // Write to memory stream
    fdoSchemaCol->WriteXml(fmis);
    fmis->Reset(); // TODO: We should not be calling reset here. A defect in FDO should be fixed.

    GisInt64 len = fmis->GetLength();
    GisByte *bytes = new GisByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerDescribeSchema.DescribeSchema");

    fmis->Read(bytes, (GisSize)len);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    string out = MgUtil::GetTextFromReader(byteReader);
    serializedXml = MgUtil::MultiByteToWideChar(out);

    delete [] bytes;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.DescribeSchema")

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

    this->ExecuteDescribeSchema(resource,L""); // No schema name
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.DescribeSchema");

    strCol = new MgStringCollection();
    GisInt32 cnt = m_ffsc->GetCount();

    for (GisInt32 i = 0; i < cnt; i++)
    {
        GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(i);
        // TODO: Should we return qualified or non-qualified name
        GisStringP pname = ffs->GetQualifiedName();
        if (pname != NULL)
        {
            int len = (int)wcslen(pname);
            if (len > 0)
            {
                strCol->Add(STRING(pname));
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSchemas.GetSchemas")

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

    this->ExecuteDescribeSchema(resource, schemaName); // No schema name
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.GetClasses");

    strCol = new MgStringCollection();
    // Get schema count
    GisInt32 cnt = m_ffsc->GetCount();
    for (GisInt32 i = 0; i < cnt; i++)
    {
        GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(i);
        // Get all classes for a schema
        GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
        GisInt32 classCnt = fcc->GetCount();

        for (GisInt32 j = 0; j < classCnt; j++)
        {
            GisPtr<FdoClassDefinition> fc = fcc->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            GisStringP pname = fc->GetQualifiedName();
            if (pname != NULL)
            {
                strCol->Add(STRING(pname));
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSchemas.GetClasses")

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
        throw new MgNullArgumentException(L"MgServerDescribeSchema.GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (className.empty())
    {
        throw new MgClassNotFoundException(L"MgServerDescribeSchema.GetClassDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    this->ExecuteDescribeSchema(resource, schemaName); // No schema name
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.GetClassDefinition");

    // classDefinition = new MgClassDefinition();
    // Get schema count
    GisInt32 cnt = m_ffsc->GetCount();
    for (GisInt32 i = 0; i < cnt; i++)
    {
        GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(i);
        // Get all classes for a schema
        GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
        GisInt32 classCnt = fcc->GetCount();

        for (GisInt32 j = 0; j < classCnt; j++)
        {
            GisPtr<FdoClassDefinition> fc = fcc->GetItem(j);
            // TODO: Should we return qualified or non-qualified name
            GisStringP qname = fc->GetQualifiedName();
            GisStringP name = fc->GetName();

            if (name != NULL && qname != NULL)
            {
                int idx1 = wcscmp(className.c_str(), qname);
                int idx2 = wcscmp(className.c_str(), name);
                // className can be either fully qualified or non-qualified name
                if (idx1 == 0 || idx2 == 0)
                {
                    // TODO: Separate the utility methods from MgServerGetFeatures
                    MgServerGetFeatures msgf;
                    classDefinition = msgf.GetMgClassDefinition(fc, true);
                    break;
                }
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSchemas.GetClassDefinition")

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

    idProps = new MgPropertyDefinitionCollection();

    this->ExecuteDescribeSchema(resource, schemaName);
    CHECKNULL((FdoFeatureSchemaCollection*)m_ffsc, L"MgServerDescribeSchema.GetIdentityProperties");

    // There should be at least one schema for the primary feature source
    GisInt32 cnt = m_ffsc->GetCount();
    if (cnt > 0)
    {
        // Retrieve schema for primary feature source
        GisPtr<FdoFeatureSchema> ffs = m_ffsc->GetItem(0);

        // Get all classes for a schema
        GisPtr<FdoClassCollection> fcc = ffs->GetClasses();
        GisInt32 classCnt = fcc->GetCount();

        for (GisInt32 j = 0; j < classCnt; j++)
        {
            GisPtr<FdoClassDefinition> fc = fcc->GetItem(j);

            // check both the qualified and non-qualified names
            GisStringP qname = fc->GetQualifiedName();
            GisStringP name = fc->GetName();

            STRING qualifiedName = (const wchar_t*)qname;
            STRING::size_type nLength = qualifiedName.length();

            // Get identity properties for only the primary source (ie extensionName is blank)
            STRING::size_type nExtensionStart = qualifiedName.find(L"[");
            if (STRING::npos == nExtensionStart)
            {
                // retrieve identity properties from FDO
                GisPtr<FdoDataPropertyDefinitionCollection> fdpdc = fc->GetIdentityProperties();

                MgServerGetFeatures msgf;
                msgf.GetClassProperties(idProps, fdpdc);
                break;
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetSchemas.GetIdentityProperties")

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

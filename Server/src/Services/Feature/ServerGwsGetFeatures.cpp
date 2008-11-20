//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "Services/FeatureService.h"
#include "ServerGwsGetFeatures.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"
#include "ServerGwsFeatureReader.h"

MgServerGwsGetFeatures::MgServerGwsGetFeatures()
{
    m_featureSet = NULL;
    m_classDef = NULL;
    m_relationNames = NULL;
    m_gwsFeatureReader = NULL;
    m_gwsFeatureIterator = NULL;
    m_bReadNextDone = false;
    m_bReadNextHasData = false;
}


MgServerGwsGetFeatures::MgServerGwsGetFeatures(MgServerGwsFeatureReader* gwsFeatureReader,
                                               IGWSFeatureIterator* gwsFeatureIterator,
                                               MgStringCollection* attributeNameDelimiters,
                                               IGWSExtendedFeatureDescription* primaryExtendedFeatureDescription,
                                               bool bForceOneToOne)
{
    CHECKNULL(gwsFeatureReader, L"MgServerGwsGetFeatures.MgServerGwsGetFeatures");

    m_featureSet = NULL;
    m_classDef = NULL;
    m_relationNames = NULL;

    m_gwsFeatureReader = gwsFeatureReader;
    m_gwsFeatureIterator = SAFE_ADDREF(gwsFeatureIterator);

    m_attributeNameDelimiters = SAFE_ADDREF(attributeNameDelimiters);
    m_primaryExtendedFeatureDescription = FDO_SAFE_ADDREF(primaryExtendedFeatureDescription);

    m_bForceOneToOne = bForceOneToOne;
    m_bAdvancePrimaryIterator = true;
    m_bNoMoreData = false;
    m_bReadNextDone = false;
    m_bReadNextHasData = false;
}

MgServerGwsGetFeatures::~MgServerGwsGetFeatures()
{
    ClearGwsFeatureIterator();
}

void MgServerGwsGetFeatures::ClearGwsFeatureIterator()
{
    if (m_gwsFeatureIterator != NULL)
        m_gwsFeatureIterator->Close();

    FDO_SAFE_RELEASE(m_gwsFeatureIterator);
}

//////////////////////////////////////////////////////////////////
MgFeatureSet* MgServerGwsGetFeatures::GetFeatures(INT32 count)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsGetFeatures.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    INT32 featCount = count;

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Get MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = this->GetMgClassDefinition(true);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGwsGetFeatures.GetFeatures");
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);
    }

    if (NULL == (MgFeatureSet*)m_featureSet)
    {
        // Create a feature set for a pool of features
        m_featureSet = new MgFeatureSet();
        CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerGwsGetFeatures.GetFeatures");

        // Assign feature class definition to Feature Set
        m_featureSet->SetClassDefinition(m_classDef);
    }
    else
    {
        m_featureSet->ClearFeatures();
    }

    // If class definition contains raster property
    // we can only retrieve one feature at a time
    // because, user would need to supply x,y and extents before
    // they could fetch the raster data. Therefore we can not
    // advance FdoIReader.
    if (m_classDef->HasRasterProperty())
    {
        featCount = 1;
    }

    // Add all features to feature set
    this->AddFeatures(featCount);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsGetFeatures.GetFeatures")

    return SAFE_ADDREF((MgFeatureSet*)m_featureSet);
}


//////////////////////////////////////////////////////////////////
// Builds the MgClassDefinition for the feature join results
/////
MgClassDefinition* MgServerGwsGetFeatures::GetMgClassDefinition(bool bSerialize)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_gwsFeatureIterator->GetClassDefinition();
        CHECKNULL((FdoClassDefinition*)fdoClassDefinition, L"MgServerGwsGetFeatures.GetMgClassDefinition");

        // Convert FdoClassDefinition to MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = this->GetMgClassDefinition(fdoClassDefinition, bSerialize);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGwsGetFeatures.GetMgClassDefinition");

        // Advance the primary feature source iterator
        if (m_gwsFeatureIterator->ReadNext())
        {
            m_bReadNextHasData = true;
            // Retrieve the secondary feature source iterators, get the property definitions and add to class definition
            FdoPtr<IGWSExtendedFeatureDescription> desc;
            m_gwsFeatureIterator->DescribeFeature(&desc);

            int nSecondaryFeatures = desc->GetCount();
            if (nSecondaryFeatures > 0)
            {
                if (m_attributeNameDelimiters->GetCount() != nSecondaryFeatures)
                {
                    // Should never get here
                    throw new MgInvalidArgumentException(L"MgServerGwsGetFeatures.GetMgClassDefinition", __LINE__, __WFILE__, NULL, L"MgInvalidCollectionSize", NULL);
                }
            }

            for (int i = 0; i < nSecondaryFeatures; i++)
            {
                try
                {
                    FdoPtr<IGWSFeatureIterator> featureIter = m_gwsFeatureIterator->GetJoinedFeatures(i);
                    if (featureIter)
                    {
                        STRING attributeNameDelimiter = m_attributeNameDelimiters->GetItem(i);

                        // Retrieve the secondary class definitions
                        FdoPtr<FdoClassDefinition> secFdoClassDefinition;

                        if (featureIter->ReadNext())
                        {
                            m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(attributeNameDelimiter, featureIter));
                            secFdoClassDefinition = featureIter->GetClassDefinition();
                        }
                        else
                        {
                            CGwsQueryResultDescriptors * primaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> (desc.p);
                            int descriptorCount = primaryDescriptors->GetCount();
                            for (int dcIndex = 0; dcIndex < descriptorCount; dcIndex++)
                            {
                                CGwsQueryResultDescriptors* secondaryDescriptors = dynamic_cast<CGwsQueryResultDescriptors *> ( primaryDescriptors->GetItem(dcIndex) );
                                secFdoClassDefinition = secondaryDescriptors->ClassDefinition();
                            }
                        }
                        CHECKNULL((FdoClassDefinition*)secFdoClassDefinition, L"MgServerGwsGetFeatures.GetMgClassDefinition");

                        FdoStringP qname = secFdoClassDefinition->GetQualifiedName();

                        // Convert FdoClassDefinition to MgClassDefinition
                        Ptr<MgClassDefinition> secMgClassDef = this->GetMgClassDefinition(secFdoClassDefinition, bSerialize);
                        CHECKNULL((MgClassDefinition*)secMgClassDef, L"MgServerGwsGetFeatures.GetMgClassDefinition");

                        // retrieve the secondary properites and prefix them with the relation name
                        Ptr<MgPropertyDefinitionCollection> mpdc2 = secMgClassDef->GetProperties();
                        INT32 mpdc2Count = mpdc2->GetCount();

                        // Add the secondary properties to the classDefinition
                        Ptr<MgPropertyDefinitionCollection> mpdc = mgClassDef->GetProperties();
                        for (INT32 secPropIndex = 0; secPropIndex < mpdc2Count; secPropIndex++)
                        {
                            Ptr<MgPropertyDefinition> propDef = mpdc2->GetItem(secPropIndex);
                            STRING secPropName = propDef->GetName();
                            STRING relationName = (wchar_t*)m_relationNames->GetString(i + 1);
                            secPropName = relationName + attributeNameDelimiter + secPropName;
                            propDef->SetName(secPropName);
                            mpdc->Add(propDef);
                        }
                    }

                    mgClassDef->SetName(m_extensionName);
                }
                catch (FdoException* e)
                {
                    STRING messageId;
                    MgStringCollection arguments;
                    wchar_t* buf = (wchar_t*)e->GetExceptionMessage();

                    if (NULL != buf)
                    {
                        messageId = L"MgFormatInnerExceptionMessage";
                        arguments.Add(buf);
                    }

                    FDO_SAFE_RELEASE(e);

                    throw new MgFdoException(L"MgServerGwsGetFeatures.GetMgClassDefinition",
                        __LINE__, __WFILE__, NULL, messageId, &arguments);
                }
            }

            // Convert MgClassDefinition to FdoClassDefinition.
            MgServerDescribeSchema msds;
            Ptr<MgClassDefinitionCollection> mcdc = new MgClassDefinitionCollection();
            mcdc->Add(mgClassDef);
            FdoPtr<FdoClassCollection> fcc;
            fcc = FdoClassCollection::Create(NULL);
            msds.GetFdoClassCollection(fcc, mcdc);
            int nFccCount = fcc->GetCount();

            // Get the FdoClassDefinition
            FdoPtr<FdoClassDefinition> fdc = msds.GetFdoClassDefinition(mgClassDef, fcc);

            // Pass the FdoClassDefinition to SerializeToXml
            if (bSerialize)
            {
                STRING str;
                Ptr<MgByteReader> byteReader = SerializeToXml(fdc);
                str = byteReader->ToString();

                STRING str1 = L"";

                size_t idx = str.find(L"?>");

                if (idx >= 0)
                {
                    str1 = str.substr(idx+2);
                }
                else if (idx < 0)
                {
                    idx = str.find(L"<Class");
                    if (idx >= 0)
                    {
                        str1 = str.substr(idx);
                    }
                }

                // Set the serialized feature join result xml for the MgClassDefinition.
                mgClassDef->SetSerializedXml(str1);
            }
        }
        m_bReadNextDone = true;

        // Store the it for future use
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsGetFeatures.GetMgClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerGwsGetFeatures::GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize)
{
    CHECKNULL(fdoClassDefinition, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    // Create MgClassDefinition
    Ptr<MgClassDefinition> mgClassDef = new MgClassDefinition();
    CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store property definitions
    Ptr<MgPropertyDefinitionCollection> propDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store key property definitions i.e. which makes key for this feature class
    Ptr<MgPropertyDefinitionCollection> identityPropDefCol = mgClassDef->GetIdentityProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)identityPropDefCol, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    // description
    FdoString* desc = fdoClassDefinition->GetDescription();
    if (desc != NULL)
    {
        mgClassDef->SetDescription(STRING(desc));
    }

    // Class name
    FdoString* className = fdoClassDefinition->GetName();
    if (className != NULL)
    {
        mgClassDef->SetName(STRING(className));
    }

    bool isComputed = fdoClassDefinition->GetIsComputed();
    if (isComputed)
    {
        mgClassDef->MakeClassComputed(isComputed);
    }

    bool isAbstract = fdoClassDefinition->GetIsComputed();
    if (isAbstract)
    {
        mgClassDef->MakeClassAbstract(isAbstract);
    }

    // Retrieve Class properties from FDO
    FdoPtr<FdoPropertyDefinitionCollection> fpdc = fdoClassDefinition->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*)fpdc, L"MgServerGwsGetFeatures.GetMgClassDefinition");

    // Retrieve Base class properties from FDO
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> frpdc = fdoClassDefinition->GetBaseProperties();

    // Retrieve identity properties from FDO
    FdoPtr<FdoDataPropertyDefinitionCollection> fdpdc = fdoClassDefinition->GetIdentityProperties();

    // Add properties
    this->GetClassProperties(propDefCol, fpdc);

    FdoClassType classType = fdoClassDefinition->GetClassType();
    if (classType == FdoClassType_FeatureClass)
    {
        FdoPtr<FdoGeometricPropertyDefinition> geomPropDef = ((FdoFeatureClass*)fdoClassDefinition)->GetGeometryProperty();
        if (geomPropDef != NULL)
        {
            FdoString* defaultGeomName = geomPropDef->GetName();
            if (defaultGeomName != NULL)
            {
                mgClassDef->SetDefaultGeometryPropertyName(STRING(defaultGeomName));
            }
        }
    }

    // Add identity properties
    this->GetClassProperties(identityPropDefCol, fdpdc);

    // Add base properties
//  this->GetClassProperties(propDefCol, frpdc);

    if (bSerialize)
    {
        // TODO: Need to optimize this ByteReader/ByteSink - too much conversion
        STRING str;
        Ptr<MgByteReader> byteReader = SerializeToXml(fdoClassDefinition);
        MgByteSink sink(byteReader);
        sink.ToString(str);

        STRING str1 = L"";

        size_t idx = str.find(L"?>");

        if (idx >= 0)
        {
            str1 = str.substr(idx+2);
        }
        else if (idx < 0)
        {
            idx = str.find(L"<Class");
            if (idx >= 0)
            {
                str1 = str.substr(idx);
            }
        }

        mgClassDef->SetSerializedXml(str1);
    }

    FdoPtr<FdoClassDefinition> baseDefinition = fdoClassDefinition->GetBaseClass();
    if (baseDefinition != NULL)
    {
        Ptr<MgClassDefinition> mgBaseClsDef = GetMgClassDefinition(baseDefinition, bSerialize);
        mgClassDef->SetBaseClassDefinition(mgBaseClsDef);
    }

    return mgClassDef.Detach();
}


//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                                FdoPropertyDefinitionCollection* fdoPropDefCol)
{
    // CHECKNULL((FdoPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetClassProperties");

    // Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgServerGwsGetFeatures.GetClassProperties");

        // Create MgProperty
        Ptr<MgPropertyDefinition> prop = this->GetMgPropertyDefinition(fpd);

        // Add it to class definition
        if (prop != NULL)
        {
            propDefCol->Add(prop);
        }
    }
}

//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                                FdoDataPropertyDefinitionCollection* fdoPropDefCol)
{
    // CHECKNULL((FdoDataPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetClassProperties");

    // Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt  = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgServerGwsGetFeatures.GetClassProperties");

        // Create MgProperty
        Ptr<MgPropertyDefinition> prop = this->GetMgPropertyDefinition(fpd);

        // Add it to class definition
        if (prop != NULL)
        {
            propDefCol->Add(prop);
        }
    }
}

//////////////////////////////////////////////////////////////////
MgPropertyDefinition* MgServerGwsGetFeatures::GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoPropertyDefinition*)fdoPropDef, L"MgServerGwsGetFeatures.GetMgPropertyDefinition");

    Ptr<MgPropertyDefinition> propDef;

    FdoPropertyType fpt = fdoPropDef->GetPropertyType();
    switch (fpt)
    {
        // Represents a Data Property type.
        case FdoPropertyType_DataProperty:
        {
            propDef = this->GetDataPropertyDefinition((FdoDataPropertyDefinition*)fdoPropDef);
            break;
        }
        // Represents an Object Property type.
        case FdoPropertyType_ObjectProperty:
        {
            propDef = this->GetObjectPropertyDefinition((FdoObjectPropertyDefinition*)fdoPropDef);
            break;
        }

        // Represents a Geometric Property type.
        case FdoPropertyType_GeometricProperty:
        {
            propDef = this->GetGeometricPropertyDefinition((FdoGeometricPropertyDefinition*)fdoPropDef);
            break;
        }
        // Represents an Association Property type.
        case FdoPropertyType_AssociationProperty:
        {
            // TODO:
            break;
        }

        // Represents a Raster (image) Property type.
        case FdoPropertyType_RasterProperty:
        {
            propDef = this->GetRasterPropertyDefinition((FdoRasterPropertyDefinition*)fdoPropDef);
            break;
        }
    }

    return propDef.Detach();
}


//////////////////////////////////////////////////////////////////
MgDataPropertyDefinition* MgServerGwsGetFeatures::GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef)
{
    if (fdoPropDef == NULL)
    {
        return NULL;
    }

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgDataPropertyDefinition> propDef = new MgDataPropertyDefinition(name);

    // Get data members from FDO
    FdoString* defaultVal = fdoPropDef->GetDefaultValue();
    FdoInt32 length = fdoPropDef->GetLength();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 precision = fdoPropDef->GetPrecision();
    bool isNullable = fdoPropDef->GetNullable();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    FdoInt32 scale = fdoPropDef->GetScale();
    bool isAutoGenerated = fdoPropDef->GetIsAutoGenerated();

    // Set it for MapGuide
    FdoDataType dataType = fdoPropDef->GetDataType();
    propDef->SetDataType(GetMgPropertyType(dataType));

    if (defaultVal != NULL)
    {
        propDef->SetDefaultValue(STRING(defaultVal));
    }

    propDef->SetLength((INT32)length);
    propDef->SetReadOnly(isReadOnly);

    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetPrecision((INT32)precision);
    propDef->SetNullable(isNullable);

    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    propDef->SetAutoGeneration(isAutoGenerated);

    propDef->SetScale((INT32)scale);

    return propDef.Detach();
}

//////////////////////////////////////////////////////////////////
MgGeometricPropertyDefinition* MgServerGwsGetFeatures::GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoGeometricPropertyDefinition*)fdoPropDef, L"MgServerGwsGetFeatures.GetGeometricPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgGeometricPropertyDefinition> propDef = new MgGeometricPropertyDefinition(name);

    // Get data members from FDO
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 geomTypes = fdoPropDef->GetGeometryTypes();
    FdoInt32 specificGeomCount = 0;
    FdoGeometryType * specificGeomTypes = fdoPropDef->GetSpecificGeometryTypes(specificGeomCount);
    bool hasElev = fdoPropDef->GetHasElevation();
    bool hasMeasure = fdoPropDef->GetHasMeasure();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    FdoString* spatialContextName = fdoPropDef->GetSpatialContextAssociation();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    INT32 geomTypeList[MG_MAX_GEOMETRY_TYPE_SIZE];
    INT32 geomTypeCount = (INT32)specificGeomCount;
    for (INT32 i = 0;  i < geomTypeCount && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        geomTypeList[i] = (INT32)specificGeomTypes[i];
    }
    Ptr<MgGeometryTypeInfo> geomTypeInfo = new MgGeometryTypeInfo;
    geomTypeInfo->SetTypes(geomTypeList, geomTypeCount);
    propDef->SetGeometryTypes((INT32)geomTypes);
    propDef->SetSpecificGeometryTypes(geomTypeInfo);
    propDef->SetHasElevation(hasElev);
    propDef->SetHasMeasure(hasMeasure);
    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    if (spatialContextName != NULL)
    {
        propDef->SetSpatialContextAssociation(STRING(spatialContextName));
    }

    return propDef.Detach();
}

//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::AddFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_gwsFeatureIterator, L"MgServerGwsGetFeatures.AddFeatures");
    CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerGwsGetFeatures.AddFeatures");

    INT32 desiredFeatures = 0;

    // Access the class definition
    Ptr<MgClassDefinition> classDef = m_featureSet->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgServerGwsGetFeatures.AddFeatures");

    // Access the property definition collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetPropertiesIncludingBase();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerGwsGetFeatures.AddFeatures");

    // How many properties are we fetching, it should be atleast one
    INT32 cnt = propDefCol->GetCount();

    // We only read if there is atleast one property requested
    if (cnt > 0)
    {
        bool found = false;

        do
        {
            if(m_bReadNextDone)
            {
                found = m_bReadNextHasData;
                m_bReadNextDone = false;
            }
            else
            {
                // Advance the reader
                // Read next throws exception which it should not (therefore we just ignore it)
                try
                {
                    found = ReadNext();
                }
                catch (FdoException* e)
                {
                    // Note: VB 05/10/06 The assert has been commented out as
                    // Linux does not remove them from a release build. The assert
                    // will cause the server to crash on Linux. The Oracle provider will throw
                    // an exception if the ReadNext() method is called after it returns false.
                    // This is a known problem and it is safe to ignore the exception.
                    //assert(false);
                    e->Release();
                    found = false;
                }
                catch(...)
                {
                    found = false;
                }
            }

            if(found)
            {
                AddFeature((MgPropertyDefinitionCollection*)propDefCol);
                if (count > 0)
                {
                    desiredFeatures++;
                    // Collected required features therefore do not process more
                    if (desiredFeatures == count)
                    {
                        break;
                    }
                }
            }
        } while (found);
    }
}

//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::AddFeature(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((IGWSFeatureIterator*)m_gwsFeatureIterator, L"MgServerGwsGetFeatures.AddFeature");
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerGwsGetFeatures.AddFeature");

    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);

        // Get the name of property
        STRING propName = propDef->GetName();

        INT16 type = this->GetMgPropertyType(propDef);
        Ptr<MgProperty> prop = this->GetMgProperty(propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }

    m_featureSet->AddFeature(propCol);
}

//////////////////////////////////////////////////////////////////
MgProperty* MgServerGwsGetFeatures::GetMgProperty(CREFSTRING qualifiedPropName, INT16 type)
{
    // No propertyname specified, return NULL
    if (qualifiedPropName.empty())
        return NULL;

    Ptr<MgNullableProperty> prop;

    STRING propName = qualifiedPropName.substr(qualifiedPropName.rfind(L".")+1);
    // If not qualified name specified, take qualifiedPropName as property name
    if (propName.empty())
    {
        propName = qualifiedPropName;
    }

    switch(type)
    {
        case MgPropertyType::Boolean: /// Boolean true/false value
        {
            bool val = false;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetBoolean(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgBooleanProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Byte: /// Unsigned 8 bit value
        {
            FdoByte val = 0;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetByte(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgByteProperty(propName, (BYTE)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::DateTime: /// DateTime object
        {
            Ptr<MgDateTime> dateTime;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    FdoDateTime val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
                    dateTime = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                              (INT8)val.hour, (INT8)val.minute, val.seconds);
                    isNull = false;
                }

                prop = new MgDateTimeProperty(propName, dateTime);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Single: /// Single precision floating point value
        {
            float val = 0.0f;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetSingle(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgSingleProperty(propName, (float)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Double: /// Double precision floating point value
        {
            double val = 0.0;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetDouble(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgDoubleProperty(propName, (double)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Int16: /// 16 bit signed integer value
        {
            FdoInt16 val = 0;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetInt16(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgInt16Property(propName, (INT16)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Int32: // 32 bit signed integer value
        {
            FdoInt32 val = 0;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetInt32(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgInt32Property(propName, (INT32)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Int64: // 64 bit signed integer value
        {
            FdoInt64 val = 0;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetInt64(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgInt64Property(propName, (INT64)val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::String: // String MgProperty
        {
            STRING val = L"";
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    val = gwsFeatureIter->GetString(parsedPropertyName.c_str());
                    isNull = false;
                }

                prop = new MgStringProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Blob: // BLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    isNull = false;
    //                val = this->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);
                }

                prop = new MgBlobProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Clob: // CLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    isNull = false;
    //                val = this->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);
                }

                prop = new MgClobProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Feature: // Feature object
        {
            Ptr<MgFeatureReader> val;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    isNull = false;
                    FdoPtr<FdoIFeatureReader> featureObject = gwsFeatureIter->GetFeatureObject(parsedPropertyName.c_str());
                    if (featureObject != NULL)
                    {
                        Ptr<MgServerFeatureReaderIdentifier> sfi = new MgServerFeatureReaderIdentifier(featureObject);
                        val = dynamic_cast<MgFeatureReader*>(new MgServerFeatureReader(sfi));
                    }
                }

                prop = new MgFeatureProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Geometry: // Geometry object
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    FdoInt32 length = 0;
                    const FdoByte* bytes = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &length);

                    if (bytes != NULL)
                    {
                        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)length);
                        val = byteSource->GetReader();
                        isNull = false;
                    }
                }

                prop = new MgGeometryProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
        case MgPropertyType::Raster: // Raster object
        {
            Ptr<MgRaster> val;
            bool isNull = true;

            IGWSFeatureIterator* gwsFeatureIter = NULL;
            STRING parsedPropertyName;
            if (this->DeterminePropertyFeatureSource(propName.c_str(), &gwsFeatureIter, parsedPropertyName))
            {
                if (!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
                {
                    FdoPtr<FdoIRaster> raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
                    val = MgServerFeatureUtil::GetMgRaster(raster, parsedPropertyName);
                    isNull = false;
                }

                prop = new MgRasterProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
    }

    return prop.Detach();
}


//////////////////////////////////////////////////////////////////
INT32 MgServerGwsGetFeatures::GetMgPropertyType(FdoDataType fdoDataType)
{
    INT32 mgPropType;
    switch(fdoDataType)
    {
        // Represents a Boolean value of true or false.
        case FdoDataType_Boolean:
        {
            mgPropType = MgPropertyType::Boolean;
            break;
        }
        // Represents unsigned 8-bit integers with values between 0 and 255.
        case FdoDataType_Byte:
        {
            mgPropType = MgPropertyType::Byte;
            break;
        }
        // Represents a date and time value.
        case FdoDataType_DateTime:
        {
            mgPropType = MgPropertyType::DateTime;
            break;
        }
        // Represents values ranging from 1.0 x 10^-28 to approximately 7.9 x 10^28
        // with 28-29 significant digits.
        // Implementation Note:  FdoDataType_Decimal is currently mapped to MgPropertyType::Double.
        // An MgDecimalProperty class should be implemented in a future release.
        case FdoDataType_Decimal:
        // Fall through to double handling
        case FdoDataType_Double:
        {
            mgPropType = MgPropertyType::Double;
            break;
        }
        // Represents signed 16-bit integers with values between -32768 and 32767.
        case FdoDataType_Int16:
        {
            mgPropType = MgPropertyType::Int16;
            break;
        }
        // Represents signed 32-bit integers with values between -2147483648 and
        // 2147483647.
        case FdoDataType_Int32:
        {
            mgPropType = MgPropertyType::Int32;
            break;
        }
        // Represents signed 64-bit integers with values between
        // -9223372036854775808 and 9223372036854775807.
        case FdoDataType_Int64:
        {
            mgPropType = MgPropertyType::Int64;
            break;
        }
        // Represents floating point values ranging from approximately 1.5 x 10^-45
        // to 3.4 x 10^38: with a precision of 7 digits.
        case FdoDataType_Single:
        {
            mgPropType = MgPropertyType::Single;
            break;
        }
        // Represents a Unicode character strings.
        case FdoDataType_String:
        {
            mgPropType = MgPropertyType::String;
            break;
        }
        // Represents a binary large object stored as a collection of bytes.
        case FdoDataType_BLOB:
        {
            mgPropType = MgPropertyType::Blob;
            break;
        }
        // Represents a character large object stored as a collection of
        // characters.
        case FdoDataType_CLOB:
        {
            mgPropType = MgPropertyType::Clob;
            break;
        }
        default:
        {
            // We will probably never hit this code
            STRING buffer;
            MgUtil::Int32ToString(fdoDataType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgServerGwsGetFeatures.GetMgPropertyType",
                __LINE__, __WFILE__, &arguments, L"MgInvalidFdoDataType", NULL);
        }
    }

    return mgPropType;
}


//////////////////////////////////////////////////////////////////
INT16 MgServerGwsGetFeatures::GetMgPropertyType(MgPropertyDefinition* propDef)
{
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgServerGwsGetFeatures.GetMgPropertyType")

    return MgServerFeatureUtil::GetMgPropertyType(propDef);
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGwsGetFeatures::SerializeToXml(FdoClassDefinition* classDef)
{
    CHECKNULL(classDef, L"MgServerGwsGetFeatures.SerializeToXml");

    FdoString* className = classDef->GetName();
    FdoFeatureSchemaP pSchema = classDef->GetFeatureSchema();
    FdoFeatureSchemaP tempSchema;
    FdoClassDefinitionP featClass;
    FdoInt32 index = 0;

    if (pSchema != NULL)
    {
        //Get the position of the class in the collecion
        FdoPtr<FdoClassCollection> fcc = pSchema->GetClasses();
        index = fcc->IndexOf( className );

        // Move class of interest to its own schema
        tempSchema = FdoFeatureSchema::Create( pSchema->GetName(), L"" );
        featClass = FdoClassesP(pSchema->GetClasses())->GetItem( className );
        FdoClassesP(pSchema->GetClasses())->Remove(featClass);
        FdoClassesP(tempSchema->GetClasses())->Add(featClass);
    }
    else
    {
        tempSchema = FdoFeatureSchema::Create(L"TempSchema", L"" );
        FdoClassesP(tempSchema->GetClasses())->Add(classDef);
    }

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    tempSchema->WriteXml(fmis);
    fmis->Reset();

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerGwsGetFeatures::SerializeToXml");

    fmis->Read(bytes, (FdoSize)len);

    // Get byte reader from memory stream
    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)len);
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    // Cleanup the above addition/deletion of class definition
    if (pSchema != NULL)
    {
        if (featClass != NULL)
        {
            FdoClassesP(tempSchema->GetClasses())->Remove(featClass);
            FdoClassesP(pSchema->GetClasses())->Insert(index, featClass);
        }
    }
    else
    {
        FdoClassesP(tempSchema->GetClasses())->Remove(classDef);
    }

    delete[] bytes;

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGwsGetFeatures::GetRaster(INT32 xSize, INT32 ySize, STRING rasterPropName)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsGetFeatures.GetRaster");
    //CHECKNULL(m_classDef, L"MgServerGwsGetFeatures.GetRaster");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()
/*
    // If there is no raster property, GetRaster should not be called
    if (!m_classDef->HasRasterProperty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgServerGwsGetFeatures.GetRaster", __LINE__, __WFILE__);
    }

    // There can be more than one Raster property
    if (rasterPropName.empty())
    {
        rasterPropName = m_classDef->GetRasterPropertyName();
    }

    // If this property is requested then we fetch the raster data
    byteReader = MgServerFeatureUtil::GetRaster(m_gwsFeatureIterator, rasterPropName, xSize, ySize);
*/

    throw new MgNotImplementedException(L"MgServerGwsGetFeatures.GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsGetFeatures.GetRaster")

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
MgObjectPropertyDefinition* MgServerGwsGetFeatures::GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoObjectPropertyDefinition*)fdoPropDef, L"MgServerGwsGetFeatures.GetObjectPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgObjectPropertyDefinition> propDef = new MgObjectPropertyDefinition(name);

    FdoString* desc = fdoPropDef->GetDescription();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    FdoString* qualifiedName = (FdoString*)qname;

    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    FdoPtr<FdoClassDefinition> fdoClsDef = fdoPropDef->GetClass();
    CHECKNULL((FdoClassDefinition*)fdoClsDef, L"MgServerGwsGetFeatures.GetObjectPropertyDefinition")

    FdoPtr<FdoDataPropertyDefinition> idenProp = fdoPropDef->GetIdentityProperty(); // Can return NULL

    FdoObjectType objType = fdoPropDef->GetObjectType();
    FdoOrderType orderType = fdoPropDef->GetOrderType();

    Ptr<MgClassDefinition> objPropClsDef = this->GetMgClassDefinition(fdoClsDef, true);
    Ptr<MgDataPropertyDefinition> objIdenProp = this->GetDataPropertyDefinition(idenProp);
    INT32 orderOption = MgServerFeatureUtil::FdoOrderTypeToMgOrderingOption(orderType);
    INT32 mgObjType = MgServerFeatureUtil::FdoObjectTypeToMgObjectPropertyType(objType);

    propDef->SetClassDefinition(objPropClsDef);
    propDef->SetIdentityProperty(objIdenProp);
    propDef->SetOrderType(orderOption);
    propDef->SetObjectType(mgObjType);

    return propDef.Detach();
}

//////////////////////////////////////////////////////////////////
MgRasterPropertyDefinition* MgServerGwsGetFeatures::GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoRasterPropertyDefinition*)fdoPropDef, L"MgServerGwsGetFeatures.GetRasterPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgRasterPropertyDefinition> propDef = new MgRasterPropertyDefinition(name);

    // Get data members from FDO
    FdoString* desc = fdoPropDef->GetDescription();
    FdoInt32 xsize = fdoPropDef->GetDefaultImageXSize();
    FdoInt32 ysize = fdoPropDef->GetDefaultImageYSize();
    bool isNullable = fdoPropDef->GetNullable();
    FdoStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetDefaultImageXSize((INT32)xsize);
    propDef->SetDefaultImageYSize((INT32)ysize);
    propDef->SetNullable(isNullable);

    FdoString* qualifiedName = (FdoString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    return propDef.Detach();
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGwsGetFeatures::GetLOBFromFdo(CREFSTRING propName, IGWSFeatureIterator* gwsFeatureIterator)
{
    CHECKNULL(gwsFeatureIterator, L"MgServerGwsGetFeatures.GetLOBFromFdo");

    Ptr<MgByteReader> byteReader;

    // TODO: We need to switch to FdoIStreamReader when we have streaming capability
    // in MgByteReader
    FdoPtr<FdoLOBValue> fdoVal = gwsFeatureIterator->GetLOB(propName.c_str());
    if (fdoVal != NULL)
    {
        FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
        if (byteArray != NULL)
        {
            FdoByte* bytes = byteArray->GetData();
            FdoInt32 len = byteArray->GetCount();
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
            byteReader = byteSource->GetReader();
        }
    }

    return byteReader.Detach();
}

bool MgServerGwsGetFeatures::DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsGetFeatures.DeterminePropertyFeatureSource");

    bool bPropertyFound = false;

    FdoPtr<IGWSFeatureIterator> secondaryFeatureIter;

    // Parse the input property name to determine the feature source,
    // and the property name.  The qualified name could
    // look something like this: Join1PropA, where
    // Join1 = relation name
    // PropA = property name

    STRING qualifiedName;
    STRING className;
    STRING relationName;

    if (inputPropName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Check if the input propName is prefixed with the relationName
    // by comparing with primary feature source property names
    FdoPtr<IGWSExtendedFeatureDescription> primaryDesc;
    m_gwsFeatureIterator->DescribeFeature(&primaryDesc);
    FdoPtr<FdoStringCollection> primaryPropNames = primaryDesc->PropertyNames();
    int nPrimaryDescCount = primaryDesc->GetCount();

    int primaryIndex = primaryPropNames->IndexOf(inputPropName.c_str());
    if( -1 != primaryIndex)
    {
        // No prefix, but the property name does exist in the primary feature source
        parsedPropName = inputPropName;
        *gwsFeatureIter = m_gwsFeatureIterator;
        bPropertyFound = true;
    }
    else
    {
        // cycle thru secondary feature sources and check property names
        if (!m_secondaryGwsFeatureIteratorMap.empty())
        {
            for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
                iter != m_secondaryGwsFeatureIteratorMap.end();
                iter++)
            {
                secondaryFeatureIter = FDO_SAFE_ADDREF(iter->second);
                if (secondaryFeatureIter == NULL)
                {
                    continue;
                }

                FdoPtr<IGWSExtendedFeatureDescription> secondaryDesc;
                secondaryFeatureIter->DescribeFeature(&secondaryDesc);

                GWSQualifiedName secQualifiedClassName = secondaryDesc->ClassName();

                FdoString* featureSource = secQualifiedClassName.FeatureSource();

                FdoPtr<FdoStringCollection> secondaryPropNames = secondaryDesc->PropertyNames();

                STRING attributeNameDelimiter = iter->first;

                // cycle thru secondaryPropNames looking for substring occurrence in inputPropName

                // If the delimiter is not blank, then look for it.
                // Else parse the extened property name by trying to match it to the known secondary property names.

                STRING::size_type delimiterIndex = inputPropName.find(attributeNameDelimiter);
                FdoInt32 secPropCnt = secondaryPropNames->GetCount();
                relationName.clear();
                for (FdoInt32 secPropIndex = 0; secPropIndex < secPropCnt; secPropIndex++)
                {
                    if (!attributeNameDelimiter.empty() && STRING::npos != delimiterIndex)
                    {
                        parsedPropName = inputPropName.substr(delimiterIndex +1).c_str();
                        relationName = inputPropName.substr(0, delimiterIndex).c_str();
                    }
                    else
                    {
                        STRING secondaryProp = (STRING)secondaryPropNames->GetString(secPropIndex);
                        STRING::size_type nPropStartIndex = inputPropName.find(secondaryProp);
                        if (std::wstring::npos != nPropStartIndex)
                        {
                            parsedPropName = inputPropName.substr(nPropStartIndex).c_str();
                            relationName = inputPropName.substr(0, nPropStartIndex).c_str();
                        }
                    }
                    if ( wcscmp(featureSource, relationName.c_str()) == 0 )
                    {
                        *gwsFeatureIter = secondaryFeatureIter;
                        bPropertyFound = true;
                        iter = m_secondaryGwsFeatureIteratorMap.end();
                        iter--;
                        break;
                    }
                }  //end loop through secondary property names
            }  // end loop through secondary iterators
        }  // end if ( !secondaryIteratorMap.empty() )
    }  // end check secondary feature sources

    return bPropertyFound;
}

void MgServerGwsGetFeatures::SetRelationNames(FdoStringCollection* relationNames)
{
    m_relationNames = FDO_SAFE_ADDREF(relationNames);
}

void MgServerGwsGetFeatures::SetExtensionName(CREFSTRING extensionName)
{
    m_extensionName = extensionName;
}

FdoStringCollection* MgServerGwsGetFeatures::GetRelationNames()
{
    return FDO_SAFE_ADDREF((FdoStringCollection*)m_relationNames);
}

STRING MgServerGwsGetFeatures::GetExtensionName()
{
    return m_extensionName;
}

bool MgServerGwsGetFeatures::ReadNext()
{
    return m_gwsFeatureReader->ReadNext();
}

void MgServerGwsGetFeatures::SetFilter(FdoExpressionEngine* expressionEngine, FdoFilter* filter)
{
    m_expressionEngine = FDO_SAFE_ADDREF(expressionEngine);
    m_filter = FDO_SAFE_ADDREF(filter);
}

MgServerGwsFeatureReader* MgServerGwsGetFeatures::GetGwsFeatureReader()
{
    return SAFE_ADDREF(m_gwsFeatureReader);
}

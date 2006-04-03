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
}


MgServerGwsGetFeatures::MgServerGwsGetFeatures(IGWSFeatureIterator* gwsFeatureReader)
{
    CHECKNULL(gwsFeatureReader, L"MgServerGwsGetFeatures.MgServerGwsGetFeatures");

    m_featureSet = NULL;
    m_classDef = NULL;
    m_relationNames = NULL;
    m_gwsFeatureReader = GIS_SAFE_ADDREF(gwsFeatureReader);
}

MgServerGwsGetFeatures::~MgServerGwsGetFeatures()
{
    GIS_SAFE_RELEASE(m_gwsFeatureReader);
}

//////////////////////////////////////////////////////////////////
MgFeatureSet* MgServerGwsGetFeatures::GetFeatures(INT32 count)
{
    CHECKNULL(m_gwsFeatureReader, L"MgServerGwsGetFeatures.GetFeatures");

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
    CHECKNULL(m_gwsFeatureReader, L"MgServerGwsGetFeatures.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        GisPtr<FdoClassDefinition> fdoClassDefinition = m_gwsFeatureReader->GetClassDefinition();
        CHECKNULL((FdoClassDefinition*)fdoClassDefinition, L"MgServerGwsGetFeatures.GetFeatures");

        // Convert FdoClassDefinition to MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = this->GetMgClassDefinition(fdoClassDefinition, bSerialize);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGwsGetFeatures.GetFeatures");

        // Advance the primary feature source iterator
        if (m_gwsFeatureReader->ReadNext())
        {

            // Retrieve the secondary feature source iterators, get the property definitions and add to class definition
            GisPtr<IGWSExtendedFeatureDescription> desc;
            m_gwsFeatureReader->DescribeFeature(&desc);

            for (int i = 0; i < desc->GetCount(); i++)
            {
                GisPtr<IGWSFeatureIterator> featureIter = m_gwsFeatureReader->GetJoinedFeatures(i);
                CHECKNULL(featureIter, L"MgServerGwsGetFeatures.GetMgClassDefinition");

                // Retrieve the secondary class definitions
                GisPtr<FdoClassDefinition> secFdoClassDefinition = featureIter->GetClassDefinition();
                CHECKNULL((FdoClassDefinition*)secFdoClassDefinition, L"MgServerGwsGetFeatures.GetFeatures");

                GisStringP qname = secFdoClassDefinition->GetQualifiedName();

                // Convert FdoClassDefinition to MgClassDefinition
                Ptr<MgClassDefinition> secMgClassDef = this->GetMgClassDefinition(secFdoClassDefinition, bSerialize);
                CHECKNULL((MgClassDefinition*)secMgClassDef, L"MgServerGwsGetFeatures.GetFeatures");

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
                    secPropName = relationName + secPropName;
                    propDef->SetName(secPropName);
                    mpdc->Add(propDef);
                }

                mgClassDef->SetName(m_extensionName);

            }

            // Convert MgClassDefinition to FdoClassDefinition.
            MgServerDescribeSchema msds;
            Ptr<MgClassDefinitionCollection> mcdc = new MgClassDefinitionCollection();
            mcdc->Add(mgClassDef);
            GisPtr<FdoClassCollection> fcc;
            fcc = FdoClassCollection::Create(NULL);
            msds.GetFdoClassCollection(fcc, mcdc);
            int nFccCount = fcc->GetCount();

            // Get the FdoClassDefinition
            GisPtr<FdoClassDefinition> fdc = msds.GetFdoClassDefinition(mgClassDef, fcc);

            // Pass the FdoClassDefinition to SerializeToXml
            if (bSerialize)
            {
                // TODO: Need to optimize this ByteReader/ByteSink - too much conversion
                STRING str;
                Ptr<MgByteReader> byteReader = SerializeToXml(fdc);
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

                // Set the serialized feature join result xml for the MgClassDefinition.
                mgClassDef->SetSerializedXml(str1);
            }

        }

        // Store the it for future use
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);

    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsGetFeatures.GetMgClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}



//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerGwsGetFeatures::GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize)
{
    CHECKNULL(fdoClassDefinition, L"MgClassDefinition.GetMgClassDefinition");

    // Create MgClassDefinition
    Ptr<MgClassDefinition> mgClassDef = new MgClassDefinition();
    CHECKNULL((MgClassDefinition*)mgClassDef, L"MgClassDefinition.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store property definitions
    Ptr<MgPropertyDefinitionCollection> propDefCol = mgClassDef->GetProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetMgClassDefinition");

    // Get PropertyDefinitionCollection to store key property definitions i.e. which makes key for this feature class
    Ptr<MgPropertyDefinitionCollection> identityPropDefCol = mgClassDef->GetIdentityProperties();
    CHECKNULL((MgPropertyDefinitionCollection*)identityPropDefCol, L"MgClassDefinition.GetMgClassDefinition");

    // description
    GisString* desc = fdoClassDefinition->GetDescription();
    if (desc != NULL)
    {
        mgClassDef->SetDescription(STRING(desc));
    }

    // Class name
    GisString* className = fdoClassDefinition->GetName();
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
    GisPtr<FdoPropertyDefinitionCollection> fpdc = fdoClassDefinition->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*)fpdc, L"MgClassDefinition.GetMgClassDefinition");

    // Retrieve Base class properties from FDO
    // TODO: Should we add Base class properties into the list of properties?
    // TODO: Can this be null?
    GisPtr<FdoReadOnlyPropertyDefinitionCollection> frpdc = fdoClassDefinition->GetBaseProperties();

    // Retrieve identity properties from FDO
    // TODO: Can this be null?
    GisPtr<FdoDataPropertyDefinitionCollection> fdpdc = fdoClassDefinition->GetIdentityProperties();

    // Add properties
    this->GetClassProperties(propDefCol, fpdc);

    FdoClassType classType = fdoClassDefinition->GetClassType();
    if (classType == FdoClassType_FeatureClass)
    {
        GisPtr<FdoGeometricPropertyDefinition> geomPropDef = ((FdoFeatureClass*)fdoClassDefinition)->GetGeometryProperty();
        if (geomPropDef != NULL)
        {
            GisString* defaultGeomName = geomPropDef->GetName();
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

    GisPtr<FdoClassDefinition> baseDefinition = fdoClassDefinition->GetBaseClass();
    if (baseDefinition != NULL)
    {
        Ptr<MgClassDefinition> mgBaseClsDef = GetMgClassDefinition(baseDefinition, bSerialize);
        mgClassDef->SetBaseClassDefinition(mgBaseClsDef);
    }

    return SAFE_ADDREF((MgClassDefinition*)mgClassDef);
}


//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                             FdoPropertyDefinitionCollection* fdoPropDefCol)
{
    // CHECKNULL((FdoPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetClassProperties");

    // TODO: Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    GisInt32 cnt = fdoPropDefCol->GetCount();
    for (GisInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        GisPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgClassDefinition.GetClassProperties");

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

    // TODO: Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    GisInt32 cnt  = fdoPropDefCol->GetCount();
    for (GisInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        GisPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
        CHECKNULL((FdoPropertyDefinition*)fpd, L"MgClassDefinition.GetClassProperties");

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
    CHECKNULL((FdoPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetMgPropertyDefinition");

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

    return SAFE_ADDREF((MgPropertyDefinition*)propDef);
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
    GisString* defaultVal = fdoPropDef->GetDefaultValue();
    GisInt32 length = fdoPropDef->GetLength();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    GisString* desc = fdoPropDef->GetDescription();
    GisInt32 precision = fdoPropDef->GetPrecision();
    bool isNullable = fdoPropDef->GetNullable();
    GisStringP qname = fdoPropDef->GetQualifiedName();
    GisInt32 scale = fdoPropDef->GetScale();
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

    GisString* qualifiedName = (GisString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    propDef->SetAutoGeneration(isAutoGenerated);

    propDef->SetScale((INT32)scale);

    return SAFE_ADDREF((MgDataPropertyDefinition*)propDef);
}

//////////////////////////////////////////////////////////////////
MgGeometricPropertyDefinition* MgServerGwsGetFeatures::GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoGeometricPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetGeometricPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgGeometricPropertyDefinition> propDef = new MgGeometricPropertyDefinition(name);

    // Get data members from FDO
    GisString* desc = fdoPropDef->GetDescription();
    GisInt32 geomTypes = fdoPropDef->GetGeometryTypes();
    bool hasElev = fdoPropDef->GetHasElevation();
    bool hasMeasure = fdoPropDef->GetHasMeasure();
    GisStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();
    GisString* spatialContextName = fdoPropDef->GetSpatialContextAssociation();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetGeometryTypes((INT32)geomTypes);
    propDef->SetHasElevation(hasElev);
    propDef->SetHasMeasure(hasMeasure);
    GisString* qualifiedName = (GisString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    if (spatialContextName != NULL)
    {
        propDef->SetSpatialContextAssociation(STRING(spatialContextName));
    }

    return SAFE_ADDREF((MgGeometricPropertyDefinition*)propDef);
}

//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::AddFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_gwsFeatureReader, L"MgServerGwsGetFeatures.AddFeatures");
    CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerGwsGetFeatures.AddFeatures");

    INT32 desiredFeatures = 0;

    // Access the class definition
    Ptr<MgClassDefinition> classDef = m_featureSet->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgServerGwsGetFeatures.AddFeatures");

    // Access the property definition collection
    // Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
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
            // Read next throws exception which it should not (therefore we just ignore it)
            try { found = m_gwsFeatureReader->ReadNext(); } catch(...) {found = false;}
        } while (found);

    }

}

//////////////////////////////////////////////////////////////////
void MgServerGwsGetFeatures::AddFeature(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((IGWSFeatureIterator*)m_gwsFeatureReader, L"MgServerGwsGetFeatures.AddFeature");
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
    Ptr<MgNullableProperty> prop;

    // No propertyname specified, return NULL
    if (qualifiedPropName.empty())
        return SAFE_ADDREF((MgProperty*)prop);

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
            GisByte val = 0;
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
                    GisDateTime val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
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
            float val = 0;
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
            double val = 0;
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
            GisInt16 val = 0;
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
            GisInt32 val = 0;
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
            GisInt64 val = 0;
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

                prop = new MgBlobProperty(propName,val);
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

                prop = new MgClobProperty(propName,val);
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
                    GisPtr<FdoIFeatureReader> featureObject = gwsFeatureIter->GetFeatureObject(parsedPropertyName.c_str());
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
                    GisInt32 length = 0;
                    const GisByte* bytes = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &length);

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
                    GisPtr<FdoIRaster> raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
                    val = MgServerFeatureUtil::GetMgRaster(raster, parsedPropertyName);
                    isNull = false;
                }

                prop = new MgRasterProperty(propName, val);
                prop->SetNull(isNull);
            }
            break;
        }
    }

    return SAFE_ADDREF((MgProperty*)prop);
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
        case FdoDataType_Decimal:
        {
            // TODO: Fdo has some inconsistency with Decimal, therefore MapGuide
            // TODO: does not support it. Making this type as invalid argument
            // TODO: until we determine what to do with it???
            // propDef = new MgPropertyDefinition(name, ptDecimal);
            // break;

            // Treat decimal as double until FDO decideds what to do with it?
            // throw new MgInvalidArgumentException(L"MgServerGwsGetFeatures.GetMgPropertyType", __LINE__, __WFILE__);
        }
        // Represents a floating point value ranging from approximately 5.0 x
        // 10^-324 to 1.7 x 10^308 with a precision of 15-16 digits.
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

    GisString* className = classDef->GetName();
    FdoFeatureSchemaP pSchema = classDef->GetFeatureSchema();
    FdoFeatureSchemaP tempSchema;
    FdoClassDefinitionP featClass;
    GisInt32 index = 0;

    if (pSchema != NULL)
    {
        //Get the position of the class in the collecion
        GisPtr<FdoClassCollection> fcc = pSchema->GetClasses();
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

    GisIoMemoryStreamP fmis = GisIoMemoryStream::Create();
    tempSchema->WriteXml( fmis );
    fmis->Reset();

    GisInt64 len = fmis->GetLength();
    GisByte *bytes = new GisByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerGwsGetFeatures::SerializeToXml");

    fmis->Read(bytes, (GisSize)len);

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

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGwsGetFeatures::GetRaster(INT32 xSize, INT32 ySize, STRING rasterPropName)
{
    CHECKNULL(m_gwsFeatureReader, L"MgServerGwsGetFeatures.GetRaster");
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
    byteReader = MgServerFeatureUtil::GetRaster(m_gwsFeatureReader, rasterPropName, xSize, ySize);
*/

    throw new MgNotImplementedException(L"MgServerGwsGetFeatures.GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsGetFeatures.GetRaster")

    return SAFE_ADDREF((MgByteReader*)byteReader);
}

//////////////////////////////////////////////////////////////////
MgObjectPropertyDefinition* MgServerGwsGetFeatures::GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoObjectPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetObjectPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgObjectPropertyDefinition> propDef = new MgObjectPropertyDefinition(name);

    GisString* desc = fdoPropDef->GetDescription();
    GisStringP qname = fdoPropDef->GetQualifiedName();
    GisString* qualifiedName = (GisString*)qname;

    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    GisPtr<FdoClassDefinition> fdoClsDef = fdoPropDef->GetClass();
    CHECKNULL((FdoClassDefinition*)fdoClsDef, L"MgServerGetFeatures.GetObjectPropertyDefinition")

    GisPtr<FdoDataPropertyDefinition> idenProp = fdoPropDef->GetIdentityProperty(); // Can return NULL

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

    return SAFE_ADDREF((MgObjectPropertyDefinition*)propDef);
}

//////////////////////////////////////////////////////////////////
MgRasterPropertyDefinition* MgServerGwsGetFeatures::GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoRasterPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetRasterPropertyDefinition");

    STRING name = STRING(fdoPropDef->GetName());
    Ptr<MgRasterPropertyDefinition> propDef = new MgRasterPropertyDefinition(name);

    // Get data members from FDO
    GisString* desc = fdoPropDef->GetDescription();
    GisInt32 xsize = fdoPropDef->GetDefaultImageXSize();
    GisInt32 ysize = fdoPropDef->GetDefaultImageYSize();
    bool isNullable = fdoPropDef->GetNullable();
    GisStringP qname = fdoPropDef->GetQualifiedName();
    bool isReadOnly = fdoPropDef->GetReadOnly();

    // Set it for MapGuide
    if (desc != NULL)
    {
        propDef->SetDescription(STRING(desc));
    }

    propDef->SetDefaultImageXSize((INT32)xsize);
    propDef->SetDefaultImageYSize((INT32)ysize);
    propDef->SetNullable(isNullable);

    GisString* qualifiedName = (GisString*)qname;
    if (qualifiedName != NULL)
    {
        propDef->SetQualifiedName(STRING(qualifiedName));
    }
    propDef->SetReadOnly(isReadOnly);

    return SAFE_ADDREF((MgRasterPropertyDefinition*)propDef);
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGwsGetFeatures::GetLOBFromFdo(CREFSTRING propName, IGWSFeatureIterator* gwsFeatureIterator)
{
    CHECKNULL(gwsFeatureIterator, L"MgServerGetFeatures.GetLOBFromFdo");

    Ptr<MgByteReader> val;

    // TODO: We need to switch to GisIStreamReader when we have streaming capability
    // in MgByteReader
    GisPtr<FdoLOBValue> fdoVal = gwsFeatureIterator->GetLOB(propName.c_str());
    if (fdoVal != NULL)
    {
        GisPtr<GisByteArray> byteArray = fdoVal->GetData();
        if (byteArray != NULL)
        {
            GisByte* bytes = byteArray->GetData();
            GisInt32 len = byteArray->GetCount();
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
            val = byteSource->GetReader();
        }
    }

    return SAFE_ADDREF((MgByteReader*)val);
}

bool MgServerGwsGetFeatures::DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName)
{
    CHECKNULL(m_gwsFeatureReader, L"MgServerGwsGetFeatures.DeterminePropertyFeatureSource");

    bool bPropertyFound = false;

    IGWSFeatureIterator* secondaryFeatureIter = NULL;

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
    GisPtr<IGWSExtendedFeatureDescription> primaryDesc;
    m_gwsFeatureReader->DescribeFeature(&primaryDesc);
    GisPtr<GisStringCollection> primaryPropNames = primaryDesc->PropertyNames();
    int nPrimaryDescCount = primaryDesc->GetCount();

    int primaryIndex = primaryPropNames->IndexOf(inputPropName.c_str());
    if( -1 != primaryIndex)
    {
        // No prefix, but the property name does exist in the primary feature source
        parsedPropName = inputPropName;
        *gwsFeatureIter = m_gwsFeatureReader;
        bPropertyFound = true;
    }
    else
    {
        // cycle thru secondary feature sources and check property names
        for (int descIndex = 0; descIndex < nPrimaryDescCount; descIndex++)
        {
            secondaryFeatureIter = m_gwsFeatureReader->GetJoinedFeatures(descIndex);
            CHECKNULL(secondaryFeatureIter, L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource");

            if (secondaryFeatureIter->ReadNext())
            {
                GisPtr<IGWSExtendedFeatureDescription> secondaryDesc;
                secondaryFeatureIter->DescribeFeature(&secondaryDesc);

                GWSQualifiedName secQualifiedClassName = secondaryDesc->ClassName();

                GisString* featureSource = secQualifiedClassName.FeatureSource();

                GisPtr<GisStringCollection> secondaryPropNames = secondaryDesc->PropertyNames();

                // cycle thru secondaryPropNames looking for substring occurrence in inputPropName
                GisInt32 secPropCnt = secondaryPropNames->GetCount();
                for (GisInt32 secPropIndex = 0; secPropIndex < secPropCnt; secPropIndex++)
                {
                    STRING secondaryProp = (STRING)secondaryPropNames->GetString(secPropIndex);
                    STRING::size_type nPropStartIndex = inputPropName.find(secondaryProp);
                    if (std::wstring::npos != nPropStartIndex)
                    {
                        parsedPropName = inputPropName.substr(nPropStartIndex).c_str();
                        relationName = inputPropName.substr(0, nPropStartIndex).c_str();

                        if ( wcscmp(featureSource, relationName.c_str()) == 0 )
                        {
                            *gwsFeatureIter = secondaryFeatureIter;
                            bPropertyFound = true;
                            break;
                        }
                    }
                }
            }

        }
    }

    return bPropertyFound;

}


void MgServerGwsGetFeatures::SetRelationNames(GisStringCollection* relationNames)
{
    m_relationNames = relationNames;
}

void MgServerGwsGetFeatures::SetExtensionName(CREFSTRING extensionName)
{
    m_extensionName = extensionName;
}

GisStringCollection* MgServerGwsGetFeatures::GetRelationNames()
{
    return m_relationNames;
}

STRING MgServerGwsGetFeatures::GetExtensionName()
{
    return m_extensionName;
}


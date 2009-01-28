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

#include "ServerFeatureServiceDefs.h"
#include "Services/FeatureService.h"
#include "ServerGetFeatures.h"
#include "ServerFeatureConnection.h"
#include "ServerFeatureUtil.h"
#include "ServerFeatureReader.h"


//////////////////////////////////////////////////////////////////
MgServerGetFeatures::MgServerGetFeatures()
{
    m_featureSet = NULL;
    m_classDef = NULL;
    m_featureReader = NULL;
}


//////////////////////////////////////////////////////////////////
MgServerGetFeatures::MgServerGetFeatures(MgServerFeatureReaderIdentifier* featureReaderIdentifier)
{
    CHECKNULL(featureReaderIdentifier, L"MgServerGetFeatures.MgServerGetFeatures");

    m_featureSet = NULL;
    m_classDef = NULL;
    m_featureReader = featureReaderIdentifier->GetFeatureReader();
}


//////////////////////////////////////////////////////////////////
MgServerGetFeatures::~MgServerGetFeatures()
{
    // TODO: Should we close the FeatureReader or just releasing is fine??
    if (m_featureReader != NULL)
        m_featureReader->Close();

    FDO_SAFE_RELEASE(m_featureReader);
}


//////////////////////////////////////////////////////////////////
MgFeatureSet* MgServerGetFeatures::GetFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_featureReader, L"MgServerGetFeatures.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    INT32 featCount = count;

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Get MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = this->GetMgClassDefinition(true);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGetFeatures.GetFeatures");
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);
    }

    if (NULL == (MgFeatureSet*)m_featureSet)
    {
        // Create a feature set for a pool of features
        m_featureSet = new MgFeatureSet();
        CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerGetFeatures.GetFeatures");

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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetFeatures.GetFeatures")

    return SAFE_ADDREF((MgFeatureSet*)m_featureSet);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerGetFeatures::GetMgClassDefinition(bool bSerialize)
{
    CHECKNULL((FdoIFeatureReader*)m_featureReader, L"MgServerGetFeatures.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_featureReader->GetClassDefinition();
        CHECKNULL((FdoClassDefinition*)fdoClassDefinition, L"MgServerGetFeatures.GetFeatures");

        // Convert FdoClassDefinition to MgClassDefinition
        Ptr<MgClassDefinition> mgClassDef = this->GetMgClassDefinition(fdoClassDefinition, bSerialize);
        CHECKNULL((MgClassDefinition*)mgClassDef, L"MgServerGetFeatures.GetFeatures");

        // Store it for future use
        m_classDef = SAFE_ADDREF((MgClassDefinition*)mgClassDef);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetFeatures.GetMgClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}


//////////////////////////////////////////////////////////////////
MgClassDefinition* MgServerGetFeatures::GetMgClassDefinition(FdoClassDefinition* fdoClassDefinition, bool bSerialize)
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

    bool isAbstract = fdoClassDefinition->GetIsAbstract();
    if (isAbstract)
    {
        mgClassDef->MakeClassAbstract(isAbstract);
    }

    // Retrieve Class properties from FDO
    FdoPtr<FdoPropertyDefinitionCollection> fpdc = fdoClassDefinition->GetProperties();
    CHECKNULL((FdoPropertyDefinitionCollection*)fpdc, L"MgClassDefinition.GetMgClassDefinition");

    // Retrieve Base class properties from FDO
    // TODO: Should we add Base class properties into the list of properties?
    // TODO: Can this be null?
    FdoPtr<FdoReadOnlyPropertyDefinitionCollection> frpdc = fdoClassDefinition->GetBaseProperties();

    // Retrieve identity properties from FDO
    // TODO: Can this be null?
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
        STRING str;
        Ptr<MgByteReader> byteReader = SerializeToXml(fdoClassDefinition);
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
void MgServerGetFeatures::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                             FdoPropertyDefinitionCollection* fdoPropDefCol)
{
    // CHECKNULL((FdoPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetClassProperties");

    // TODO: Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
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
// TODO: Not sure why FdoDataPropertyDefinitionCollection derives from SchemaCollection??
void MgServerGetFeatures::GetClassProperties(MgPropertyDefinitionCollection* propDefCol,
                                             FdoDataPropertyDefinitionCollection* fdoPropDefCol)
{
    // CHECKNULL((FdoDataPropertyDefinitionCollection*)propDefCol, L"MgClassDefinition.GetClassProperties");

    // TODO: Assuming FdoPropertyDefinition can be null
    if (NULL == fdoPropDefCol)
        return;

    FdoInt32 cnt  = fdoPropDefCol->GetCount();
    for (FdoInt32 i =0; i < cnt; i++)
    {
        // Get Fdo Property
        FdoPtr<FdoPropertyDefinition> fpd = fdoPropDefCol->GetItem(i);
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
MgPropertyDefinition* MgServerGetFeatures::GetMgPropertyDefinition(FdoPropertyDefinition* fdoPropDef)
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

    return propDef.Detach();
}


//////////////////////////////////////////////////////////////////
MgDataPropertyDefinition* MgServerGetFeatures::GetDataPropertyDefinition(FdoDataPropertyDefinition* fdoPropDef)
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
MgObjectPropertyDefinition* MgServerGetFeatures::GetObjectPropertyDefinition(FdoObjectPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoObjectPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetObjectPropertyDefinition");

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
    CHECKNULL((FdoClassDefinition*)fdoClsDef, L"MgServerGetFeatures.GetObjectPropertyDefinition")

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
MgGeometricPropertyDefinition* MgServerGetFeatures::GetGeometricPropertyDefinition(FdoGeometricPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoGeometricPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetGeometricPropertyDefinition");

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
MgRasterPropertyDefinition* MgServerGetFeatures::GetRasterPropertyDefinition(FdoRasterPropertyDefinition* fdoPropDef)
{
    CHECKNULL((FdoRasterPropertyDefinition*)fdoPropDef, L"MgClassDefinition.GetRasterPropertyDefinition");

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
void MgServerGetFeatures::AddFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_featureReader, L"MgServerGetFeatures.AddFeatures");
    CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerGetFeatures.AddFeatures");

    INT32 desiredFeatures = 0;

    // Access the class definition
    Ptr<MgClassDefinition> classDef = m_featureSet->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgServerGetFeatures.AddFeatures");

    // Access the property definition collection
    // Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetPropertiesIncludingBase();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerGetFeatures.AddFeatures");

    // How many properties are we fetching, it should be atleast one
    INT32 cnt = propDefCol->GetCount();

    // We only read if there is atleast one property requested
    if (cnt > 0 && count > 0)
    {
        try
        {
            while (m_featureReader->ReadNext())
            {
                AddFeature((MgPropertyDefinitionCollection*)propDefCol);

                // Collected required features therefore do not process more
                if (++desiredFeatures == count)
                    break;
            }
        }
        //some providers will throw if ReadNext is called more than once
        catch (FdoException* e)
        {
            // Note: VB 05/10/06 The assert has been commented out as
            // Linux does not remove them from a release build. The assert
            // will cause the server to crash on Linux. The Oracle provider will throw
            // an exception if the ReadNext() method is called after it returns false.
            // This is a known problem and it is safe to ignore the exception.
            //assert(false);
            e->Release();
        }
    }
}


//////////////////////////////////////////////////////////////////
void MgServerGetFeatures::AddFeature(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((FdoIFeatureReader*)m_featureReader, L"MgServerGetFeatures.AddFeature");
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerGetFeatures.AddFeature");

    //intentionally turn off duplicate checking for better performance
    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection(true, false);
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
MgProperty* MgServerGetFeatures::GetMgProperty(CREFSTRING qualifiedPropName, INT16 type)
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

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetBoolean(propName.c_str());
                isNull = false;
            }

            prop = new MgBooleanProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Byte: /// Unsigned 8 bit value
        {
            FdoByte val = 0;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetByte(propName.c_str());
                isNull = false;
            }

            prop = new MgByteProperty(propName, (BYTE)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::DateTime: /// DateTime object
        {
            Ptr<MgDateTime> dateTime;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                FdoDateTime val = m_featureReader->GetDateTime(propName.c_str());
                dateTime = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                          (INT8)val.hour, (INT8)val.minute, val.seconds);
                isNull = false;
            }

            prop = new MgDateTimeProperty(propName, dateTime);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Single: /// Single precision floating point value
        {
            float val = 0.0f;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetSingle(propName.c_str());
                isNull = false;
            }

            prop = new MgSingleProperty(propName, (float)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Double: /// Double precision floating point value
        {
            double val = 0.0;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetDouble(propName.c_str());
                isNull = false;
            }

            prop = new MgDoubleProperty(propName, (double)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int16: /// 16 bit signed integer value
        {
            FdoInt16 val = 0;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetInt16(propName.c_str());
                isNull = false;
            }

            prop = new MgInt16Property(propName, (INT16)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int32: // 32 bit signed integer value
        {
            FdoInt32 val = 0;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetInt32(propName.c_str());
                isNull = false;
            }

            prop = new MgInt32Property(propName, (INT32)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Int64: // 64 bit signed integer value
        {
            FdoInt64 val = 0;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                val = m_featureReader->GetInt64(propName.c_str());
                isNull = false;
            }

            prop = new MgInt64Property(propName, (INT64)val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::String: // String MgProperty
        {
            STRING val = L"";
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                // A try/catch block is used here for case where the FDO computed
                // property field is used.  When the property value is null, the computed
                // property isNull flag is not set  which causes the IsNull() test to fail, and
                // leading to GetString() to result in an exception.
                // Instead, it will be handled by catching the exception and setting the isNull flag.
                try
                {
                    val = m_featureReader->GetString(propName.c_str());
                    isNull = false;
                }
                catch (FdoException* e)
                {
                    isNull = true;
                    FDO_SAFE_RELEASE(e);
                }
                catch (MgException* e)
                {
                    isNull = true;
                    SAFE_RELEASE(e);
                }
                catch (...)
                {
                    isNull = true;
                }
            }

            prop = new MgStringProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Blob: // BLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                isNull = false;
                val = this->GetLOBFromFdo(propName, m_featureReader);
            }

            prop = new MgBlobProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Clob: // CLOB
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                isNull = false;
                val = this->GetLOBFromFdo(propName, m_featureReader);
            }

            prop = new MgClobProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Feature: // Feature object
        {
            Ptr<MgFeatureReader> val;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                isNull = false;
                FdoPtr<FdoIFeatureReader> featureObject = m_featureReader->GetFeatureObject(propName.c_str());
                if (featureObject != NULL)
                {
                    Ptr<MgServerFeatureReaderIdentifier> sfi = new MgServerFeatureReaderIdentifier(featureObject);
                    val = dynamic_cast<MgFeatureReader*>(new MgServerFeatureReader(sfi));
                }
            }

            prop = new MgFeatureProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Geometry: // Geometry object
        {
            Ptr<MgByteReader> val;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                FdoInt32 length = 0;
                const FdoByte* bytes = m_featureReader->GetGeometry(propName.c_str(), &length);

                if (bytes != NULL)
                {
                    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes, (INT32)length);
                    val = byteSource->GetReader();
                    isNull = false;
                }
            }

            prop = new MgGeometryProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
        case MgPropertyType::Raster: // Raster object
        {
            Ptr<MgRaster> val;
            bool isNull = true;

            if (!m_featureReader->IsNull(propName.c_str()))
            {
                FdoPtr<FdoIRaster> raster = m_featureReader->GetRaster(propName.c_str());
                val = MgServerFeatureUtil::GetMgRaster(raster, propName);
                isNull = false;
            }

            prop = new MgRasterProperty(propName, val);
            prop->SetNull(isNull);
            break;
        }
    }

    return prop.Detach();
}


//////////////////////////////////////////////////////////////////
INT32 MgServerGetFeatures::GetMgPropertyType(FdoDataType fdoDataType)
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

            throw new MgInvalidArgumentException(L"MgServerGetFeatures.GetMgPropertyType",
                __LINE__, __WFILE__, &arguments, L"MgInvalidFdoDataType", NULL);
        }
    }

    return mgPropType;
}


//////////////////////////////////////////////////////////////////
INT16 MgServerGetFeatures::GetMgPropertyType(MgPropertyDefinition* propDef)
{
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgServerGetFeatures.GetMgPropertyType")

    return MgServerFeatureUtil::GetMgPropertyType(propDef);
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGetFeatures::SerializeToXml(FdoClassDefinition* classDef)
{
    CHECKNULL(classDef, L"MgServerGetFeatures.SerializeToXml");

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
        // NOTE: schema name needs to be something non-empty in order to
        // avoid XSL warnings
        tempSchema = FdoFeatureSchema::Create(L"TempSchema", L"" );
        FdoClassesP(tempSchema->GetClasses())->Add(classDef);

        // Add the base class
        FdoPtr<FdoClassDefinition> fdoBaseClass = classDef->GetBaseClass();
        while (fdoBaseClass != NULL)
        {
            FdoClassesP(tempSchema->GetClasses())->Add(fdoBaseClass);
            fdoBaseClass = fdoBaseClass->GetBaseClass();
        }
    }

    FdoIoMemoryStreamP fmis = FdoIoMemoryStream::Create();
    tempSchema->WriteXml(fmis);
    fmis->Reset();

    FdoInt64 len = fmis->GetLength();
    FdoByte *bytes = new FdoByte[(size_t)len];
    CHECKNULL(bytes, L"MgServerGetFeatures::SerializeToXml");

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
MgByteReader* MgServerGetFeatures::GetRaster(INT32 xSize, INT32 ySize, STRING rasterPropName)
{
    CHECKNULL(m_featureReader, L"MgServerGetFeatures.GetRaster");
    CHECKNULL(m_classDef, L"MgServerGetFeatures.GetRaster");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // If there is no raster property, GetRaster should not be called
    if (!m_classDef->HasRasterProperty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgServerGetFeatures.GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // There can be more than one Raster property
    if (rasterPropName.empty())
    {
        rasterPropName = m_classDef->GetRasterPropertyName();
    }

    // If this property is requested then we fetch the raster data
    byteReader = MgServerFeatureUtil::GetRaster(m_featureReader, rasterPropName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGetFeatures.GetRaster")

    return byteReader.Detach();
}


//////////////////////////////////////////////////////////////////
MgByteReader* MgServerGetFeatures::GetLOBFromFdo(CREFSTRING propName, FdoIFeatureReader* featureReader)
{
    CHECKNULL((FdoIFeatureReader*)featureReader, L"MgServerGetFeatures.GetLOBFromFdo");

    Ptr<MgByteReader> byteReader;

    // TODO: We need to switch to FdoIStreamReader when we have streaming capability
    // in MgByteReader
    FdoPtr<FdoLOBValue> fdoVal = featureReader->GetLOB(propName.c_str());
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

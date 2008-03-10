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

#include "JoinFeatureReader.h"

MgJoinFeatureReader::MgJoinFeatureReader(MgServerGwsFeatureReader* reader)
{
    m_reader = SAFE_ADDREF(reader);
}

MgJoinFeatureReader::~MgJoinFeatureReader()
{
}

void MgJoinFeatureReader::Dispose()
{
    delete this;
}

bool MgJoinFeatureReader::GetBoolean(FdoString* propertyName)
{
    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retreive the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetBoolean");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetBoolean(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetBoolean");

    return retVal;
}

FdoByte MgJoinFeatureReader::GetByte(FdoString* propertyName)
{
    FdoByte retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetByte");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetByte(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetByte");

    return retVal;
}

FdoDateTime MgJoinFeatureReader::GetDateTime(FdoString* propertyName)
{
    FdoDateTime val;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetDateTime");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetDateTime");

    return val;
}

double MgJoinFeatureReader::GetDouble(FdoString* propertyName)
{
    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetDouble");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetDouble(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetDouble");

    return retVal;
}

FdoInt16 MgJoinFeatureReader::GetInt16(FdoString* propertyName)
{
    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetInt16");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)gwsFeatureIter->GetInt16(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetInt16");

    return retVal;
}

FdoInt32 MgJoinFeatureReader::GetInt32(FdoString* propertyName)
{
    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetInt32");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)gwsFeatureIter->GetInt32(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetInt32");

    return retVal;
}

FdoInt64 MgJoinFeatureReader::GetInt64(FdoString* propertyName)
{
    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetInt64");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)gwsFeatureIter->GetInt64(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetInt64");

    return retVal;
}

float MgJoinFeatureReader::GetSingle(FdoString* propertyName)
{
    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetSingle");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetSingle(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetSingle");

    return retVal;
}

FdoString* MgJoinFeatureReader::GetString(FdoString* propertyName)
{
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetString");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetString(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetString");

    return ((const wchar_t*)retVal);
}

FdoLOBValue* MgJoinFeatureReader::GetLOB(FdoString* propertyName)
{
    FdoLOBValue* retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetString");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = gwsFeatureIter->GetLOB(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetLOB");

    return retVal;
}

FdoIStreamReader* MgJoinFeatureReader::GetLOBStreamReader(const wchar_t* propertyName )
{
    return NULL;
}

bool MgJoinFeatureReader::IsNull(FdoString* propertyName)
{
    bool retVal = true;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    if (gwsFeatureIter)
    {
        retVal = gwsFeatureIter->IsNull(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.IsNull");

    return retVal;
}

FdoByteArray* MgJoinFeatureReader::GetGeometry(FdoString* propertyName)
{
    FdoByteArray* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetGeometry");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetGeometry");

    return data;
}

FdoIRaster* MgJoinFeatureReader::GetRaster(FdoString* propertyName)
{
    FdoPtr<FdoIRaster> raster;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetRaster");

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data. 
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
        CHECKNULL((FdoIRaster*)raster, L"MgServerGwsFeatureReader.GetRaster");
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetRaster");

    return raster.Detach();
}

bool MgJoinFeatureReader::ReadNext()
{
    return m_reader->ReadNext();
}

void MgJoinFeatureReader::Close()
{
    m_reader->Close();
}

FdoClassDefinition* MgJoinFeatureReader::GetClassDefinition()
{
    Ptr<MgClassDefinition> classDef = m_reader->GetClassDefinition();
    FdoPtr<FdoClassDefinition> fdoClassDef = GetFdoClassDefinition(classDef);
    return fdoClassDef.Detach();
}

FdoInt32 MgJoinFeatureReader::GetDepth()
{
    FdoPtr<IGWSFeatureIterator> iterator = m_reader->GetFeatureIterator();
    return iterator->GetDepth();
}

const FdoByte * MgJoinFeatureReader::GetGeometry(FdoString* propertyName, FdoInt32 * count)
{
    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetGeometry");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoInt32 len = 0;
        data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &len);
        if(count)
        {
            *count = len;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetGeometry");

    return data;
}

FdoIFeatureReader* MgJoinFeatureReader::GetFeatureObject(FdoString* propertyName)
{
    FdoPtr<FdoIFeatureReader> featureObjectReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    m_reader->DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgJoinFeatureReader.GetFeatureObject");

    if(gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgJoinFeatureReader.GetFeatureObject",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        featureObjectReader = gwsFeatureIter->GetFeatureObject(parsedPropertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgJoinFeatureReader.GetFeatureObject");

    return featureObjectReader.Detach();
}

FdoClassDefinition* MgJoinFeatureReader::GetFdoClassDefinition(MgClassDefinition* mgClassDef)
{
    FdoPtr<FdoClassDefinition> fdoClassDef;

    MG_FEATURE_SERVICE_TRY()
    CHECKNULL(mgClassDef, L"MgServerDescribeSchema.GetFdoClassDefinition");

    STRING name = mgClassDef->GetName();
    assert(!name.empty());

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
    GetClassProperties(fdoPropDefCol, mgPropDefCol);

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
            fdoBaseDef = GetFdoClassDefinition(awBaseDef); // Create a new one
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

void MgJoinFeatureReader::GetClassProperties(FdoPropertyDefinitionCollection* fdoPropDefCol,
                                                MgPropertyDefinitionCollection* propDefCol)
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
        FdoPtr<FdoPropertyDefinition> fdoProp = GetFdoPropertyDefinition(awpd);
        // Add it to class definition
        if (fdoProp != NULL)
        {
            fdoPropDefCol->Add(fdoProp);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassProperties")
}

void MgJoinFeatureReader::GetClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol,
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
        FdoPtr<FdoDataPropertyDefinition> fdoPropDef = (FdoDataPropertyDefinition*)GetFdoPropertyDefinition(mgPropDef);
        //Add it to class definition
        if (fdoPropDef != NULL)
        {
            fdoPropDefCol->Add(fdoPropDef);
        }
    }
    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetClassProperties")
}

FdoPropertyDefinition* MgJoinFeatureReader::GetFdoPropertyDefinition(MgPropertyDefinition* mgPropDef)
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
            fdoPropDef = GetObjectPropertyDefinition((MgObjectPropertyDefinition*)mgPropDef);
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

FdoDataPropertyDefinition* MgJoinFeatureReader::GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef)
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
FdoObjectPropertyDefinition* MgJoinFeatureReader::GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef)
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
    FdoPtr<FdoClassDefinition> fdoClsDef = GetFdoClassDefinition(clsDef);
    CHECKNULL(fdoClsDef, L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    FdoPtr<FdoDataPropertyDefinition> fdoDataPropDef = GetDataPropertyDefinition(idProp);
    FdoObjectType fdoObjectType = MgObjectPropertyTypeToFdoObjectType(objType);
    FdoOrderType fdoOrderType = MgOrderingOptionToFdoOrderType(orderType);
    // Set them to Fdo object property
    fdoPropDef->SetName(name.c_str());
    fdoPropDef->SetDescription(desc.c_str());
    fdoPropDef->SetObjectType(fdoObjectType);
    fdoPropDef->SetOrderType(fdoOrderType);
    fdoPropDef->SetClass(fdoClsDef);
    fdoPropDef->SetIdentityProperty(fdoDataPropDef);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDescribeSchema.GetObjectPropertyDefinition")

    return fdoPropDef.Detach();
}


//////////////////////////////////////////////////////////////////
FdoGeometricPropertyDefinition* MgJoinFeatureReader::GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef)
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
    Ptr<MgGeometryTypeInfo> geomTypeInfo = mgPropDef->GetSpecificGeometryTypes();
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

    FdoGeometryType geomTypeList[MG_MAX_GEOMETRY_TYPE_SIZE];
    FdoInt32 geomTypeCount = (FdoInt32) geomTypeInfo->GetCount();
    for (FdoInt32 i = 0;  i < geomTypeCount && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        geomTypeList[i] = (FdoGeometryType)geomTypeInfo->GetType((INT32)i);
    }
    fdoPropDef->SetGeometryTypes((FdoInt32)geomTypes);
    fdoPropDef->SetSpecificGeometryTypes(geomTypeList, geomTypeCount);
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
FdoRasterPropertyDefinition* MgJoinFeatureReader::GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef)
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

FdoDataType MgJoinFeatureReader::GetFdoDataType(INT32 awPropType)
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

FdoObjectType MgJoinFeatureReader::MgObjectPropertyTypeToFdoObjectType(INT32 type)
{
    MgObjectPropertyType::ValidateRange(type);
    FdoObjectType objectType = FdoObjectType_Value;

    switch(type)
    {
        case MgObjectPropertyType::Value:
        {
            // It is already set as a default value
            break;
        }
        case MgObjectPropertyType::Collection:
        {
            objectType = FdoObjectType_Collection;
            break;
        }
        case MgObjectPropertyType::OrderedCollection:
        {
            objectType = FdoObjectType_OrderedCollection;
            break;
        }
    }
    return objectType;
}

FdoOrderType MgJoinFeatureReader::MgOrderingOptionToFdoOrderType(INT32 type)
{
    MgOrderingOption::ValidateRange(type);
    FdoOrderType orderType = FdoOrderType_Ascending;

    switch(type)
    {
        case MgOrderingOption::Ascending:
        {
            // It is already set as a default value
            break;
        }
        case MgOrderingOption::Descending:
        {
            orderType = FdoOrderType_Descending;
            break;
        }
    }
    return orderType;
}

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
#include "ServerSqlDataReader.h"
#include "ServerSqlDataReaderPool.h"
#include "ServerFeatureUtil.h"

MgServerSqlDataReader::MgServerSqlDataReader(MgServerFeatureConnection* connection, FdoISQLDataReader* sqlReader, CREFSTRING providerName)
{
    MG_FEATURE_SERVICE_TRY()

    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_connection = SAFE_ADDREF(connection);
    m_sqlReader = FDO_SAFE_ADDREF(sqlReader);
    m_providerName = providerName;

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.MgServerSqlDataReader")
}

MgServerSqlDataReader::MgServerSqlDataReader()
{
    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_connection = NULL;
    m_sqlReader = NULL;
    m_providerName = L"";
}

MgServerSqlDataReader::~MgServerSqlDataReader()
{
    // DO NOT Close() the FDO reader from here or free the reader resources because
    // we may be reading incrementally from the web tier and the ServerFeatureInstance
    // will live much shorter than the Proxy reader on the web tier which needs to
    // keep reading from the underlying FDO reader.
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Advances the reader to the next item and returns true if there is
/// another object to read or false if reading is complete. The default
/// position of the reader is prior to the first item. Thus you must
/// call ReadNext to begin accessing any data.
/// </summary>
/// <returns>
/// Returns true if there is a next item.
/// </returns>
bool MgServerSqlDataReader::ReadNext()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->ReadNext();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.ReadNext")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of properties in the result set.
/// </summary>
/// <returns>Returns the number of properties.</returns>
INT32 MgServerSqlDataReader::GetPropertyCount()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyCount");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT32)m_sqlReader->GetColumnCount();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyCount")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the property at the given ordinal position.
/// </summary>
/// <param name="index">Input the position of the property.</param>
/// <returns>Returns the property name</returns>
STRING MgServerSqlDataReader::GetPropertyName(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyName");

    STRING retVal;

    MG_FEATURE_SERVICE_TRY()

    FdoString* str = m_sqlReader->GetColumnName(index);
    if (str != NULL)
    {
        retVal = (wchar_t*)str;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyName")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the index of the property with the specified property name.
/// </summary>
/// <param name="propertyName">Input the name of the property.</param>
/// <returns>Returns the property index</returns>
INT32 MgServerSqlDataReader::GetPropertyIndex(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyIndex");

    FdoInt32 index;

    MG_FEATURE_SERVICE_TRY()

    index = m_sqlReader->GetColumnIndex(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyIndex")

    return (INT32)index;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property with the specified name.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgServerSqlDataReader::GetPropertyType(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyType");

    INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoPropertyType propType = m_sqlReader->GetPropertyType(propertyName.c_str());

    switch(propType)
    {
        // If geometric property, return geometric data type.
        case FdoPropertyType_GeometricProperty:
        {
            type = MgPropertyType::Geometry;
            break;
        }
        case FdoPropertyType_DataProperty:
        {
            FdoDataType dataType = m_sqlReader->GetColumnType(propertyName.c_str());
            type = MgServerFeatureUtil::GetMgPropertyType(dataType);
            break;
        }
        case FdoPropertyType_RasterProperty:
        {
            type = MgPropertyType::Raster;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgServerSqlDataReader.GetPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyType")

    return type;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property at the specified index.
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgServerSqlDataReader::GetPropertyType(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyType");

    INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoPropertyType propType = m_sqlReader->GetPropertyType(index);

    switch(propType)
    {
        // If geometric property, return geometric data type.
        case FdoPropertyType_GeometricProperty:
        {
            type = MgPropertyType::Geometry;
            break;
        }
        case FdoPropertyType_DataProperty:
        {
            FdoDataType dataType = m_sqlReader->GetColumnType(index);
            type = MgServerFeatureUtil::GetMgPropertyType(dataType);
            break;
        }
        case FdoPropertyType_RasterProperty:
        {
            type = MgPropertyType::Raster;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgServerSqlDataReader.GetPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyType")

    return type;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerSqlDataReader::IsNull(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->IsNull(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="index">Property index.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerSqlDataReader::IsNull(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->IsNull(index);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerSqlDataReader::GetBoolean(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetBoolean(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerSqlDataReader::GetBoolean(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetBoolean(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerSqlDataReader::GetByte(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)m_sqlReader->GetByte(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerSqlDataReader::GetByte(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)m_sqlReader->GetByte(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerSqlDataReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = m_sqlReader->GetDateTime(propertyName.c_str());
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerSqlDataReader::GetDateTime(INT32 index)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = m_sqlReader->GetDateTime(index);
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDateTime");

    return retVal.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgServerSqlDataReader::GetSingle(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetSingle(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the single value.</returns>
float MgServerSqlDataReader::GetSingle(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetSingle(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgServerSqlDataReader::GetDouble(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetDouble(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the double value.</returns>
double MgServerSqlDataReader::GetDouble(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetDouble(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerSqlDataReader::GetInt16(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)m_sqlReader->GetInt16(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerSqlDataReader::GetInt16(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)m_sqlReader->GetInt16(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerSqlDataReader::GetInt32(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)m_sqlReader->GetInt32(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt32");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerSqlDataReader::GetInt32(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)m_sqlReader->GetInt32(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt32");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 64 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 64 bits or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 64 bits value.
/// Note: INT64 is actually a pointer to an Integer64 object
///</returns>
INT64 MgServerSqlDataReader::GetInt64(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)m_sqlReader->GetInt64(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 64 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 64 bits or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 64 bits value.
/// Note: INT64 is actually a pointer to an Integer64 object
///</returns>
INT64 MgServerSqlDataReader::GetInt64(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)m_sqlReader->GetInt64(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerSqlDataReader::GetString(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        INT32 length = 0;
        const wchar_t* str = this->GetString(propertyName.c_str(), length);
        if (str != NULL)
        {
            retVal = str;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerSqlDataReader::GetString(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        INT32 length = 0;
        const wchar_t* str = this->GetString(index, length);
        if (str != NULL)
        {
            retVal = str;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetBLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(propertyName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetBLOB(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetCLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(propertyName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetCLOB(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerSqlDataReader::GetGeometry(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    try
    {
        FdoPtr<FdoByteArray> byteArray = m_sqlReader->GetGeometry(propertyName.c_str());
        INT32 len = (INT32)byteArray->GetCount();
        const FdoByte* data = byteArray->GetData();

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            retVal = bSource->GetReader();
        }
    }
    catch(...)
    {
        if(m_sqlReader->IsNull(propertyName.c_str()))
        {
            MgStringCollection arguments;
            arguments.Add(propertyName);

            throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetGeometry");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerSqlDataReader::GetGeometry(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    try
    {
        FdoPtr<FdoByteArray> byteArray = m_sqlReader->GetGeometry(index);
        INT32 len = (INT32)byteArray->GetCount();
        const FdoByte* data = byteArray->GetData();

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            retVal = bSource->GetReader();
        }
    }
    catch(...)
    {
        if(m_sqlReader->IsNull(index))
        {
            STRING buffer;
            MgUtil::Int32ToString(index, buffer);

            MgStringCollection arguments;
            arguments.Add(buffer);

            throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetGeometry",
                __LINE__, __WFILE__, &arguments, L"", NULL);
        }
        else
            throw;
    }


    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetGeometry");

    return retVal.Detach();
}

MgByteReader* MgServerSqlDataReader::GetLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetLOB");

    Ptr<MgByteReader> byteReader;

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        // TODO: We need to switch to FdoIStreamReader when we have streaming capability in MgByteReader
        FdoPtr<FdoLOBValue> fdoVal = m_sqlReader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                // TODO: We need to differentiate between CLOB and BLOB
                // TODO: How do we fine the MimeType of data for CLOB
                byteSource->SetMimeType(MgMimeType::Binary);
                byteReader = byteSource->GetReader();
            }
        }
    }
    return byteReader.Detach();
}

MgByteReader* MgServerSqlDataReader::GetLOB(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetLOB");

    Ptr<MgByteReader> byteReader;

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        // TODO: We need to switch to FdoIStreamReader when we have streaming capability in MgByteReader
        FdoPtr<FdoLOBValue> fdoVal = m_sqlReader->GetLOB(index);
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                // TODO: We need to differentiate between CLOB and BLOB
                // TODO: How do we fine the MimeType of data for CLOB
                byteSource->SetMimeType(MgMimeType::Binary);
                byteReader = byteSource->GetReader();
            }
        }
    }
    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerSqlDataReader::Close()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.Close");

    MG_FEATURE_SERVICE_TRY()

    m_sqlReader->Close();
    FDO_SAFE_RELEASE(m_sqlReader);

    // Get the FDO connection
    FdoPtr<FdoIConnection> fdoConnection = m_connection->GetConnection();

    // Release the connection.
    m_connection = NULL;

    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        fdoConnectionManager->Close(fdoConnection);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.Close");
}

void MgServerSqlDataReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration

    Ptr<MgPropertyDefinitionCollection> propDefCol;
    Ptr<MgBatchPropertyCollection> bpCol;
    bool operationCompleted = false;
    STRING sqlReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the sql reader into a pool for ReadNext operation
    MgServerSqlDataReaderPool* dataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL((MgServerSqlDataReaderPool*)dataReaderPool, L"MgServerSqlDataReader.Serialize");

    sqlReader = dataReaderPool->Add(this);// Add the reference to pool

    propDefCol = GetColumnDefinitions();  // Get column definitions
    bpCol = GetRows(count);               // Get rows

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerSqlDataReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteString(sqlReader);                                         // Write the reader ID so we can retrieve it for later use
        stream->WriteString(m_providerName);                                    // Provider Name for XML
        stream->WriteObject((MgPropertyDefinitionCollection*)propDefCol);       // Write the property definition
        stream->WriteObject((MgBatchPropertyCollection*)bpCol);                 // Write the property data
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }

    // Serialize method must not THROW the exception because it has already been written to the stream
    // and in doing so would only end up writing the same exception to the stream twice.
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerSqlDataReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerSqlDataReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerSqlDataReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerSqlDataReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullPropertyValueException
/// MgFdoException
MgRaster* MgServerSqlDataReader::GetRaster(CREFSTRING propertyName)
{
    throw new MgInvalidPropertyTypeException(L"MgServerSqlDataReader.GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullPropertyValueException
/// MgFdoException
MgRaster* MgServerSqlDataReader::GetRaster(INT32 index)
{
    throw new MgInvalidPropertyTypeException(L"MgServerSqlDataReader.GetRaster",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerSqlDataReader::GetString(CREFSTRING propertyName, INT32& length)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetString(propertyName.c_str());
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerSqlDataReader::GetString(INT32 index, INT32& length)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_sqlReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerSqlDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_sqlReader->GetString(index);
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}


MgBatchPropertyCollection* MgServerSqlDataReader::GetRows(INT32 count)
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlReader, L"MgServerSqlDataReader.GetRows");

    MG_FEATURE_SERVICE_TRY()

    // All column properties
    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Get MgPropertyDefinitionCollection
        Ptr<MgPropertyDefinitionCollection> mgPropDef = GetColumnDefinitions();
        CHECKNULL((MgPropertyDefinitionCollection*)mgPropDef, L"MgServerSqlDataReader.GetRows");

        m_propDefCol = SAFE_ADDREF((MgPropertyDefinitionCollection*)mgPropDef);
    }

    if (NULL == (MgBatchPropertyCollection*)m_bpCol)
    {
        // Create a feature set for a pool of features
        m_bpCol = new MgBatchPropertyCollection();
        CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerSqlDataReader.GetRows");
    }
    else
    {
        m_bpCol->Clear();
    }

    // Add all features to feature set
    AddRows(count);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetRows")

    return SAFE_ADDREF((MgBatchPropertyCollection*)m_bpCol);
}

void MgServerSqlDataReader::AddRows(INT32 count)
{
    CHECKNULL((MgServerSqlDataReader*)m_sqlReader, L"MgServerSqlDataReader.AddRows");
    CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerSqlDataReader.AddRows");

    INT32 desiredFeatures = 0;

    while (m_sqlReader->ReadNext())
    {
        AddCurrentRow();
        if (count > 0)
        {
            desiredFeatures++;
            if (desiredFeatures == count) // Collected required features therefore do not process more
            {
                break;
            }
        }
        else // 0 or -ve value means fetch all features
        {
            continue;
        }
    }
}

void MgServerSqlDataReader::AddCurrentRow()
{
    // Access the property definition
    Ptr<MgPropertyDefinitionCollection> propDefCol = GetColumnDefinitions();
    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);
        // Get the name of property
        STRING propName = propDef->GetName();
        INT16 type = propDef->GetPropertyType();

        Ptr<MgProperty> prop = MgServerFeatureUtil::GetMgProperty(this, i, propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }
    m_bpCol->Add(propCol);
}

void MgServerSqlDataReader::AddRow(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.AddRow");
    CHECKNULL(propDefCol, L"MgServerSqlDataReader.AddRow");

    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);
        // Get the name of property
        STRING propName = propDef->GetName();
        INT16 type = propDef->GetPropertyType();

        Ptr<MgProperty> prop = MgServerFeatureUtil::GetMgProperty(this, propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }

    m_bpCol->Add(propCol);
}

MgPropertyDefinitionCollection* MgServerSqlDataReader::GetColumnDefinitions()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetColumnDefinitions");

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Create a new collection
        m_propDefCol = new MgPropertyDefinitionCollection();

        // Collect all property names and types
        INT32 cnt = GetPropertyCount();
        for (INT32 i = 0; i < cnt; i++)
        {
            STRING colName = GetPropertyName(i);
            INT32 colType = GetPropertyType(i);
            Ptr<MgPropertyDefinition> propDef = new MgPropertyDefinition(colName, colType);
            m_propDefCol->Add(propDef);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetColumnDefinitions")

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_propDefCol);
}

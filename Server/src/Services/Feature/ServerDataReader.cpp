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
#include "MapGuideCommon.h"
#include "Services/FeatureService.h"
#include "ServerDataReader.h"
#include "ServerDataReaderPool.h"
#include "ServerFeatureUtil.h"
#include "ServiceManager.h"

MgServerDataReader::MgServerDataReader(MgServerFeatureConnection* connection, FdoIDataReader* dataReader, CREFSTRING providerName)
{
    MG_FEATURE_SERVICE_TRY()

    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_connection = SAFE_ADDREF(connection);
    m_dataReader = FDO_SAFE_ADDREF(dataReader);
    m_providerName = providerName;
    m_removeFromPoolOnDestruction = false;

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.MgServerDataReader")
}

MgServerDataReader::MgServerDataReader()
{
    m_bpCol = NULL;
    m_propDefCol = NULL;
    m_connection = NULL;
    m_dataReader = NULL;
    m_providerName = L"";
    m_removeFromPoolOnDestruction = false;
}

MgServerDataReader::~MgServerDataReader()
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
bool MgServerDataReader::ReadNext()
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_dataReader->ReadNext();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.ReadNext")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of Propertys in the result set.
/// </summary>
/// <returns>Returns the number of Propertys.</returns>
INT32 MgServerDataReader::GetPropertyCount()
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetPropertyCount");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT32)m_dataReader->GetPropertyCount();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetPropertyCount")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the Property at the given ordinal position.
/// </summary>
/// <param name="index">Input the position of the Property.</param>
/// <returns>Returns the Property name</returns>
STRING MgServerDataReader::GetPropertyName(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetPropertyName");

    STRING retVal;

    MG_FEATURE_SERVICE_TRY()

    FdoString* str = m_dataReader->GetPropertyName(index);
    if (str != NULL)
    {
        retVal = (wchar_t*)str;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetPropertyName")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the index of the Property with the specified name.
/// </summary>
/// <param name="index">Input the name of the Property.</param>
/// <returns>Returns the Property index</returns>
INT32 MgServerDataReader::GetPropertyIndex(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetPropertyIndex");

    FdoInt32 index;

    MG_FEATURE_SERVICE_TRY()

    index = m_dataReader->GetPropertyIndex(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetPropertyIndex")

    return (INT32)index;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the Property with the specified name.
/// </summary>
/// <param name="propertyName">Input the Property name.</param>
/// <returns>Returns the type of the Property.</returns>
INT32 MgServerDataReader::GetPropertyType(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetPropertyType");

    INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoPropertyType propType = m_dataReader->GetPropertyType(propertyName.c_str());

    switch (propType)
    {
        // If geometric property, return geometric data type.
        case FdoPropertyType_GeometricProperty:
        {
            type = MgPropertyType::Geometry;
            break;
        }
        case FdoPropertyType_DataProperty:
        {
            FdoDataType dataType = m_dataReader->GetDataType(propertyName.c_str());
            if (FdoDataType(-1) == dataType)
            {
                throw new MgInvalidPropertyTypeException(L"MgServerDataReader.GetPropertyType",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }

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
            throw new MgInvalidPropertyTypeException(L"MgServerDataReader.GetPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetPropertyType")

    return type;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerDataReader::IsNull(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_dataReader->IsNull(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="index">Property index.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerDataReader::IsNull(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_dataReader->IsNull(index);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerDataReader::GetBoolean(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetBoolean(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerDataReader::GetBoolean(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetBoolean(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerDataReader::GetByte(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)m_dataReader->GetByte(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerDataReader::GetByte(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)m_dataReader->GetByte(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetByte");

    return retVal;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerDataReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_dataReader, L"MgServerDataReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = m_dataReader->GetDateTime(propertyName.c_str());
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                            (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerDataReader::GetDateTime(INT32 index)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_dataReader, L"MgServerDataReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = m_dataReader->GetDateTime(index);
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                            (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgServerDataReader::GetSingle(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetSingle(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the single value.</returns>
float MgServerDataReader::GetSingle(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetSingle(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgServerDataReader::GetDouble(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetDouble(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the double value.</returns>
double MgServerDataReader::GetDouble(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetDouble(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerDataReader::GetInt16(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)m_dataReader->GetInt16(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerDataReader::GetInt16(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)m_dataReader->GetInt16(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerDataReader::GetInt32(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)m_dataReader->GetInt32(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt32");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerDataReader::GetInt32(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)m_dataReader->GetInt32(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt32");

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
INT64 MgServerDataReader::GetInt64(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)m_dataReader->GetInt64(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt64");

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
INT64 MgServerDataReader::GetInt64(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)m_dataReader->GetInt64(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerDataReader::GetString(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetString",
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerDataReader::GetString(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetString",
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerDataReader::GetBLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(propertyName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerDataReader::GetBLOB(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerDataReader::GetCLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(propertyName);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerDataReader::GetCLOB(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        byteReader = this->GetLOB(index);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerDataReader::GetGeometry(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoByteArray> byteArray = m_dataReader->GetGeometry(propertyName.c_str());
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetGeometry");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerDataReader::GetGeometry(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoByteArray> byteArray = m_dataReader->GetGeometry(index);
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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetGeometry");

    return retVal.Detach();
}

// Get the Raster data
MgRaster* MgServerDataReader::GetRaster(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetRaster");

    Ptr<MgRaster> retVal;
    STRING dataReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIRaster> raster = m_dataReader->GetRaster(propertyName.c_str());
        CHECKNULL((FdoIRaster*)raster, L"MgServerDataReader.GetRaster");

        retVal = MgServerFeatureUtil::GetMgRaster(raster, propertyName);
        CHECKNULL((MgRaster*)retVal, L"MgServerDataReader.GetRaster");

        // Set the service to MgRaster for GetRaster operation called in GetStream method
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);

        // Get the service from service manager
        Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
            serviceMan->RequestService(MgServiceType::FeatureService));
        assert(featureService != NULL);

        // Collect the data reader into a pool for ReadNext operation
        MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
        CHECKNULL((MgServerDataReaderPool*)dataReaderPool, L"MgServerDataReader.GetRaster");

        dataReader = dataReaderPool->GetReaderId(this);
        if(L"" == dataReader)
        {
            dataReader = dataReaderPool->Add(this); // Add the reference
            m_removeFromPoolOnDestruction = true;
        }

        retVal->SetMgService(featureService);
        retVal->SetHandle(dataReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetRaster");

    return retVal.Detach();
}

// Get the Raster data
MgRaster* MgServerDataReader::GetRaster(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetRaster");

    Ptr<MgRaster> retVal;
    STRING dataReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIRaster> raster = m_dataReader->GetRaster(index);
        CHECKNULL((FdoIRaster*)raster, L"MgServerDataReader.GetRaster");

        STRING propertyName = GetPropertyName(index);
        retVal = MgServerFeatureUtil::GetMgRaster(raster, propertyName);
        CHECKNULL((MgRaster*)retVal, L"MgServerDataReader.GetRaster");

        // Set the service to MgRaster for GetRaster operation called in GetStream method
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);

        // Get the service from service manager
        Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
            serviceMan->RequestService(MgServiceType::FeatureService));
        assert(featureService != NULL);

        // Collect the data reader into a pool for ReadNext operation
        MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
        CHECKNULL((MgServerDataReaderPool*)dataReaderPool, L"MgServerDataReader.GetRaster");

        dataReader = dataReaderPool->GetReaderId(this);
        if(L"" == dataReader)
        {
            dataReader = dataReaderPool->Add(this); // Add the reference
            m_removeFromPoolOnDestruction = true;
        }

        retVal->SetMgService(featureService);
        retVal->SetHandle(dataReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetRaster");

    return retVal.Detach();
}

MgByteReader* MgServerDataReader::GetLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetLOB");

    Ptr<MgByteReader> retVal;

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        // TODO: We need to switch to FdoIStreamReader when we have streaming capability in MgByteReader
        FdoPtr<FdoLOBValue> fdoVal = m_dataReader->GetLOB(propertyName.c_str());
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
                retVal = byteSource->GetReader();
            }
        }
    }

    return retVal.Detach();
}

MgByteReader* MgServerDataReader::GetLOB(INT32 index)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetLOB");

    Ptr<MgByteReader> retVal;

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        // TODO: We need to switch to FdoIStreamReader when we have streaming capability in MgByteReader
        FdoPtr<FdoLOBValue> fdoVal = m_dataReader->GetLOB(index);
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
                retVal = byteSource->GetReader();
            }
        }
    }

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerDataReader::Close()
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.Close");

    MG_FEATURE_SERVICE_TRY()

    // If m_getFeatures was added to pool by the local service
    // this flag will be set to true. In this case we need to
    // remove this from pool on ServerFeatureReader close operation
    if (m_removeFromPoolOnDestruction)
    {
        MgServerDataReaderPool* drPool = MgServerDataReaderPool::GetInstance();
        if(NULL != drPool)
        {
            STRING dataReader = drPool->GetReaderId(this);
            if (L"" != dataReader)
                drPool->Remove(dataReader);
        }
    }

    m_dataReader->Close();

    FDO_SAFE_RELEASE(m_dataReader);

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

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.Close");
}

void MgServerDataReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration

    Ptr<MgPropertyDefinitionCollection> propDefCol;
    Ptr<MgBatchPropertyCollection> bpCol;
    bool operationCompleted = false;
    STRING dataReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the data reader into a pool for ReadNext operation
    MgServerDataReaderPool* dataReaderPool = MgServerDataReaderPool::GetInstance();
    CHECKNULL((MgServerDataReaderPool*)dataReaderPool, L"MgServerDataReader.Serialize");

    dataReader = dataReaderPool->GetReaderId(this);
    // Add data reader to pool
    if (L"" == dataReader)
    {
        dataReader = dataReaderPool->Add(this); // Add the reference
    }

    propDefCol = GetColumnDefinitions();  // Get column definitions
    bpCol = GetRows(count);               // Get rows

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerDataReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteString(dataReader);                                            // Write the reader ID so we can retrieve it for later use
        stream->WriteString(m_providerName);                                        // Provider Name for XML
        stream->WriteObject((MgPropertyDefinitionCollection*)propDefCol);           // Write the Property definition
        stream->WriteObject((MgBatchPropertyCollection*)bpCol);                     // Write the Property data
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

void MgServerDataReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerDataReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerDataReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerDataReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

MgByteReader* MgServerDataReader::GetRaster(INT32 xSize, INT32 ySize, STRING rasterPropName)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetRaster");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if (rasterPropName.empty())
        rasterPropName = GetRasterPropertyName();

    // If there is no raster property, GetRaster should not be called
    if (rasterPropName.empty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgServerDataReader.GetRaster",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    byteReader = MgServerFeatureUtil::GetRaster(m_dataReader, rasterPropName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetRaster")

    return byteReader.Detach();
}


STRING MgServerDataReader::GetRasterPropertyName()
{
    STRING name = L"";

    INT32 cnt = GetPropertyCount();
    for (INT32 i = 0; i < cnt; i++)
    {
        name = GetPropertyName(i);

        INT32 type = GetPropertyType(name);
        if (type == MgPropertyType::Raster)
        {
            break;
        }
    }

    return name;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerDataReader::GetString(CREFSTRING propertyName, INT32& length)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetString");

    FdoString* retVal = NULL;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetString(propertyName.c_str());
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
const wchar_t* MgServerDataReader::GetString(INT32 index, INT32& length)
{
    CHECKNULL(m_dataReader, L"MgServerDataReader.GetString");

    FdoString* retVal = NULL;

    MG_FEATURE_SERVICE_TRY()

    if(m_dataReader->IsNull(index))
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgServerDataReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_dataReader->GetString(index);
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}

MgBatchPropertyCollection* MgServerDataReader::GetRows(INT32 count)
{
    MG_FEATURE_SERVICE_TRY()

    INT32 featCnt = count;

    // All Column properties
    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Get MgPropertyDefinitionCollection
        Ptr<MgPropertyDefinitionCollection> mgPropDef = GetColumnDefinitions();
        CHECKNULL((MgPropertyDefinitionCollection*)mgPropDef, L"MgServerDataReader.GetRows");

        m_propDefCol = SAFE_ADDREF((MgPropertyDefinitionCollection*)mgPropDef);
    }

    if (NULL == (MgBatchPropertyCollection*)m_bpCol)
    {
        // Create a feature set for a pool of features
        m_bpCol = new MgBatchPropertyCollection();
        CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerDataReader.GetRows");
    }
    else
    {
        m_bpCol->Clear();
    }

    // If we have raster property, we only send one feature
    // override the configuration property setting
    STRING rasterProp = GetRasterPropertyName();
    if (!rasterProp.empty())
    {
        featCnt = 1;
    }

    // Add all features to feature set
    AddRows(featCnt);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetRows")

    return SAFE_ADDREF((MgBatchPropertyCollection*)m_bpCol);
}

MgPropertyDefinitionCollection* MgServerDataReader::GetColumnDefinitions()
{
    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        // Create a new collection
        m_propDefCol = MgServerFeatureUtil::GetPropertyDefinitions(this);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetColumnDefinitions")

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_propDefCol);
}


void MgServerDataReader::AddRows(INT32 count)
{
    CHECKNULL((MgBatchPropertyCollection*)m_bpCol, L"MgServerDataReader.AddRows");

    INT32 desiredFeatures = 0;

    // Access the property definition collection
    Ptr<MgPropertyDefinitionCollection> propDefCol = GetColumnDefinitions();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerDataReader.AddRows");

    bool found = false;

    // FDO throws exception on ReadNext() which is not correct.
    // We catch the exception and make it false as it could not fetch the rows any more
    try
    {
        found = m_dataReader->ReadNext();
    }
    catch(FdoException* e)
    {
        FDO_SAFE_RELEASE(e);
        found = false;
    }
    catch(...)
    {
        found = false;
    }

    while (found)
    {
        AddRow((MgPropertyDefinitionCollection*)propDefCol);
        if (count > 0)
        {
            desiredFeatures++;
            if (desiredFeatures == count) // Collected required features therefore do not process more
            {
                break;
            }
        }
        // FDO throws exception on ReadNext() which is not correct.
        // We catch the exception and make it false as it could not fetch the rows any more
        try
        {
            found = m_dataReader->ReadNext();
        }
        catch(FdoException* e)
        {
            FDO_SAFE_RELEASE(e);
            found = false;
        }
        catch(...)
        {
            found = false;
        }
    }
}


void MgServerDataReader::AddRow(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerDataReader.AddRow");

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

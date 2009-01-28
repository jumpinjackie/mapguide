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

#include "ServerFdoFeatureReader.h"

MgServerFdoFeatureReader::MgServerFdoFeatureReader(FdoIFeatureReader* fdoFeatureReader) :
    m_internalReader(fdoFeatureReader),
    m_featureIndex(-1),
    m_maxFeatures(-1)
{
}

MgServerFdoFeatureReader::~MgServerFdoFeatureReader()
{
}

// Set the max number of features returned by this reader
void MgServerFdoFeatureReader::SetMaxFeatures(int maxFeatures)
{
    m_maxFeatures = maxFeatures;
}

/// \brief
/// Gets the definition of the object currently being read. If the user
/// has requested only a subset of the class properties, the class
/// definition reflects what the user has asked, rather than the full class
/// definition.
///
/// \return
/// Returns the class definition object.
///
FdoClassDefinition* MgServerFdoFeatureReader::GetClassDefinition()
{
    return m_internalReader->GetClassDefinition();
}

/// \brief
/// Gets a value indicating the depth of nesting for the current reader.
/// The depth value increases each time GetFeatureObject is called and a new
/// reader is returned. The outermost reader has a depth of 0.
///
/// \return
/// Returns the depth
///
FdoInt32 MgServerFdoFeatureReader::GetDepth()
{
    return m_internalReader->GetDepth();
}

/// \brief
/// Gets the geometry value of the specified property as a byte array in
/// FGF format. Because no conversion is performed, the property must be
/// of Geometric type; otherwise, an exception is thrown.
/// This method is a language-specific performance optimization that returns a
/// pointer to the array data, rather than to an object that encapsulates
/// the array.  The array's memory area is only guaranteed to be valid
/// until a call to ReadNext() or Close(), or the disposal of this reader
/// object.
///
/// \param propertyName
/// Input the property name.
/// \param count
/// Output the number of bytes in the array.
///
/// \return
/// Returns a pointer to the byte array in FGF format.
///
const FdoByte * MgServerFdoFeatureReader::GetGeometry(FdoString* propertyName, FdoInt32 * count)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetGeometry(propertyName, count);
    }
}

/// \brief
/// Gets the geometry value of the specified property as a byte array in
/// FGF format. Because no conversion is performed, the property must be
/// of Geometric type; otherwise, an exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the byte array in FGF format.
///
FdoByteArray* MgServerFdoFeatureReader::GetGeometry(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetGeometry(propertyName);
    }
}

/// \brief
/// Gets a reference to an FdoIFeatureReader to read the data contained in
/// the object or object collection property. If the property is not an
/// object property, an exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the nested feature reader
///
FdoIFeatureReader* MgServerFdoFeatureReader::GetFeatureObject(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetFeatureObject",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetFeatureObject(propertyName);
    }
}

/// \brief
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Boolean or an
/// exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the Boolean value.
///
bool MgServerFdoFeatureReader::GetBoolean(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetBoolean(propertyName);
    }
}

/// \brief
/// Gets the byte value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Byte or an
/// exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the byte value.
///
FdoByte MgServerFdoFeatureReader::GetByte(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetByte(propertyName);
    }
}

/// \brief
///  Gets the date and time value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_DateTime or an
/// exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the date and time value.
///
FdoDateTime MgServerFdoFeatureReader::GetDateTime(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetDateTime(propertyName);
    }
}

/// \brief
/// Gets the double-precision floating point value of the specified property. No
/// conversion is performed, thus the property must be FdoDataType_Double
/// or an exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the double floating point value
///
double MgServerFdoFeatureReader::GetDouble(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetDouble(propertyName);
    }
}

/// \brief
/// Gets the 16-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int16 or an exception
/// is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the FdoInt16 value.
///
FdoInt16 MgServerFdoFeatureReader::GetInt16(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetInt16(propertyName);
    }
}

/// \brief
/// Gets the 32-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int32 or an exception
/// is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the FdoInt32 value
///
FdoInt32 MgServerFdoFeatureReader::GetInt32(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetInt32(propertyName);
    }
}

/// \brief
/// Gets the 64-bit integer value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_Int64 or an exception
/// is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the FdoInt64 value.
///
FdoInt64 MgServerFdoFeatureReader::GetInt64(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetInt64(propertyName);
    }
}

/// \brief
/// Gets the Single floating point value of the specified property. No
/// conversion is performed, thus the property must be FdoDataType_Single
/// or an exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the single value
///
float MgServerFdoFeatureReader::GetSingle(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetSingle(propertyName);
    }
}

/// \brief
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be FdoDataType_String or an exception
/// is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the string value
///
FdoString* MgServerFdoFeatureReader::GetString(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetString(propertyName);
    }
}

/// \brief
/// Gets a LOBValue reference. The LOB is fully read in and data available.
/// Because no conversion is performed, the property must be FdoDataType_BLOB or
/// FdoDataType_CLOB etc. (a LOB type)
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the reference to LOBValue
///
FdoLOBValue* MgServerFdoFeatureReader::GetLOB(FdoString* propertyName)
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetLOB(propertyName);
    }
}

/// \brief
/// Gets a reference of the specified LOB property as a FdoBLOBStreamReader or
/// FdoCLOBStreamReader etc. to allow reading in blocks of data.
/// Because no conversion is performed, the property must be FdoDataType_BLOB
/// or FdoDataType_CLOB etc. (a LOB type)
/// Cast the FdoIStreamReader to the appropiate LOB Stream Reader.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns a reference to a LOB stream reader
///
FdoIStreamReader* MgServerFdoFeatureReader::GetLOBStreamReader(const wchar_t* propertyName )
{
    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetLOBStreamReader",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetLOBStreamReader(propertyName);
    }
}

/// \brief
/// Returns true if the value of the specified property is null.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns true if the value is null.
///
bool MgServerFdoFeatureReader::IsNull(FdoString* propertyName)
{
    return m_internalReader->IsNull(propertyName);
}

/// \brief
/// Gets the raster object of the specified property.
/// Because no conversion is performed, the property must be
/// of Raster type; otherwise, an exception is thrown.
///
/// \param propertyName
/// Input the property name.
///
/// \return
/// Returns the raster object.
///
FdoIRaster* MgServerFdoFeatureReader::GetRaster(FdoString* propertyName)
{
    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(m_internalReader->IsNull(propertyName))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        return m_internalReader->GetRaster(propertyName);
    }
}

/// \brief
/// Advances the reader to the next item and returns true if there is
/// another object to read or false if reading is complete. The default
/// position of the reader is prior to the first item. Thus you must
/// call ReadNext to begin accessing any data.
///
/// \return
/// Returns true if there is a next item.
///
bool MgServerFdoFeatureReader::ReadNext()
{
    if(m_maxFeatures < 0 || m_featureIndex < m_maxFeatures)
    {
        m_featureIndex++;
        return m_internalReader->ReadNext();
    }
    else
    {
        return false;
    }
}

/// \brief
/// Closes the FdoIFeatureReader object, freeing any resources it may be holding.
///
/// \return
/// Returns nothing
///
void MgServerFdoFeatureReader::Close()
{
    return m_internalReader->Close();
}

/// \brief
/// Dispose this object.
///
/// \return
/// Returns nothing
///
void MgServerFdoFeatureReader::Dispose()
{
}

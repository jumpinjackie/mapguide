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

#ifndef _MG_FEATURE_READER_H
#define _MG_FEATURE_READER_H

/// \defgroup MgFeatureReader MgFeatureReader
/// \ingroup Feature_Service_classes
/// \{

#ifndef FEATURESET_TEMPL
#define FEATURESET_TEMPL
class MgFeatureSet;
template class MG_PLATFORMBASE_API Ptr<MgFeatureSet>;
#endif

class MgRaster;

class MgFeatureReader;
template class MG_PLATFORMBASE_API Ptr<MgFeatureReader>;

//////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Provides a forward-only, read-only iterator for reading
/// features selected from the FDO Provider by the
/// MgFeatureService::SelectFeatures() method call.
///
/// \remarks
/// You must call ReadNext() before you can access the data. Call
/// GetProperties() to get the property values for the feature.
/// For each property, determine the property type and then call
/// the appropriate Get<type>() method to get the value of the
/// property. With one exception the Get<type> methods belong
/// to the MgReader base class. The exception is
/// MgFeatureReader::GetFeatureObject(). Obtain the next feature
/// by calling the ReadNext() method.
///
/// <!-- Example (PHP) -->
/// \htmlinclude PHPExampleTop.html
/// \code
/// // each ReadNext() gets a "row" object, that is,
/// // an instantiation of a class with properties
/// // returns number of features in reader
/// function printFeatureReader($featureReader)
/// {
///    global $logFileHandle;
///    $i = 0;
///    while ($featureReader->ReadNext())
///    {
///       fwrite($logFileHandle, "Printing a feature.n");
///       $i++;
///       $propCount = $featureReader->GetPropertyCount();
///       for($j=0; $j<$propCount; $j++)
///       {
///           $propertyName = $featureReader->GetPropertyName($j);
///           $boolVal = $featureReader->IsNull($propertyName);
///           if ($boolVal)
///              continue;
///           $propertyType = $featureReader->GetPropertyType($propertyName);
///           printPropertyValueFromFeatReader($featureReader, $propertyType, $propertyName);
///       }
///    }
///    return $i;
/// }
///
/// function printPropertyValueFromFeatReader($featureReader, $propertyType, $propertyName)
/// {
///    global $logFileHandle;
///    global $agfReaderWriter;
///    global $wktReaderWriter;
///    switch ($propertyType)
///    {
///       case MgPropertyType::Null :
///              fwrite($logFileHandle, "$propertyName is a null propertyn");
///              break;
///       case MgPropertyType::Boolean :
///              $val = $featureReader->GetBoolean($propertyName);
///              $valStr = printBoolean($val);
///              fwrite($logFileHandle, "$propertyName is boolean "$valStr"n");
///              break;
///       case MgPropertyType::Byte :
///              $val = $featureReader->GetByte($propertyName);
///              fwrite($logFileHandle, "$propertyName has a byte value of "$val"n");
///              break;
///       case MgPropertyType::DateTime :
///              $val = $featureReader->GetDateTime($propertyName);
///              $valStr = printDateTime($val);
///              fwrite($logFileHandle, "DateTime is "$valStr"n");
///              break;
///       case MgPropertyType::Single :
///              $val = $featureReader->GetSingle($propertyName);
///              fwrite($logFileHandle, "$propertyName is single "$val"n");
///              break;
///       case MgPropertyType::Double :
///              $val = $featureReader->GetDouble($propertyName);
///              fwrite($logFileHandle, "$propertyName is double "$val"n");
///              break;
///       case MgPropertyType::Int16 :
///              $val = $featureReader->GetInt16($propertyName);
///              fwrite($logFileHandle, "$propertyName is int16 "$val"n");
///              break;
///       case MgPropertyType::Int32 :
///              $val = $featureReader->GetInt32($propertyName);
///              fwrite($logFileHandle, "$propertyName is int32 "$val"n");
///              break;
///       case MgPropertyType::Int64 :
///              $val = $featureReader->GetInt64($propertyName);
///              fwrite($logFileHandle, "$propertyName is int64 "$val"n");
///              break;
///       case MgPropertyType::String :
///              $val = $featureReader->GetString($propertyName);
///              fwrite($logFileHandle, "$propertyName is string "$val"n");
///              break;
///       case MgPropertyType::Blob :
///              fwrite($logFileHandle, "$propertyName is blobn");
///              break;
///       case MgPropertyType::Clob :
///              fwrite($logFileHandle, "$propertyName is clobn");
///              break;
///       case MgPropertyType::Feature :
///              $val = $featureReader->GetFeatureObject($propertyName);
///             if ($val != NULL)
///             {
///                  fwrite($logFileHandle, "$propertyName is a featuren");
///                  printFeatureReader($val);
///             }
///             break;
///       case MgPropertyType::Geometry :
///              fwrite($logFileHandle, "$propertyName is a geometryn");
///              $val = $featureReader->GetGeometry($propertyName);
///              if ($val != NULL)
///              {
///                 $aGeometry = $agfReaderWriter->Read($val);
///                 $wktRepresentation = $wktReaderWriter->Write($aGeometry);
///                 fwrite($logFileHandle, "WKT Representation: "$wktRepresentation"n");
///              }
///              else
///              {
///                 fwrite($logFileHandle, "This geometry property is nulln");
///              }
///              break;
///       case MgPropertyType::Raster :
///              $val = $featureReader->GetRaster($propertyName);
///              fwrite($logFileHandle, "$propertyName is a rastern");
///              break;
///       default : fwrite($logFileHandle, "Unknown property typen");
///    }
/// }
/// \endcode
/// \htmlinclude ExampleBottom.html
///
/// <!-- Example (C#) -->
/// \htmlinclude CSharpExampleTop.html
/// \code
/// using OSGeo.MapGuide;
/// private StringBuilder sb;
/// private const int sbLength = 512;
///
/// public int PrintFeatureReader(MgFeatureReader featureReader, out String propertiesToStr)
/// {
///     int featureCount = 0;
///     int propertyCount;
///     String propertyName;
///     int propertyType;
///     sb = new StringBuilder(sbLength);
///     while (featureReader.ReadNext())
///     {
///         featureCount++;
///         propertyCount = featureReader.GetPropertyCount();
///         for (int i = 0; i < propertyCount; i++)
///         {
///             propertyName = featureReader.GetPropertyName(i);
///             propertyType = featureReader.GetPropertyType(propertyName);
///             PrintPropertyValueFromReader(featureReader, propertyType, propertyName, ref sb);
///         }
///     }
///     propertiesToStr = sb.ToString();
///     return featureCount;
/// }
///
/// private void PrintPropertyValueFromReader(MgReader reader,
///     int propertyType, String propertyName, ref StringBuilder sb)
/// {
///     Boolean isNull = reader.IsNull(propertyName);
///     if (propertyType == MgPropertyType.Blob)
///     {
///         sb.Append(propertyName + "=Blob(" + (isNull ? "null" : "notNull") + ");");
///     }
///     else if (propertyType == MgPropertyType.Boolean)
///     {
///         sb.Append(propertyName + "=Boolean(" + (isNull ? "null" : reader.GetBoolean(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Byte)
///     {
///         sb.Append(propertyName + "=Byte(" + (isNull ? "null" : reader.GetByte(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Clob)
///     {
///         sb.Append(propertyName + "=Clob(" + (isNull ? "null" : "notNull") + ");");
///     }
///     else if (propertyType == MgPropertyType.DateTime)
///     {
///         String dateTimeStr = null;
///         if (!isNull)
///         {
///             MgDateTime aDateTime = reader.GetDateTime(propertyName);
///             dateTimeStr = aDateTime.Month.ToString() + '.'
///                 + aDateTime.Day.ToString() + '.' + aDateTime.Year.ToString()
///                 + ':' + aDateTime.Hour.ToString() + ':' + aDateTime.Minute.ToString()
///                 + ':' + aDateTime.Second.ToString();
///         }
///         sb.Append(propertyName + "=DateTime(" + (isNull ? "null" : dateTimeStr) + ");");
///     }
///     else if (propertyType == MgPropertyType.Double)
///     {
///         sb.Append(propertyName + "=Double(" + (isNull ? "null" : reader.GetDouble(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Feature)
///     {
///         sb.Append(propertyName + "=Feature(" + (isNull ? "null" : "notNull") + ");");
///     }
///     else if (propertyType == MgPropertyType.Geometry)
///     {
///         sb.Append(propertyName + "=Geometry(" + (isNull ? "null" : geometryServices.MgByteReaderToWktText(reader.GetGeometry(propertyName))) + ");");
///     }
///     else if (propertyType == MgPropertyType.Int16)
///     {
///         sb.Append(propertyName + "=Int16(" + (isNull ? "null" : reader.GetInt16(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Int32)
///     {
///         sb.Append(propertyName + "=Int32(" + (isNull ? "null" : reader.GetInt32(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Int64)
///     {
///         sb.Append(propertyName + "=Int64(" + (isNull ? "null" : reader.GetInt64(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.Raster)
///     {
///         sb.Append(propertyName + "=Raster(" + (isNull ? "null" : "notNull") + ");");
///     }
///     else if (propertyType == MgPropertyType.Single)
///     {
///         sb.Append(propertyName + "=Single(" + (isNull ? "null" : reader.GetSingle(propertyName).ToString()) + ");");
///     }
///     else if (propertyType == MgPropertyType.String)
///     {
///         sb.Append(propertyName + "=String(" + (isNull ? "null" : reader.GetString(propertyName)) + ");");
///     }
///     else
///     {
///         sb.Append(propertyName + "=Unknown(" + propertyType.ToString() + ");");
///     }
/// }
///
/// \endcode
/// \htmlinclude ExampleBottom.html  
/// 
class MG_PLATFORMBASE_API MgFeatureReader : public MgReader
{
    DECLARE_CLASSNAME(MgFeatureReader)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class definition of the object currently being read.
    /// If the user has requested only a subset of the class
    /// properties (as specified in the filter text), the class
    /// definition reflects what the user has requested, rather than
    /// the full class definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgClassDefinition object representing the current
    /// feature object.
    ///
    /// \exception MgConnectionNotOpenException
    /// \exception MgFdoException
    ///
    virtual MgClassDefinition* GetClassDefinition() = 0;

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Use this method to obtain the values of the properties
    /// belonging to an object contained in the feature class
    /// instance. Such an object is a property of the feature class
    /// instance with a type of MgPropertyType::Feature.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader GetFeatureObject(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader GetFeatureObject(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader GetFeatureObject(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns an MgFeatureReader object, which can be used to
    /// access properties of an object contained in the feature
    /// object.
    ///
    /// \exception MgInvalidArgumentException
    ///  if the property type is not a
    ///  feature.
    /// \exception MgConnectionNotOpenException
    /// \exception MgNullPropertyValueException
    /// \exception MgFdoException
    ///
    virtual MgFeatureReader* GetFeatureObject(CREFSTRING propertyName) = 0;

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of properties in the result set.
    ///
    /// \return
    /// Returns the number of properties.
    ///
    virtual INT32 GetPropertyCount();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the property at the given ordinal position.
    ///
    /// \param index
    /// Input the position of the property.
    ///
    /// \return
    /// Returns the property name
    ///
    virtual STRING GetPropertyName(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the data type of the property with the specified name.
    /// Please refer to MgPropertyType for list of values
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the type of the property.
    ///
    virtual INT32 GetPropertyType(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type reader.
    /// Please refer to MgReaderType for list of values
    ///
    /// \return
    /// Returns the type of the Reader.
    ///
    virtual INT32 GetReaderType();

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

INTERNAL_API:

    //Same as GetClassDefinition, however does not serialize
    //the class definition to XML in the case where our feature
    //reader and its caller are local
    virtual MgClassDefinition* GetClassDefinitionNoXml() = 0;

private:
    INT16 GetMgPropertyType(MgPropertyDefinition* propDef);

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureReader;
};
/// \}

#endif

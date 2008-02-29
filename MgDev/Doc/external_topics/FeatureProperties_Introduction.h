/**
\addtogroup Feature_Properties_Module

<p>
A feature property is a key-value pair where the key is a
property name as defined in a feature class definition, and
the value is the property's value. The property's type is one
of the classes derived from the base class MgProperty. The
following list shows the names of the classes derived from
MgProperty and, for each class, the type returned by the
GetValue() method:
  \li  MgBlobProperty::GetValue() returns MgByteReader
  \li  MgBooleanProperty::GetValue() returns bool
  \li  MgByteProperty::GetValue() returns BYTE (unsigned char)
  \li  MgClobProperty::GetValue() returns MgByteReader
  \li  MgDateTimeProperty::GetValue() returns MgDateTime
  \li  MgDoubleProperty::GetValue() returns double
  \li  MgFeatureProperty::GetValue() returns MgFeatureReader
  \li  MgGeometryProperty::GetValue() returns MgByteReader
  \li  MgInt16Property::GetValue() returns Int16 (short)
  \li  MgInt32Property::GetValue() returns Int32 (long)
  \li  MgInt64Property::GetValue() returns Int64 (long long)
  \li  MgRasterProperty::GetValue() returns MgRaster
  \li  MgSingleProperty::GetValue() returns float
  \li  MgStringProperty::GetValue() returns STRING (wstring)
 </p>
 
**/

/**
\addtogroup Feature_Schema_Module
<p>
  A feature schema is a class or set of classes that define the
  structure of data contained in a feature source (datastore),
  for example, an Oracle database. A feature class consists of
  a set of properties. Each property has a set of attributes
  appropriate to the type of information it contains. Class and
  property here correspond roughly to table and column in a
  relational database.
</p>
<p>
  The class and property definition classes have methods for
  both getting and setting attributes. In general, however, the
  set methods are of no use except in the case of the FDO
  Provider for SDF. MgFeatureService::CreateFeatureSource
  permits you to create an SDF schema, that is, an SDF file
  containing a schema. The intent is that this method be used
  as part of a mechanism for caching feature data locally.
  The schema used for this SDF cache file would typically be
  the schema extracted by \link MgFeatureService::DescribeSchema MgFeatureService::DescribeSchema Method \endlink
  from the feature source whose features are being cached.
  It is possible to create a schema from scratch; see the sample code below.
</p>
<p>
  Five property types are distinguished, and there is a
  property definition class for each type. The five types are:
  \li  Simple data. The MgDataPropertyDefinition class is used
  to define the attributes of simple data properties. The
  simple data types are BLOB, boolean, byte, CLOB, MgDateTime,
  double, Int16, Int32, Int64, single, and string.
  \li  Geomtry data. The MgGeometricPropertyDefinition class is
  used to define the attributes of geometry data. The geometry
  data type is MgGeometry.
  \li  Association data. The MgAssociationPropertyDefinition
  class is used to define the attributes of association
  properties. This is not currently used.
  \li  Object data. The MgObjectPropertyDefinition class is used
  to define the attributes of object data. This allows us to
  model information using object composition. The type of
  object data is user-defined.
  \li  Raster data. The MgRasterPropertyDefinition class is used
  to define the attributes of raster data. The type of raster
  data is MgRaster. 
</p>
<!-- Example (C#) -->
\htmlinclude CSharpExampleTop.html
\code
using OSGeo.MapGuide;
MgDataPropertyDefinition dataProperty;
MgGeometricPropertyDefinition geometricProperty;
MgClassDefinition classDef = new MgClassDefinition();

className = "SdfFeatureClass";

classDef.SetName(className);
classDef.SetDefaultGeometryPropertyName("theFeatureGeometry");

// feature geometry property that takes 2D XY geometries
geometricProperty = new MgGeometricPropertyDefinition("theFeatureGeometry");
geometricProperty.SetGeometryTypes(MgFeatureGeometricType.Point |
   MgFeatureGeometricType.Curve | MgFeatureGeometricType.Surface);
geometricProperty.SetHasElevation(false);
geometricProperty.SetHasMeasure(false);
geometricProperty.SetReadOnly(false);
geometricProperty.SetSpatialContextAssociation(spatialContextName);
classDef.GetProperties().Add(geometricProperty);

// non-feature geometry
geometricProperty = new MgGeometricPropertyDefinition("aNonFeatureGeometry");
geometricProperty.SetGeometryTypes(MgFeatureGeometricType.Point |
  MgFeatureGeometricType.Curve | MgFeatureGeometricType.Surface);
geometricProperty.SetHasElevation(false);
geometricProperty.SetHasMeasure(false);
geometricProperty.SetReadOnly(false);
geometricProperty.SetSpatialContextAssociation(spatialContextName);
classDef.GetProperties().Add(geometricProperty);

// identity property
dataProperty = new MgDataPropertyDefinition("FeatId");
dataProperty.SetDataType(MgPropertyType.Int32);
dataProperty.SetAutoGeneration(false);
dataProperty.SetNullable(false);
dataProperty.SetReadOnly(true);
classDef.GetIdentityProperties().Add(dataProperty);
classDef.GetProperties().Add(dataProperty);

// boolean property
dataProperty = new MgDataPropertyDefinition("aBoolean");
dataProperty.SetDataType(MgPropertyType.Boolean);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// byte property
dataProperty = new MgDataPropertyDefinition("aByte");
dataProperty.SetDataType(MgPropertyType.Byte);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// DataTime property
dataProperty = new MgDataPropertyDefinition("aDateTime");
dataProperty.SetDataType(MgPropertyType.DateTime);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// cannot create Decimal property because MgPropertyType.Decimal doesn't exist
// even though SDF schema capabilities says that Decimal property is supported

// double property
dataProperty = new MgDataPropertyDefinition("aDouble");
dataProperty.SetDataType(MgPropertyType.Double);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// Int16 property
dataProperty = new MgDataPropertyDefinition("anInt16");
dataProperty.SetDataType(MgPropertyType.Int16);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// Int32 property
dataProperty = new MgDataPropertyDefinition("anInt32");
dataProperty.SetDataType(MgPropertyType.Int32);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// Int64 property
dataProperty = new MgDataPropertyDefinition("anInt64");
dataProperty.SetDataType(MgPropertyType.Int64);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// Single property
dataProperty = new MgDataPropertyDefinition("aSingle");
dataProperty.SetDataType(MgPropertyType.Single);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

// String property
dataProperty = new MgDataPropertyDefinition("aString");
dataProperty.SetDataType(MgPropertyType.String);
dataProperty.SetNullable(true);
dataProperty.SetReadOnly(false);
classDef.GetProperties().Add(dataProperty);

schema = new MgFeatureSchema();
schemaName = "SdfFeatureClassSchema";
schema.SetName(schemaName);
schema.GetClasses().Add(classDef);

\endcode
\htmlinclude ExampleBottom.html

**/ 

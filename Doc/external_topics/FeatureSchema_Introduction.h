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
**/ 
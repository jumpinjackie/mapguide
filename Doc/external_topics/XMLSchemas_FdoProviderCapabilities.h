/**
\addtogroup FdoProviderCapabilities_schema

<p>
This schema governs the output of
MgFeatureService::GetCapabilities(). What follows is a
representation of the element hierarchy in
FdoProviderCapabilities-1.0.0.xsd. Frequency of element
occurrence is indicated by adding a quantifier symbol (* for
0 or more, + for 1 or more, ? for 0 or 1 and nothing for 1)
to the end of the element identifier.
</p>
<pre>
FeatureProviderCapabilities
   Provider Name
   Connection*
      ThreadCapability is one of SingleThreaded, PerConnectionThreaded, PerCommandThreaded, or MultiThreaded
      SpatialContextExtent
            Type+ is one of Static or Dynamic
      SupportsLocking
      SupportsTimeout
      SupportsTransactions
      SupportLongTransactions
      SupportsSQL
      SupportsConfiguration
   Schema*
      Class
            Type+ is one of Class or FeatureClass
      Data
            Type+ is one of Boolean, Byte, DateTime, Decimal, Double, Int16, Int32, Int64, Single, String, BLOB, CLOB, or UniqueID
      SupportsInheritance
      SupportsMultipleSchemas
      SupportsObjectProperties
      SupportsAssociationProperties
      SupportsSchemaOverrides
      SupportsNetworkModel
   Command*
      SupportedCommands
            Name+ is one of
                Select
                Insert
                Delete
                Update
                DescribeSchema
                ApplySchema
                DestroySchema
                ActivateSpatialContext
                CreateSpatialContext
                DestroySpatialContext
                GetSpatialContexts
                CreateMeasureUnit
                DestroyMeasureUnit
                GetMeasureUnits
                SQLCommand
                AcquireLock
                GetLockInfo
                GetLockedObjects
                GetLockOwners
                ReleaseLock
                ActivateLongTransaction
                CommitLongTransaction
                CreateLongTransaction
                GetLongTransactions
                FreezeLongTransaction
                RollbackLongTransaction
                ActivateLongTransactionCheckpoint
                CreateLongTransactionCheckpoint
                GetLongTransactionCheckpoints
                RollbackLongTransactionCheckpoint
                ChangeLongTransactionPrivileges
                GetLongTransactionPrivileges
                ChangeLongTransactionSet
                GetLongTransactionsInSet
                FirstProviderCommand
                DeactivateLongTransaction
         SupportsParameters
         SupportsTimeout
   Filter*
      Condition*
             Type+ is one of Comparison, Like, In, Null, Spatial, or Distance
      Spatial*
             Operation+ is one of
                Contains
                Crosses
                Disjoint
                Equals
                Intersects
                Overlaps
                Touches
                Within
                CoveredBy
                Inside
                EnvelopeIntersects
      Distance*
             Operation+ is one of Beyond or Within
      SupportsGeodesicDistance
      SupportsNonLinearGeometricOperations
   Expression*
      Type
             Name+ is one of Basic, Function, or Parameter
      FunctionDefinitionList*
            FunctionDefinition+
                Name
                 Description*
                 ReturnType is one of Boolean, Byte, DateTime, Decimal, Double, Int16, Int32, Int64, Single, String, BLOB, CLOB, or UniqueID
                 ArgumentDefinitionList*
                    ArgumentDefinition+
                        Name
                        Description*
                        DataType is one of Boolean, Byte, DateTime, Decimal, Double, Int16, Int32, Int64, Single, String, BLOB, CLOB, or UniqueID
   Raster
      SupportsRaster
      SupportsStitching
      SupportsSubsampling
   Topology*
      SupportsTopology
      SupportsTopologicalHierarchy
      BreaksCurveCrossingsAutomatically
      ActivatesTopologyByArea
      ConstrainsFeatureMovements
    Geometry*
        Type+ is one of
            Point
            MultiPoint
            LineString
            MultiLineString
            Polygon
            MultiPolygon
            CurveString
            MultiCurveString
            CurvePolygon
            MultiCurvePolygon
            MultiGeometry
        Components
            Type+ is one of
                LinearRing
                LinearSegment
                ArcSegment
                CurveRing
        Dimensionality
</pre>

<P>
There are now two versions of the FdoProviderCapabilities schema.
</P>
<P>
Version 1.1.0 was introduced in MapGuide Open Source 2.0.
</P>

<a href="../../xml_schemas/FdoProviderCapabilities/FdoProviderCapabilities.html">FdoProviderCapabilities-1.0.0.xsd -  Details</a><BR>
<a href="../../xml_schemas/FdoProviderCapabilities110/FdoProviderCapabilities.htm">FdoProviderCapabilities-1.1.0.xsd -  Details</a>
**/
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

#ifndef _MGCOORDINATESYSTEMFACTORY_H_
#define _MGCOORDINATESYSTEMFACTORY_H_

/// \defgroup MgCoordinateSystemFactory MgCoordinateSystemFactory
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Supports the creation of MgCoordinateSystem objects based on
/// a OpenGIS Well-Known Text (WKT) definition of the coordinate
/// system. This class is the main entry point for accessing assorted classes
/// and functionalities of the Coordinate System API
///
class MG_GEOMETRY_API MgCoordinateSystemFactory : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemFactory)

PUBLISHED_API:
    ///////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgCoordinateSystemFactory object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgCoordinateSystemFactory();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// $coordSysFactory = new MgCoordinateSystemFactory();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    /// <!-- Example (Java) -->
    /// \htmlinclude JavaExampleTop.html
    /// \code
    /// MgCoordinateSystemFactory csFactory = new MgCoordinateSystemFactory();
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgCoordinateSystemFactory();

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgCoordinateSystem instance given a definition of
    /// the coordinate system in OpenGIS Well-Known Text (WKT)
    /// format. See \link WKTofSRS WKT of Spatial Reference System \endlink
    /// and \link CSTestValues Coordinate System Test Values \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(string wkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(String wkt);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinateSystem Create(string wkt);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param wkt (String/string)
    /// A string defining the coordinate system
    /// in OpenGIS WKT format.
    ///
    /// \return
    /// Returns an MgCoordinateSystem instance that corresponds to
    /// the specified WKT definition.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    ///
    /// $wktProj = 'PROJCS["UTM Zone 18 (NAD 83)",
    ///        GEOGCS ["NAD 83 (Continental US)", DATUM ["NAD 83 (Continental US)",
    ///        SPHEROID ["GRS 80", 6378137, 298.257222101]],
    ///        PRIMEM [ "Greenwich", 0.000000 ],
    ///        UNIT ["Decimal Degree", 0.01745329251994330]],
    ///        PROJECTION ["Transverse Mercator"],
    ///        PARAMETER ["Scale_Factor", 0.999600], PARAMETER ["Central_Meridian", -75.000000],
    ///        PARAMETER ["False_Easting", 500000.000000],
    ///        UNIT ["Meter", 1.000000000000]]';
    ///
    /// $wktGeog = 'GEOGCS [ "Longitude / Latitude (NAD 83)",
    ///        DATUM ["NAD 83", SPHEROID ["GRS 80", 6378137, 298.257222101]],
    ///        PRIMEM [ "Greenwich", 0.000000 ],
    ///        UNIT ["Decimal Degree", 0.01745329251994330]]';
    ///
    /// $coordSysFactory = new MgCoordinateSystemFactory();
    /// $coordSysProj = $coordSysFactory($wktProj);
    /// $coordSysGeog = $coordSysFactory->Create($wktGeog);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemFactory coordSysFactory;
    ///
    /// private MgCoordinateSystem projCS;
    /// private String projCSWkt = "PROJCS[\"UTM83-4\",GEOGCS[\"LL83\",
    ///        DATUM[\"NAD83\",SPHEROID[\"GRS1980\",6378137.000,298.25722210]],
    ///        PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],
    ///        PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",500000.000],
    ///        PARAMETER[\"false_northing\",0.000],PARAMETER[\"central_meridian\",-159.00000000000000],
    ///        PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"latitude_of_origin\",0.000],
    ///        UNIT[\"Meter\",1.00000000000000]]";

    /// private MgCoordinateSystem geogCS;
    /// private String geogCSWkt = "GEOGCS[\"LL83\",DATUM[\"NAD83\",
    ///        SPHEROID[\"GRS1980\",6378137.000,298.25722210]],
    ///        PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";

    /// coordSysFactory = new MgCoordinateSystemFactory();
    /// geogCS = coordSysFactory.Create(geogCSWkt);
    /// projCS = coordSysFactory.Create(projCSWkt);
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgCoordinateSystem* Create(CREFSTRING wkt);

    //factory
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate system catalog
    /// \return
    /// Returns the MgCoordinateSystemCatalog
    virtual MgCoordinateSystemCatalog* GetCatalog();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgCoordinateSystemTransform that can be used to transform
    /// coordinates from the source coordinate system to the target coordinate
    /// system.
    /// \param pSource (MgCoordinateSystem)
    /// The source coordinate system
    /// \param pTarget (MgCoordinateSystem)
    /// The target coordinate system
    /// \return
    /// The coordinate system transform that can be used to transform coordinates
    /// \since 2.0
    virtual MgCoordinateSystemTransform* GetTransform(MgCoordinateSystem* pSource,
                                                      MgCoordinateSystem* pTarget);
                                                      
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a MgCoordinateSystem object from the specified CS-Map coordinate
    /// system code
    /// \param code (String/string)
    /// The CS-Map coordinate system code
    /// \return
    /// The coordinate system
    virtual MgCoordinateSystem* CreateFromCode(CREFSTRING code);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the matching CS-Map coordinate system code based on the specified
    /// coordinate system WKT text. See \link WKTofSRS WKT of Spatial Reference System \endlink
    /// \param wkt (String/string)
    /// The coordinate system WKT string
    /// \return
    /// The CS-Map coordinate system code that corresponds to the specified WKT
    virtual STRING ConvertWktToCoordinateSystemCode(CREFSTRING wkt);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the matching coordinate system WKT based on the specified CS-Map
    /// coordinate system code
    /// \param code (String/string)
    /// The CS-Map coordinate system code
    /// \return
    /// The coordinate system WKT string that corresponds to the specified CS-Map
    /// coordinate system code.\n
    /// See \link WKTofSRS WKT of Spatial Reference System \endlink
    virtual STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING code);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a list of the available coordinate system categories
    /// \return
    /// A MgStringCollection containing the names of the available coordinate 
    /// system categories
    virtual MgStringCollection* EnumerateCategories();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a MgBatchPropertyCollection containing the properties of each
    /// coordinate system under the specified category
    /// \param category (String/string)
    /// The coordinate system category to enumerate
    /// \return
    /// A MgBatchPropertyCollection containing the properties of each coordinate system
    /// under the specified category
    virtual MgBatchPropertyCollection* EnumerateCoordinateSystems(CREFSTRING category);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the matching coordinate system WKT based on the specified EPSG code
    /// \param code
    /// The EPSG code
    /// \return
    /// The coordinate system WKT string that corresponds to the specified EPSG code
    /// See \link WKTofSRS WKT of Spatial Reference System \endlink
    virtual STRING ConvertEpsgCodeToWkt(INT32 code);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the matching EPSG code based on the specified coordinate system WKT
    /// \param wkt (String/string)
    /// The coordinate system WKT string. See \link WKTofSRS WKT of Spatial Reference System \endlink
    /// \return
    /// The EPSG code that corresponds to the specified coordinate system WKT string
    virtual INT32 ConvertWktToEpsgCode(CREFSTRING wkt);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets name of the underlying coordinate system library
    /// \return
    /// Returns the name of the underlying coordinate system library
    virtual STRING GetBaseLibrary();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the specified coordinate system WKT string is a valid one
    /// \param wkt (String/string)
    /// The coordinate system WKT string. See \link WKTofSRS WKT of Spatial Reference System \endlink
    /// \return
    /// Returns <b>true</b> if the specified WKT is valid. Returns <b>false</b> otherwise
    virtual bool IsValid(CREFSTRING wkt);

    // Grids and Graticules -- General

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a grid boundary object.
    /// \param southwest
    /// The coordinates of the the southwest corner of a rectangular region
    /// which represents the grid region.  This point <b>MUST</b> indeed be
    /// southwest of the coordinate provided by the <c>northeast</c> parameter.
    /// \param northeast
    /// The coordinates of the the northeast corner of a rectangular region
    /// which represents the grid region.  This point <b>MUST</b> indeed be
    /// northeast of the coordinate provided by the <c>southwest</c> parameter.
    /// \return
    /// Returns the grid boundary in the form used by the grid/graticule
    /// sub-system.
    /// \exception MgOutOfMemoryException
    /// Thrown in the event of heap memory allocation failure.
    /// \remarks
    /// Externally, grid bundary objects are in viewport coordinates and
    /// define the extents of the region within which a grid is to be drawn.
    /// Such objects are often simple rectangles, but his is not a
    /// requirement.
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgCoordinate* southwest,
                                                         MgCoordinate* northeast);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a grid boundary object.  Externally, grid objects are in
    /// viewport coordinates and define the extents of the region within
    /// which a grid is to be drawn.  Such objects are often simple
    /// rectangles, but his is not a requirement.
    /// \param boundary
    /// The grid boundary in the form of a MgPolygon with no interior rings.
    /// \return
    /// Returns the boundary in the form used by the grid/graticule sub-system.
    /// \exception MgOutOfMemoryException
    /// Thrown in the event of heap memory allocation failure.
    /// \remarks
    /// The boundary argument need not be a rectangle, but is assumed
    /// to be closed.
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgPolygon* boundary);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a grid specification object with all specifications set to
    /// <b>invalid</b> values.
    /// \return
    /// An object which carries all of the several parameter which determine
    /// nature of a grid or graticule and any sub-feature thereof.
    /// \exception std::bad_alloc
    /// Thrown in the event of a heap memory allocation failure.
    /// \remarks
    /// When manufactured by this overload, the consuming application must
    /// set all values to valid values before using the returned object
    /// in any way.
    virtual MgCoordinateSystemGridSpecification* GridSpecification (void);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a grid specification object with all specifications set to
    /// the values indicated by the supplied parameters.
    /// \param increment
    /// The distance between grid lines of a grid or graticule.  This value is
    /// used for both the easting and northing grid lines; and the value must
    /// be in the units specified by the <c>unitCode</c> parameter.
    /// \param tickIncrement
    /// The distance between tick marks of a grid or graticule.  This value is
    /// used for both the easting and northing tick marks; and the value must
    /// be in the units specified by the <c>unitCode</c> parameter.
    /// \param unitCode
    /// One of the values defined by the MgCoordinateSystemUnitCode object
    /// which indicates the units in which the <c>increment</c>,
    /// <c>tickIncrement</c>, and <c>curvePrecision</c> parameters are
    /// specified.
    /// \param curvePrecision
    /// In the units indicated by the <c>unitCode</c> parameter, the
    /// <b>desired</b> maximum distance between the true complex curve and
    /// the multi-segment approximation of the complex curve of any grid or
    /// graticule line generated using this specification.
    /// \return
    /// A fully initialized grid specification object.
    /// \exception std::bad_alloc
    /// Thrown in the event of a heap memory allocation failure.
    /// \remarks
    /// This particular overload is designed for the most common situations.
    /// It presumes the units used are of thelinear type, and thus this
    /// particular overload cannot be used to generate a specification
    /// object for use in graticule generation.
    virtual MgCoordinateSystemGridSpecification* GridSpecification (double increment,
                                                                    double tickIncrement,
                                                                    INT32 unitCode,
                                                                    double curvePrecision);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a grid specification object with all specifications set to
    /// the values indicated by the supplied parameters.
    /// \param gridType
    /// A value defined by the MgCoordinateSystemGridSpecializationType object
    /// which indicates the Type of specialized grid desired.
    /// \param gridLevel
    /// A value which indicates the density of the grid desired.  The exact
    /// value of this parameter is dependent upon the value of the
    /// <c>gridType</c> argument.  See Remarks below.
    /// \return
    /// Returns a fully initialized grid specification structure.
    /// \exception std::bad_alloc
    /// Thrown in the event of a heap memory allocation failure.
    /// \remarks
    /// The value of the <c>gridLevel</c> parameter is depenent upon the
    /// <c>gridType</c> argument.  For each type of grid, there exists
    /// a different set legal values for the <c>gridLevel</c> parameter.
    /// For example, the most common usgae would have the <c>gridType</c>
    /// parameter set to MgCoordinateSystemGridSpecializationType::MGRS
    /// and therefore the value of the <c>gridLevel</c> argument would
    /// need to be a value defined by the MgCoordinateSystemMgrsGridLevel
    /// object.
    virtual MgCoordinateSystemGridSpecification* GridSpecification (INT32 gridType,
                                                                    INT32 gridLevel);

    // Grids and Graticules -- Generic
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures generic grid object.  A generic grid is one which simply
    /// draws isolines of one coordinate system (the Grid coordinate system)
    /// into the drawing space of another (the frame system).
    /// \param sGridCs
    /// The coordinate system code name of the grid coordinate system.
    /// \param sFrameCs
    /// The coordinate system code name of the frame coordinate system.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on all
    /// exceptional conditions.
    /// \return
    /// Returns a disposable pointer to the abstract interface class from which the
    /// generic grid object (and all other grid objects) derive.
    /// object derive.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    /// \remarks
    /// Refer to MgCoordinateSystemFactory::CreateFromCode for
    /// additional exception information.
    virtual MgCoordinateSystemGridBase* GenericGrid (CREFSTRING sGridCs,
                                                     CREFSTRING sFrameCs,
                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures generic grid object.  A generic grid is one which simply
    /// draws isolines of one coordinate system (the Grid coordinate system)
    /// into the drawing space of another (the frame system).
    /// \param pGridCs
    /// The grid coordinate system.
    /// \param pFrameCs
    /// The frame coordinate system.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on all
    /// exceptional conditions.
    /// \return
    /// Returns a disposable pointer to the abstract interface class from which the
    /// generic grid object (and all other grid objects) derive.
    /// object derive.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    /// \remarks
    /// Refer to MgCoordinateSystemFactory::CreateFromCode for
    /// additional exception information.
    virtual MgCoordinateSystemGridBase* GenericGrid (MgCoordinateSystem* pGridCs,
                                                     MgCoordinateSystem* pFrameCs,
                                                     bool bSetExceptionsOn);

    // Grids and Graticules -- MGRS
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// \param dEquatorialRadius
    /// Equatorial radius of the ellipsod upon which conversions to and from
    /// MGRS strings are to be based.
    /// \param dEccentricity
    /// Eccentricity of the ellipsod upon which conversions to and from
    /// MGRS strings are to be based.
    /// \param nLetteringScheme
    /// A value as defined in the MgCoordinateSystemMgrsLetteringScheme
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// \return
    /// Returns a disposable pointer to an MgCoordinateSystemMgrs
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    virtual MgCoordinateSystemMgrs* GetMgrs(double dEquatorialRadius,double dEccentricity,
                                                                     INT8 nLetteringScheme,
                                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// \param sEllipsoidCode
    /// Dictionary code name of the ellipsoid upon which conversions to and from
    /// MGRS strings are to be based.
    /// \param nLetteringScheme
    /// A value as defined in the MgCoordinateSystemMgrsLetteringScheme
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// \return
    /// Returns a disposable pointer to an MgCoordinateSystemMgrs
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    virtual MgCoordinateSystemMgrs* GetMgrsEllipsoid(CREFSTRING sEllipsoidCode,
                                                     INT8 nLetteringScheme,
                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// \param sDatumCode
    /// Dictionary code name of the datum whose referenced ellipsoid is to be
    /// used for all conversions to and from MGRS strings.
    /// \param nLetteringScheme
    /// A value as defined in the MgCoordinateSystemMgrsLetteringScheme
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// \return
    /// Returns a disposable pointer to an MgCoordinateSystemMgrs
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    /// \remarks
    /// The datum specification is simply a means of specifying the ellipsoid
    /// upon which the calculations are to be based.  It does <b>not</b> mean
    /// that any datum shift calculations will be performed.
    virtual MgCoordinateSystemMgrs* GetMgrsDatum(CREFSTRING sDatumCode,
                                                 INT8 nLetteringScheme,
                                                 bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures an MGRS grid object which is suitable for generating an
    /// actual MGRS grid.
    /// \param pFrameCs
    /// The frame coordinate system.
    /// \param bUseFrameDatum
    /// <c>true</c> causes the frame datum to be used for all geodetic calculations
    /// neccessary to generate the grid; otherwise WGS84 is assumed.
    /// \param nLetteringScheme
    /// A value as defined in the MgCoordinateSystemMgrsLetteringScheme
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason. This applies only to MGRS string
    /// conversions.
    /// \return
    /// Returns a disposable pointer to the abstract interface class from which the
    /// MGRS grid object (and all other grid objects) derive.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    /// \remarks
    /// Refer to MgCoordinateSystemFactory::CreateFromCode for
    /// additional exception information.\n
    /// Coordinate system transformation exceptions are always suppressed during
    /// grid generation.
    virtual MgCoordinateSystemGridBase* MgrsGrid (MgCoordinateSystem* pFrameCs,
                                                  bool bUseFrameDatum,
                                                  INT8 nLetteringScheme,
                                                  bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Manufactures an MGRS grid object which is suitable for generating an
    /// actual MGRS grid.
    /// \param sFrameCs
    /// The dictionary code name of the frame coordinate system.
    /// \param bUseFrameDatum
    /// <c>true</c> causes the frame datum to be used for all geodetic calculations
    /// neccessary to generate the grid; otherwise WGS84 is assumed.
    /// \param nLetteringScheme
    /// A value as defined in the MgCoordinateSystemMgrsLetteringScheme
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// \param bSetExceptionsOn
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason. This applies only to MGRS string
    /// conversions.
    /// \return
    /// Returns a disposable pointer to the abstract interface class from which the
    /// MGRS grid object (and all other grid objects) derive.
    /// \exception MgOutOfMemoryException
    /// Thrown on heap memory allocation failure.
    /// \remarks
    /// Refer to MgCoordinateSystemFactory::CreateFromCode for
    /// additional exception information.\n
    /// Coordinate system transformation exceptions are always suppressed during
    /// grid generation.
    virtual MgCoordinateSystemGridBase* MgrsGrid (CREFSTRING sFrameCs,
                                                  bool bUseFrameDatum,
                                                  INT8 nLetteringScheme,
                                                  bool bSetExceptionsOn);

INTERNAL_API:
    virtual ~MgCoordinateSystemFactory();

    static void DeleteCatalog();

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return m_cls_id;};

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

/// Data Members
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemFactory;

private:
    static MgCoordinateSystemCatalog* sm_pCatalog;
};
/// \}

#endif //_MGCOORDINATESYSTEMFACTORY_H_

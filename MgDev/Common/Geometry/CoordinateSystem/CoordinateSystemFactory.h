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

#ifndef _MGCOORDINATESYSTEMFACTORY_H_
#define _MGCOORDINATESYSTEMFACTORY_H_

/// \defgroup MgCoordinateSystemFactory MgCoordinateSystemFactory
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Supports the creation of MgCoordinateSystem objects based on
/// a OpenGIS Well-Known Text (WKT) definition of the coordinate
/// system.
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
    virtual MgCoordinateSystemCatalog* GetCatalog();
    virtual MgCoordinateSystemTransform* GetTransform(MgCoordinateSystem* pSource,
                                                      MgCoordinateSystem* pTarget);
    virtual MgCoordinateSystem* CreateFromCode(CREFSTRING code);
    virtual STRING ConvertWktToCoordinateSystemCode(CREFSTRING wkt);
    virtual STRING ConvertCoordinateSystemCodeToWkt(CREFSTRING code);
    virtual MgStringCollection* EnumerateCategories();
    virtual MgBatchPropertyCollection* EnumerateCoordinateSystems(CREFSTRING category);
    virtual STRING ConvertEpsgCodeToWkt(INT32 code);
    virtual INT32 ConvertWktToEpsgCode(CREFSTRING wkt);
    virtual STRING GetBaseLibrary();
    virtual bool IsValid(CREFSTRING wkt);

    // Grids and Graticules -- General

    ///////////////////////////////////////////////////////////////////////////
    /// /brief
    /// Constructs a grid boundary object.
    /// /param southwest
    /// The coordinates of the the southwest corner of a rectangular region
    /// which represents the grid region.  This point <b>MUST</b> indeed be
    /// southwest of the coordinate provided by the <c>northeast</c> parameter.
    /// /param northeast
    /// The coordinates of the the northeast corner of a rectangular region
    /// which represents the grid region.  This point <b>MUST</b> indeed be
    /// northeast of the coordinate provided by the <c>southwest</c> parameter.
    /// /return
    /// Returns the grid boundary in the ofrm used by the grid/graticule
    /// sub-system.
    /// \exception MgOutOfMemoryException
    /// Thrown in the event of heap memory allocation failure.
    /// /remarks
    /// Externally, grid bundary objects are in viewport coordinates and
    /// define the extents of the region within which a grid is to be drawn.
    /// Such objects are often simple rectangles, but his is not a
    /// requirement.
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgCoordinate* southwest,
                                                         MgCoordinate* northeast);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs a grid boundary object.  Externally, grid objects are in
    /// viewport coordinates and define the extents of the region within
    /// which a grid is to be drawn.  Such objects are often simple
    /// rectangles, but his is not a requirement.
    /// </summary>
    /// <param name="boundary">
    /// The grid boundary in the form of a MgPolygon with no interior rings.
    /// </param>
    /// <returns>
    /// Returns the boundary in the form used by the grid/graticule sub-system.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown in the event of heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// The <c>boundary</c> argument need not be a rectangle, but is assumed
    /// to be closed.
    /// </remarks>
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgPolygon* boundary);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs a grid specification object with all specifications set to
    /// <b>invalid</b> values.
    /// </summary>
    /// <returns>
    /// An object which carries all of the several parameter which determine
    /// nature of a grid or graticule and any sub-feature thereof.
    /// </returns>
    /// <exception cref="std::bad_alloc">
    /// Thrown in the event of a heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// When manufactured by this overload, the consuming application must
    /// set all values to valid values before using the returned object
    /// in any way.
    /// </remarks>
    virtual MgCoordinateSystemGridSpecification* GridSpecification (void);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs a grid specification object with all specifications set to
    /// the values indicated by the supplied parameters.
    /// </summary>
    /// <param name="increment">
    /// The distance between grid lines of a grid or graticule.  This value is
    /// used for both the easting and northing grid lines; and the value must
    /// be in the units specified by the <c>unitCode</c> parameter.
    /// </param>
    /// <param name="tickIncrement">
    /// The distance between tick marks of a grid or graticule.  This value is
    /// used for both the easting and northing tick marks; and the value must
    /// be in the units specified by the <c>unitCode</c> parameter.
    /// </param>
    /// <param name="unitCode">
    /// One of the values defined by the MgCoordinateSystemUnitCode object
    /// which indicates the units in which the <c>increment</c>,
    /// <c>tickIncrement</c>, and <c>curvePrecision</c> parameters are
    /// specified.
    /// </param>
    /// <param name="curvePrecision">
    /// In the units indicated by the <c>unitCode</c> parameter, the
    /// <b>desired</b> maximum distance between the true complex curve and
    /// the multi-segment approximation of the complex curve of any grid or
    /// graticule line generated using this specification.
    /// </param>
    /// <returns>
    /// A fully initialized grid specification object.
    /// </returns>
    /// <exception cref="std::bad_alloc">
    /// Thrown in the event of a heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// This particular overload is designed for the most common situations.
    /// It presumes the units used are of thelinear type, and thus this
    /// particular overload cannot be used to generate a specification
    /// object for use in graticule generation.
    /// </remarks>
    virtual MgCoordinateSystemGridSpecification* GridSpecification (double increment,
                                                                    double tickIncrement,
                                                                    INT32 unitCode,
                                                                    double curvePrecision);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs a grid specification object with all specifications set to
    /// the values indicated by the supplied parameters.
    /// </summary>
    /// <param name="gridType">
    /// A value defined by the MgCoordinateSystemGridSpecializationType object
    /// which indicates the Type of specialized grid desired.
    /// </param>
    /// <param name="gridLevel">
    /// A value which indicates the density of the grid desired.  The exact
    /// value of this parameter is dependent upon the value of the
    /// <c>gridType</c> argument.  See Remarks below.
    /// </param>
    /// <returns>
    /// Returns a fully initialized grid specification structure.
    /// </returns>
    /// <exception cref="std::bad_alloc">
    /// Thrown in the event of a heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// The value of the <c>gridLevel</c> parameter is depenent upon the
    /// <c>gridType</c> argument.  For each type of grid, there exists
    /// a different set legal values for the <c>gridLevel</c> parameter.
    /// For example, the most common usgae would have the <c>gridType</c>
    /// parameter set to MgCoordinateSystemGridSpecializationType::MGRS
    /// and therefore the value of the <c>gridLevel</c> argument would
    /// need to be a value defined by the MgCoordinateSystemMgrsGridLevel
    /// object.
    /// </remarks>
    virtual MgCoordinateSystemGridSpecification* GridSpecification (INT32 gridType,
                                                                    INT32 gridLevel);
                                                                   
    // Grids and Graticules -- Generic
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures generic grid object.  A generic grid is one which simply
    /// draws isolines of one coordinate system (the Grid coordinate system)
    /// into the drawing space of another (the frame system).
    /// </summary>
    /// <param name="sGridCs">
    /// The coordinate system code name of the grid coordinate system.
    /// </param>
    /// <param name="sFrameCs">
    /// The coordinate system code name of the frame coordinate system.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on all
    /// exceptional conditions.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to the abstract interface class from which the
    /// generic grid object (and all other grid objects) derive.
    /// object derive.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// Refer to <see cref="MgCoordinateSystemFactory::CreateFromCode"> for
    /// additional exception information.
    /// </remarks>
    virtual MgCoordinateSystemGridBase* GenericGrid (CREFSTRING sGridCs,
                                                     CREFSTRING sFrameCs,
                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures generic grid object.  A generic grid is one which simply
    /// draws isolines of one coordinate system (the Grid coordinate system)
    /// into the drawing space of another (the frame system).
    /// </summary>
    /// <param name="pGridCs">
    /// The grid coordinate system.
    /// </param>
    /// <param name="pFrameCs">
    /// The frame coordinate system.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on all
    /// exceptional conditions.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to the abstract interface class from which the
    /// generic grid object (and all other grid objects) derive.
    /// object derive.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// Refer to <see cref="MgCoordinateSystemFactory::CreateFromCode"> for
    /// additional exception information.
    /// </remarks>
    virtual MgCoordinateSystemGridBase* GenericGrid (MgCoordinateSystem* pGridCs,
                                                     MgCoordinateSystem* pFrameCs,
                                                     bool bSetExceptionsOn);

    // Grids and Graticules -- MGRS
    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// </summary>
    /// <param name="dEquatorialRadius">
    /// Equatorial radius of the ellipsod upon which conversions to and from
    /// MGRS strings are to be based.
    /// </param>
    /// <param name="dEccentricy">
    /// Eccentricity of the ellipsod upon which conversions to and from
    /// MGRS strings are to be based.
    /// </param>
    /// <param name="nLetteringScheme">
    /// A value as defined in the <c>MgCoordinateSystemMgrsLetteringScheme</c>
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// </param>
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to an <c>MgCoordinateSystemMgrs</c>
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    virtual MgCoordinateSystemMgrs* GetMgrs(double dEquatorialRadius,double dEccentricity,
                                                                     INT8 nLetteringScheme,
                                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// </summary>
    /// <param name="sEllispoidCode">
    /// Dictionary code name of the ellipsoid upon which conversions to and from
    /// MGRS strings are to be based.
    /// </param>
    /// <param name="nLetteringScheme">
    /// A value as defined in the <c>MgCoordinateSystemMgrsLetteringScheme</c>
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to an <c>MgCoordinateSystemMgrs</c>
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    virtual MgCoordinateSystemMgrs* GetMgrsEllipsoid(CREFSTRING sEllipsoidCode,
                                                     INT8 nLetteringScheme,
                                                     bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures an MGRS grid object.  The returned object is suitable only
    /// for use in simple coordinate <-> MGRS string calculations.
    /// </summary>
    /// <param name="sDatumCode">
    /// Dictionary code name of the datum whose referenced ellipsoid is to be
    /// used for all conversions to and from MGRS strings.
    /// </param>
    /// <param name="nLetteringScheme">
    /// A value as defined in the <c>MgCoordinateSystemMgrsLetteringScheme</c>
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to an <c>MgCoordinateSystemMgrs</c>
    /// object which is suitable <b>only</b> for MGRS string conversions.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// The datum specification is simply a means of specifying the ellipsoid
    /// upon which the calculations are to be based.  It does <b>not</b> mean
    /// that any datum shift calculations will be performed.
    /// </remarks>
    virtual MgCoordinateSystemMgrs* GetMgrsDatum(CREFSTRING sDatumCode,
                                                 INT8 nLetteringScheme,
                                                 bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures an MGRS grid object which is suitable for generating an
    /// actual MGRS grid.
    /// </summary>
    /// <param name="pFrameCs">
    /// The frame coordinate system.
    /// </param>
    /// <param name="bUseFrameDatum">
    /// <c>true</c> causes the frame datum to be used for all geodetic calculations
    /// neccessary to generate the grid; otherwise WGS84 is assumed.
    /// <param name="nLetteringScheme">
    /// A value as defined in the <c>MgCoordinateSystemMgrsLetteringScheme</c>
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason. This applies only to MGRS string
    /// conversions.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to the abstract interface class from which the
    /// MGRS grid object (and all other grid objects) derive.
    /// object derive.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// Refer to <see cref="MgCoordinateSystemFactory::CreateFromCode"> for
    /// additional exception information.<para>
    /// Coordinate system transformation exceptions are always suppressed during
    /// grid generation.
    /// </remarks>
    virtual MgCoordinateSystemGridBase* MgrsGrid (MgCoordinateSystem* pFrameCs,
                                                  bool bUseFrameDatum,
                                                  INT8 nLetteringScheme,
                                                  bool bSetExceptionsOn);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Manufactures an MGRS grid object which is suitable for generating an
    /// actual MGRS grid.
    /// </summary>
    /// <param name="sFrameCs">
    /// The dictionary code name of the frame coordinate system.
    /// </param>
    /// <param name="bUseFrameDatum">
    /// <c>true</c> causes the frame datum to be used for all geodetic calculations
    /// neccessary to generate the grid; otherwise WGS84 is assumed.
    /// <param name="nLetteringScheme">
    /// A value as defined in the <c>MgCoordinateSystemMgrsLetteringScheme</c>
    /// object which specifies the lettering scheme to be used on MGRS
    /// string conversions.
    /// </param>
    /// <param name="bSetExceptionsOn">
    /// <c>true</c> indicates that exceptions are to be thrown on coordinate
    /// conversions which fail for any reason. This applies only to MGRS string
    /// conversions.
    /// </param>
    /// <returns>
    /// Returns a disposable pointer to the abstract interface class from which the
    /// MGRS grid object (and all other grid objects) derive.
    /// object derive.
    /// </returns>
    /// <exception cref="MgOutOfMemoryException">
    /// Thrown on heap memory allocation failure.
    /// </exception>
    /// <remarks>
    /// Refer to <see cref="MgCoordinateSystemFactory::CreateFromCode"> for
    /// additional exception information.<para>
    /// Coordinate system transformation exceptions are always suppressed during
    /// grid generation.
    /// </remarks>
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

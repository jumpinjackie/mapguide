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
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgCoordinate* southwest,
                                                         MgCoordinate* northeast);
    virtual MgCoordinateSystemGridBoundary* GridBoundary(MgPolygon* boundary);

    virtual MgCoordinateSystemGridSpecification* GridSpecification (void);
    virtual MgCoordinateSystemGridSpecification* GridSpecification (double increment,
                                                                    INT32 subdivisions,
                                                                    INT32 unitCode,
                                                                    double curvePrecision);
    virtual MgCoordinateSystemGridSpecification* GridSpecification (INT32 gridType,
                                                                    INT32 gridLevel);
                                                                   
    // Grids and Graticules -- Generic
    virtual MgCoordinateSystemGridBase* GenericGrid (CREFSTRING sGridCs,
                                                     CREFSTRING sFrameCs,
                                                     bool bSetExceptionsOn);
    virtual MgCoordinateSystemGridBase* GenericGrid (MgCoordinateSystem* pGridCs,
                                                     MgCoordinateSystem* pFrameCs,
                                                     bool bSetExceptionsOn);

    // Grids and Graticules -- MGRS
    virtual MgCoordinateSystemMgrs* GetMgrs(double dEquatorialRadius,double dEccentricity,
                                                                     INT32 nLetteringScheme,
                                                                     bool bSetExceptionsOn);
    virtual MgCoordinateSystemMgrs* GetMgrsEllipsoid(CREFSTRING sEllipsoidCode,
                                                     INT8 nLetteringScheme,
                                                     bool bSetExceptionsOn);
    virtual MgCoordinateSystemMgrs* GetMgrsDatum(CREFSTRING sDatumCode,
                                                 INT8 nLetteringScheme,
                                                 bool bSetExceptionsOn);
    virtual MgCoordinateSystemGridBase* MgrsGrid (MgCoordinateSystem* pFrameCs,
                                                  bool bUseTargetDatum,
                                                  INT8 nLetteringScheme,
                                                  bool bSetExceptionsOn);
    virtual MgCoordinateSystemGridBase* MgrsGrid (CREFSTRING sFrameCs,
                                                  bool bUseTargetDatum,
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

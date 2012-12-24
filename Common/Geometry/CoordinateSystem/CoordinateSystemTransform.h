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

#ifndef _MGCOORDINATESYSTEMTRANSFORM_H_
#define _MGCOORDINATESYSTEMTRANSFORM_H_


/// \defgroup MgCoordinateSystemTransform MgCoordinateSystemTransform
/// \ingroup Coordinate_System_classes
/// \{

/////////////////////////////////////////////////////////////
/// \brief
/// Provides operations for transforming coordinates from one
/// coordinate system to another.
///
class MgCoordinateSystemTransform : public MgTransform
{
    DECLARE_CLASSNAME(MgCoordinateSystemTransform)

PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the given X and Y values whose frame of reference
    /// is the source coordinate system into an XY coordinate whose
    /// frame of reference is the target coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    ///
    /// \return
    /// Returns an MgCoordinate object which is the equivalent in
    /// the target coordinate system of the given X and Y ordinates.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See the code example in \link MgCoordinateSystemTransform::Transform MgCoordinateSystemTransform::Transform Method (MgCoordinate*) \endlink
    /// and make the appropriate substitutions.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgCoordinate* Transform(double x, double y)=0;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the given X, Y, and M values whose frame of
    /// reference is the source coordinate system into a coordinate
    /// whose frame of reference is the target coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double m);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    /// \param m (double)
    /// The measure to transform.
    ///
    /// \return
    /// Returns an MgCoordinate object which is the equivalent in
    /// the target coordinate system of the given X, Y, and M values.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See the example code in \link MgCoordinateSystemTransform::Transform MgCoordinateSystemTransform::Transform Method (MgCoordinate*) \endlink
    /// and make the appropriate substitutions.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgCoordinate* TransformM(double x, double y, double m)=0;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the given X, Y and Z values whose frame of
    /// reference is the source coordinate system into a coordinate
    /// whose frame of reference is the target coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate Transform(double x, double y, double z);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    /// \param z (double)
    /// The Z ordinate to transform.
    ///
    /// \return
    /// Returns an MgCoordinate object which is the equivalent in
    /// the target coordinate system of the given X, Y, and Z
    /// ordinates.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See the example code in \link MgCoordinateSystemTransform::Transform MgCoordinateSystemTransform::Transform Method (MgCoordinate*) \endlink
    /// and make the appropriate substitutions.
    ///
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgCoordinate* Transform(double x, double y, double z)=0;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the given X, Y, Z, and M values whose frame of
    /// reference is the source coordinate system into a coordinate
    /// whose frame of reference is the target coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate TransformM(double x, double y, double z, double m);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param x (double)
    /// The X ordinate to transform.
    /// \param y (double)
    /// The Y ordinate to transform.
    /// \param z (double)
    /// The Z ordinate to transform.
    /// \param m (double)
    /// The measure to transform.
    ///
    /// \return
    /// Returns an MgCoordinate object which is the equivalent in
    /// the target coordinate system of the given X, Y, Z, and M
    /// values.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See the example code in \link MgCoordinateSystemTransform::Transform MgCoordinateSystemTransform::Transform Method (MgCoordinate*) \endlink
    /// and make the appropriate substitutions.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgCoordinate* TransformM(double x, double y, double z, double m)=0;

    ///////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the given coordinate whose frame of reference is
    /// the source coordinate system into a coordinate whose frame of
    /// reference is the target coordinate system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgCoordinate Transform(MgCoordinate coordinate);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param coordinate (MgCoordinate)
    /// An MgCoordinate object defined in terms
    /// of the source coordinate system.
    ///
    /// \return
    /// Returns an MgCoordinate object which is the equivalent in
    /// the target coordinate system of the given coordinate.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// The name of $coordSys1 is "Longitude / Latitude (NAD 83)".
    /// The name of $coordSys2 is "UTM Zone 18 (NAD 83)".
    /// $csTransform12 transforms coordinates defined in terms of
    /// $coordSys1 to coordinates defined in terms of $coordSys2.
    /// $csTransform21 does the reverse.
    ///
    /// The MgCoordinate object can contain an XY, XYM, XYZ, or XYZM
    /// coordinate. This example uses an XY coordinate.
    ///
    /// The value of $x1Arg is -71.061342, and the value of $y1Arg is
    /// 42.355892. These values are the longitude and latitude of
    /// Boston, Massachusetts.
    ///
    /// The output is:
    ///
    /// $xArg: -71.061342; $x21: -71.061341999861
    ///
    /// $yArg: 42.355892; $y21: 42.355891999573
    ///
    /// $x12: 824390.29381426; $y12 : 4696809.9055963
    /// \code
    /// $csTransform12 = new MgCoordinateSystemTransform($coordSys1, $coordSys2);
    /// $csTransform21 = new MgCoordinateSystemTransform($coordSys2, $coordSys1);
    /// $coordinateArg = $geometryFactory->CreateCoordinateXY($xArg, $yArg);
    /// $coordinate12 = $csTransform12->Transform($coordinateArg);
    /// $x12 = $coordinate12->GetX();
    /// $y12 = $coordinate12->GetY();
    /// $coordinate21 = $csTransform21->Transform($coordinate12);
    /// $x21 = $coordinate21->GetX();
    /// $y21 = $coordinate21->GetY();
    /// echo "$xArg: $xArg; $x21: $x21n";
    /// echo "$yArg: $yArg; $y21: $y21n";
    /// echo "$x12: $x12; $y12 : $y12n";
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemTransform coordSysTransformGeogToProj;
    /// private MgCoordinateSystemTransform coordSysTransformProjToGeog;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private double projCSX = 386323.97632;
    /// private double projCSY = 2435829.67936;
    /// private MgCoordinate XY;
    /// Boolean isEquivalent;
    /// private double tolerance = 0.001;
    ///
    /// XY = coordSysTransformGeogToProj.Transform(geogCSX, geogCSY);
    /// the X and Y values of XY are equal to projCSX and projCSY
    /// isEquivalent = Math.Abs(projCSX - XY.GetX()) < tolerance && Math.Abs(projCSY - XY.GetY()) < tolerance;
    ///
    /// XY = coordSysTransformProjToGeog.Transform(projCSX, projCSY);
    /// the X and Y values of XY are equal to geogCSX and geogCSY
    /// isEquivalent = Math.Abs(geogCSX - XY.GetX()) < tolerance && Math.Abs(geogCSY - XY.GetY()) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgCoordinate* Transform(MgCoordinate* coordinate)=0;

    //////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms an MgEnvelope instance for the source coordinate
    /// system into an MgEnvelope instance for target coordinate
    /// system.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual MgEnvelope Transform(MgEnvelope envelope);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param envelope (MgEnvelope)
    /// An MgEnvelope object obtained from the
    /// MgCoordinateSystemMeasure for the source
    /// coordinate system.
    ///
    /// \return
    /// Returns an MgEnvelope object for the target coordinate
    /// system..
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// The result of executing this code is that $env1 is the same
    /// as $env21 and $env2 is the same as $env12.
    /// \code
    /// $coordSys1 = $coordSysFactory->Create($wkt1);
    /// $coordSys2 = $coordSysFactory->Create($wkt2);
    /// $csTransform12 = new MgCoordinateSystemTransform($coordSys1, $coordSys2);
    /// $csTransform21 = new MgCoordinateSystemTransform($coordSys2, $coordSys1);
    /// $coordSysMeasure1 = new MgCoordinateSystemMeasure($coordSys1);
    /// $coordSysMeasure2 = new MgCoordinateSystemMeasure($coordSys2);
    /// $env1 = $coordSysMeasure1->GetEnvelope();
    /// $env2 = $coordSysMeasure2->GetEnvelope();
    /// $env12 = $csTransform12->Transform($env1);
    /// $env21 = $csTransform21->Transform($env2);
    ///
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// This code creates a geography coordinate system envelope that is
    /// approximately 30 km on the diagonal and transforms that envelope
    /// into its equivalent in a projected coordinate system.
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgCoordinateSystemTransform coordSysTransformGeogToProj;
    /// private MgCoordinateSystemTransform coordSysTransformProjToGeog;
    /// private double geogCSX = -160.101421317;
    /// private double geogCSY = 22.0234263273;
    /// private MgEnvelope geogCSEnv;
    /// private MgEnvelope projCSEnv;
    /// private double urX = geogCSX + 0.2;
    /// private double urY = geogCSY + 0.2;
    /// double expectedProjLLX = 386323.976322775;
    /// double expectedProjLLY = 2435694.36972722;
    /// double expectedProjURX = 407098.230068439;
    /// double expectedProjURY = 2457970.15998103;
    /// double tolerance = 0.001;
    /// Boolean lowerLeftIsEquivalent;
    /// Boolean upperRightIsEquivalent;
    ///
    /// geogCSEnv = new MgEnvelope(geogCSX, geogCSY, urX, urY);
    /// projCSEnv = coordSysTransformGeogToProj.Transform(geogCSEnv);
    /// lowerLeftIsEquivalent = Math.Abs(expectedProjLLX - projCSEnv.GetLowerLeftCoordinate().GetX()) < tolerance && Math.Abs(expectedProjLLY - projCSEnv.GetLowerLeftCoordinate().GetY()) < tolerance;
    /// upperRightIsEquivalent = Math.Abs(expectedProjURX - projCSEnv.GetUpperRightCoordinate().GetX()) < tolerance && Math.Abs(expectedProjURY - projCSEnv.GetUpperRightCoordinate().GetY()) < tolerance;
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    /// \exception MgCoordinateSystemTransformFailedException
    ///
    virtual MgEnvelope* Transform(MgEnvelope* envelope)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the specified coordinate in-place.
    ///
    /// \param coordinate (MgCoordinate)
    /// The coordinate that will be transformed in-place
    ///
    virtual void TransformCoordinate(MgCoordinate* coordinate)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Generates a MgLineString which represents, in the target coordinate
    /// system, the linear line segment provided in the source coordinate
    /// system.
    ///
    /// \param fromPnt
    /// The starting point of the linear segment in source system coordinates.
    /// \param toPnt
    /// The end point of the linear segment in source system coordinates.
    /// \param curvePrecision
    /// The returned line string is segmented to the degree necessary such
    /// that the distance between the LineString approximation of the true
    /// curve and the true curve istself is never more than this value.  Units
    /// must be the same as the target coordinate system.
    /// \param maxPoints
    /// The generation algorithm will always quit after generating this
    /// number of points in result.  A typical value is 500.  DO NOIT rely
    /// on the algorithm stopping at exactly this value.  The typical value
    /// of 500 can and will produce a LineString with as many of 750 points
    /// before terminating.
    /// \return
    /// The result of the transformation is returned in the form of a
    /// disposable LineString object.  While the result can be as simple as
    /// a single straight line segment, more often it will be a multi-segment
    /// approximation of a complex curve.
    ///
    virtual MgLineString* GridLine (MgCoordinate* fromPnt,MgCoordinate* toPnt,
                                                          double curvePrecision,
                                                          UINT32 maxPoints) = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given source point is valid
    ///
    /// \param x (double)
    /// The X coordinate of the source point
    /// \param y (double)
    /// The Y coordinate of the source point
    ///
    /// \return
    /// true if this given source point is valid. false otherwise
    ///
    virtual bool IsValidSourcePoint(double x, double y)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given source point is valid
    ///
    /// \param x (double)
    /// The X coordinate of the source point
    /// \param y (double)
    /// The Y coordinate of the source point
    /// \param z (double)
    /// The Z coordinate of the source point
    ///
    /// \return
    /// true if this given source point is valid. false otherwise
    ///
    virtual bool IsValidSourcePoint(double x, double y, double z)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given target point is valid
    ///
    /// \param x (double)
    /// The X coordinate of the target point
    /// \param y (double)
    /// The Y coordinate of the target point
    ///
    /// \return
    /// true if this given target point is valid. false otherwise
    ///
    virtual bool IsValidTargetPoint(double x, double y)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether the given target point is valid
    ///
    /// \param x (double)
    /// The X coordinate of the target point
    /// \param y (double)
    /// The Y coordinate of the target point
    /// \param z (double)
    /// The Z coordinate of the target point
    ///
    /// \return
    /// true if this given target point is valid. false otherwise
    ///
    virtual bool IsValidTargetPoint(double x, double y, double z)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source coordinate system (MgCoordinateSystem)
    ///
    /// \return
    /// The source coordinate system
    ///
    virtual MgCoordinateSystem* GetSource()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the target coordinate system (MgCoordinateSystem)
    ///
    /// \return
    /// The target coordinate system
    ///
    virtual MgCoordinateSystem* GetTarget()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the source and target coordinate systems
    ///
    /// \param pSource (MgCoordinateSystem)
    /// The source coordinate system
    /// \param pTarget (MgCoordinateSystem)
    /// The target coordinate system
    ///
    virtual void SetSourceAndTarget(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)=0;
    
    // Geodetic Transformation Information
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the number of geodetic transformations used in the implicit or
    /// explicit path used to convert the source to the target coordinate system.
    /// For a geodetic transformation to take place, both the source and the target
    /// coordinate systems have to be referenced to a datum.
    /// \return
    /// Returns the number of geodetic transformations in the path as described above.
    /// Returns 0, if no geodetic transformation will have to be done when converting from
    /// the source coordinate system to the target coordinate system. In 
    /// particular, this method returns 0 in the following cases:
    /// \li The source and the target coordinate system is referenced to the same datum
    /// \li Either the source or the target coordinate system is not referenced
    ///     to a datum
    /// \li This transformation object transforms between arbitrary coordinate systems.
    ///
    /// \remarks
    /// This method returns a non-zero geodetic transformation count even if a geodetic transformation
    /// doesn't perform any calculations but simply assumes the datum the source coordinate system is referenced to,
    /// to be equivalent to that datum the target coordinate system is referenced to.
    virtual INT32 GetGeodeticTransformationCount()=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a pointer to the catalog-resident(!) geodetic transformation definition indicated
    /// by the index parameter.
    /// \param index
    /// A zero based index indicating the specific transformation definition
    /// which is to be returned.
    /// \return
    /// Returns a disposable pointer to the geodetic transformation definition
    /// indicated by the index parameter. The MgCoordinateSystemGeodeticTransformDef
    /// instance is guaranteed to exist in the geodetic transformation definition
    /// dictionary.
    /// \exception  MgIndexOutOfRangeException if index is out of range. See MgCoordinateSystemTransform::GetGeodeticTransformationCount.
    /// \exception  MgCoordinateSystemLoadFailedException if the geodetic transformation
    ///             denoted by the index is not contained in the dictionries
    ///             but has been created in-memory only. Note though, that this will only
    ///             in very rare circumstances.
    /// \exception  MgCoordinateSystemInitializationFailedException if the 
    ///             the transformation definition object could not be setup.
    /// \exception  MgException if the dictionaries could not be accessed or
    ///             any other error condition has been encountered.
    /// \see GetGeodeticTransformationCount
    ///
    virtual MgCoordinateSystemGeodeticTransformDef* GetGeodeticTransformation (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the direction of the transformation definition indicated by
    /// the index parameter.
    /// \param index
    /// A zero based index indicating the specific transformation definition
    /// for which the direction is to be returned.
    /// \return
    /// Returns a memebr of the MgCoordinateSystemGeodeticDirection class:
    /// * MgCoordinateSystemGeodeticDirection::GeodeticDirectionForward for the forward direction,
    /// * MgCoordinateSystemGeodeticDirection::GeodeticDirectionInverse or the inverse direction,
    /// * MgCoordinateSystemGeodeticDirection::GeodeticDirectionNone if there is no specification (e.g. a null transformation), and
    /// * MgCoordinateSystemGeodeticDirection::GeodeticDirectionError if the index parameter is invalid.
    /// \remarks
    /// No exceptions are thrown.
    ///
    virtual INT32 GetGeodeticTransformationDirection (INT32 index)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the definition of the geodetic path used to build the
    /// transformation, but only if the path was explicitly obtained from the
    /// Geodetic Path dictionary.
    /// \return
    /// Returns a disposable pointer to the Geodetic Path definition which
    /// was used to build the conversion from the source to target datums.
    /// Returns null is the path from source to taregt datums was implicitly
    /// generated by CS-MAP.
    /// \remarks
    /// No exceptions are thrown.
    ///
    virtual MgCoordinateSystemGeodeticPath* GetExplicitGeodeticPath()=0;

    // Transformation warnings

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets whether datum shift warnings will be ignored
    ///
    /// \param bIgnoreDatumShiftWarning (boolean/bool)
    /// true if datum shift warnings will be ignored. false otherwise
    ///
    virtual void IgnoreDatumShiftWarning(bool bIgnoreDatumShiftWarning)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether datum shift warnings will be ignored
    ///
    /// \return
    /// true if this datum shift warnings will be ignored. false otherwise
    ///
    virtual bool IsIgnoreDatumShiftWarning()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets whether outside domain warnings will be ignored
    ///
    /// \param bIgnoreOutsideDomainWarning (boolean/bool)
    /// true if outside domain warnings will be ignored. false otherwise
    ///
    virtual void IgnoreOutsideDomainWarning(bool bIgnoreOutsideDomainWarning)=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets whether outside domain warnings will be ignored
    ///
    /// \return
    /// true if this outside domain warnings will be ignored. false otherwise
    ///
    virtual bool IsIgnoreOutsideDomainWarning()=0;

    // Status Accumulator

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source warning count
    ///
    /// \return
    /// The source warning count
    ///
    /// \since 2.4
    ///
    virtual INT32 GetSourceWarningCount (void)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the datum warning count
    ///
    /// \return
    /// The datum warning count
    ///
    /// \since 2.4
    ///
    virtual INT32 GetdatumWarningCount (void)=0;
    
    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the target warning count
    ///
    /// \return
    /// The target warning count
    ///
    /// \since 2.4
    ///
    virtual INT32 GetTargetWarningCount (void)=0;

    // Transformation status

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transformation status: ok
    ///
    static const INT32 TransformOk = 0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transformation status: resulted in being outside the domain warning
    ///
    static const INT32 TransformOutsideDomainWarning = 1;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transformation status: resulted in a datum shift warning
    ///
    static const INT32 TransformDatumShiftWarning = 2;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transformation status: was a total failure
    ///
    static const INT32 TransformTotalFailure = 3;

    // Status Accumulation Status bit map.

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Status Accumulation Status bit map: Source CRS error
    ///
    static const INT32 SourceCrsError = 1;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Status Accumulation Status bit map: Datum shift fallback
    ///
    static const INT32 DatumShiftFallback = 2;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Status Accumulation Status bit map: Datum shift error
    ///
    static const INT32 DatumShiftError = 4;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Status Accumulation Status bit map: Target CRS error
    ///
    static const INT32 TargetCrsError = 8;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the last transformation status
    ///
    /// \return
    /// The last transformation status
    ///
    virtual INT32 GetLastTransformStatus()=0;

    ///////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets the last transformation status
    ///
    virtual void ResetLastTransformStatus()=0;

INTERNAL_API:
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X and Y ordinates.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double* x, double* y)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X and Y ordinates.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double x[], double y[], int arraySize)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y ordinates and the measure.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param m
    /// The measure to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double* x, double* y, double* m)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y ordinates and the measure.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param m
    /// The measures to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double x[], double y[], double m[], int arraySize)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y, and Z ordinates.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param z
    /// The Z ordinate to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double* x, double* y, double* z)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y, and Z ordinates.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param z
    /// The Z ordinates to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Transform(double x[], double y[], double z[], int arraySize)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transform 2D conversion on an array of 2D XY points.
    ///
    /// \param xy
    /// The array of 2D XY points to be converted.
    /// \param pointCount
    /// The number of 2D points in the array which are to be converted.
    /// \return
    /// Nothing.
    /// \remark
    /// Use the Get...WarningCount functions above to determine the status of
    /// the conversion.  A conversion of all points will be attemted regardless
    /// of the status of any specific point conversion.  Warning counts will
    /// accumulate on successive calls to this function unless the
    /// ResetLastTransformStatus member function is called.  This function
    /// will throw an exception only in the case of what is considered a
    /// total failure.  That is, a condition which suggests that there is a
    /// a problem with the conversion process (i.e. a physical I>O error on
    /// disk) that is not related to the data being converted.  In the case of
    /// this function, GetConversionStatus will return the most serious (i.e.
    /// highest value) warning condition encountered.
    virtual void Transform2D (double xy[][2],INT32 pointCount)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms a 3D conversion on an array of 3D XYZ points.
    ///
    /// \param xy
    /// The array of 3D XYZ points to be converted.
    /// \param pointCount
    /// The number of 3D points in the array which are to be converted.
    /// \return
    /// Nothing.
    /// \remark
    /// Use the Get...WarningCount functions above to determine the status of
    /// the conversion.  A conversion of all points will be attemted regardless
    /// of the status of any specific point conversion.  Warning counts will
    /// accumulate on successive calls to this function unless the
    /// ResetLastTransformStatus member function is called.  This function
    /// will throw an exception only in the case of what is considered a
    /// total failure.  That is, a condition which suggests that there is a
    /// a problem with the conversion process (i.e. a physical I>O error on
    /// disk) that is not related to the data being converted.  In the case of
    /// this function, GetConversionStatus will return the most serious (i.e.
    /// highest value) warning condition encountered.
    virtual void Transform3D (double xy[][3],INT32 pointCount)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the X, Y, Z ordinates and the measure.
    ///
    /// \param x
    /// The X ordinate to transform.
    /// \param y
    /// The Y ordinate to transform.
    /// \param z
    /// The Z ordinate to transform.
    /// \param m
    /// The measure to transform.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double* x, double* y, double* z, double* m)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Transforms the array of X, Y, Z ordinates and the measure.
    ///
    /// \param x
    /// The X ordinates to transform.
    /// \param y
    /// The Y ordinates to transform.
    /// \param z
    /// The Z ordinates to transform.
    /// \param m
    /// The measures to transform.
    /// \param arraySize
    /// The size of the arrays. All the arrays must be the same size.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void TransformM(double x[], double y[], double z[], double m[], int arraySize)=0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Give a line segement in coordinates of the source system via the the
    /// fromPnt and toPnt parameters, this function returns the position of
    /// a point on that line segment which, when converted to target
    /// coordinates, will have an ordinate value equal to the ordinateValue
    /// parameter.  Used, primarily, to locate the position of a tick line
    /// on a map border.
    ///
    /// \param position
    /// The result of the calculation is returned here.  This must be a
    /// valid pointer to an exisitng MgCOordinate object.  THis function will
    /// not alter the reference count status in any way.
    /// \param ordinateValue
    /// The specific value whose position is to be located.
    /// \param orientation
    /// A value of the MgCoordinateSystemGridOrientation class.  This parameter
    /// indicates whether the ordinateValue parameter is an X ordinate or a Y
    /// ordinate.
    /// \param fromPnt
    /// A two dimensional point given in coordinates of the source coordinate
    /// system of this <b>Transform<\b> object which represents the starting
    /// point of the line segment which is to be processed.
    /// \param toPnt
    /// A two dimensional point given in coordinates of the source coordinate
    /// system of this <b>Transform<\b> object which represents the ending
    /// point of the line segment which is to be processed.
    /// \return
    /// Will return a zero for success.  A positive non-zero return indicates
    /// a failure to calculate a position for one of the normal reasons:
    /// 1> for no such value on this line segment;
    /// 2> line orientation is inappropriate for this ordinate
    /// For failure of anyother type, and MgException is thrown.
    /// \remarks
    /// The order of the two points provided is immaterial.  This object
    /// attempts to eliminate possible duplicate positions by ignoring
    /// the to pont in the calculation of poosition.
    virtual INT32 PositionOfValue (MgCoordinate* position,double ordinateValue,
                                                          INT32 orientation,
                                                          MgCoordinate* fromPnt,
                                                          MgCoordinate* toPnt) = 0;

protected:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId(){return CoordinateSystem_CoordinateSystemTransform;};

    /// Data Members
CLASS_ID:
    static const INT32 m_cls_id = CoordinateSystem_CoordinateSystemTransform;
};
/// \}

#endif //_MGCOORDINATESYSTEMTRANSFORM_H_

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

    virtual void TransformCoordinate(MgCoordinate* coordinate)=0;

    virtual bool IsValidSourcePoint(double x, double y)=0;
    virtual bool IsValidSourcePoint(double x, double y, double z)=0;
    virtual bool IsValidTargetPoint(double x, double y)=0;
    virtual bool IsValidTargetPoint(double x, double y, double z)=0;

    virtual MgCoordinateSystem* GetSource()=0;
    virtual MgCoordinateSystem* GetTarget()=0;
    virtual void SetSourceAndTarget(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)=0;

    // Transformation warnings
    virtual void IgnoreDatumShiftWarning(bool bIgnoreDatumShiftWarning)=0;
    virtual bool IsIgnoreDatumShiftWarning()=0;
    virtual void IgnoreOutsideDomainWarning(bool bIgnoreOutsideDomainWarning)=0;
    virtual bool IsIgnoreOutsideDomainWarning()=0;

    // Transformation status
    static const INT32 TransformOk = 0;
    static const INT32 TransformDatumShiftWarning = 1;
    static const INT32 TransformOutsideDomainWarning = 2;
    static const INT32 TransformTotalFailure = 3;

    virtual INT32 GetLastTransformStatus()=0;
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

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMTRANSFORM_H_
#define _CCOORDINATESYSTEMTRANSFORM_H_

class OGRCoordinateTransformation;

namespace CSLibrary
{
class CCoordinateSystem;
class CEnvelope;

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystemTransform class provides operations for transforming
/// coordinates from one coordinate system to another. The transformation
/// includes a shift of the coordinate datum if required.
///</summary>

class COORDINATE_SYSTEM_API CCoordinateSystemTransform
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Initializes a new instance of CCoordinateSystemTransform given source
    /// and target coordinate system objects. If the source and target
    /// coordinate systems are not compatible (e.g. transformations cannot be
    /// performed) an exception will be thrown.
    ///</summary>
    ///<param name="CCoordinateSystem source">
    /// An CCoordinateSystem that defines the coordinate system of the source
    /// coordiantes.
    ///</param>
    ///<param name="CCoordinateSystem target">
    /// An CCoordinateSystem that defines the coordinate system for the target
    /// coordiantes.
    ///</param>
    CCoordinateSystemTransform(const CCoordinateSystem* source, const CCoordinateSystem* target);

    virtual ~CCoordinateSystemTransform();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the specified source envelope and returns a new envelope.
    ///</summary>
    ///<param name="CEnvelope envelope">
    /// The input CEnvelope to transform.
    ///</param>
    ///<returns>
    /// A new CEnvelope transformed from the specified envelope.
    ///</returns>
    virtual CEnvelope* Transform(CEnvelope* envelope);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the X and Y ordinates.
    ///</summary>
    ///<param name="x">
    /// The X ordinate to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinate to transform.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void Transform(double& x, double& y);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the array of X and Y ordinates.
    ///</summary>
    ///<param name="x">
    /// The X ordinates to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinates to transform.
    ///</param>
    ///<param name="arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void Transform(double x[], double y[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the X, Y ordinates and the measure.
    ///</summary>
    ///<param name="x">
    /// The X ordinate to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinate to transform.
    ///</param>
    ///<param name="m">
    /// The measure to transform.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void TransformM(double& x, double& y, double& m);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the array of X, Y ordinates and the measure.
    ///</summary>
    ///<param name="x">
    /// The X ordinates to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinates to transform.
    ///</param>
    ///<param name="m">
    /// The measures to transform.
    ///</param>
    ///<param name="arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void TransformM(double x[], double y[], double m[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the X, Y, and Z ordinates.
    ///</summary>
    ///<param name="x">
    /// The X ordinate to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinate to transform.
    ///</param>
    ///<param name="z">
    /// The Z ordinate to transform.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void Transform(double& x, double& y, double& z);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the array of X, Y, and Z ordinates.
    ///</summary>
    ///<param name="x">
    /// The X ordinates to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinates to transform.
    ///</param>
    ///<param name="z">
    /// The Z ordinates to transform.
    ///</param>
    ///<param name="arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void Transform(double x[], double y[], double z[], int arraySize);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the X, Y, Z ordinates and the measure.
    ///</summary>
    ///<param name="x">
    /// The X ordinate to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinate to transform.
    ///</param>
    ///<param name="z">
    /// The Z ordinate to transform.
    ///</param>
    ///<param name="m">
    /// The measure to transform.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void TransformM(double& x, double& y, double& z, double& m);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Transforms the array of X, Y, Z ordinates and the measure.
    ///</summary>
    ///<param name="x">
    /// The X ordinates to transform.
    ///</param>
    ///<param name="y">
    /// The Y ordinates to transform.
    ///</param>
    ///<param name="z">
    /// The Z ordinates to transform.
    ///</param>
    ///<param name="m">
    /// The measures to transform.
    ///</param>
    ///<param name="arraySize">
    /// The size of the arrays. All the arrays must be the same size.
    ///</param>
    ///<returns>
    /// Nothing.
    ///</returns>
    virtual void TransformM(double x[], double y[], double z[], double m[], int arraySize);

private:
    CCoordinateSystemTransform() {};

    void InternalTransform(int numPts, double* x, double* y, double* z, double* m);

    CEnvelope* XYExtentToLL(OGRCoordinateTransformation* transform, CEnvelope* envelope);
    CEnvelope* LLExtentToXY(OGRCoordinateTransformation* transform, CEnvelope* envelope);

    void XYToLL(OGRCoordinateTransformation* transform, double dX, double dY, double& dLon, double& dLat);
    void LLToXY(OGRCoordinateTransformation* transform, double dLon, double dLat, double& dX, double& dY);

    CCoordinateSystem* m_coordSysSource;
    CCoordinateSystem* m_coordSysTarget;
    OGRCoordinateTransformation* m_transformForward;
    OGRCoordinateTransformation* m_transformInverse;

    enum TransformHint
    {
        TH_IDENTITY,
        TH_LOCAL,
        TH_GEOGRAPHIC_TO_PROJECTED,
        TH_PROJECTED_TO_GEOGRAPHIC,
        TH_PROJECTED_TO_PROJECTED
    };

    TransformHint m_transformHint;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTRANSFORM_H_

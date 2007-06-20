//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"
#include "CoordSysUtil.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Initializes a new instance of CCoordinateSystemTransform given source
/// and target coordinate system objects. If the source and target
/// coordinate systems are not compatible (e.g. transformations cannot be
/// performed) an exception will be thrown.
///</summary>
///<param name="source">
/// An CCoordinateSystem that defines the coordinate system of the source
/// coordiantes.
///</param>
///<param name="target">
/// An CCoordinateSystem that defines the coordinate system for the target
/// coordiantes.
///</param>
CCoordinateSystemTransform::CCoordinateSystemTransform(const CCoordinateSystem* source, const CCoordinateSystem* target) :
    m_transformForward(NULL),
    m_transformInverse(NULL),
    m_coordSysSource(NULL),
    m_coordSysTarget(NULL),
    m_transformHint(TH_IDENTITY)
{
    bool bPossibleDatumConversion = false;

    if(NULL == source)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.CCoordinateSystemTransform", __LINE__, __WFILE__, L"[1] - CCoordinateSystem pointer.");
    }

    if(NULL == target)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.CCoordinateSystemTransform", __LINE__, __WFILE__, L"[2] - CCoordinateSystem pointer.");
    }

    m_coordSysSource = const_cast<CCoordinateSystem*>(source)->Clone();
    if(NULL == m_coordSysSource)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemTransform.CCoordinateSystemTransform", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystem.");
    }

    m_coordSysTarget = const_cast<CCoordinateSystem*>(target)->Clone();
    if(NULL == m_coordSysTarget)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemTransform.CCoordinateSystemTransform", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystem.");
    }

    if ((m_coordSysSource->GetType() == CCoordinateSystemType::Arbitrary) &&
        (m_coordSysTarget->GetType() == CCoordinateSystemType::Arbitrary))
    {
        bPossibleDatumConversion = false;

        // Make sure the unit scale values are set so that we can't get a
        // divide by zero error.
        if ((m_coordSysSource->GetUnitScale() != 0.0) &&
            (m_coordSysTarget->GetUnitScale() != 0.0))
        {
            m_transformHint = TH_LOCAL;
        }
        else
        {
            m_transformHint = TH_IDENTITY;
        }
    }
    else if ((m_coordSysSource->GetType() == CCoordinateSystemType::Arbitrary) ||
            (m_coordSysTarget->GetType() == CCoordinateSystemType::Arbitrary))
    {
        bPossibleDatumConversion = false;
        m_transformHint = TH_IDENTITY;
    }
    else if ((m_coordSysSource->GetType() == CCoordinateSystemType::Geographic) &&
            (m_coordSysTarget->GetType() == CCoordinateSystemType::Geographic))
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_IDENTITY;
    }
    else if (m_coordSysSource->GetInternalCoordinateSystem()->IsSame(m_coordSysTarget->GetInternalCoordinateSystem()))
    {
        bPossibleDatumConversion = false;
        m_transformHint = TH_IDENTITY;
    }
    else if(m_coordSysSource->GetType() == CCoordinateSystemType::Geographic)
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_GEOGRAPHIC_TO_PROJECTED;
    }
    else if(m_coordSysTarget->GetType() == CCoordinateSystemType::Geographic)
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_PROJECTED_TO_GEOGRAPHIC;
    }
    else
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_PROJECTED_TO_PROJECTED;
    }

    if(bPossibleDatumConversion)
    {
        // Setup the transformation with datum shift
        m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_coordSysSource->GetInternalCoordinateSystem(), m_coordSysTarget->GetInternalCoordinateSystem(), true);
        m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(m_coordSysTarget->GetInternalCoordinateSystem(), m_coordSysSource->GetInternalCoordinateSystem(), true);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
CCoordinateSystemTransform::~CCoordinateSystemTransform()
{
    // Free resources
    if(m_coordSysSource)
    {
        delete m_coordSysSource;
        m_coordSysSource = NULL;
    }

    if(m_coordSysTarget)
    {
        delete m_coordSysTarget;
        m_coordSysTarget = NULL;
    }

    if(m_transformForward)
    {
        delete m_transformForward;
        m_transformForward = NULL;
    }

    if(m_transformInverse)
    {
        delete m_transformInverse;
        m_transformInverse = NULL;
    }
}

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
void CCoordinateSystemTransform::Transform(double& x, double& y)
{
    try
    {
        InternalTransform(&x, &y, NULL, NULL, 1);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::Transform(double x[], double y[], int arraySize)
{
    if(NULL == x)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[1] - double[].");
    }

    if(NULL == y)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[2] - double[].");
    }

    try
    {
        InternalTransform(x, y, NULL, NULL, arraySize);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::TransformM(double& x, double& y, double& m)
{
    try
    {
        InternalTransform(&x, &y, NULL, &m, 1);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::TransformM(double x[], double y[], double m[], int arraySize)
{
    if(NULL == x)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[1] - double[].");
    }

    if(NULL == y)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[2] - double[].");
    }

    if(NULL == m)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[3] - double[].");
    }

    try
    {
        InternalTransform(x, y, NULL, m, arraySize);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::Transform(double& x, double& y, double& z)
{
    try
    {
        InternalTransform(&x, &y, &z, NULL, 1);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::Transform(double x[], double y[], double z[], int arraySize)
{
    if(NULL == x)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[1] - double[].");
    }

    if(NULL == y)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[2] - double[].");
    }

    if(NULL == z)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[3] - double[].");
    }

    try
    {
        InternalTransform(x, y, z, NULL, arraySize);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::TransformM(double& x, double& y, double& z, double& m)
{
    try
    {
        InternalTransform(&x, &y, &z, &m, 1);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, message);
    }
}

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
void CCoordinateSystemTransform::TransformM(double x[], double y[], double z[], double m[], int arraySize)
{
    if(NULL == x)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[1] - double[].");
    }

    if(NULL == y)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[2] - double[].");
    }

    if(NULL == z)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[3] - double[].");
    }

    if(NULL == m)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, L"[4] - double[].");
    }

    try
    {
        InternalTransform(x, y, z, m, arraySize);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;

        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, message);
    }
}

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
CEnvelope* CCoordinateSystemTransform::Transform(CEnvelope* envelope)
{
    CEnvelope* pEnvelope = NULL;

    if(NULL == envelope)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"[1] - CEnvelope pointer.");
    }

    try
    {
        switch(m_transformHint)
        {
        case TH_IDENTITY:
            pEnvelope = new CEnvelope(envelope);
            break;

        case TH_GEOGRAPHIC_TO_PROJECTED:
            pEnvelope = LLExtentToXY(m_transformForward, envelope);
            break;

        case TH_PROJECTED_TO_GEOGRAPHIC:
            pEnvelope = XYExtentToLL(m_transformForward, envelope);
            break;

        case TH_LOCAL:
        case TH_PROJECTED_TO_PROJECTED:
        default:
            if(envelope->GetDimension() == CCoordinateSystemDimension::XYZ)
            {
                double llx = envelope->GetLowerLeftXCoordinate();
                double lly = envelope->GetLowerLeftYCoordinate();
                double llz = envelope->GetLowerLeftZCoordinate();
                Transform(llx, lly, llz);

                double urx = envelope->GetUpperRightXCoordinate();
                double ury = envelope->GetUpperRightYCoordinate();
                double urz = envelope->GetUpperRightZCoordinate();
                Transform(urx, ury, urz);

                pEnvelope = new CEnvelope(llx, lly, llz, urx, ury, urz);
            }
            else
            {
                double llx = envelope->GetLowerLeftXCoordinate();
                double lly = envelope->GetLowerLeftYCoordinate();
                Transform(llx, lly);

                double urx = envelope->GetUpperRightXCoordinate();
                double ury = envelope->GetUpperRightYCoordinate();
                Transform(urx, ury);

                pEnvelope = new CEnvelope(llx, lly, urx, ury);
            }

            break;
        }
    }
    catch(...)
    {
        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.Transform", __LINE__, __WFILE__, L"Unexpected error.");
    }

    return pEnvelope;
}

void CCoordinateSystemTransform::InternalTransform(double* x, double* y, double* z, double* m, int numPts)
{
    // We must at least have the x and y
    if(NULL == x)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, L"[1] - double pointer.");
    }

    if(NULL == y)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, L"[2] - double pointer.");
    }

    try
    {
        switch(m_transformHint)
        {
        case TH_IDENTITY: // No conversion required
            {
                if(m_transformForward)
                {
                    //pass on to proj for the transform
                    if (TRUE != m_transformForward->TransformEx(numPts, x, y, z, NULL))
                    {
                        //if proj failed to convert, generate an exception
                        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, L"PROJ4 failed to transform the data.");
                    }
                }
            }
            break;

        case TH_LOCAL: // Scaling only
            {
                double unitScale = m_coordSysSource->GetUnitScale() / m_coordSysTarget->GetUnitScale();

                if (unitScale != 1.0)
                {
                    for (int i=0; i<numPts; i++)
                    {
                        x[i] *= unitScale;
                        y[i] *= unitScale;
                    }

                    if (NULL != z)
                        for (int i=0; i<numPts; i++)
                            z[i] *= unitScale;
                }
            }
            break;

        //all cases that involve projection
        case TH_PROJECTED_TO_GEOGRAPHIC:
        case TH_GEOGRAPHIC_TO_PROJECTED:
        case TH_PROJECTED_TO_PROJECTED:
        default:
            {
                //pass on to proj for the transform
                if (TRUE != m_transformForward->TransformEx(numPts, x, y, z, NULL))
                {
                    //if proj failed to convert, generate an exception
                    throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, L"PROJ4 failed to transform the data.");
                }
            }
            break;
        }

        if(NULL != m)
        {
            double unitScale = m_coordSysSource->GetUnitScale() / m_coordSysTarget->GetUnitScale();

            if (unitScale != 1.0)
                for (int i=0; i<numPts; i++)
                    m[i] *= unitScale;
        }
    }
    catch(CCoordinateSystemTransformFailedException* e)
    {
        throw e;
    }
    catch(...)
    {
        throw new CCoordinateSystemTransformFailedException(L"CCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, L"Unexpected error.");
    }
}

CEnvelope* CCoordinateSystemTransform::XYExtentToLL(OGRCoordinateTransformation* transform, CEnvelope* envelope)
{
    CEnvelope* pEnvelope = NULL;

    if(NULL == transform)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.XYExtentToLL", __LINE__, __WFILE__, L"[1] - OGRCoordinateTransformation pointer.");
    }

    if(NULL == envelope)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.XYExtentToLL", __LINE__, __WFILE__, L"[2] - CEnvelope pointer.");
    }

    double ptUpperRightX = envelope->GetUpperRightXCoordinate();
    double ptUpperRightY = envelope->GetUpperRightYCoordinate();
    double ptUpperRightZ = envelope->GetUpperRightZCoordinate();
    double ptLowerLeftX = envelope->GetLowerLeftXCoordinate();
    double ptLowerLeftY = envelope->GetLowerLeftYCoordinate();
    double ptLowerLeftZ = envelope->GetLowerLeftZCoordinate();

    int dimension = envelope->GetDimension();

    double ptNWX = ptLowerLeftX;
    double ptNWY = ptUpperRightY;
    double ptSEX = ptUpperRightX;
    double ptSEY = ptLowerLeftY;

    double xMax = -HUGE_VAL;
    double yMax = -HUGE_VAL;
    double xMin = HUGE_VAL;
    double yMin = HUGE_VAL;

    //temporary arrays for transformation
    double tess_x[400];
    double tess_y[400];

    //temporary counters
    double* dstx = tess_x;
    double* dsty = tess_y;

    //top and bottom edges

    double xInc = (ptNWX - ptSEX) / 100.0;
    double xInterp = ptSEX;   

    for (int i = 0; i < 100; i++)
    {
        *dstx++ = xInterp;
        *dsty++ = ptNWY;
        xInterp += xInc;
    }

    xInterp = ptSEX;

    for (int i = 0; i < 100; i++)
    {
        *dstx++ = xInterp;
        *dsty++ = ptSEY;
        xInterp += xInc;
    }

    // left and right borders

    double yInc = (ptNWY - ptSEY) / 100.0;
    double yInterp = ptSEY;

    for (int i = 0; i < 100; i++)
    {
        *dstx++ = ptNWX;
        *dsty++ = yInterp;
        yInterp += yInc;
    }

    yInterp = ptSEY;

    for (int i = 0; i < 100; i++)
    {
        *dstx++ = ptSEX;
        *dsty++ = yInterp;
        yInterp += yInc;
    }

    transform->Transform(400, tess_x, tess_y);

    dstx = tess_x;
    dsty = tess_y;
    for (int i=0; i<400; i++)
    {
        xMax = max(*dstx, xMax);
        xMin = min(*dstx, xMin);
        yMax = max(*dsty, yMax);
        yMin = min(*dsty, yMin);
        dstx++;
        dsty++;
    }

    if(dimension == CCoordinateSystemDimension::XYZ)
    {
        double zMax = 0.0;
        double zMin = 0.0;

        zMax = max(ptUpperRightZ, ptLowerLeftZ);
        zMin = min(ptUpperRightZ, ptLowerLeftZ);

        pEnvelope = new CEnvelope(xMin, yMin, zMin, xMax, yMax, zMax);
    }
    else
    {
        // Assume XY only
        pEnvelope = new CEnvelope(xMin, yMin, xMax, yMax);
    }

    return pEnvelope;
}

CEnvelope* CCoordinateSystemTransform::LLExtentToXY(OGRCoordinateTransformation* transform, CEnvelope* envelope)
{
    CEnvelope* pEnvelope = NULL;

    if(NULL == transform)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.LLExtentToXY", __LINE__, __WFILE__, L"[1] - OGRCoordinateTransformation pointer.");
    }

    if(NULL == envelope)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.LLExtentToXY", __LINE__, __WFILE__, L"[2] - CEnvelope pointer.");
    }

    double ptUpperRightX = envelope->GetUpperRightXCoordinate();
    double ptUpperRightY = envelope->GetUpperRightYCoordinate();
    double ptUpperRightZ = envelope->GetUpperRightZCoordinate();
    double ptLowerLeftX = envelope->GetLowerLeftXCoordinate();
    double ptLowerLeftY = envelope->GetLowerLeftYCoordinate();
    double ptLowerLeftZ = envelope->GetLowerLeftZCoordinate();

    int dimension = envelope->GetDimension();

    double ptNWX = ptLowerLeftX;
    double ptNWY = ptUpperRightY;
    double ptSEX = ptUpperRightX;
    double ptSEY = ptLowerLeftY;

    double lon = (ptSEX + ptNWX) / 2.0;
    double lat = (ptSEY + ptNWY) / 2.0;
    double xDist = ::fabs(ptSEX - ptNWX);

    double tempX = 0.0;
    double tempY = 0.0;

    // Bottom center
    LLToXY(transform, lon, ptSEY, tempX, tempY);

    double xMax = tempX;
    double xMin = tempX;
    double yMax = tempY;
    double yMin = tempY;

    // Bottom right
    LLToXY(transform, (lon + xDist / 2), ptSEY, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Bottom left
    LLToXY(transform, (lon - xDist / 2), ptSEY, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Top center
    LLToXY(transform, lon, ptNWY, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Top right
    LLToXY(transform, (lon + xDist / 2), ptNWY, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Top left
    LLToXY(transform, (lon - xDist / 2), ptNWY, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Middle center
    LLToXY(transform, lon, lat, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Middle right
    LLToXY(transform, (lon + xDist / 2), lat, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    // Middle left
    LLToXY(transform, (lon - xDist / 2), lat, tempX, tempY);

    xMax = max(tempX, xMax);
    xMin = min(tempX, xMin);
    yMax = max(tempY, yMax);
    yMin = min(tempY, yMin);

    if(dimension == CCoordinateSystemDimension::XYZ)
    {
        double zMax = 0.0;
        double zMin = 0.0;

        zMax = max(ptUpperRightZ, ptLowerLeftZ);
        zMin = min(ptUpperRightZ, ptLowerLeftZ);

        pEnvelope = new CEnvelope(xMin, yMin, zMin, xMax, yMax, zMax);
    }
    else
    {
        // Assume XY only
        pEnvelope = new CEnvelope(xMin, yMin, xMax, yMax);
    }

    return pEnvelope;
}


void CCoordinateSystemTransform::LLToXY(OGRCoordinateTransformation* transform, double dLon, double dLat, double& dX, double& dY)
{
    if(NULL == transform)
    {
        throw new CNullArgumentException(L"CCoordinateSystemTransform.LLToXY", __LINE__, __WFILE__, L"[1] - OGRCoordinateTransformation pointer.");
    }

    double ll[3] = { dLon, dLat, 0.0 };

    transform->Transform(1, &ll[0], &ll[1], &ll[2]);

    dX = ll[0];
    dY = ll[1];
}

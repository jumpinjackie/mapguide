//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordSys.h"                   //for CCoordinateSystem

#include "ogr_spatialref.h"

#include "CoordSysTransformation.h"

using namespace CSLibrary;

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
CCoordinateSystemTransform::CCoordinateSystemTransform(MgCoordinateSystem* source, MgCoordinateSystem* target) :
    m_transformForward(NULL),
    m_transformInverse(NULL),
    m_transformHint(TH_IDENTITY),
    m_bIgnoreDatumShiftWarning(false),
    m_bIgnoreOutsideDomainWarning(false),
    m_nTransformStatus(TransformOk)

{
    bool bPossibleDatumConversion = false;

    if (NULL == source || NULL == target)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.CCoordinateSystemTransform",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordSysSource = SAFE_ADDREF(source);
    m_coordSysTarget = SAFE_ADDREF(target);

    if ((m_coordSysSource->GetType() == MgCoordinateSystemType::Arbitrary) &&
        (m_coordSysTarget->GetType() == MgCoordinateSystemType::Arbitrary))
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
    else if ((m_coordSysSource->GetType() == MgCoordinateSystemType::Arbitrary) ||
            (m_coordSysTarget->GetType() == MgCoordinateSystemType::Arbitrary))
    {
        bPossibleDatumConversion = false;
        m_transformHint = TH_IDENTITY;
    }
    else if ((m_coordSysSource->GetType() == MgCoordinateSystemType::Geographic) &&
            (m_coordSysTarget->GetType() == MgCoordinateSystemType::Geographic))
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_IDENTITY;
    }
    else if (((CCoordinateSystem*)m_coordSysSource.p)->GetInternalCoordinateSystem()->IsSame(((CCoordinateSystem*)m_coordSysTarget.p)->GetInternalCoordinateSystem()))
    {
        bPossibleDatumConversion = false;
        m_transformHint = TH_IDENTITY;
    }
    else if(m_coordSysSource->GetType() == MgCoordinateSystemType::Geographic)
    {
        bPossibleDatumConversion = true;
        m_transformHint = TH_GEOGRAPHIC_TO_PROJECTED;
    }
    else if(m_coordSysTarget->GetType() == MgCoordinateSystemType::Geographic)
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
        m_transformForward = CCoordinateSystemTransformation::CreateCoordinateTransformation(((CCoordinateSystem*)m_coordSysSource.p)->GetInternalCoordinateSystem(), ((CCoordinateSystem*)m_coordSysTarget.p)->GetInternalCoordinateSystem(), true);
        m_transformInverse = CCoordinateSystemTransformation::CreateCoordinateTransformation(((CCoordinateSystem*)m_coordSysTarget.p)->GetInternalCoordinateSystem(), ((CCoordinateSystem*)m_coordSysSource.p)->GetInternalCoordinateSystem(), true);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
CCoordinateSystemTransform::~CCoordinateSystemTransform()
{
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
void CCoordinateSystemTransform::Transform(double* x, double* y)
{
    try
    {
        InternalTransform(x, y, NULL, NULL, 1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, &arguments, L"", NULL);
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
    if (NULL == x || NULL == y)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.Transform",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        InternalTransform(x, y, NULL, NULL, arraySize);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, &arguments, L"", NULL);
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
void CCoordinateSystemTransform::TransformM(double* x, double* y, double* m)
{
    try
    {
        InternalTransform(x, y, NULL, m, 1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, &arguments, L"", NULL);
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
    if (NULL == x || NULL == y || NULL == m)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.TransformM",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        InternalTransform(x, y, NULL, m, arraySize);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, &arguments, L"", NULL);
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
void CCoordinateSystemTransform::Transform(double* x, double* y, double* z)
{
    try
    {
        InternalTransform(x, y, z, NULL, 1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, &arguments, L"", NULL);
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
    if (NULL == x || NULL == y || NULL == z)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.Transform",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        InternalTransform(x, y, z, NULL, arraySize);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, &arguments, L"", NULL);
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
void CCoordinateSystemTransform::TransformM(double* x, double* y, double* z, double* m)
{
    try
    {
        InternalTransform(x, y, z, m, 1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, &arguments, L"", NULL);
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
    if (NULL == x || NULL == y || NULL == z || NULL == m)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.TransformM",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        InternalTransform(x, y, z, m, arraySize);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage();
        SAFE_RELEASE(e);

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source envelope and returns a new envelope.
///</summary>
///<param name="MgEnvelope envelope">
/// The input MgEnvelope to transform.
///</param>
///<returns>
/// A new MgEnvelope transformed from the specified envelope.
///</returns>
MgEnvelope* CCoordinateSystemTransform::Transform(MgEnvelope* envelope)
{
    MgEnvelope* pEnvelope = NULL;

    if (NULL == envelope)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.Transform",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        switch(m_transformHint)
        {
        case TH_IDENTITY:
            pEnvelope = new MgEnvelope(envelope);
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
        {
            Ptr<MgCoordinate> lowerLeft = envelope->GetLowerLeftCoordinate();
            Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

            double llx = lowerLeft->GetX();
            double lly = lowerLeft->GetY();
            double urx = upperRight->GetX();
            double ury = upperRight->GetY();

            int dimension = lowerLeft->GetDimension();

            if(dimension == MgCoordinateDimension::XYZ)
            {
                double llz = lowerLeft->GetZ();
                Transform(&llx, &lly, &llz);

                double urz = upperRight->GetZ();
                Transform(&urx, &ury, &urz);

                Ptr<MgCoordinate> ll = new MgCoordinateXYZ(llx, lly, llz);
                Ptr<MgCoordinate> ur = new MgCoordinateXYZ(urx, ury, urz);
                pEnvelope = new MgEnvelope(ll, ur);
            }
            else
            {
                Transform(&llx, &lly);
                Transform(&urx, &ury);

                pEnvelope = new MgEnvelope(llx, lly, urx, ury);
            }
        }
            break;
        }
    }
    catch(...)
    {
        MgStringCollection arguments;
        arguments.Add(L""); // dummy argument
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemUnexpectedError", NULL);
    }

    return pEnvelope;
}

void CCoordinateSystemTransform::InternalTransform(double* x, double* y, double* z, double* m, int numPts)
{
    // We must at least have the x and y
    if (NULL == x || NULL == y)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.InternalTransform",
            __LINE__, __WFILE__, NULL, L"", NULL);
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
                        MgStringCollection arguments;
                        arguments.Add(L""); // dummy argument
                        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, &arguments, L"", NULL);
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
                if(m_transformForward)
                {
                    if (TRUE != m_transformForward->TransformEx(numPts, x, y, z, NULL))
                    {
                        //if proj failed to convert, generate an exception
                        MgStringCollection arguments;
                        arguments.Add(L""); // dummy argument
                        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, &arguments, L"", NULL);
                    }
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
    catch(MgCoordinateSystemTransformFailedException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        MgStringCollection arguments;
        arguments.Add(L""); // dummy argument
        throw new MgCoordinateSystemTransformFailedException(L"MgCoordinateSystemTransform.InternalTransform", __LINE__, __WFILE__, &arguments, L"MgCoordinateSystemUnexpectedError", NULL);
    }
}

MgEnvelope* CCoordinateSystemTransform::XYExtentToLL(OGRCoordinateTransformation* transform, MgEnvelope* envelope)
{
    MgEnvelope* pEnvelope = NULL;

    if (NULL == transform || NULL == envelope)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.XYExtentToLL",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgCoordinate> lowerLeft = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

    double ptUpperRightX = upperRight->GetX();
    double ptUpperRightY = upperRight->GetY();
    double ptUpperRightZ = upperRight->GetZ();
    double ptLowerLeftX = lowerLeft->GetX();
    double ptLowerLeftY = lowerLeft->GetY();
    double ptLowerLeftZ = lowerLeft->GetZ();

    int dimension = lowerLeft->GetDimension();

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

    if(dimension == MgCoordinateDimension::XYZ)
    {
        double zMax = 0.0;
        double zMin = 0.0;

        zMax = max(ptUpperRightZ, ptLowerLeftZ);
        zMin = min(ptUpperRightZ, ptLowerLeftZ);

        Ptr<MgCoordinate> ll = new MgCoordinateXYZ(xMin, yMin, zMin);
        Ptr<MgCoordinate> ur = new MgCoordinateXYZ(xMax, yMax, zMax);
        pEnvelope = new MgEnvelope(ll, ur);
    }
    else
    {
        // Assume XY only
        pEnvelope = new MgEnvelope(xMin, yMin, xMax, yMax);
    }

    return pEnvelope;
}

MgEnvelope* CCoordinateSystemTransform::LLExtentToXY(OGRCoordinateTransformation* transform, MgEnvelope* envelope)
{
    MgEnvelope* pEnvelope = NULL;

    if (NULL == transform || NULL == envelope)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.LLExtentToXY",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgCoordinate> lowerLeft = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

    double ptUpperRightX = upperRight->GetX();
    double ptUpperRightY = upperRight->GetY();
    double ptUpperRightZ = upperRight->GetZ();
    double ptLowerLeftX = lowerLeft->GetX();
    double ptLowerLeftY = lowerLeft->GetY();
    double ptLowerLeftZ = lowerLeft->GetZ();

    int dimension = lowerLeft->GetDimension();

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

    if(dimension == MgCoordinateDimension::XYZ)
    {
        double zMax = 0.0;
        double zMin = 0.0;

        zMax = max(ptUpperRightZ, ptLowerLeftZ);
        zMin = min(ptUpperRightZ, ptLowerLeftZ);

        Ptr<MgCoordinate> ll = new MgCoordinateXYZ(xMin, yMin, zMin);
        Ptr<MgCoordinate> ur = new MgCoordinateXYZ(xMax, yMax, zMax);
        pEnvelope = new MgEnvelope(ll, ur);
    }
    else
    {
        // Assume XY only
        pEnvelope = new MgEnvelope(xMin, yMin, xMax, yMax);
    }

    return pEnvelope;
}


void CCoordinateSystemTransform::LLToXY(OGRCoordinateTransformation* transform, double dLon, double dLat, double& dX, double& dY)
{
    if (NULL == transform)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemTransform.LLToXY",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    double ll[3] = { dLon, dLat, 0.0 };

    transform->Transform(1, &ll[0], &ll[1], &ll[2]);

    dX = ll[0];
    dY = ll[1];
}

MgCoordinate* CCoordinateSystemTransform::Transform(MgCoordinate* coordinate)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()

    if(NULL == coordinate)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Need to determine type of coordinate to transform
    if(coordinate->GetDimension() == MgCoordinateDimension::XY)
    {
        // 2D transform
        pCoordinate = Transform(coordinate->GetX(), coordinate->GetY());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        // 2D transform + measure
        pCoordinate = TransformM(coordinate->GetX(), coordinate->GetY(), coordinate->GetM());
    }
    else if(coordinate->GetDimension() == MgCoordinateDimension::XYZ)
    {
        // 3D transform
        pCoordinate = Transform(coordinate->GetX(), coordinate->GetY(), coordinate->GetZ());
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        // 3D transform + measure
        pCoordinate = TransformM(coordinate->GetX(), coordinate->GetY(), coordinate->GetZ(), coordinate->GetM());
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")

    return pCoordinate;
}

MgCoordinate* CCoordinateSystemTransform::Transform(double x, double y, double z)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y), dZ(z);
    InternalTransform(&dX, &dY, &dZ, NULL, 1);
    pCoordinate = new MgCoordinateXYZ(dX, dY, dZ);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
    return pCoordinate;
}

MgCoordinate* CCoordinateSystemTransform::Transform(double x, double y)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y);
    InternalTransform(&dX, &dY, NULL, NULL, 1);
    pCoordinate = new MgCoordinateXY(dX, dY);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
    return pCoordinate;
}

MgCoordinate* CCoordinateSystemTransform::TransformM(double x, double y, double z, double m)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y), dZ(z), dM(m);
    InternalTransform(&dX, &dY, &dZ, &dM, 1);
    pCoordinate = new MgCoordinateXYZM(dX, dY, dZ, dM);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
    return pCoordinate;
}

MgCoordinate* CCoordinateSystemTransform::TransformM(double x, double y, double m)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y), dM(m);
    InternalTransform(&dX, &dY, NULL, &dM, 1);
    if (!m_coordSysSource || !m_coordSysTarget)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    pCoordinate = new MgCoordinateXYM(dX, dY, dM);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
    return pCoordinate;
}

void CCoordinateSystemTransform::TransformCoordinate(MgCoordinate* coordinate)
{
    MG_TRY()

    if(NULL == coordinate)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.TransformCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    // Need to determine type of coordinate to transform
    if(coordinate->GetDimension() == MgCoordinateDimension::XY)
    {
        // 2D transform
        double x=coordinate->GetX();
        double y=coordinate->GetY();
        Transform(&x, &y);
        coordinate->SetX(x);
        coordinate->SetY(y);
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        // 2D transform + measure
        double x=coordinate->GetX();
        double y=coordinate->GetY();
        double m=coordinate->GetM();
        TransformM(&x, &y, &m);
        coordinate->SetX(x);
        coordinate->SetY(y);
        coordinate->SetM(m);
    }
    else if(coordinate->GetDimension() == MgCoordinateDimension::XYZ)
    {
        // 3D transform
        double x=coordinate->GetX();
        double y=coordinate->GetY();
        double z=coordinate->GetZ();
        Transform(&x, &y, &z);
        coordinate->SetX(x);
        coordinate->SetY(y);
        coordinate->SetZ(z);
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        // 3D transform + measure
        double x=coordinate->GetX();
        double y=coordinate->GetY();
        double z=coordinate->GetZ();
        double m=coordinate->GetM();
        TransformM(&x, &y, &z, &m);
        coordinate->SetX(x);
        coordinate->SetY(y);
        coordinate->SetZ(z);
        coordinate->SetM(m);
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.TransformCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformCoordinate")
}

bool CCoordinateSystemTransform::IsValidSourcePoint(double x, double y)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemTransform.IsValidSourcePoint", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystemTransform::IsValidSourcePoint(double x, double y, double z)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemTransform.IsValidSourcePoint", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystemTransform::IsValidTargetPoint(double x, double y)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemTransform.IsValidTargetPoint", __LINE__, __WFILE__, NULL, L"", NULL);
}

bool CCoordinateSystemTransform::IsValidTargetPoint(double x, double y, double z)
{
    throw new MgNotImplementedException(L"MgCoordinateSystemTransform.IsValidTargetPoint", __LINE__, __WFILE__, NULL, L"", NULL);
}

MgCoordinateSystem* CCoordinateSystemTransform::GetSource()
{
    return SAFE_ADDREF(m_coordSysSource.p);
}

MgCoordinateSystem* CCoordinateSystemTransform::GetTarget()
{
    return SAFE_ADDREF(m_coordSysTarget.p);
}

void CCoordinateSystemTransform::SetSourceAndTarget(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)
{
    if (NULL == pSource || NULL == pTarget)
    {
        throw new MgNullArgumentException(
            L"CCoordinateSystemTransform.SetSourceAndTarget",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordSysSource = SAFE_ADDREF(pSource);
    m_coordSysTarget = SAFE_ADDREF(pTarget);
}

//MgDisposable
void CCoordinateSystemTransform::Dispose()
{
    delete this;
}

void CCoordinateSystemTransform::IgnoreDatumShiftWarning(bool bIgnoreDatumShiftWarning)
{
    m_bIgnoreDatumShiftWarning = bIgnoreDatumShiftWarning;
}


bool CCoordinateSystemTransform::IsIgnoreDatumShiftWarning()
{
    return m_bIgnoreDatumShiftWarning;
}

void CCoordinateSystemTransform::IgnoreOutsideDomainWarning(bool bIgnoreOutsideDomainWarning)
{
    m_bIgnoreOutsideDomainWarning = bIgnoreOutsideDomainWarning;
}

bool CCoordinateSystemTransform::IsIgnoreOutsideDomainWarning()
{
    return m_bIgnoreOutsideDomainWarning;
}

INT32 CCoordinateSystemTransform::GetLastTransformStatus()
{
    return m_nTransformStatus;
}

void CCoordinateSystemTransform::ResetLastTransformStatus()
{
    m_nTransformStatus = TransformOk;
}

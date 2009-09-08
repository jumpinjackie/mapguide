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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordSysUtil.h"               //for CsDictionaryOpenMode
#include "MentorUtil.h"                 //for IsLegalMentorName

using namespace CSLibrary;

#ifdef WIN32
#ifdef GEOMETRY_EXPORTS
#    define MG_GEOMETRY_API __declspec(dllexport)
#else
#    define MG_GEOMETRY_API __declspec(dllimport)
#endif
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
CCoordinateSystemTransform::CCoordinateSystemTransform(
    MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget) :
    m_pDtcprm(NULL),
    m_bIgnoreDatumShiftWarning(false),
    m_bIgnoreOutsideDomainWarning(false),
    m_bSourceTargetSame(false),
    m_nTransformStatus(TransformOk)
{
    memset(&m_src, 0, sizeof(m_src));
    memset(&m_dst, 0, sizeof(m_dst));

    //a converter can be created with NULL pointers
    //because in some instances we want to initialize it
    //with SetSourceAndTarget later on
    if (pSource && pTarget)
    {
        SetSourceAndTarget(pSource, pTarget);
    }
}

CCoordinateSystemTransform::~CCoordinateSystemTransform()
{
    Uninitialize();
}

//Utility function which returns whether the object has
//been initialized or not.
//
bool CCoordinateSystemTransform::IsInitialized()
{
    return (NULL != m_pDtcprm);
}


//Utility function which uninitializes the object, freeing all
//resources as needed.
//
void CCoordinateSystemTransform::Uninitialize()
{
    if (NULL != m_pDtcprm)
    {
        CS_dtcls(m_pDtcprm);
        m_pDtcprm = NULL;
    }
    m_pCsSource = NULL;
    m_pCsTarget = NULL;
    memset(&m_src, 0, sizeof(m_src));
    memset(&m_dst, 0, sizeof(m_dst));
    assert(!IsInitialized());
}

//Convert an individual point from source to destination coordinate
//system.  Should only be called when the object has already been
//initialized.
void CCoordinateSystemTransform::TransformPoint(double& x, double& y, double *pdZ)
{
    MG_TRY()

    m_nTransformStatus = TransformOk;

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    assert(IsInitialized());
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.TransformPoint", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    bool isGeographic = MgCoordinateSystemType::Geographic == m_pCsSource->GetType();

    double lonMin = m_pCsSource->GetLonMin();
    double lonMax = m_pCsSource->GetLonMax();
    double latMin = m_pCsSource->GetLatMin();
    double latMax = m_pCsSource->GetLatMax();

    TransformPointInternal(x, y, pdZ, isGeographic, lonMin, lonMax, latMin, latMax);

    //Return success
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformPoint")
}

//-----------------------------------------------------------------------------
bool CCoordinateSystemTransform::IsValidPoint(cs_Csprm_& csprm, double x, double y, double z)
{
    bool bIsValid = true;

    MG_TRY()

    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.IsValidPoint", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Test the point
    double dCoords3d[3] = { x, y, z };
    CriticalClass.Enter();
    int nResult = CS_xychk(&csprm, 1, &dCoords3d);
    CriticalClass.Leave();

    //Decide on a result
    if (nResult & cs_CNVRT_DOMN)
    {
        //Outside the mathematical domain of the function.
        bIsValid = false;
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.IsValidPoint")

    //And return.
    return bIsValid;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X and Y ordinates and returns a new coordinate.
///</summary>
///<param name="x">
/// The X ordinate to transform.
///</param>
///<param name="y">
/// The Y ordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate defined by the transformed X and Y ordinates.
///</returns>
MgCoordinate* CCoordinateSystemTransform::Transform(double x, double y)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y);
    TransformPoint(dX, dY, NULL);
    pCoordinate = new MgCoordinateXY(dX, dY);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
    return pCoordinate;
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
/// A new MgCoordinate defined by the transformed X,Y ordinates and measure.
///</returns>
MgCoordinate* CCoordinateSystemTransform::TransformM(double x, double y, double m)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y);
    TransformPoint(dX, dY, NULL);
    if (!m_pCsSource || !m_pCsTarget)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }
    double dM = (m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    pCoordinate = new MgCoordinateXYM(dX, dY, dM);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the X, Y, and Z ordinates and returns a new coordinate.
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
/// A new MgCoordinate defined by the transformed X, Y, and Z ordinates.
///</returns>
MgCoordinate* CCoordinateSystemTransform::Transform(double x, double y, double z)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y), dZ(z);
    TransformPoint(dX, dY, &dZ);
    pCoordinate = new MgCoordinateXYZ(dX, dY, dZ);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
    return pCoordinate;
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
/// A new MgCoordinate defined by the transformed X, Y, Z ordinates and measure.
///</returns>
MgCoordinate* CCoordinateSystemTransform::TransformM(double x, double y, double z, double m)
{
    MgCoordinate* pCoordinate = NULL;
    MG_TRY()
    double dX(x), dY(y), dZ(z);
    TransformPoint(dX, dY, &dZ);
    double dM = (m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    pCoordinate = new MgCoordinateXYZM(dX, dY, dZ, dM);
    if (!pCoordinate)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source coordinate and returns a new coordinate
/// if the target coordinate is NULL, otherwise updates and returns the
/// target coordinate with the values of the transformed source coordinate.
///</summary>
///<param name="coordinate">
/// The input MgCoordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate transformed from the specified coordinate.
///</returns>
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

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source coordinate and returns a new coordinate
/// if the target coordinate is NULL, otherwise updates and returns the
/// target coordinate with the values of the transformed source coordinate.
///</summary>
///<param name="coordinate">
/// The input MgCoordinate to transform.
///</param>
///<returns>
/// A new MgCoordinate transformed from the specified coordinate.
///</returns>
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

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Transforms the specified source envelope and returns a new envelope
/// if the target envelope is NULL, otherwise updates and returns the
/// target envelope with the values of the transformed source envelope.
///</summary>
///<param name="envelope">
/// The input MgEnvelope to transform.
///</param>
///<returns>
/// A new MgEnvelope transformed from the specified envelope.
///</returns>
MgEnvelope* CCoordinateSystemTransform::Transform(MgEnvelope* envelope)
{
    MgEnvelope* pEnvelope = NULL;
    MG_TRY()

    if(NULL == envelope)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    Ptr<MgCoordinate> lowerLeft = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

    Ptr<MgCoordinate> lowerLeftTarget;
    Ptr<MgCoordinate> upperRightTarget;

    // Leave commented except for temporary debugging.  The printf output gets
    // sent to stdout and ends up being included in the web-tier HTTP response.
//  printf("\nTransform(envelope) -- Source: %S  Dest: %S\n", m_pCsSource->GetCode().c_str(), m_pCsTarget->GetCode().c_str());

    if((m_pCsSource->GetType() == MgCoordinateSystemType::Projected) && (m_pCsTarget->GetType() == MgCoordinateSystemType::Geographic))
    {
        // This is a projected to geographic transform.
        double ptUpperRightX = upperRight->GetX();
        double ptUpperRightY = upperRight->GetY();
        double ptUpperRightZ = upperRight->GetZ();
        double ptLowerLeftX = lowerLeft->GetX();
        double ptLowerLeftY = lowerLeft->GetY();
        double ptLowerLeftZ = lowerLeft->GetZ();

        int dimension = upperRight->GetDimension();

        double ptNWX = ptLowerLeftX;
        double ptNWY = ptUpperRightY;
        double ptSEX = ptUpperRightX;
        double ptSEY = ptLowerLeftY;

        double xMax = -HUGE_VAL;
        double yMax = -HUGE_VAL;
        double xMin = HUGE_VAL;
        double yMin = HUGE_VAL;

        int i;

        // compute the max and min longitude along the top and bottom borders

        double xInc = (ptNWX - ptSEX) / 100.0;
        double tempX;
        double tempY;

        for (i = 0; i <= 100; i++)
        {
            tempX = ptSEX + i * xInc;
            tempY = ptNWY;

            Transform(&tempX, &tempY);

            xMax = max(tempX, xMax);
            xMin = min(tempX, xMin);
            yMax = max(tempY, yMax);
            yMin = min(tempY, yMin);
        }

        xInc = (ptNWX - ptSEX) / 100.0;

        for (i = 0; i <= 100; i++)
        {
            tempX = ptSEX + i * xInc;
            tempY = ptSEY;

            Transform(&tempX, &tempY);

            xMax = max(tempX, xMax);
            xMin = min(tempX, xMin);
            yMax = max(tempY, yMax);
            yMin = min(tempY, yMin);
        }

        // compute the max and min longitude along the left and right borders

        double yInc = (ptNWY - ptSEY) / 100.0;

        for (i = 0; i <= 100; i++)
        {
            tempX = ptNWX;
            tempY = ptSEY + i * yInc;

            Transform(&tempX, &tempY);

            xMax = max(tempX, xMax);
            xMin = min(tempX, xMin);
            yMax = max(tempY, yMax);
            yMin = min(tempY, yMin);
        }

        yInc = (ptNWY - ptSEY) / 100.0;

        for (i = 0; i <= 100; i++)
        {
            tempX = ptSEX;
            tempY = ptSEY + i * yInc;

            Transform(&tempX, &tempY);

            xMax = max(tempX, xMax);
            xMin = min(tempX, xMin);
            yMax = max(tempY, yMax);
            yMin = min(tempY, yMin);
        }

        if(dimension == MgCoordinateDimension::XYZ)
        {
            double zMax = 0.0;
            double zMin = 0.0;

            zMax = max(ptUpperRightZ, ptLowerLeftZ);
            zMin = min(ptUpperRightZ, ptLowerLeftZ);

            lowerLeftTarget = new MgCoordinateXYZ(xMin, yMin, zMin);
            upperRightTarget = new MgCoordinateXYZ(xMax, yMax, zMax);
        }
        else
        {
            lowerLeftTarget = new MgCoordinateXY(xMin, yMin);
            upperRightTarget = new MgCoordinateXY(xMax, yMax);
        }

        pEnvelope = new MgEnvelope(lowerLeftTarget, upperRightTarget);
    }
    else if((m_pCsSource->GetType() == MgCoordinateSystemType::Geographic) && (m_pCsTarget->GetType() == MgCoordinateSystemType::Projected))
    {
        // This is a geographic to projected transform.
        double ptUpperRightX = upperRight->GetX();
        double ptUpperRightY = upperRight->GetY();
        double ptUpperRightZ = upperRight->GetZ();
        double ptLowerLeftX = lowerLeft->GetX();
        double ptLowerLeftY = lowerLeft->GetY();
        double ptLowerLeftZ = lowerLeft->GetZ();

        int dimension = upperRight->GetDimension();

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
        tempX = lon;
        tempY = ptSEY;
        Transform(&tempX, &tempY);

        double xMax = tempX;
        double xMin = tempX;
        double yMax = tempY;
        double yMin = tempY;

        // Bottom right
        tempX = (lon + xDist / 2);
        tempY = ptSEY;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Bottom left
        tempX = (lon - xDist / 2);
        tempY = ptSEY;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Top center
        tempX = lon;
        tempY = ptNWY;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Top right
        tempX = (lon + xDist / 2);
        tempY = ptNWY;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Top left
        tempX = (lon - xDist / 2);
        tempY = ptNWY;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Middle center
        tempX = lon;
        tempY = lat;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Middle right
        tempX = (lon + xDist / 2);
        tempY = lat;
        Transform(&tempX, &tempY);

        xMax = max(tempX, xMax);
        xMin = min(tempX, xMin);
        yMax = max(tempY, yMax);
        yMin = min(tempY, yMin);

        // Middle left
        tempX = (lon - xDist / 2);
        tempY = lat;
        Transform(&tempX, &tempY);

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

            lowerLeftTarget = new MgCoordinateXYZ(xMin, yMin, zMin);
            upperRightTarget = new MgCoordinateXYZ(xMax, yMax, zMax);
        }
        else
        {
            lowerLeftTarget = new MgCoordinateXY(xMin, yMin);
            upperRightTarget = new MgCoordinateXY(xMax, yMax);
        }

        pEnvelope = new MgEnvelope(lowerLeftTarget, upperRightTarget);

    }
    else
    {
        lowerLeftTarget = Transform(lowerLeft);
        upperRightTarget = Transform(upperRight);

        pEnvelope = new MgEnvelope(lowerLeftTarget, upperRightTarget);
    }

    if (!pEnvelope)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")

    // Leave commented except for temporary debugging.  The printf output gets
    // sent to stdout and ends up being included in the web-tier HTTP response.
/*
    Ptr<MgCoordinate> ll = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> ur = envelope->GetUpperRightCoordinate();
    printf("Transform(envelope) -- Source: %f,%f to %f,%f\n", ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());

    ll = pEnvelope->GetLowerLeftCoordinate();
    ur = pEnvelope->GetUpperRightCoordinate();
    printf("Transform(envelope) -- Dest  : %f,%f to %f,%f\n", ll->GetX(), ll->GetY(), ur->GetX(), ur->GetY());
*/

    return pEnvelope;
}

//MgDisposable
void CCoordinateSystemTransform::Dispose()
{
    delete this;
}

//Gets a copy of the source coordinate system of the converter (will
//be set to NULL if none has been set).  Caller is responsible for
//freeing the returned pointer via Release().
//
MgCoordinateSystem* CCoordinateSystemTransform::GetSource()
{
    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.GetSource", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    return SAFE_ADDREF(m_pCsSource.p);
}

//Gets a copy of the destination coordinate system of the converter
//(will be set to NULL if none has been set).  Caller is responsible
//for freeing the returned pointer via Release().
//
MgCoordinateSystem* CCoordinateSystemTransform::GetTarget()
{
    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.GetTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    return SAFE_ADDREF(m_pCsTarget.p);
}

//Sets the source and target coordinate systems of the converter.
//The converter will copy any necessary information out of the provided
//coordinate systems (rather than assuming ownership), so the caller is
//free to dispose of them after the function returns.  Upon obtaining an
//MgCoordinateSystem object from a catalog, this function
//must be successfully called before any conversions can be made with the
//object.
//Throws an exception if there's a
//problem with one of the input coordinate systems (for example, if they
//haven't been properly initialized).
//
void CCoordinateSystemTransform::SetSourceAndTarget(MgCoordinateSystem* pSource, MgCoordinateSystem* pTarget)
{
    MG_TRY()

    if (NULL == pSource || NULL == pTarget)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    bool bValid;

    //Make sure the source coordinate system is valid
    bValid = pSource->IsValid();
    if (!bValid)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure the destination coordinate system is valid
    bValid = pTarget->IsValid();
    if (!bValid)
    {
        throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Build Mentor structs out of the source and destination
    cs_Csprm_ srcCsprm, dstCsprm;
    bool bResult = BuildCsprmFromInterface(pSource, srcCsprm);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    bResult = BuildCsprmFromInterface(pTarget, dstCsprm);
    if (!bResult)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Attempt to set up a datum conversion.
    CriticalClass.Enter();
    cs_Dtcprm_ *pDtcprm = CS_dtcsu(&srcCsprm, &dstCsprm, 0, cs_DTCFLG_BLK_W);
    CriticalClass.Leave();
    if (NULL == pDtcprm)
    {
        //This is a legal result from Mentor.  It means there
        //was a problem setting up the datum conversion.
        //This can easily happen if, for example, it's a NADCON
        //datum shift and Mentor couldn't find the NADCON
        //data files at run time.
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemGeodeticTransformationSetupException", NULL);
    }

    //Okay, we're all set!  Uninitialize ourselves, and
    //set our data members.
    Uninitialize();
    m_pCsSource = SAFE_ADDREF(pSource);
    m_pCsTarget = SAFE_ADDREF(pTarget);
    m_pDtcprm = pDtcprm;
    m_src = srcCsprm;
    m_dst = dstCsprm;
    assert(IsInitialized());

    // Optimization
    // If the source and target coordinate systems are the same then the transformation methods should just return
    m_bSourceTargetSame = m_pCsSource->IsSameAs(m_pCsTarget);

    //And return success!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.SetSourceAndTarget")
}

//Tests the specified X,Y point for validity in the source coordinate
//system.  Returns false in any of the following cases:  the source
//coordinate system has not properly been set; the specified point is
//not mathematically valid for the source coordinate system; the
//coordinate system has a defined useful range, the converter has
//range checking turned on, and the specified point lies outside that
//range.  Otherwise, returns true.
//Throws an exception if the source
//coordinate system has not been successfully set.
//
bool CCoordinateSystemTransform::IsValidSourcePoint(double x, double y)
{
    return IsValidPoint(m_src, x, y, 0.);
}

//-----------------------------------------------------------------------------
bool CCoordinateSystemTransform::IsValidSourcePoint(double x, double y, double z)
{
    return IsValidPoint(m_src, x, y, z);
}

//Same as IsValidSourcePoint(), except tests with the destination
//coordinate system instead of the source.
//
bool CCoordinateSystemTransform::IsValidTargetPoint(double x, double y)
{
    return IsValidPoint(m_dst, x, y, 0.);
}

//-----------------------------------------------------------------------------
bool CCoordinateSystemTransform::IsValidTargetPoint(double x, double y, double z)
{
    return IsValidPoint(m_dst, x, y, z);
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double* x, double* y)
{
    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    if (NULL == x || NULL == y)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert the point
    TransformPoint(*x, *y, NULL);

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double x[], double y[], int arraySize)
{
    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    if (NULL == x || NULL == y)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    assert(IsInitialized());
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    bool isGeographic = MgCoordinateSystemType::Geographic == m_pCsSource->GetType();

    double lonMin = m_pCsSource->GetLonMin();
    double lonMax = m_pCsSource->GetLonMax();
    double latMin = m_pCsSource->GetLatMin();
    double latMax = m_pCsSource->GetLatMax();

    for(int i=0;i<arraySize;i++)
    {
        TransformPointInternal(x[i], y[i], NULL, isGeographic, lonMin, lonMax, latMin, latMax);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double* x, double* y, double* m)
{
    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != m);
    if (NULL == x || NULL == y || NULL == m)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    TransformPoint(*x, *y, NULL);
    *m = (*m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double x[], double y[], double m[], int arraySize)
{
    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != m);
    if (NULL == x || NULL == y || NULL == m)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    for(int i=0;i<arraySize;i++)
    {
        TransformPoint(x[i], y[i], NULL);
        m[i] = (m[i])*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double* x, double* y, double* z)
{
    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    if (NULL == x || NULL == y || NULL == z)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Convert the point
    TransformPoint(*x, *y, z);

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double x[], double y[], double z[], int arraySize)
{
    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    if (NULL == x || NULL == y || NULL == z)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    for(int i=0;i<arraySize;i++)
    {
        //Convert the point
        TransformPoint(x[i], y[i], &z[i]);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double* x, double* y, double* z, double* m)
{
    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    assert(NULL != m);
    if (NULL == x || NULL == y || NULL == z || NULL == m)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    TransformPoint(*x, *y, z);
    *m = (*m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double x[], double y[], double z[], double m[], int arraySize)
{
    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    assert(NULL != m);
    if (NULL == x || NULL == y || NULL == z || NULL == m)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.TransformM", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    for(int i=0;i<arraySize;i++)
    {
        TransformPoint(x[i], y[i], &z[i]);
        m[i] = (m[i])*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

///////////////////////////////////////////////////////////////////////////
MgLineString* CCoordinateSystemTransform::GridLine (MgCoordinate* fromPnt,MgCoordinate* toPnt,
                                                                          double curvePrecision,
                                                                          UINT32 maxPoints)
{
    MgGeometryFactory factory;

    Ptr<MgCoordinateCollection> coordinateCollection;
    Ptr<MgLineString> lineString;
    
    struct rx_Linseg_
    {
        struct rx_Linseg_ *next;
        double srcX;
        double srcY;
        double trgX;
        double trgY;
    };

    unsigned segCnt;
    struct rx_Linseg_ *segList = 0;
    struct rx_Linseg_ *curPtr = 0;
    struct rx_Linseg_ *nxtPtr = 0;

    double num, denom;
    double newX, newY;
    double delX, delX2, delY, delY2;
    double dist2, chord2, maxChord;

    double wrkPntX, wrkPntY;
    double midSrcX, midSrcY;
    double midTrgX, midTrgY;

//TODO: This algorithm calls Transform, which can and does throw.  There are no provisions for this in this algorithm.

    chord2 = curvePrecision * curvePrecision;

    // Allocate and populate the first point in the segment list.
    segList = new rx_Linseg_;
    segList->next = NULL;

    wrkPntX = segList->srcX = segList->trgX = fromPnt->GetX ();
    wrkPntY = segList->srcY = segList->trgY = fromPnt->GetY ();
    Transform (&segList->trgX,&segList->trgY);

    // Allocate and populate the last point in the segment list.
    curPtr = new rx_Linseg_;
    curPtr->next = NULL;
    segList->next = curPtr;

    wrkPntX = curPtr->srcX = curPtr->trgX = toPnt->GetX ();
    wrkPntY = curPtr->srcY = curPtr->trgY = toPnt->GetY ();
    Transform (&curPtr->trgX,&curPtr->trgY);

    // We now have the two end points of the line in the segment list.  Start a
    // loop which will examine the segment list and add points where necessary
    // to make sure the chord distance from the real point to the segment point
    // is less than that required.
    segCnt = 2;
    do
    {
        // Loop through the list and bisect each segment as appropriate.
        curPtr = segList;
        maxChord = 0.0;
        do
        {
            // Compute the midpoint of the segment in lin coordinates.
            delX = curPtr->next->srcX - curPtr->srcX;
            delY = curPtr->next->srcY - curPtr->srcY;
            midTrgX = midSrcX = curPtr->srcX + delX * 0.5;
            midTrgY = midSrcY = curPtr->srcY + delY * 0.5;
            Transform (&midTrgX,&midTrgY);

            // Compute the distance from the converted point to the line, doing all
            // of this in dwg coordinates.  The result is the chord distance.
            // The following algorithm was derived from taking the intersection of a
            // line from the converted point perdendicular to the existing line.
            delX = curPtr->next->trgX - curPtr->trgX;
            delY = curPtr->next->trgY - curPtr->trgY;
            delX2 = delX * delX;
            delY2 = delY * delY;
            denom = delX2 + delY2;

            // Make sure we don't divide by zero.  denom is the sum of two
            // squares, so it can't be negative.
            if (denom > 0.0)
            {
                num = (delX2 * midTrgX) + (delY2 * curPtr->trgX) +
                      (midTrgY - curPtr->trgY) * delX * delY;
                newX = num / denom;
                num = (delY2 * midTrgY) + (delX2 * curPtr->trgY) +
                      (midTrgX - curPtr->trgX) * delX * delY;
                newY = num / denom;
                delX = newX - midTrgX;
                delY = newY - midTrgY;
                dist2 = delX * delX + delY * delY;
            }
            else
            {
                // This is overkill unless the drawing units are lat/longs or something
                // else that is very weird.
                delX = curPtr->trgX - midTrgX;
                delY = curPtr->trgY - midTrgY;
                dist2 = delX * delX + delY * delY;
            }

            // Accumulate the maximum chord distance so that we know when we are done.
            if (dist2 > maxChord) maxChord = dist2;

            // If the chord distance (it's not really a chord, but I don't know what
            // the real name of this thing is) is greater than the requested maximum,
            // we bisect this line segment.
            if (dist2 > chord2)
            {
                // Allocate a new segment structure.
                nxtPtr = new rx_Linseg_;
                nxtPtr->next = NULL;

                // Insert the computed mid point.  We have both the dwg and lin values
                // available.
                nxtPtr->trgX = midTrgX;
                nxtPtr->trgY = midTrgY;
                nxtPtr->srcX = midSrcX;
                nxtPtr->srcY = midSrcY;

                // Count this point.
                segCnt += 1;

                // Link this point into the list.
                nxtPtr->next = curPtr->next;
                curPtr->next = nxtPtr;

                // Move on to the next segment.
                curPtr = nxtPtr->next;
            }
            else
            {
                // Move on to the next segment.
                curPtr = curPtr->next;
            }
        } while (curPtr->next != NULL);
    } while (maxChord > chord2 && segCnt < maxPoints);

    // Add all of our dwg points to the provided point array.
    coordinateCollection = new MgCoordinateCollection ();
    for (curPtr = segList;curPtr != NULL;curPtr = curPtr->next)
    {
        MgCoordinate* pntPtr = factory.CreateCoordinateXY (curPtr->trgX,curPtr->trgY);
        coordinateCollection->Add (pntPtr);
    }
    lineString = factory.CreateLineString (coordinateCollection);

    // Release all resources.
    curPtr = segList;
    while (curPtr != NULL)
    {
        nxtPtr = curPtr->next;
        delete curPtr;
        curPtr = nxtPtr;
    }
    return lineString.Detach ();
}
// Returns integer status, two values currently supported.  Zero return
// indicates that no position is returned for whatever reason.  Zero
// return indicates that a position is returend.
INT32 CCoordinateSystemTransform::PositionOfValue (MgCoordinate* position,double ordinateValue,INT32 orientation,MgCoordinate* from,MgCoordinate* to)
{
    INT32 status;

    double ratio;
    double xx, yy;
    double deltaX, deltaY;
    double minValue, maxValue;
    double tmpDbl;

    Ptr<MgCoordinate> gridFrom;
    Ptr<MgCoordinate> gridTo;

    MG_TRY ()
        status = 0;

        // Convert the from and to points to the grid coordinate system.
        gridFrom = Transform (from);
        gridTo = Transform (to);

        // Extract the from and to ordinate values which will match the
        // ordinate value we've been provided.
        if (orientation == MgCoordinateSystemGridOrientation::EastWest)
        {
            // Here if value is a easting ordinate.
            minValue = gridFrom->GetX ();
            maxValue = gridTo->GetX ();
        }
        else if (orientation == MgCoordinateSystemGridOrientation::NorthSouth)
        {
            // Here if value is a northing ordinate.
            minValue = gridFrom->GetY ();
            maxValue = gridTo->GetY ();
        }
        else
        {
            // TODO: Parameter error.
        }
        if (minValue > maxValue)
        {
            // We swap the points and min/max values to preserve our sanity.
            // This makes the rest of this code almost trivial.
            tmpDbl = minValue;
            minValue = maxValue;
            maxValue = tmpDbl;

            xx = gridFrom->GetX ();
            gridFrom->SetX (gridTo->GetX ());
            gridTo->SetX (xx);

            yy = gridFrom->GetY ();
            gridFrom->SetY (gridTo->GetY ());
            gridTo->SetY (yy);
        }

        // To prevent duplicate tics, we place a tick at the minPoint if appropriate,
        // but never on the max point.  If this 'if' fails, there are no instances of
        // the provided ordinate value on the line provided.
        if (ordinateValue >= minValue && ordinateValue < maxValue)
        {
            deltaX = gridTo->GetX () - gridFrom->GetX ();
            deltaY = gridTo->GetY () - gridFrom->GetY ();
            // We do not try to place northing ticks on a line which more horizontal than it
            // is vertical; and vice versa.
            if ((deltaX > deltaY) && (orientation == MgCoordinateSystemGridOrientation::EastWest) ||
                (deltaY > deltaX) && (orientation == MgCoordinateSystemGridOrientation::NorthSouth))
            {
                // Use some simple geomtery to calculate the approximate position of the
                // the point on the provided line segment at which the indicated ordinate
                // approximates the provided ordinate value.
                ratio = (ordinateValue - minValue) / (maxValue - minValue);
                xx = gridFrom->GetX () + deltaX * ratio;
                yy = gridFrom->GetY () + deltaY * ratio;

                // This object does not, currently, support this inverse
                // function.  It does, however, have all the information
                // necessary to support one.  So, next on the TODO list is
                // to write a private TransformInverse function.
                TransformInverse (xx,yy);

                // Frame temp is now an approximation (usually a pretty good
                // one) of the position of the provided ordinate value on the
                // provided line in frame corodinates.  We _could_ tighten this
                // up a little but here; but for know we'll just go with this
                // approximation until such time as we decide we like this
                // algorithm well enough to invest more resources in it.
                position->SetX (xx);
                position->SetY (yy);
                status = 0;
            }
            else
            {
                status = 2;
            }
        }
        else
        {
            status = 1;
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
    return status;
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

inline void CCoordinateSystemTransform::TransformPointInternal(double& x, double& y, double *pdZ, bool isGeographic,
                                                               double lonMin, double lonMax, double latMin, double latMax)
{
    m_nTransformStatus = TransformOk;

    //If range checking is turned on, find out whether
    //we'll need to warn the caller later on.
    double z=0.;
    bool bWarn = false;
    int nResult = cs_CNVRT_NRML;
    if (pdZ)
    {
        z=*pdZ;
    }

    //Do the conversion to lat/long
    double dCoords[3] = { x, y, z };

    // Is the coordinate system geographic?
    if((m_bIgnoreOutsideDomainWarning) && isGeographic)
    {
        // If so, adjust the coordinates that are outside the legal extents because Mentor automatically
        // adjusts them which ends up clipping data.
        if(dCoords[0] > lonMax)
        {
            dCoords[0] = lonMax;
        }

        if(dCoords[0] < lonMin)
        {
            dCoords[0] = lonMin;
        }

        if(dCoords[1] > latMax)
        {
            dCoords[1] = latMax;
        }

        if(dCoords[1] < latMin)
        {
            dCoords[1] = latMin;
        }
    }

    if (!pdZ)
    {
        nResult = CS_cs2ll(&m_src, dCoords, dCoords);
    }
    else
    {
        nResult = CS_cs3ll(&m_src, dCoords, dCoords);
    }

    if(cs_CNVRT_NRML != nResult)
    {
        m_nTransformStatus = TransformOutsideDomainWarning;
    }

    //dCoords[] now contains the point converted from the
    //source coordinate system to lat/longs, but still in
    //the source coordinate system's datum.  Shift it to
    //the destination coordinate system's datum.
    assert(NULL != m_pDtcprm);
    INT32 nResultShift = GeodeticTransformationPoint(m_pDtcprm, dCoords[0], dCoords[1], pdZ?&dCoords[2]:NULL);
    if (1==nResultShift)
    {
        //partial failure
        m_nTransformStatus = TransformDatumShiftWarning;
    }
    else if (-1==nResultShift)
    {
        //total failure
        m_nTransformStatus = TransformTotalFailure;
    }

    // Check for total failure
    if(TransformTotalFailure != m_nTransformStatus)
    {
        //dCoords[] now contains a lat/long point in the destination
        //datum.  Convert it to the destination coordinate system.
        if (!pdZ)
        {
            nResult = CS_ll2cs(&m_dst, dCoords, dCoords);
        }
        else
        {
            nResult = CS_ll3cs(&m_dst, dCoords, dCoords);
        }

        if(cs_CNVRT_NRML != nResult)
        {
            m_nTransformStatus = TransformOutsideDomainWarning;
        }

        //dCoords[] now contains the completely converted point.
        //Copy it into our output parameters.
        x = dCoords[0];
        y = dCoords[1];
        if (pdZ)
        {
            *pdZ=dCoords[2];
        }
    }

    // Check status in order of severity
    // Check if warnings are ignored
    // Throw the exception if needed
    if(TransformTotalFailure == m_nTransformStatus)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.TransformPointInternal", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
    }

    if((TransformDatumShiftWarning == m_nTransformStatus) && (!m_bIgnoreDatumShiftWarning))
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.TransformPointInternal", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
    }

    if((TransformOutsideDomainWarning == m_nTransformStatus) && (!m_bIgnoreOutsideDomainWarning))
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.TransformPointInternal", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }
}
int CCoordinateSystemTransform::TransformInverse (double& xx,double& yy)
{
    int st;
    double xy [3];
    double ll [3];
    
    xy [0] = xx;
    xy [1] = yy;
    xy [2] = 0.0;
    
    st = CS_cs2ll (&m_dst,ll,xy);
    if (st >= 0)
    {
        CriticalClass.Enter();
        st = CS_dtcvt(m_pDtcprm,ll,ll);
        CriticalClass.Leave();
        if (st >= 0)
        {
            st = CS_ll2cs (&m_src,xy,ll);
        }
    }
    if (st >= 0)
    {
        xx = xy [0];
        yy = xy [1];
    }
    return st;
}

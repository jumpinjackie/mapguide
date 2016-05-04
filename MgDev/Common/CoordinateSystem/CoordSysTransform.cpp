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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"
#include "CriticalSection.h"

#include "CoordSysTransform.h"          //for CCoordinateSystemTransform
#include "CoordSysUtil.h"               //for CsDictionaryOpenMode
#include "MentorUtil.h"                 //for IsLegalMentorName

using namespace CSLibrary;

#ifdef _WIN32
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
    m_bSrcIsGeographic(false),
    m_bDatumXfrmIsNull(false),
    m_bIsReentrant(false),
    m_bIgnoreDatumShiftWarning(false),
    m_bIgnoreOutsideDomainWarning(false),
    m_bSourceTargetSame(false),
    m_nTransformStatus(TransformOk),
    m_nSourceCount(0),
    m_nDatumCount(0),
    m_nTargetCount(0)
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

   	m_bSrcIsGeographic   = false;
    m_bDatumXfrmIsNull   = false;
    m_bIsReentrant = false;

    memset(&m_src, 0, sizeof(m_src));
    memset(&m_dst, 0, sizeof(m_dst));
    assert(!IsInitialized());
}

//-----------------------------------------------------------------------------
bool CCoordinateSystemTransform::IsValidPoint(cs_Csprm_& csprm, double x, double y, double z)
{
    bool bIsValid = true;
    int nResult;

    MG_TRY()

    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.IsValidPoint", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    //Test the point
    double dCoords3d[3] = { x, y, z };
    
    if (CS_isCsPrmReentrant (&csprm))
    {
        nResult = CS_xychk(&csprm, 1, &dCoords3d);
    }
    else
    {
        CriticalClass.Enter();
        nResult = CS_xychk(&csprm, 1, &dCoords3d);
        CriticalClass.Leave();
    }

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

    //Convert the point
    INT32 status;
    double dCoords [3] = {x, y, 0.0};

    // Optimization
    if(m_bSourceTargetSame)
    {
        status = TransformOk;
    }
    else
    {
        if (m_bIsReentrant)
        {
            status = TransformPoint2D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint2D (dCoords);
            CriticalClass.Leave();
        }
    }
    m_nTransformStatus = status;

    /* Make sure we throw any exception that needs to be thrown before we
       allocate the new MgCoordinate object. */
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    /* OK, construct the converted coordinate in MgCoordinate form.
       Note that user will own this point and need to delete/release it,
       whatever it is you do with MgCoordinate objects. */
    pCoordinate = new MgCoordinateXY(dCoords [0], dCoords [1]);
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

    //Convert the point
    INT32 status;
    double lclM (m);
    double dCoords [3] = {x, y, 0.0};

    // Optimization
    if(m_bSourceTargetSame)
    {
        status = TransformOk;
    }
    else
    {
        if (m_bIsReentrant)
        {
            status = TransformPoint2D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint2D (dCoords);
            CriticalClass.Leave();
        }
        lclM *= (m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    }
    m_nTransformStatus = status;

    /* Make sure we throw any exception that needs to be thrown before we
       allocate the new MgCoordinate object. */
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    pCoordinate = new MgCoordinateXYM(dCoords [0], dCoords [1], lclM);
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

    //Convert the point
    INT32 status;
    double dCoords [3] = {x, y, z};

    // Optimization
    if(m_bSourceTargetSame)
    {
        status = TransformOk;
    }
    else
    {
        if (m_bIsReentrant)
        {
            status = TransformPoint3D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint3D (dCoords);
            CriticalClass.Leave();
        }
    }
    m_nTransformStatus = status;

    /* Make sure we throw any exception that needs to be thrown before we
       allocate the new MgCoordinate object. */
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    pCoordinate = new MgCoordinateXYZ(dCoords [0], dCoords [1], dCoords [2]);
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
    INT32 status;
    double lclM (m);
    double dCoords [3] = {x, y, z};

    // Optimization
    if(m_bSourceTargetSame)
    {
        status = TransformOk;
    }
    else
    {
        if (m_bIsReentrant)
        {
            status = TransformPoint3D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint3D (dCoords);
            CriticalClass.Leave();
        }
        lclM *= (m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    }
    m_nTransformStatus = status;

    /* Make sure we throw any exception that needs to be thrown before we
       allocate the new MgCoordinate object. */
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    pCoordinate = new MgCoordinateXYZM(dCoords [0], dCoords [1], dCoords [2], lclM);
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
    INT32 status;
    double m;
    double dCoords [3];

    MG_TRY()

    CHECKARGUMENTNULL(coordinate, L"MgCoordinateSystemTransform.TransformCoordinate");

    m_nTransformStatus = TransformOk;           // until we know different

    // Since we're converting inplace, if the source and destination are the
    // same systems, we have nothing to do.  Let's do nothing, very fast.
    if (m_bSourceTargetSame)
    {
        return;
    }

    // Need to determine type of coordinate to transform
    status = TransformTotalFailure;             // until we know different.
    if(coordinate->GetDimension() == MgCoordinateDimension::XY)
    {
        // 2D transform -- As this is, perhaps, the most frequent, we do
        // this option first.
        dCoords [0] = coordinate->GetX();
        dCoords [1] = coordinate->GetY();
        dCoords [2] = 0.0;
        if (m_bIsReentrant)
        {
            status = TransformPoint2D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint2D (dCoords);
            CriticalClass.Leave();
        }
        coordinate->SetX(dCoords [0]);
        coordinate->SetY(dCoords [1]);
    }
    else if(coordinate->GetDimension() == MgCoordinateDimension::XYZ)
    {
        // 3D transform -- Second most frequent, perhaps, so we do this second.
        dCoords [0] = coordinate->GetX();
        dCoords [1] = coordinate->GetY();
        dCoords [2] = coordinate->GetZ();
        if (m_bIsReentrant)
        {
            status = TransformPoint3D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint3D (dCoords);
            CriticalClass.Leave();
        }
        coordinate->SetX(dCoords [0]);
        coordinate->SetY(dCoords [1]);
        coordinate->SetZ(dCoords [2]);
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XY | MgCoordinateDimension::M))
    {
        // 2D transform + measure -- Rarely used
        dCoords [0] = coordinate->GetX();
        dCoords [1] = coordinate->GetY();
        dCoords [2] = 0.0;
        m = coordinate->GetM();
        if (m_bIsReentrant)
        {
            status = TransformPoint2D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint2D (dCoords);
            CriticalClass.Leave();
        }
        m *= (m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
        coordinate->SetX(dCoords [0]);
        coordinate->SetY(dCoords [1]);
        coordinate->SetM(m);
    }
    else if(coordinate->GetDimension() == (MgCoordinateDimension::XYZ | MgCoordinateDimension::M))
    {
        // 3D transform + measure --  Rarely used
        dCoords [0] = coordinate->GetX();
        dCoords [1] = coordinate->GetY();
        dCoords [2] = coordinate->GetZ();
        m = coordinate->GetM();
        if (m_bIsReentrant)
        {
            status = TransformPoint3D (dCoords);
        }
        else
        {
            CriticalClass.Enter();
            status = TransformPoint3D (dCoords);
            CriticalClass.Leave();
        }
        m *= (m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
        coordinate->SetX(dCoords [0]);
        coordinate->SetY(dCoords [1]);
        coordinate->SetZ(dCoords [2]);
        coordinate->SetM(m);
    }
    else
    {
        // What dimension is this?
        throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.TransformCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    if (status != TransformOk)
    {
        InterpretStatus (status);
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

    CHECKARGUMENTNULL(coordinate, L"MgCoordinateSystemTransform.Transform");
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
/// Transforms the specified source envelope and returns a new envelope.
///</summary>
///<param name="envelope">
/// The MgEnvelope to be transformed.
///</param>
///<returns>
/// A new MgEnvelope transformed from the specified envelope.
///</returns>
///<remarks>
///By its definition, an envelope is a rectangle whose sides are parallel
///to the X and Y axes of the coordinate system to which the coordinate
///values are reference the extents of which define the extents of the
///collection of geometries associated with the envelope.  The transformed
///geometry is an approximation (a very good one we hope) of an envelope
///in a new and different coordinate system.  Since a transformation may
///include some rotation (among other transofrmation affects), the resulting
///envelop is often enlarged to meet the above described requirements.
///</remarks>
MgEnvelope* CCoordinateSystemTransform::Transform(MgEnvelope* envelope)
{
    // The following value puts limit on the number of points generated
    // by the GridLine function which is used below.  The value is not an
    // actual limit, but more a threshold which when exceeded the algorithm
    // start a clean shut down.  Thus, there may be as many as twice this
    // number of points in a line returned by GridLine.  To increase the
    // accuracy of this function, increase the value of this constant.  Of
    // course, you'll pay a penalty in performance and memory usage.
    static const INT32 maxPoints = 127;

    INT32 dimension;

    double tmpDbl;
    double magnitude;
    double xMax = -HUGE_VAL;
    double yMax = -HUGE_VAL;
    double zMax = -HUGE_VAL;
    double xMin = HUGE_VAL;
    double yMin = HUGE_VAL;
    double zMin = HUGE_VAL;

    Ptr<MgEnvelope> pEnvelope;
    Ptr<MgCoordinate> xyPoint;
    Ptr<MgLineString> lineString;
    Ptr<MgCoordinateIterator> stringItr;

    // Leave commented except for temporary debugging.  The printf output gets
    // sent to stdout and ends up being included in the web-tier HTTP response.
    // printf("\nTransform(envelope) -- Source: %S  Dest: %S\n", m_pCsSource->GetCsCode().c_str(), m_pCsTarget->GetCsCode().c_str());

    MG_TRY()

    CHECKARGUMENTNULL(envelope, L"MgCoordinateSystemTransform.Transform");

    // Extract the two defining corners of this envelope.
    Ptr<MgCoordinate> lowerLeft  = envelope->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> upperRight = envelope->GetUpperRightCoordinate();

    // Determine if this is a 3D envelope.  Don't know that this is possible,
    // but the code I modified handles 3D envelopes, so I will too.
    dimension = upperRight->GetDimension();
    if (dimension == MgCoordinateDimension::XYZ)
    {
        // Extract the Z minimum and maximum.
        tmpDbl = lowerLeft->GetZ ();
        if (tmpDbl < zMin) zMin = tmpDbl;
        if (tmpDbl > zMax) zMax = tmpDbl;

        tmpDbl = upperRight->GetZ ();
        if (tmpDbl < zMin) zMin = tmpDbl;
        if (tmpDbl > zMax) zMax = tmpDbl;
    }

    // Determine a value which represents a good tolerance value for the
    // GridLine function.  A smaller value will provide greater accuracy
    // at the expense of performance.
    xyPoint = Transform (lowerLeft);
    magnitude = fabs (xyPoint->GetX ());
    tmpDbl = fabs (xyPoint->GetY ());
    if (tmpDbl > magnitude) magnitude = tmpDbl;
    xyPoint = Transform (upperRight);
    tmpDbl = fabs (xyPoint->GetX ());
    if (tmpDbl > magnitude) magnitude = tmpDbl;
    tmpDbl = fabs (xyPoint->GetY ());
    if (tmpDbl > magnitude) magnitude = tmpDbl;
    magnitude *= 1.0E-07;               // Make this constant even smaller if more accuracy is required.

    // Extract the other two corners of the envelope.
    Ptr<MgCoordinate> upperLeft  = new MgCoordinateXY (lowerLeft->GetX (),upperRight->GetY ());
    Ptr<MgCoordinate> lowerRight = new MgCoordinateXY (upperRight->GetX (),lowerLeft->GetY ());

    // Generate line strings from one corner to the next.  Magnitude is the
    // maximum distance between the real line and the multi-segment
    // approximation returned by GridLine.  Thus, this is a measure of the
    // accuracy.  To prevent a serious looping problem, the maxPoints
    // argument sets an upper limit on the number of points generated in the
    // the returned line string.
    lineString = GridLine (lowerLeft,lowerRight,magnitude,maxPoints);
    stringItr = lineString->GetCoordinates ();
    while (stringItr->MoveNext ())
    {
        xyPoint = stringItr->GetCurrent ();
        if (xyPoint->GetX () < xMin) xMin = xyPoint->GetX ();
        if (xyPoint->GetX () > xMax) xMax = xyPoint->GetX ();
        if (xyPoint->GetY () < yMin) yMin = xyPoint->GetY ();
        if (xyPoint->GetY () > yMax) yMax = xyPoint->GetY ();
    }

    lineString = GridLine (lowerRight,upperRight,magnitude,maxPoints);
    stringItr = lineString->GetCoordinates ();
    while (stringItr->MoveNext ())
    {
        xyPoint = stringItr->GetCurrent ();
        if (xyPoint->GetX () < xMin) xMin = xyPoint->GetX ();
        if (xyPoint->GetX () > xMax) xMax = xyPoint->GetX ();
        if (xyPoint->GetY () < yMin) yMin = xyPoint->GetY ();
        if (xyPoint->GetY () > yMax) yMax = xyPoint->GetY ();
    }

    lineString = GridLine (upperRight,upperLeft,magnitude,maxPoints);
    stringItr = lineString->GetCoordinates ();
    while (stringItr->MoveNext ())
    {
        xyPoint = stringItr->GetCurrent ();
        if (xyPoint->GetX () < xMin) xMin = xyPoint->GetX ();
        if (xyPoint->GetX () > xMax) xMax = xyPoint->GetX ();
        if (xyPoint->GetY () < yMin) yMin = xyPoint->GetY ();
        if (xyPoint->GetY () > yMax) yMax = xyPoint->GetY ();
    }

    lineString = GridLine (upperLeft,lowerLeft,magnitude,maxPoints);
    stringItr = lineString->GetCoordinates ();
    while (stringItr->MoveNext ())
    {
        xyPoint = stringItr->GetCurrent ();
        if (xyPoint->GetX () < xMin) xMin = xyPoint->GetX ();
        if (xyPoint->GetX () > xMax) xMax = xyPoint->GetX ();
        if (xyPoint->GetY () < yMin) yMin = xyPoint->GetY ();
        if (xyPoint->GetY () > yMax) yMax = xyPoint->GetY ();
    }

    // Construct an evelope of the appropriate dimensions.
    if (dimension != MgCoordinateDimension::XYZ)    // 2D
    {
        Ptr<MgCoordinate> xyLowerLeft = new MgCoordinateXY (xMin,yMin);
        Ptr<MgCoordinate> xyUpperRight = new MgCoordinateXY (xMax,yMax);
        pEnvelope = new MgEnvelope (xyLowerLeft,xyUpperRight);
    }
    else                                            // 3D
    {
        Ptr<MgCoordinate> xyzLowerLeft = new MgCoordinateXYZ (xMin,yMin,zMin);
        Ptr<MgCoordinate> xyzUpperRight = new MgCoordinateXYZ (xMax,yMax,zMax);
        pEnvelope = new MgEnvelope (xyzLowerLeft,xyzUpperRight);
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

    // Return the computed envelope.
    return pEnvelope.Detach ();
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

    CHECKARGUMENTNULL(pSource, L"MgCoordinateSystemTransform.SetSourceAndTarget");
    CHECKARGUMENTNULL(pTarget, L"MgCoordinateSystemTransform.SetSourceAndTarget");

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
    cs_Dtcprm_ *pDtcprm = CS_dtcsu(&srcCsprm, &dstCsprm, cs_DTCFLG_DAT_W, cs_DTCFLG_BLK_W);
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
    m_bSrcIsGeographic = m_pCsSource->GetType() == MgCoordinateSystemType::Geographic;
    m_bDatumXfrmIsNull = (0 == pDtcprm->xfrmCount) ||
                         (1 == pDtcprm->xfrmCount && NULL != pDtcprm->xforms[0] && cs_DTCMTH_NULLX == pDtcprm->xforms[0]->methodCode);
    m_bIsReentrant = (CS_isCsPrmReentrant (&m_src) != 0) && (CS_isDtXfrmReentrant (m_pDtcprm) != 0) && (CS_isCsPrmReentrant (&m_dst) != 0);

    // Status Accumulator;
    m_nSourceCount = 0;
    m_nDatumCount  = 0;
    m_nTargetCount = 0;

    //And return success!
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.SetSourceAndTarget")
}

// Geodetic Transformation Information
INT32 CCoordinateSystemTransform::GetGeodeticTransformationCount ()
{
    //see the ctor - when passing in NULL for either the source or the target, this guy here
    //won't get initialized
    if (NULL == this->m_pDtcprm) 
        return 0;

    return m_pDtcprm->xfrmCount;
}
MgCoordinateSystemGeodeticTransformDef* CCoordinateSystemTransform::GetGeodeticTransformation (INT32 index)
{
    if (index < 0 || index >= this->GetGeodeticTransformationCount())
        throw new MgArgumentOutOfRangeException(L"GetGeodeticTransformation.GetGeodeticTransformation",__LINE__,__WFILE__, NULL, L"", NULL);

    // indexparameter is valid, get a pointer to the appropriate
    // CS-MAP transformation object.
    cs_GxXform_ *xfrmPtr = m_pDtcprm->xforms [index];
    if (NULL == xfrmPtr)
        throw new MgCoordinateSystemInitializationFailedException(L"GetGeodeticTransformation.GetGeodeticTransformation",__LINE__,__WFILE__, NULL, L"", NULL);

    cs_GeodeticTransform_* xfrmDefPtr = &xfrmPtr->gxDef;
    if (NULL == xfrmDefPtr)
        throw new MgCoordinateSystemInitializationFailedException(L"GetGeodeticTransformation.GetGeodeticTransformation",__LINE__,__WFILE__, NULL, L"", NULL);

    // Extract the name of the transformation.
    Ptr<MgCoordinateSystemGeodeticTransformDef> catalogTransformationDef;
    wchar_t* pDefinitionName = NULL;

    MG_TRY()

        //actually, CSMAP automatically sets a name up for the transformation
        //presumably, [xfrmName] is never empty
        pDefinitionName = Convert_UTF8_To_Wide (xfrmDefPtr->xfrmName);
        bool hasCatalogDef = (NULL != pDefinitionName && L'\0' != pDefinitionName[0]);
        if (hasCatalogDef)
        {
            // We have a transformation name.  Get a copy of the named
            // transformation from the dictionary.
            MgCoordinateSystemFactory csFactory;
            Ptr<MgCoordinateSystemCatalog> pCatalog = csFactory.GetCatalog();
            Ptr<MgCoordinateSystemGeodeticTransformDefDictionary> transformationDictionary = pCatalog->GetGeodeticTransformDefDictionary();
            catalogTransformationDef = transformationDictionary->GetGeodeticTransformationDef((STRING)pDefinitionName);
        }

        //this can happen (at least) in the following case:
        //the datum dictionary contains a datum which carries geocentric transformation information with it, e.g. molodensky
        //but the transformation dictionary does not contain that guy
        //in this case, CSMAP returns a [xfrmDefPtr] but the actual (explicit) transformation definition
        //doesn't exist in the dictionaries
        if (NULL == catalogTransformationDef)
            throw new MgCoordinateSystemLoadFailedException(L"GetGeodeticTransformation.GetGeodeticTransformation",__LINE__,__WFILE__, NULL, L"", NULL);

    MG_CATCH(L"GetGeodeticTransformation.GetGeodeticTransformation")

    delete[] pDefinitionName;
    pDefinitionName = NULL;

    MG_THROW()

    return catalogTransformationDef.Detach ();
}
INT32 CCoordinateSystemTransform::GetGeodeticTransformationDirection (INT32 index)
{
   	INT32 direction (MgCoordinateSystemGeodeticDirection::GeodeticDirectionError);
    cs_GxXform_ *xfrmPtr;
   
    MgCoordinateSystemFactory csFactory;

    if (index >= 0 && index < m_pDtcprm->xfrmCount)
    {
        // indexparameter is valid, get a pointer to the appropriate
        // CS-MAP transformation object.
        xfrmPtr = m_pDtcprm->xforms [index];
        if (xfrmPtr != NULL)
        {
            if (xfrmPtr->userDirection == cs_DTCDIR_NONE)
            {
                direction = MgCoordinateSystemGeodeticDirection::GeodeticDirectionNone;
            }
            else if (xfrmPtr->userDirection == cs_DTCDIR_FWD)
            {
                direction = MgCoordinateSystemGeodeticDirection::GeodeticDirectionForward;
            }
            else if (xfrmPtr->userDirection == cs_DTCDIR_INV)
            {
                direction = MgCoordinateSystemGeodeticDirection::GeodeticDirectionInverse;
            }
        }
     }
     return direction;
}
MgCoordinateSystemGeodeticPath* CCoordinateSystemTransform::GetExplicitGeodeticPath()
{
    STRING pathDefName;
   	Ptr<MgCoordinateSystemCatalog> pCatalog;
    Ptr<MgCoordinateSystemGeodeticPath> rtnValue;
    Ptr<MgCoordinateSystemGeodeticPathDictionary> pGpDefDict;
    
    MgCoordinateSystemFactory csFactory;

    // index parameter is valid, get a pointer to the appropriate
    // CS-MAP transformation object.
    if (m_pDtcprm->pathName [0] != '\0')
    {
        wchar_t* pwszDtName = Convert_UTF8_To_Wide (m_pDtcprm->pathName);
        pathDefName = pwszDtName;
        delete[] pwszDtName;
        pCatalog = csFactory.GetCatalog();
        pGpDefDict = pCatalog->GetGeodeticPathDictionary ();
        MgDisposable* tmpPtr = pGpDefDict.p->Get(pathDefName);
        rtnValue = dynamic_cast<MgCoordinateSystemGeodeticPath*>(tmpPtr);
    }
    return rtnValue.Detach ();
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
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    //Convert the point
    INT32 status;
    double dCoords [3] = {*x, *y, 0.0};
    if (m_bIsReentrant)
    {
        status = TransformPoint2D (dCoords);
    }
    else
    {
        CriticalClass.Enter();
        status = TransformPoint2D (dCoords);
        CriticalClass.Leave();
    }
    *x = dCoords [0];
    *y = dCoords [1];
    m_nTransformStatus = status;

    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double x[], double y[], int arraySize)
{
    int idx;
    INT32 status;

    double dCoords [3];

    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");

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

    dCoords [2] = 0.0;
    m_nTransformStatus = TransformOk;
    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    /* Loop for each point.  In the normal case, pretty straight forward.
       If we get a non-normal status return, we capture the worst status
       that we encounter and keep trucking.  This represents a departure
       from the previous code, which upon an exception being thrown, would
       leave the provided arrays in a "half" converted, and "half" unconverted
       state.  Also, the previous code would (assuming that the warning
       exceptions were turned off, would simply return the last status.  This
       code will now return the "worst" status which was encountered in the
       conversion. Again, a slight departure from the previous code. */
    for (idx = 0;idx < arraySize;idx++)
    {
        dCoords [0] = x [idx];
        dCoords [1] = y [idx];

        status = TransformPoint2D (dCoords);
        x [idx] = dCoords [0];
        y [idx] = dCoords [1];
        if (TransformOk != status)
        {
            if (status > m_nTransformStatus)
            {
                m_nTransformStatus = status;
            }
        }
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }

    /* Now that we have done all of the points and the provided
       array is in a consistent state, we InterpretStatus which
       will cause an exception to be thrown if appropriate. */
    if (TransformOk != m_nTransformStatus)
    {
        InterpretStatus (m_nTransformStatus);
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
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    //Convert the point
    INT32 status;
    double dCoords [3] = {*x, *y, 0.0};
    if (m_bIsReentrant)
    {
        status = TransformPoint2D (dCoords);
    }
    else
    {
        CriticalClass.Enter();
        status = TransformPoint2D (dCoords);
        CriticalClass.Leave();
    }
    *x = dCoords [0];
    *y = dCoords [1];
    *m = (*m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
    m_nTransformStatus = status;
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double x[], double y[], double m[], int arraySize)
{
    int idx;
    INT32 status;

    double dCoords [3];

    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != m);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(m, L"MgCoordinateSystemTransform.Transform");

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

    dCoords [2] = 0.0;
    m_nTransformStatus = TransformOk;
    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    /* Loop for each point.  In the normal case, pretty straight forward.
       If we get a non-normal status return, we capture the worst status
       that we encounter and keep trucking.  This represents a departure
       from the previous code, which upon an exception being thrown, would
       leave the provided arrays in a "half" converted, and "half" unconverted
       state.  Also, the previous code would (assuming that the warning
       exceptions were turned off, would simply return the last status.  This
       code will now return the "worst" status which was encountered in the
       conversion. Again, a slight departure from the previous code. */
    for (idx = 0;idx < arraySize;idx++)
    {
        dCoords [0] = x [idx];
        dCoords [1] = y [idx];

        status = TransformPoint2D (dCoords);
        x [idx] = dCoords [0];
        y [idx] = dCoords [1];
        m[idx] = (m[idx])*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
        if (TransformOk != status)
        {
            if (status > m_nTransformStatus)
            {
                m_nTransformStatus = status;
            }
        }
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }

    /* Now that we have done all of the points and the provided
       array is in a consistent state, we InterpretStatus which
       will cause an exception to be thrown if appropriate. */
    if (TransformOk != m_nTransformStatus)
    {
        InterpretStatus (m_nTransformStatus);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double* x, double* y, double* z)
{
    INT32 status;

    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(z, L"MgCoordinateSystemTransform.Transform");

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    //Convert the point
    double dCoords [3] = {*x, *y, *z};
    if (m_bIsReentrant)
    {
        status = TransformPoint3D (dCoords);
    }
    else
    {
        CriticalClass.Enter();
        status = TransformPoint3D (dCoords);
        CriticalClass.Leave();
    }
    *x = dCoords [0];
    *y = dCoords [1];
    *z = dCoords [2];
    m_nTransformStatus = status;

    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::Transform(double x[], double y[], double z[], int arraySize)
{
    int idx;
    INT32 status;

    double dCoords [3];

    MG_TRY()

    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.Transform");
    CHECKARGUMENTNULL(z, L"MgCoordinateSystemTransform.Transform");

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

    m_nTransformStatus = TransformOk;
    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    /* Loop for each point.  In the normal case, pretty straight forward.
       If we get a non-normal status return, we capture the worst status
       that we encounter and keep trucking.  This represents a departure
       from the previous code, which upon an exception being thrown, would
       leave the provided arrays in a "half" converted, and "half" unconverted
       state.  Also, the previous code would (assuming that the warning
       exceptions were turned off, would simply return the last status.  This
       code will now return the "worst" status which was encountered in the
       conversion. Again, a slight departure from the previous code. */
    for (idx = 0;idx < arraySize;idx++)
    {
        dCoords [0] = x [idx];
        dCoords [1] = y [idx];
        dCoords [2] = z [idx];

        status = TransformPoint2D (dCoords);
        x [idx] = dCoords [0];
        y [idx] = dCoords [1];
        z [idx] = dCoords [2];
        if (TransformOk != status)
        {
            if (status > m_nTransformStatus)
            {
                m_nTransformStatus = status;
            }
        }
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }

    /* Now that we have done all of the points and the provided
       array is in a consistent state, we InterpretStatus which
       will cause an exception to be thrown if appropriate. */
    if (TransformOk != m_nTransformStatus)
    {
        InterpretStatus (m_nTransformStatus);
    }

    //And return.
    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.Transform")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double* x, double* y, double* z, double* m)
{
    INT32 status;

    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    assert(NULL != m);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(z, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(m, L"MgCoordinateSystemTransform.TransformM");

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    //Convert the point
    double dCoords [3] = {*x, *y, *z};
    if (m_bIsReentrant)
    {
        status = TransformPoint2D (dCoords);
    }
    else
    {
        CriticalClass.Enter();
        status = TransformPoint2D (dCoords);
        CriticalClass.Leave();
    }
    *x = dCoords [0];
    *y = dCoords [1];
    *z = dCoords [2];
    *m = (*m)*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());

    m_nTransformStatus = status;
    if (TransformOk != status)
    {
        InterpretStatus (status);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

///////////////////////////////////////////////////////////////////////////
void CCoordinateSystemTransform::TransformM(double x[], double y[], double z[], double m[], int arraySize)
{
    int idx;
    INT32 status;

    double dCoords [3];

    MG_TRY()
    assert(NULL != x);
    assert(NULL != y);
    assert(NULL != z);
    assert(NULL != m);
    CHECKARGUMENTNULL(x, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(y, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(z, L"MgCoordinateSystemTransform.TransformM");
    CHECKARGUMENTNULL(m, L"MgCoordinateSystemTransform.TransformM");

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

    m_nTransformStatus = TransformOk;
    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    /* Loop for each point.  In the normal case, pretty straight forward.
       If we get a non-normal status return, we capture the worst status
       that we encounter and keep trucking.  This represents a departure
       from the previous code, which upon an exception being thrown, would
       leave the provided arrays in a "half" converted, and "half" unconverted
       state.  Also, the previous code would (assuming that the warning
       exceptions were turned off, would simply return the last status.  This
       code will now return the "worst" status which was encountered in the
       conversion. Again, a slight departure from the previous code. */
    for (idx = 0;idx < arraySize;idx++)
    {
        dCoords [0] = x [idx];
        dCoords [1] = y [idx];
        dCoords [2] = z [idx];

        status = TransformPoint2D (dCoords);
        x [idx] = dCoords [0];
        y [idx] = dCoords [1];
        z [idx] = dCoords [2];
        m[idx]    = (m[idx])*(m_pCsSource->GetUnitScale() / m_pCsTarget->GetUnitScale());
        if (TransformOk != status)
        {
            if (status > m_nTransformStatus)
            {
                m_nTransformStatus = status;
            }
        }
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }

    /* Now that we have done all of the points and the provided
       array is in a consistent state, we InterpretStatus which
       will cause an exception to be thrown if appropriate. */
    if (TransformOk != m_nTransformStatus)
    {
        InterpretStatus (m_nTransformStatus);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemTransform.TransformM")
}

void CCoordinateSystemTransform::Transform2D (double xy[][2],INT32 pointCount)
{
    int idx;
    INT32 status;

    double* pDblXY;

    assert(NULL != xy);
    CHECKARGUMENTNULL(xy, L"MgCoordinateSystemTransform.Transform");
    assert(pointCount > 0);
    if (0 >= pointCount)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(IsInitialized());
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    // Loop for each point.  TransformPoint2D will accumulate the status for
    // us. We need only concern ourselves with a "TotalFailure".
    pDblXY = xy [0];
    for (idx = 0;idx < pointCount;idx++)
    {
        status = TransformPoint2D (pDblXY);
        
        // Warning status values will have already been counted.  We only need
        // to check for a total failure.  The "disable exeception" conditions
        // are considered to be on by default for this function.
        if (TransformTotalFailure <= status)
        {
            m_nTransformStatus = TransformTotalFailure;
            break;
        }
       
        // On to the next point.
        pDblXY += 2;
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }
    if (TransformTotalFailure <= status)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.InterpretStatus", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
    }
    return;
}

void CCoordinateSystemTransform::Transform3D (double xyz[][3],INT32 pointCount)
{
    int idx;
    INT32 status;

    double* pDblXYZ;

    assert(NULL != xyz);
    CHECKARGUMENTNULL(xyz, L"MgCoordinateSystemTransform.Transform");
    assert(pointCount > 0);
    if (0 >= pointCount)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    assert(IsInitialized());
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemTransform.Transform", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotReadyException", NULL);
    }

    // Optimization
    if(m_bSourceTargetSame)
    {
        return;
    }

    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }

    // Loop for each point.  TransformPoint2D will accumulate the status for
    // us. We need only concern ourselves with a "TotalFailure".
    pDblXYZ = xyz [0];
    for (idx = 0;idx < pointCount;idx++)
    {
        status = TransformPoint3D (pDblXYZ);
        
        // Warning status values will have already been counted.  We only need
        // to check for a total failure.  The "disable exeception" conditions
        // are considered to be on by default for this function.
        if (TransformTotalFailure <= status)
        {
            m_nTransformStatus = TransformTotalFailure;
            break;
        }
        
        // On to the next point.
        pDblXYZ += 3;
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }
    if (TransformTotalFailure <= status)
    {
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.InterpretStatus", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
    }
    return;
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

    double midSrcX, midSrcY;
    double midTrgX, midTrgY;

    MG_TRY()

        chord2 = curvePrecision * curvePrecision;

        // Allocate and populate the first point in the segment list.
        segList = new rx_Linseg_;
        segList->next = NULL;

        segList->srcX = segList->trgX = fromPnt->GetX ();
        segList->srcY = segList->trgY = fromPnt->GetY ();
        Transform (&segList->trgX,&segList->trgY);

        // Allocate and populate the last point in the segment list.
        curPtr = new rx_Linseg_;
        curPtr->next = NULL;
        segList->next = curPtr;

        curPtr->srcX = curPtr->trgX = toPnt->GetX ();
        curPtr->srcY = curPtr->trgY = toPnt->GetY ();
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
            Ptr<MgCoordinate> pntPtr = factory.CreateCoordinateXY (curPtr->trgX,curPtr->trgY);
            coordinateCollection->Add (pntPtr);
        }
        lineString = factory.CreateLineString (coordinateCollection);

    MG_CATCH (L"MgCoordinateSystemTransform.GridLine")

    // Release all resources.
    curPtr = segList;
    while (curPtr != NULL)
    {
        nxtPtr = curPtr->next;
        delete curPtr;
        curPtr = nxtPtr;
    }

    MG_THROW ()

    return lineString.Detach ();
}
// Given a straight line segment in source coordinate system coordinates, this
// function retuns a position precisely on that line segment such that the
// indicated ordinate in the target coordinate system equals (very closely) the
// provided value.
//
// The function was devised to position tick marks on a viewport frame boundary
// where the ordinate values selected are (of course) the ordinate values of the
// grid coordinate system.  So, while rather general in nature, some of the
// graticule/grid nomenclature has unavoidably crept into the comments describing
// this function.
//
// Returns integer status, four values currently supported.
//  0 ==> Success!  A point with the provided ordinate value did exists on the
//        provided line segment and the position of that point has been returned
//        in the 'position' argument.
//  1 ==> The provided ordinate value did not occur within the domain of the line
//        segment provided.
//  2 ==> While the provided ordinate value did occur within the domain of the line
//        segment provided, the slope of the line is such that calculation to
//        determine the desired point would be unstable and was not attempted.
// -1 ==> An exception was thrown during the calculation of the position,
//        which _usually_ indicates that the data provided is inappropriate for
//        one (or both) of the coordinates systems involved in this Transform.
//
INT32 CCoordinateSystemTransform::PositionOfValue (MgCoordinate* position,double ordinateValue,INT32 orientation,MgCoordinate* from,MgCoordinate* to)
{
    INT32 status;
    INT32 killer;

    double ratio;
    double magicFactor;
    double xx, yy;
    double trgLength;
    double deltaRatio;
    double srcDeltaX, srcDeltaY;
    double trgDeltaX, trgDeltaY;
    double minValue, maxValue;
    double tmpDbl;

    MgCoordinate* srcFrom;
    MgCoordinate* srcTo;

    Ptr<MgCoordinate> trgFrom;
    Ptr<MgCoordinate> trgTo;

    status = -1;        // Until we know different.

    // We need copies of these pointers so we can swap then as necessary.
    srcFrom = from;
    srcTo = to;

    MG_TRY ()
        // We begin our work in the target coordinate system realm.
        // Convert the from and to points to the grid coordinate system.
        trgFrom = Transform (from);
        trgTo = Transform (to);

        // Extract the from and to ordinate values which will match the
        // ordinate value we've been provided.
        if (orientation == MgCoordinateSystemGridOrientation::EastWest)
        {
            // Here if value is a easting ordinate.
            minValue = trgFrom->GetX ();
            maxValue = trgTo->GetX ();
        }
        else if (orientation == MgCoordinateSystemGridOrientation::NorthSouth)
        {
            // Here if value is a northing ordinate.
            minValue = trgFrom->GetY ();
            maxValue = trgTo->GetY ();
        }
        else
        {
            // Invalid argument value.
            throw new MgInvalidArgumentException(L"MgCoordinateSystemTransform.PositionOfValue",
                                                 __LINE__, __WFILE__, NULL, L"", NULL);
        }

        if (minValue > maxValue)
        {
            // The relationship between the source and target coordinate systems,
            // with respect to the specific ordinate value we are seeking, is
            // and inverse relationship.  To preserve our sanity, we adjust so
            // that the relationship is direct and the algorithm below gets
            // a lot simpler and thus (hopefully) more reliable.
            tmpDbl = minValue;
            minValue = maxValue;
            maxValue = tmpDbl;

            srcFrom = to;
            srcTo = from;
            trgFrom = Transform (srcFrom);
            trgTo = Transform (srcTo);
        }

        // To prevent duplicate tics, we place a tick at the minPoint if appropriate,
        // but never on the max point.  If this 'if' fails, there are no instances of
        // the provided ordinate value on the line provided.
        if (ordinateValue >= minValue && ordinateValue < maxValue)
        {
            // It appears that the desired ordinate value does indeed exist on this
            // line segment.  Until we have successfully calculated a position, the
            // return status will be 2, i.e. the thing exists but the determination
            // of its position failed.
            status = 2;

            // Get the geometry of the provided segment in target terms.  You're right,
            // assuming that this segment in target coordinates is a straight line is
            // a very bad assumption.  That's what all the code below is about.
            trgDeltaX = trgTo->GetX () - trgFrom->GetX ();
            trgDeltaY = trgTo->GetY () - trgFrom->GetY ();
            trgLength = sqrt (trgDeltaX * trgDeltaX + trgDeltaY * trgDeltaY);

            // Compute the magic factor.  This is essentially the amount we will
            // move along the source line for each unit of deviation from the
            // the provided ordinate in the traget coordinate space.  The 0.75
            // factor is to prevent an oscillation around the desired value and
            // ensure convergence of the interative calculation.  This value
            // will be the subject of fine tuning in the future.
            if (orientation == MgCoordinateSystemGridOrientation::EastWest)
            {
                magicFactor = 0.75 * (trgDeltaX / trgLength) / trgLength;
            }
            else
            {
                magicFactor = 0.75 * (trgDeltaY / trgLength) / trgLength;
            }

            srcDeltaX = srcTo->GetX () - srcFrom->GetX ();
            srcDeltaY = srcTo->GetY () - srcFrom->GetY ();

            // We do not try to place northing ticks on a line which more horizontal than it
            // is vertical; and vice versa.
            if ((trgDeltaX > trgDeltaY) && (orientation == MgCoordinateSystemGridOrientation::EastWest) ||
                (trgDeltaY > trgDeltaX) && (orientation == MgCoordinateSystemGridOrientation::NorthSouth))
            {
                // Use some simple geomtery to calculate the approximate position of the
                // the ordinateValue on the provided line segment at which the indicated
                // ordinate approximates the provided ordinate value.
                ratio = (ordinateValue - minValue) / (maxValue - minValue);

                // Ratio is the magic number we will deal with in the following iterative
                // loop.  Essentially, ratio indicates how far along the line between the
                // source 'from' point and the source 'to' point is the current estimate of
                // 'position'.  As we zero in on the desired point, we will be moving
                // 'ratio' up and down.  The point here being, as long as ratio is between
                // zero and one, we know that it represents a point which is precisely on
                // the underlying line, which in this case is the provided line in the
                // source coordinate system.  Also, the ratio approach is unitless, thus
                // this makes it easier to determine when to terminate the iterative loop.
                // Currently, we use 1 part in a million.
                killer = 12;
                deltaRatio = 0.0;
                do
                {
                    ratio += deltaRatio;
                    // Calculate the new 'position' estimate based on the current value
                    // of 'ratio'.  N
                    xx = srcFrom->GetX () + srcDeltaX * ratio;
                    yy = srcFrom->GetY () + srcDeltaY * ratio;

                    // The TransformInverse function was added as a private member as part
                    // of the RFC #76 development effort.  All the information to perform
                    // this function is present in this object.  It was made private as to
                    // do otherwise would change the public API, a change which was not
                    // approved as part of the RFC.

                    // Convert the proposed 'position' point back to the coordinates of
                    // the target system (i.e. of the ordinate value argument).
                    Transform (&xx,&yy);

                    // Make an estimate as to how much, and what direction, we need to
                    // move 'ratio' to achieve the desired result.
                    if (orientation == MgCoordinateSystemGridOrientation::EastWest)
                    {
                        deltaRatio = (ordinateValue - xx) * magicFactor;
                    }
                    else
                    {
                        deltaRatio = (ordinateValue - yy) * magicFactor;
                    }
                    killer -= 1;
                } while (fabs (deltaRatio) > 1.0E-08 && killer >= 0);

                if (killer >= 0)
                {
                    xx = srcFrom->GetX () + srcDeltaX * ratio;
                    yy = srcFrom->GetY () + srcDeltaY * ratio;
                    position->SetX (xx);
                    position->SetY (yy);
                    status = 0;
                }
            }
        }
        else
        {
            status = 1;
        }
    MG_CATCH (L"MgCoordinateSystemTransform.PositionOfValue")

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
INT32 CCoordinateSystemTransform::GetSourceWarningCount (void)
{
    return m_nSourceCount;
}
INT32 CCoordinateSystemTransform::GetdatumWarningCount (void)
{
    return m_nDatumCount;
}
INT32 CCoordinateSystemTransform::GetTargetWarningCount (void)
{
    return m_nTargetCount;
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

   // Status Accumulator;
    m_nSourceCount = 0;
    m_nDatumCount  = 0;
    m_nTargetCount = 0;
}

// Essentially the inverse of the Transform (double& xx,double& yy) function.
// This is a private function used to greatly enhance the performance of the
// PositionOfValue function defined above.
int CCoordinateSystemTransform::TransformInverse (double& xx,double& yy)
{
    int st;
    double xy [3];
    double ll [3];

    xy [0] = xx;
    xy [1] = yy;
    xy [2] = 0.0;

    if (!m_bIsReentrant)
    {
        CriticalClass.Enter();
    }
    st = CS_cs2ll (&m_dst,ll,xy);
    if (st >= 0)
    {
        st = CS_dtcvt(m_pDtcprm,ll,ll);
        if (st >= 0)
        {
            st = CS_ll2cs (&m_src,xy,ll);
        }
    }
    if (!m_bIsReentrant)
    {
        CriticalClass.Leave();
    }
    if (st >= 0)
    {
        xx = xy [0];
        yy = xy [1];
    }
    return st;
}
/* Two dimensional point conversion _without_ a critical section.  No
   exceptions thrown or handled, status return only.  Full conversion
   always calulcated and returned regardless of intermediary status.

   Status return is one of the following:
   
                      TransformOk --> Perfectly normal
    TransformOutsideDomainWarning --> Outside useful range or domain error
                                      on either of the projective phases.
                                      This is quite normal if the frequency
                                      of occurence is low.
       TransformDatumShiftWarning --> Outside the coverage of the datum
                                      transformation.  Results could be
                                      unshifted, or calculated using the
                                      fallback method.  Again, quite normal
                                      if the frequency of occurence is low.
            TransformTotalFailure --> Shouldn't ever happen.  It would be
                                      necessary to have a hard core physical
                                      I/O error or something equally rare
                                      for this to happen.
*/
INT32 CCoordinateSystemTransform::TransformPoint2D (double point[3])
{
    int srcStatus;
    int dtmStatus (0);
    int trgStatus;

    INT32 status (TransformOk);

    double lclLL [3];

    srcStatus = CS_cs2ll (&m_src,lclLL,point);
    if (!m_bDatumXfrmIsNull)
    {
        dtmStatus = CS_dtcvt(m_pDtcprm,lclLL,lclLL);
    }
    trgStatus = CS_ll2cs (&m_dst,point,lclLL);

    if ((srcStatus | dtmStatus | trgStatus) != 0)
    {
        if (srcStatus != 0)
        {
            ++m_nSourceCount;
            status = TransformOutsideDomainWarning;
        }
        if (trgStatus != 0)
        {
            ++m_nTargetCount;
            status = TransformOutsideDomainWarning;
        }
        if (dtmStatus != 0)
        {
            //NOTE: Ternary expressions will trigger "undefined reference" to these 2 constants
            //For GCC 4.1 and older
            if (dtmStatus < 0)
                status = TransformTotalFailure;
            else
                status = TransformDatumShiftWarning;
            ++m_nDatumCount;
        }
    }
    return status;
}
/* The function that follows is identical to the previous function with the
   exception that the 3D conversion methods are called. */
INT32 CCoordinateSystemTransform::TransformPoint3D (double point [3])
{
    int srcStatus;
    int dtmStatus (0);
    int dstStatus;

    INT32 status (TransformOk);

    double lclLL [3];

    srcStatus = CS_cs3ll (&m_src,lclLL,point);
    if (!m_bDatumXfrmIsNull)
    {
        dtmStatus = CS_dtcvt3D (m_pDtcprm,lclLL,lclLL);
    }
    dstStatus = CS_ll3cs (&m_dst,point,lclLL);
    if ((srcStatus | dtmStatus | dstStatus) != 0)
    {
        if (srcStatus != 0)
        {
            ++m_nSourceCount;
            status = TransformOutsideDomainWarning;
        }
        if (dstStatus != 0)
        {
            ++m_nTargetCount;
            status = TransformOutsideDomainWarning;
        }
        if (dtmStatus != 0)
        {
            //NOTE: Ternary expressions will trigger "undefined reference" to these 2 constants
            //For GCC 4.1 and older
            if (dtmStatus < 0)
                status = TransformTotalFailure;
            else
                status = TransformDatumShiftWarning;
            ++m_nDatumCount;
        }
    }
    return status;
}

// For performance reasons, this function is, usually, called only in the event
// of a non-zero status value.  Note that exceptions are usually thrown should
// status be non-zero.
void CCoordinateSystemTransform::InterpretStatus (INT32 status)
{
    if (status == TransformTotalFailure)
    {
        m_nTransformStatus = TransformTotalFailure;
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.InterpretStatus", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNoConversionDone", NULL);
    }
    else if (status == TransformDatumShiftWarning)
    {
        if (m_nTransformStatus <= TransformOutsideDomainWarning)
        {
            m_nTransformStatus = TransformDatumShiftWarning;
        }
        if (!m_bIgnoreDatumShiftWarning)
        {
            throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.InterpretStatus", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
        }
    }
    else if (status != TransformOk)
    {
        if (m_nTransformStatus <= TransformOk)
        {
            m_nTransformStatus = TransformOutsideDomainWarning;
        }
        if(!m_bIgnoreOutsideDomainWarning)
        {
            throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemTransform.InterpretStatus", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
        }
    }
}

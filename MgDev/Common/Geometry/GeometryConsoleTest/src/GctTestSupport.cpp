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
#include "Spatial/MathUtility.h"
#include "Spatial/SpatialUtility.h"
#include "GeometryConsoleTest.h"

bool GctDblCmp (double first,double second,double tolerance)
{
    bool equal (false);
    
    equal = (fabs (first - second) <= tolerance);
    return equal;
}

MgCoordinateCollection* CreateCoordinateCollection (unsigned seed,MgCoordinateSystem* crs,INT32 pointCount,bool threeD)
{
    INT32 idx;

    double deltaX;
    double deltaY;
    double coordX;
    double coordY;
    double coordZ;
    double randX;
    double randY;
    double randZ;
    double denom;
    double lonMin, latMin;
    double lonMax, latMax;

    double fromXY [2];
    double toXY [2];

    Ptr<MgCoordinate> southwestXY;
    Ptr<MgCoordinate> northeastXY;

    srand (seed);
    denom = static_cast<double>(RAND_MAX+2);

    Ptr<MgCoordinate> nextCoord;
    Ptr<MgCoordinateCollection> pointCollection;
    Ptr<MgLineString> myLineString;

    MgGeometryFactory mgFactory;

    randZ = 1.0;
    lonMin = crs->GetLonMin ();
    latMin = crs->GetLatMin ();
    lonMax = crs->GetLonMax ();
    latMax = crs->GetLatMax ();

    lonMin += fabs (lonMin * .05);
    latMin += fabs (latMin * .05);
    lonMax -= fabs (lonMax * .05);
    latMax -= fabs (latMax * .05);

    southwestXY = mgFactory.CreateCoordinateXY (lonMin,latMin);
    northeastXY = mgFactory.CreateCoordinateXY (lonMax,latMax);
    crs->ConvertCoordinateFromLonLat (southwestXY);
    crs->ConvertCoordinateFromLonLat (northeastXY);

    fromXY [0] = southwestXY->GetX () + (northeastXY->GetX () - southwestXY->GetX ()) * .25;
    fromXY [1] = southwestXY->GetY () + (northeastXY->GetY () - southwestXY->GetY ()) * .25;
    toXY [0] = southwestXY->GetX () + (northeastXY->GetX () - southwestXY->GetX ()) * .75;
    toXY [1] = southwestXY->GetY () + (northeastXY->GetY () - southwestXY->GetY ()) * .75;
    deltaX = toXY [0] - fromXY [0];
    deltaY = toXY [1] - fromXY [1];

    pointCollection = new MgCoordinateCollection ();
    for (idx = 0;idx < pointCount;idx += 1)
    {

        randX = static_cast<double>(rand ()) / denom;
        randY = static_cast<double>(rand ()) / denom;
        if (threeD)
        {
            randZ = static_cast<double>(rand ()) / denom;
        }

        coordX = fromXY [0] + deltaX * randX;
        coordY = fromXY [1] + deltaY * randY;
        if (!threeD)
        {
            nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
            pointCollection->Add (nextCoord);
        }
        else
        {
            coordZ = 1624.0 * randZ;
            nextCoord = mgFactory.CreateCoordinateXYZ (coordX,coordY,coordZ);
            pointCollection->Add (nextCoord);
        }
    }
    return pointCollection.Detach ();
}

MgPolygon* CreatePolygon (MgCoordinateSystem* crs,INT32 pointCount)
{
    INT32 idx;

    double angle;
    double delta;
    double radius;
    double coordX, coordY;

    double southwestLL [2];
    double northeastLL [2];
    double workCoord [2];

    Ptr<MgCoordinate> centerXY;
    Ptr<MgCoordinate> nextCoord;
    Ptr<MgCoordinateCollection> innerCollection;
    Ptr<MgCoordinateCollection> outerCollection;
    Ptr<MgLinearRing> outerRing;
    Ptr<MgLinearRing> innerRing;
    Ptr<MgLinearRingCollection> innerRingCollection;
    Ptr<MgPolygon> myPolygon;

    MgGeometryFactory mgFactory;

    // Determine appropriate loop parameters.  In building this collection,
    // we'll build a circle of 500 units (whatever the system unit of the
    // provided CRS is).  Actualy, we'll build a polygon which pointCount
    // sides.

    // So, to build the circle, we'll need the center point.
    southwestLL [0] = crs->GetLonMin ();
    southwestLL [1] = crs->GetLatMin ();
    northeastLL [0] = crs->GetLonMax ();
    northeastLL [1] = crs->GetLatMax ();
    workCoord [0] = southwestLL [0] + (northeastLL [0] - southwestLL [0]) * .66;
    workCoord [1] = southwestLL [1] + (northeastLL [1] - southwestLL [1]) * .33;

    centerXY = mgFactory.CreateCoordinateXY (workCoord [0],workCoord [1]);
    centerXY = crs->ConvertFromLonLat (centerXY);

    // Create an outer ring for our polygon object.  Essentially a circular
    // polygon of radius 2000 (system units, whatever they are.  We presume
    // the outer ring needs to go counterclockwise, and that it needs to
    // be closed (i.e. first and last pooint are the same.
    angle = 0.0;
    delta = 2.0 * M_PI / pointCount;
    radius = (crs->IsGeodetic ()) ? 0.5 : 2000.00;
    outerCollection = new MgCoordinateCollection ();
    for (idx = 0;idx < pointCount;idx++)
    {
        coordX = centerXY->GetX () + radius * sin (angle);
        coordY = centerXY->GetY () + radius * cos (angle);
        nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
        outerCollection->Add (nextCoord);
        angle -= delta;
    }
    // Force a precise closure.
    coordX = centerXY->GetX ();
    coordY = centerXY->GetY () + radius;
    nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
    outerCollection->Add (nextCoord);
    // Create the outer ring which will be required to create a MgPolygon.
    outerRing = mgFactory.CreateLinearRing (outerCollection);

    // Now lets create an inner ring, with a radius of 1,000.0 system units,
    // whatever they are.  We presume the point need to be in clockwise
    // order and, again, the polygon needs to be closed.
    angle = 0.0;
    delta = 2.0 * M_PI / pointCount;
    radius = (crs->IsGeodetic ()) ? 0.25 : 1000.00;
    innerCollection = new MgCoordinateCollection ();
    for (idx = 0;idx < pointCount;idx++)
    {
        coordX = centerXY->GetX () + radius * sin (angle);
        coordY = centerXY->GetY () + radius * cos (angle);
        nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
        innerCollection->Add (nextCoord);
        angle += delta;
    }
    // Force a precise closure.
    coordX = centerXY->GetX ();
    coordY = centerXY->GetY () + radius;
    nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
    innerCollection->Add (nextCoord);

    // For the inner rings (i.e. holes) we need a collection of rings even
    // though we only want one.
    innerRing = mgFactory.CreateLinearRing (innerCollection);
    innerRingCollection = new MgLinearRingCollection ();
    innerRingCollection->Add (innerRing);

    // Create the end result, a polygon with an inner ring (i.e. hole).
    myPolygon = mgFactory.CreatePolygon (outerRing,innerRingCollection);

    return myPolygon.Detach ();
}

MgLineString* CreateLineString (MgCoordinateSystem* crs,INT32 pointCount)
{
    INT32 idx;

    double deltaX;
    double deltaY;
    double coordX;
    double coordY;
    double distance;

    double southwestLL [2];
    double northeastLL [2];
    double fromXY [2];
    double toXY [2];

    Ptr<MgCoordinate> nextCoord;
    Ptr<MgCoordinateCollection> pointCollection;
    Ptr<MgLineString> myLineString;

    MgGeometryFactory mgFactory;

    // So, to build the circle, we'll need the center point.
    southwestLL [0] = crs->GetLonMin ();
    southwestLL [1] = crs->GetLatMin ();
    northeastLL [0] = crs->GetLonMax ();
    northeastLL [1] = crs->GetLatMax ();
    fromXY [0] = southwestLL [0] + (northeastLL [0] - southwestLL [0]) * .33;
    fromXY [1] = southwestLL [1] + (northeastLL [1] - southwestLL [1]) * .33;
    toXY [0] = southwestLL [0] + (northeastLL [0] - southwestLL [0]) * .66;
    toXY [1] = southwestLL [1] + (northeastLL [1] - southwestLL [1]) * .66;

    deltaX = toXY [0] - fromXY [0];
    deltaY = toXY [1] - fromXY [1];
    distance = sqrt (deltaX * deltaX + deltaY * deltaY);

    deltaX /= pointCount;
    deltaY /= pointCount;
    pointCollection = new MgCoordinateCollection ();
    for (idx = 0;idx < pointCount;idx += 1)
    {
        coordX = fromXY [0] + idx * deltaX;
        coordY = fromXY [1] + idx * deltaX;
        nextCoord = mgFactory.CreateCoordinateXY (coordX,coordY);
        pointCollection->Add (nextCoord);
    }
    myLineString = mgFactory.CreateLineString (pointCollection);
    return myLineString.Detach ();
}
//newPage//
///////////////////////////////////////////////////////////////////////////////
// MgCrsThreadTestdata
//
// An object whichcontains the information necessary for a specific test case
// specifically created for the multi-thread safety (i.e. reentrancy) unit test.
//
unsigned MgCrsThreadTestDataEntry::m_RandomSeed = 5263;
unsigned MgCrsThreadTestDataEntry::SetSeed (unsigned newSeed)
{
    unsigned rtnValue = m_RandomSeed;
    m_RandomSeed = newSeed;
    return rtnValue;
}
MgCrsThreadTestDataEntry::MgCrsThreadTestDataEntry (CREFSTRING srcCrs,
                                                    CREFSTRING trgCrs,
                                                    INT32 pointCount,
                                                    bool threeD)
                                                        :
                                                    m_pSrcCrsCode (srcCrs),
                                                    m_pTrgCrsCode (trgCrs),
                                                    m_nPointCount (pointCount),
                                                    m_bThreeD     (threeD)
{
    Initialize ();
}
MgCrsThreadTestDataEntry::~MgCrsThreadTestDataEntry ()
{
    m_pSrcCRS->Release ();
    m_pTrgCRS->Release ();
    m_pTransformFwd->Release ();
    m_pTransformInv->Release ();
    m_pSrcPoints->Release ();
}
void MgCrsThreadTestDataEntry::Initialize (void)
{
    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    m_pSrcCRS = csFactory.CreateFromCode (m_pSrcCrsCode);
    m_pTrgCRS = csFactory.CreateFromCode (m_pTrgCrsCode);
    m_pTransformFwd = csFactory.GetTransform (m_pSrcCRS,m_pTrgCRS);
    m_pTransformFwd->IgnoreDatumShiftWarning (true);
    m_pTransformFwd->IgnoreOutsideDomainWarning (true);
    m_pTransformInv = csFactory.GetTransform (m_pTrgCRS,m_pSrcCRS);
    m_pTransformInv->IgnoreDatumShiftWarning (true);
    m_pTransformInv->IgnoreOutsideDomainWarning (true);

    m_sTransformName = m_pSrcCrsCode + L" to " + m_pTrgCrsCode;
   
    m_dToleranceXY = m_pSrcCRS->ConvertMetersToCoordinateSystemUnits (0.003);
    if (m_pSrcCRS->GetProjectionCode() == MgCoordinateSystemProjectionCode::LL)
    {
        // For geographic coordinates, the units of the Z value is always meters.
        m_dToleranceZ = 0.003;
    }
    else
    {
        // For non-geographic systems, the Z unit is same as the horizontal unit
        // (for CS-MAP, anyway).
        m_dToleranceZ = m_dToleranceXY;
    }

    m_pSrcPoints = CreateCoordinateCollection (m_RandomSeed,m_pSrcCRS,m_nPointCount,m_bThreeD);
}
STRING MgCrsThreadTestDataEntry::GetTransformName (void)
{
    return m_sTransformName;
}
bool MgCrsThreadTestDataEntry::VerifyResults (MgCoordinateCollection* resultCollection)
{
    bool ok;
    Ptr<MgCoordinateIterator> originalItr;
    Ptr<MgCoordinateIterator> resultsItr;

    originalItr = m_pSrcPoints->GetIterator ();
    resultsItr = resultCollection->GetIterator ();
    ok = true;
    while (originalItr->MoveNext () && ok)
    {
        ok = resultsItr->MoveNext ();
        if (ok)
        {
            bool xOk;
            bool yOk;
            bool zOk (true);

            Ptr<MgCoordinate> originalPoint = originalItr->GetCurrent ();
            Ptr<MgCoordinate> resultPoint = resultsItr->GetCurrent ();
            xOk = GctDblCmp (originalPoint->GetX(),resultPoint->GetX(),m_dToleranceXY);
            yOk = GctDblCmp (originalPoint->GetY(),resultPoint->GetY(),m_dToleranceXY);
            INT32 dimension = originalPoint->GetDimension ();
            if ((dimension & MgCoordinateDimension::XYZ) == MgCoordinateDimension::XYZ)
            {
               zOk = GctDblCmp (originalPoint->GetZ(),resultPoint->GetZ(),m_dToleranceZ);
            }
            ok = xOk && yOk && zOk;
        }
    }
    return ok;
}
MgCoordinateSystemTransform* MgCrsThreadTestDataEntry::GetForwardTransform (void)
{
    return SAFE_ADDREF (m_pTransformFwd);
}
MgCoordinateSystemTransform* MgCrsThreadTestDataEntry::GetInverseTransform (void)
{
    return SAFE_ADDREF (m_pTransformInv);
}
MgCoordinateCollection* MgCrsThreadTestDataEntry::GetSrcPointsCopy (void)
{
    Ptr<MgCoordinateCollection> collectionCopy = m_pSrcPoints->Copy ();
    return collectionCopy.Detach ();
}

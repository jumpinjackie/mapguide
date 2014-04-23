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
#include "CoordSysUtil.h"
#include "CriticalSection.h"
#include "Spatial/MathUtility.h"
#include "Spatial/SpatialUtility.h"

#include "CoordSys.h"
#include "CoordSysGrids.h"
#include "CoordSysMeasure.h"
#include "CoordSysEnumInteger32.h"
#include "MentorUtil.h"

using namespace CSLibrary;

#if !defined (_DEBUG)
    // Include heap overhead estimated at 12 bytes in release mode.
    static const INT32 kMgHeapOverhead = 12;
    static const INT32 kMgSizeOfCoordinateXY = sizeof (MgCoordinateXY) + kMgHeapOverhead;
#else
    // Include heap overhead estimated at 36 bytes in release mode.
    static const INT32 kMgHeapOverhead = 36;
    static const INT32 kMgSizeOfCoordinateXY = sizeof (MgCoordinateXY) + kMgHeapOverhead;
#endif

//=============================================================================
// CCoordinateSystemGridSpecification -- Assembly which contains all grid
//                                       parameter values.
//
// This object is basically used for convenience, placing all commonly used
// grid specification parameters into a single object.  Another grid parameter
// can be added without changing any calling sequences.  Note that the default
// constructor constructs an invalid parameter structure.
//
// The default presision value is 1 meter (currently).  If the CurvePrecision
// member is not set, this value will be used.
const double CCoordinateSystemGridSpecification::m_DefaultPrecision = 1.0;
CCoordinateSystemGridSpecification::CCoordinateSystemGridSpecification (void) : m_EastingBase           (0.0),
                                                                                m_NorthingBase          (0.0),
                                                                                m_EastingIncrement      (0.0),
                                                                                m_NorthingIncrement     (0.0),
                                                                                m_CurvePrecision        (0.0),
                                                                                m_TickEastingIncrement  (0),
                                                                                m_TickNorthingIncrement (0),
                                                                                m_UnitType              (MgCoordinateSystemUnitType::Linear),
                                                                                m_UnitCode              (MgCoordinateSystemUnitCode::Meter),
                                                                                m_MaxCurvePoints        (0)
{
}
CCoordinateSystemGridSpecification::CCoordinateSystemGridSpecification (const CCoordinateSystemGridSpecification& source)
                                                                             :
                                                                        m_EastingBase           (source.m_EastingBase),
                                                                        m_NorthingBase          (source.m_NorthingBase),
                                                                        m_EastingIncrement      (source.m_EastingIncrement),
                                                                        m_NorthingIncrement     (source.m_NorthingIncrement),
                                                                        m_CurvePrecision        (source.m_CurvePrecision),
                                                                        m_TickEastingIncrement  (source.m_TickEastingIncrement),
                                                                        m_TickNorthingIncrement (source.m_TickNorthingIncrement),
                                                                        m_UnitType              (source.m_UnitType),
                                                                        m_UnitCode              (source.m_UnitCode),
                                                                        m_MaxCurvePoints        (source.m_MaxCurvePoints)
{
}
CCoordinateSystemGridSpecification::~CCoordinateSystemGridSpecification (void)
{
}
CCoordinateSystemGridSpecification& CCoordinateSystemGridSpecification::operator= (const CCoordinateSystemGridSpecification& rhs)
{
    if (this != &rhs)
    {
        m_EastingBase           = rhs.m_EastingBase;
        m_NorthingBase          = rhs.m_NorthingBase;
        m_EastingIncrement      = rhs.m_EastingIncrement;
        m_NorthingIncrement     = rhs.m_NorthingIncrement;
        m_CurvePrecision        = rhs.m_CurvePrecision;
        m_TickEastingIncrement  = rhs.m_TickEastingIncrement;
        m_TickNorthingIncrement = rhs.m_TickNorthingIncrement;
        m_UnitType              = rhs.m_UnitType;
        m_UnitCode              = rhs.m_UnitCode;
        m_MaxCurvePoints        = rhs.m_MaxCurvePoints;
    }
    return *this;
}
double CCoordinateSystemGridSpecification::GetEastingBase(void)
{
    return m_EastingBase;
}
double CCoordinateSystemGridSpecification::GetNorthingBase(void)
{
    return m_NorthingBase;
}
double CCoordinateSystemGridSpecification::GetEastingIncrement(void)
{
    return m_EastingIncrement;
}
double CCoordinateSystemGridSpecification::GetNorthingIncrement(void)
{
    return m_NorthingIncrement;
}
double CCoordinateSystemGridSpecification::GetTickEastingIncrement(void)
{
    return m_TickEastingIncrement;
}
double CCoordinateSystemGridSpecification::GetTickNorthingIncrement(void)
{
    return m_TickNorthingIncrement;
}
INT32 CCoordinateSystemGridSpecification::GetUnitType(void)
{
    return m_UnitType;
}
INT32 CCoordinateSystemGridSpecification::GetUnitCode(void)
{
    return m_UnitCode;
}
double CCoordinateSystemGridSpecification::GetCurvePrecision(void)
{
    return m_CurvePrecision;
}
INT32 CCoordinateSystemGridSpecification::GetMaxCurvePoints(void)
{
    return m_MaxCurvePoints;
}
bool CCoordinateSystemGridSpecification::IsSameAs(MgCoordinateSystemGridSpecification* specification)
{
    bool areTheSame (false);

    double dblTol (1.0E-04);

    areTheSame  = fabs (specification->GetEastingBase()       - m_EastingBase)       < dblTol;
    areTheSame &= fabs (specification->GetNorthingBase()      - m_NorthingBase)      < dblTol;
    areTheSame &= fabs (specification->GetEastingIncrement()  - m_EastingIncrement)  < dblTol;
    areTheSame &= fabs (specification->GetNorthingIncrement() - m_NorthingIncrement) < dblTol;

    areTheSame &= fabs (specification->GetTickEastingIncrement()  - m_TickEastingIncrement)  < dblTol;
    areTheSame &= fabs (specification->GetTickNorthingIncrement() - m_TickNorthingIncrement) < dblTol;

    areTheSame &= (specification->GetUnitType() == m_UnitType);
    areTheSame &= (specification->GetUnitCode() == m_UnitCode);

    areTheSame &= fabs (specification->GetCurvePrecision()    - m_CurvePrecision)    < 1.0E-08;

    return areTheSame;
}

void CCoordinateSystemGridSpecification::SetGridBase(double eastingBase,double northingBase)
{
    m_EastingBase = eastingBase;
    m_NorthingBase = northingBase;
}
void CCoordinateSystemGridSpecification::SetGridIncrement(double eastingIncrement,double northingIncrement)
{
    m_EastingIncrement = eastingIncrement;
    m_NorthingIncrement = northingIncrement;
}
void CCoordinateSystemGridSpecification::SetTickIncrements(double eastingIncrement,double northingIncrement)
{
    m_TickEastingIncrement = eastingIncrement;
    m_TickNorthingIncrement = northingIncrement;
}
void CCoordinateSystemGridSpecification::SetUnits (INT32 unitCode,INT32 unitType)
{
    bool codeOk;
    INT32 lclUnitType;

    codeOk = GetUnitInfo(unitCode,&lclUnitType,NULL);
    if (!codeOk || unitType != lclUnitType)
    {
        throw new MgInvalidCoordinateSystemUnitsException(L"MgCoordinateSystemGridSpecification.SetUnits",
                                                          __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_UnitCode = unitCode;
    m_UnitType = unitType;
}
void CCoordinateSystemGridSpecification::SetCurvePrecision (double curvePrecision)
{
    m_CurvePrecision = curvePrecision;
}
void CCoordinateSystemGridSpecification::SetMaxCurvePoints (INT32 maxCurvePoints)
{
    m_CurvePrecision = maxCurvePoints;
}
// The following functions are identical to the published version, with the
// exception that the values returned are always converted to the units requested
// by the resultUnitCode argument.
double CCoordinateSystemGridSpecification::GetEastingBase (INT32 resultUnitCode)
{
    double eastingBase;

    eastingBase = ConvertUnitsOfValue (m_EastingBase,resultUnitCode);
    return eastingBase;
}
double CCoordinateSystemGridSpecification::GetNorthingBase (INT32 resultUnitCode)
{
    double northingBase;

    northingBase = ConvertUnitsOfValue (m_NorthingBase,resultUnitCode);
    return northingBase;
}
double CCoordinateSystemGridSpecification::GetEastingIncrement (INT32 resultUnitCode)
{
    double eastingIncrement;

    eastingIncrement = ConvertUnitsOfValue (m_EastingIncrement,resultUnitCode);
    return eastingIncrement;
}
double CCoordinateSystemGridSpecification::GetNorthingIncrement(INT32 resultUnitCode)
{
    double northingIncrement;

    northingIncrement = ConvertUnitsOfValue (m_NorthingIncrement,resultUnitCode);
    return northingIncrement;
}
double CCoordinateSystemGridSpecification::GetTickEastingIncrement(INT32 resultUnitCode)
{
    double tickEastingIncrement;

    tickEastingIncrement = ConvertUnitsOfValue (m_TickEastingIncrement,resultUnitCode);
    return tickEastingIncrement;
}
double CCoordinateSystemGridSpecification::GetTickNorthingIncrement(INT32 resultUnitCode)
{
    double tickNorthingIncrement;

    tickNorthingIncrement = ConvertUnitsOfValue (m_TickNorthingIncrement,resultUnitCode);
    return tickNorthingIncrement;
}
double CCoordinateSystemGridSpecification::GetCurvePrecision (MgCoordinateSystem* gridCS)
{
    double precisionInCsUnits (1.0);
    double precisionInMeters (1.0);

    Ptr<MgCoordinateSystemCatalog> catalogPtr = gridCS->GetCatalog ();
    Ptr<MgCoordinateSystemUnitInformation> unitInfoPtr = catalogPtr->GetUnitInformation ();

    // UnitConversion will convert the units of the coordinate system to
    // meters by multiplication.  By inverting this, we have a value which
    // will convert meters to coordinate system units by multiplication.
    double csUnitConversion = 1.0 / gridCS->GetUnitScale ();

    // If the curve precision has not been set by the user, this is easy.
    if (m_CurvePrecision < 1.0E-24)     // i.e. == 0.0
    {
        precisionInMeters = m_DefaultPrecision;
    }
    else
    {
        double toMeters;
        if (m_UnitType == MgCoordinateSystemUnitType::Linear)
        {
            toMeters = unitInfoPtr->GetLinearUnitScale (m_UnitCode);
        }
        else
        {
            double toDegrees = unitInfoPtr->GetAngularUnitScale (m_UnitCode);
            toMeters = (6378137.0 * M_PI * 2.0 / 360.0) / toDegrees;
        }
        precisionInMeters = m_CurvePrecision * toMeters;
    }
    precisionInCsUnits = precisionInMeters * csUnitConversion;
    return precisionInCsUnits;
}
double CCoordinateSystemGridSpecification::GetCurvePrecision (MgCoordinateSystemGridBoundary* gridBoundary,
                                                              MgCoordinateSystem* gridCS)
{
    double precisionInCsUnits;

    precisionInCsUnits = GetCurvePrecision (gridCS);
    return precisionInCsUnits;
}
double CCoordinateSystemGridSpecification::GetMaxCurvePoints (MgCoordinateSystemGridBoundary* frameBoundary,
                                                              MgCoordinateSystem* frameCS)
{
    INT32 maxPoints (511);

    if (m_MaxCurvePoints > 0)
    {
        maxPoints = m_MaxCurvePoints;
    }
    return maxPoints;
}
bool CCoordinateSystemGridSpecification::IsConsistent ()
{
    bool ok (false);
    INT32 unitType (MgCoordinateSystemUnitType::Unknown);
    MgCoordinateSystemFactory csFactory;
    Ptr<MgCoordinateSystemCatalog> catalogPtr = csFactory.GetCatalog ();
    Ptr<MgCoordinateSystemUnitInformation> unitInfoPtr = catalogPtr->GetUnitInformation ();

    // Check that m_UnitCode is consistent with m_UnitType.
    unitType = unitInfoPtr->GetUnitType (m_UnitCode);
    ok = (unitType != MgCoordinateSystemUnitType::Unknown && m_UnitType == unitType);

    // Currently, unitScale is not used.  It could very well be used in the
    // future as part of a scheme to filter out bogus parameter values.  That
    // was my intent when writing this code, but doing so implies that I
    // make some assumtions about what is "normal" and what is not.
    //if (ok)
    //{
    //    double unitScale (1.0);
    //
    //    // The following should never throw in this case as we have
    //    // already validated the consistency of the m_UnitCode and
    //    // m__UnitType members.
    //    unitScale = unitInfoPtr->GetScale (m_UnitCode,m_UnitType);
    //}

    // Verify that the 6 basic parameters are all positive numbers, and
    // non-zero as appropriate.
    if (ok)
    {
        ok = (m_EastingBase >= 0.0) && (m_NorthingBase >= 0.0) &&
             (m_EastingIncrement > 0.0) && (m_NorthingIncrement > 0.0) &&
             (m_TickEastingIncrement > 0.0) && (m_TickNorthingIncrement > 0.0) &&
             (m_CurvePrecision >= 0.0);
    }

    return ok;
}
void CCoordinateSystemGridSpecification::Dispose ()
{
    delete this;
}
double CCoordinateSystemGridSpecification::ConvertUnitsOfValue (double value,INT32 trgUnitCode)
{
    INT32 trgUnitType;
    double unitConversion;

    MgCoordinateSystemFactory csFactory;
    Ptr<MgCoordinateSystemCatalog> catalogPtr = csFactory.GetCatalog ();
    Ptr<MgCoordinateSystemUnitInformation> unitInfoPtr = catalogPtr->GetUnitInformation ();

    // Verify that the unit type of the specification match the unit type
    // to which we are to convert.
    GetUnitInfo(trgUnitCode,&trgUnitType,NULL);
    if (trgUnitType != m_UnitType)
    {
        throw new MgInvalidCoordinateSystemUnitsException(L"MgCoordinateSystemGridSpecification.ConvertUnitsOfValue", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Calculate the appropriate units conversion factor.  Since the unit types
    // are the same, this should succeed without error.
    if (m_UnitType == MgCoordinateSystemUnitType::Linear)
    {
        unitConversion = unitInfoPtr->GetLinearUnitScale (m_UnitCode) /
                         unitInfoPtr->GetLinearUnitScale (trgUnitCode);
    }
    else
    {
        unitConversion = unitInfoPtr->GetAngularUnitScale (m_UnitCode) /
                         unitInfoPtr->GetAngularUnitScale (trgUnitCode);
    }
    return (value * unitConversion);
}
///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The boundary of the grid or graticule.  In the case of geographic
/// coordinates, internal longitude ordinates may exceed 180 and latitude
/// ordinates may exceed 90 to support wrap around in a rational fashion.
/// In this initial release, the boundary will be a rectangle, and this will
/// be enforced by the constructors which are supported.  Expansion to support
/// more complex boundaries will be added by providing more constructors.
///</summary>
const INT32 CCoordinateSystemGridBoundary::MaxCurvePoints = 511;
CCoordinateSystemGridBoundary::CCoordinateSystemGridBoundary () : MgCoordinateSystemGridBoundary (),
                                                                  m_Large                        (false),
                                                                  m_MaxCurvePoints               (MaxCurvePoints),
                                                                  m_GridBoundary                 ()
{
}
CCoordinateSystemGridBoundary::CCoordinateSystemGridBoundary (MgCoordinate* southwest,
                                                              MgCoordinate* northeast)
                                                                :
                                                              MgCoordinateSystemGridBoundary (),
                                                              m_Large                        (false),
                                                              m_MaxCurvePoints               (MaxCurvePoints),
                                                              m_GridBoundary                 ()
{
   SetBoundaryExtents (southwest,northeast);
}
CCoordinateSystemGridBoundary::CCoordinateSystemGridBoundary (MgPolygon* boundary)
                                                                :
                                                              MgCoordinateSystemGridBoundary (),
                                                              m_Large                        (false),
                                                              m_MaxCurvePoints               (MaxCurvePoints),
                                                              m_GridBoundary                 ()
{
    m_GridBoundary = SAFE_ADDREF (boundary);
}
CCoordinateSystemGridBoundary::~CCoordinateSystemGridBoundary (void)
{
}
void CCoordinateSystemGridBoundary::SetBoundaryExtents (MgCoordinate* southwest,
                                                        MgCoordinate* northeast)
{
    MgGeometryFactory factory;
    Ptr<MgCoordinate> swPnt;
    Ptr<MgCoordinate> sePnt;
    Ptr<MgCoordinate> nePnt;
    Ptr<MgCoordinate> nwPnt;
    Ptr<MgCoordinate> clPnt;
    Ptr<MgCoordinateCollection> collection;

    MG_TRY()
        swPnt = factory.CreateCoordinateXY(southwest->GetX(),southwest->GetY());
        sePnt = factory.CreateCoordinateXY(northeast->GetX(),southwest->GetY());
        nePnt = factory.CreateCoordinateXY(northeast->GetX(),northeast->GetY());
        nwPnt = factory.CreateCoordinateXY(southwest->GetX(),northeast->GetY());
        clPnt = factory.CreateCoordinateXY(southwest->GetX(),southwest->GetY());

        if (!swPnt|| !swPnt || !nePnt || !nwPnt ||!clPnt)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemGridBoundary.SetBoundaryExtents",
                                             __LINE__, __WFILE__, NULL, L"", NULL);
        }

        collection = new MgCoordinateCollection();
        if (collection == NULL)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemGridBoundary.SetBoundaryExtents",
                                             __LINE__, __WFILE__, NULL, L"", NULL);
        }
        collection->Add (swPnt);
        collection->Add (sePnt);
        collection->Add (nePnt);
        collection->Add (nwPnt);
        collection->Add (clPnt);

        Ptr<MgLinearRing> ring = factory.CreateLinearRing (collection);
        if (ring == 0)
        {
            throw new MgOutOfMemoryException(L"MgCoordinateSystemGridBoundary.SetBoundaryExtents",
                                             __LINE__, __WFILE__, NULL, L"", NULL);
        }
        m_GridBoundary = factory.CreatePolygon (ring,NULL);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.SetBoundaryExtents")
}
void CCoordinateSystemGridBoundary::SetBoundaryExtents (MgPolygon* boundary)
{
    m_GridBoundary = SAFE_ADDREF (boundary);
}
MgPolygon* CCoordinateSystemGridBoundary::GetBoundary (void) const
{
    return SAFE_ADDREF (m_GridBoundary.p);
}
void CCoordinateSystemGridBoundary::GetBoundaryExtents (double& eastMin,double& eastMax,double& northMin,double& northMax) const
{
    double currentX;
    double currentY;
    Ptr<MgLinearRing> outerRing;
    Ptr<MgCoordinateIterator> ringItr;
    Ptr<MgCoordinate> coordPtr;

    MG_TRY ()
        // Set up the accumulation values so we will always capture the first
        // values encountered in the actual polygon.
        eastMin = northMin = 1.0E+300;
        eastMax = northMax = -1.0E+300;

        // We loop through theouter ring of the polygon.  For the purpose of this function,
        // the first ordinate in each coordinate is considered the easting and the
        // second ordinate in each coordinate is considered the northing.
        //
        // It remains unclear, at this point, if we need to know if the coordinate
        // system of the boundary is geographic or not.  We'll fund out pretty soon.
        outerRing = m_GridBoundary->GetExteriorRing ();
        ringItr = outerRing->GetCoordinates ();
        while (ringItr->MoveNext ())
        {
            coordPtr = ringItr->GetCurrent ();
            currentX = coordPtr->GetX ();
            currentY = coordPtr->GetY ();
            if (currentX < eastMin) eastMin = currentX;
            if (currentX > eastMax) eastMax = currentX;
            if (currentY < northMin) northMin = currentY;
            if (currentY > northMax) northMax = currentY;
        }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.GetBoundaryExtents")
    return;
}
void CCoordinateSystemGridBoundary::SetMaxCurvePoints (INT32 maxPoints)
{
    m_MaxCurvePoints = maxPoints;
}
// The Polygon returned by this function will not (usually) be a simple
// rectangle, but a series of complex curves which (usually) approximate
// a trapazoid.
MgPolygon* CCoordinateSystemGridBoundary::GetBoundary (MgCoordinateSystemTransform* transform,
                                                       double precision)
{
    MgGeometryFactory factory;

    INT32 index;
    INT32 interiorRingCount;

    Ptr<MgPolygon> result;
    Ptr<MgLinearRing> sourceRing;
    Ptr<MgLinearRing> targetRing;
    Ptr<MgLinearRing> exteriorRing;
    Ptr<MgLinearRingCollection> interiorRings;

    MG_TRY()
        // Convert the outer ring.
        sourceRing = m_GridBoundary->GetExteriorRing ();
        exteriorRing = TransformLinearRing (sourceRing,transform,precision);

        interiorRingCount = m_GridBoundary->GetInteriorRingCount ();
        if (interiorRingCount > 0)
        {
            // Create the repository for the interior rings, now that we know
            // there are some (this is rare).
            interiorRings = new MgLinearRingCollection ();

            for (index = 0;index < interiorRingCount;index += 1)
            {
                sourceRing = m_GridBoundary->GetInteriorRing (index);
                targetRing = TransformLinearRing (sourceRing,transform,precision);
                interiorRings->Add (targetRing);
            }
        }
        result = new MgPolygon (exteriorRing,interiorRings);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.GetBoundary")
    return result.Detach ();
}
MgLineStringCollection* CCoordinateSystemGridBoundary::ClipLineString (MgLineString* lineString) const
{
    MgGeometryFactory factory;
    Ptr<MgLineStringCollection> collection;
    Ptr<MgCoordinateIterator> polyItr;
    Ptr<MgCoordinateIterator> lineItr;

    MG_TRY()
        polyItr = m_GridBoundary->GetCoordinates ();
        lineItr = lineString->GetCoordinates ();
        collection = MgSpatialUtility::ClipStringToPolygon (polyItr,lineItr);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.ClipLineString")
    return collection.Detach ();
}
void CCoordinateSystemGridBoundary::Dispose ()
{
    delete this;
}
MgLinearRing* CCoordinateSystemGridBoundary::TransformLinearRing (MgLinearRing* linearRing,
                                                                  MgCoordinateSystemTransform* transform,
                                                                  double curvePrecision)
{
    MgGeometryFactory factory;

    Ptr<MgLinearRing> result;
    Ptr<MgCoordinateCollection> targetCollection;
    Ptr<MgLineString> convertedSegment;
    Ptr<MgCoordinate> curFromPnt;
    Ptr<MgCoordinate> curToPnt;
    Ptr<MgCoordinate> copyBufr;
    Ptr<MgCoordinateIterator> ringItr;
    Ptr<MgCoordinateIterator> lineItr;

    MG_TRY ()
        // NOTE:  Boundaries are required to proceed in a counterclockwise
        // direction, but this function will be used to transform both
        // exterior and interior rings.  So in this function we cannot count
        // on any specific order.  We should be able to rely on the fact that
        // each ring presented to us is indeed closed.

        // To create a MgLinearRing, we need an MgCoordinateCollection.  So we
        // create an empty MgCoordinateCollection, and then spend most of our
        // time adding points to this collection.  The primary source of these
        // points will be the MgLineStrings returned by the
        // MgCoordinateSystemTransform::GridLine object.  Perhaps that function
        // should be modified to return an MgCoordinateCollection?  Probably
        // doesn't make much difference as you can't (at least at thie time)
        // add one coordinate collection to another.  Too Bad!!!
        targetCollection = new MgCoordinateCollection ();

        ringItr = linearRing->GetCoordinates ();
        ringItr->MoveNext ();
        curToPnt = ringItr->GetCurrent ();
        copyBufr = transform->Transform (curToPnt);
        targetCollection->Add (copyBufr);
        while (ringItr->MoveNext ())
        {
            // Convert the current segment.
            curFromPnt = curToPnt;
            curToPnt = ringItr->GetCurrent ();
            convertedSegment = transform->GridLine (curFromPnt,curToPnt,curvePrecision,m_MaxCurvePoints);

            // Copy the converted segment to the target coordinate collection.
            lineItr = convertedSegment->GetCoordinates ();

            // To prevent duplicate points, we always skip adding the initial
            // point of a converted line string.  We dealt with the initial
            // point of the first segment above.  The first point of each
            // additional line segment will have been processed as the "to"
            // point of the last segment processed.  Always adding the last
            // point of each segment means we don't need to worry about an
            // extra point when we are done.
            lineItr->MoveNext ();

            // Copy the remainder of the converted line segment.
            while (lineItr->MoveNext ())
            {
                copyBufr = lineItr->GetCurrent ();
                targetCollection->Add (copyBufr);
            }
        }

        // OK, we should have now converted our ring to a collection of points.
        // If the ring provided was closed (as it should be) than the result we
        // have created should also be closed.  We can now convert this back to
        // a linear ring which is our required return type.
        result = new MgLinearRing (targetCollection);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.TransformLinearRing")

    return result.Detach ();
}
//=============================================================================
// CCoordinateSystemGridLine -- Associates an orientation and a value with a
//                              possibly interupted grid line.
//
CCoordinateSystemGridLine::CCoordinateSystemGridLine (INT32 orientation,double value)
                                                        :
                                                      MgCoordinateSystemGridLine (),
                                                      m_Orientation              (orientation),
                                                      m_Value                    (value),
                                                      m_LineSegments             ()

{
    m_LineSegments = new MgLineStringCollection;
}
CCoordinateSystemGridLine::~CCoordinateSystemGridLine (void)
{
}
INT32 CCoordinateSystemGridLine::GetGridOrientation(void)
{
    return m_Orientation;
}
double CCoordinateSystemGridLine::GetRealValue(void)
{
    return m_Value;
}
INT32 CCoordinateSystemGridLine::GetCount (void)
{
    return m_LineSegments->GetCount ();
}
MgLineString* CCoordinateSystemGridLine::GetSegment (INT32 index)
{
    MgLineString* rtnValue = m_LineSegments->GetItem (index);
    return rtnValue;
}
MgLineStringCollection* CCoordinateSystemGridLine::GetSegmentCollection (void)
{
    return SAFE_ADDREF(m_LineSegments.p);
}
void CCoordinateSystemGridLine::SetSegmentCollection (MgLineStringCollection* segmentCollection)
{
    m_LineSegments = SAFE_ADDREF (segmentCollection);
}
void CCoordinateSystemGridLine::SetGridOrientation (INT32 orientation)
{
    m_Orientation = orientation;
}
void CCoordinateSystemGridLine::SetRealValue (double realValue)
{
    m_Value = realValue;
}
void CCoordinateSystemGridLine::ClearSegments (void)
{
    m_LineSegments->Clear ();
}
void CCoordinateSystemGridLine::AddSegment (MgLineString* newSegment)
{
    m_LineSegments->Add (newSegment);
}
void CCoordinateSystemGridLine::Dispose ()
{
    delete this;
}
//=============================================================================
// CCoordinateSystemGridRegion -- Associates a label/designation with a
//                                MgPolygon object.
//
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
CCoordinateSystemGridRegion::CCoordinateSystemGridRegion (STRING label)
                                                               :
                                                          MgCoordinateSystemGridRegion (),
                                                          m_RegionLabel                (label),
                                                          m_RegionCenter               (),
                                                          m_RegionBoundary             (),
                                                          m_SouthLine                  (),
                                                          m_EastLine                   (),
                                                          m_NorthLine                  (),
                                                          m_WestLine                   ()
{
}
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
CCoordinateSystemGridRegion::CCoordinateSystemGridRegion (STRING label,
                                                          MgCoordinateSystemGridBoundary* frameBoundary,
                                                          MgCoordinateSystemTransform* gridToFrame,
                                                          MgCoordinate* southwest,
                                                          MgCoordinate* northeast,
                                                          double curvePrecision,
                                                          INT32 maxPoints)
                                                               :
                                                          MgCoordinateSystemGridRegion (),
                                                          m_RegionLabel                (label),
                                                          m_RegionCenter               (),
                                                          m_RegionBoundary             (),
                                                          m_SouthLine                  (),
                                                          m_EastLine                   (),
                                                          m_NorthLine                  (),
                                                          m_WestLine                   ()
{
    double xx, yy;
    double southLimit;
    double northLimit;
    double eastLimit;
    double westLimit;

    Ptr<MgCoordinate> southeast;
    Ptr<MgCoordinate> northwest;
    Ptr<MgCoordinate> centerPoint;
    Ptr<MgLineString> lineString;
    Ptr<MgCoordinateSystemGridBoundary> regionBoundaryInGridCrs;

    MgGeometryFactory mgFactory;
    MgCoordinateSystemFactory csFactory;

    MG_TRY()
        // Get the region limits in a convenient form.
        westLimit = southwest->GetX ();
        eastLimit = northeast->GetX ();
        southLimit = southwest->GetY ();
        northLimit = northeast->GetY ();
        southeast = mgFactory.CreateCoordinateXY (eastLimit,southLimit);
        northwest = mgFactory.CreateCoordinateXY (westLimit,northLimit);

        // Calculate the center of the region and convert to frame coordinates.
        xx = 0.5 * (eastLimit  + westLimit);
        yy = 0.5 * (northLimit + southLimit);
        gridToFrame->Transform (&xx,&yy);
        m_RegionCenter = mgFactory.CreateCoordinateXY (xx,yy);

        // Generate the polygon; then convert to frame coordinates.
        regionBoundaryInGridCrs = csFactory.GridBoundary (southwest,northeast);
        regionBoundaryInGridCrs->SetMaxCurvePoints (maxPoints);
        m_RegionBoundary = regionBoundaryInGridCrs->GetBoundary (gridToFrame,curvePrecision);

        // Generate and clip each of the four bounding lines.
        lineString = gridToFrame->GridLine (southwest,southeast,curvePrecision,maxPoints);
        m_SouthLine = frameBoundary->ClipLineString (lineString);

        lineString = gridToFrame->GridLine (southeast,northeast,curvePrecision,maxPoints);
        m_EastLine = frameBoundary->ClipLineString (lineString);

        lineString = gridToFrame->GridLine (northeast,northwest,curvePrecision,maxPoints);
        m_NorthLine = frameBoundary->ClipLineString (lineString);

        lineString = gridToFrame->GridLine (northwest,southwest,curvePrecision,maxPoints);
        m_WestLine = frameBoundary->ClipLineString (lineString);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGridBoundary.SetBoundaryExtents")
}
STRING CCoordinateSystemGridRegion::GetLabel ()
{
    return m_RegionLabel;
}
MgCoordinate* CCoordinateSystemGridRegion::GetRegionCenter (void)
{
    return SAFE_ADDREF(m_RegionCenter.p);
}
MgPolygon* CCoordinateSystemGridRegion::GetRegionBoundary ()
{
    return SAFE_ADDREF(m_RegionBoundary.p);
}
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
MgLineStringCollection* CCoordinateSystemGridRegion::GetSouthLine (void)
{
    return SAFE_ADDREF(m_SouthLine.p);
}
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
MgLineStringCollection* CCoordinateSystemGridRegion::GetEastLine (void)
{
    return SAFE_ADDREF(m_EastLine.p);
}
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
MgLineStringCollection* CCoordinateSystemGridRegion::GetNorthLine (void)
{
    return SAFE_ADDREF(m_NorthLine.p);
}
// This API has been found to be problematical in the initial release.  It
// will be deprecated in the next release.
MgLineStringCollection* CCoordinateSystemGridRegion::GetWestLine (void)
{
    return SAFE_ADDREF(m_WestLine.p);
}
void CCoordinateSystemGridRegion::SetRegionCenter (MgCoordinate* regionCenter)
{
    m_RegionCenter = SAFE_ADDREF (regionCenter);
}
void CCoordinateSystemGridRegion::SetRegionBoundary (MgPolygon* boundary)
{
    m_RegionBoundary = SAFE_ADDREF (boundary);
}
void CCoordinateSystemGridRegion::SetSouthLine (MgLineStringCollection* southLine)
{
    m_SouthLine = SAFE_ADDREF(southLine);
}
void CCoordinateSystemGridRegion::SetEastLine (MgLineStringCollection* eastLine)
{
    m_EastLine = SAFE_ADDREF(eastLine);
}
void CCoordinateSystemGridRegion::SetNorthLine (MgLineStringCollection* northLine)
{
    m_NorthLine = SAFE_ADDREF(northLine);
}
void CCoordinateSystemGridRegion::SetWestLine (MgLineStringCollection* westLine)
{
    m_WestLine = SAFE_ADDREF(westLine);
}
void CCoordinateSystemGridRegion::Dispose ()
{
    delete this;
}
//=============================================================================
// CCoordinateSystemGridTick -- Defines the location of a grid tick mark.
//
// Definition includes the iso value for the tick mark, the orientation of the
// tick mark (i.e. is the value an easting or a northing), and the direction
// of the reerence line at the tick insertion point.  Ticks are normally drawn
// normal to this direction.  If the tick mark was generated from a grid
// boundary line, the interior of the grid will always be to the left of the
// direction vector.
CCoordinateSystemGridTick::CCoordinateSystemGridTick (INT32 orientation,double value)
                                                        :
                                                      MgCoordinateSystemGridTick (),
                                                      m_OnGridLine               (false),
                                                      m_Orientation              (orientation),
                                                      m_Value                    (value),
                                                      m_Position                 (),
                                                      m_Direction                ()
{
}
CCoordinateSystemGridTick::~CCoordinateSystemGridTick ()
{
}
void CCoordinateSystemGridTick::SetOnGridLine (bool isOnGridLine)
{
    m_OnGridLine = isOnGridLine;
}
void CCoordinateSystemGridTick::SetOrientation (INT32 orientation)
{
    m_Orientation = orientation;
}
void CCoordinateSystemGridTick::SetValue (double value)
{
    m_Value = value;
}
void CCoordinateSystemGridTick::SetPosition (MgCoordinate* position)
{
    m_Position = SAFE_ADDREF(position);
}
void CCoordinateSystemGridTick::SetDirection (MgCoordinate* direction)
{
    m_Direction = SAFE_ADDREF(direction);
}

bool CCoordinateSystemGridTick::GetIsOnGridLine (void)
{
    return m_OnGridLine;
}
INT32 CCoordinateSystemGridTick::GetTickOrientation ()
{
    return m_Orientation;
}
double CCoordinateSystemGridTick::GetValue ()
{
    return m_Value;
}
MgCoordinate* CCoordinateSystemGridTick::GetPosition ()
{
    return SAFE_ADDREF(m_Position.p);
}
MgCoordinate* CCoordinateSystemGridTick::GetDirectionVector ()
{
    return SAFE_ADDREF(m_Direction.p);
}
void CCoordinateSystemGridTick::Dispose ()
{
    delete this;
}
//=============================================================================
// An CCoordinateSystemGridLineCollection is collection of
// MgCoordinateSystemGridLine objects.  As manufactured, grid horizontal lines
// appear first in ascending order by grid value (easting in this case);
// followed by grid vertical lines in ascending order by grid value (northing
// in this case).
CCoordinateSystemGridLineCollection::CCoordinateSystemGridLineCollection (INT64 memoryThreshold)
                                        :
                                     MgCoordinateSystemGridLineCollection (),
                                     m_MemoryThreshold                    (memoryThreshold),
                                     m_GridLineCollection                 ()
{
    m_GridLineCollection = new MgDisposableCollection();
}
CCoordinateSystemGridLineCollection::~CCoordinateSystemGridLineCollection(void)
{
    // Delete whatever lines may be there.
    Clear ();
}
INT32 CCoordinateSystemGridLineCollection::GetCount () const
{
    INT32 lineCount;

    lineCount = m_GridLineCollection->GetCount();
    return lineCount;
}
MgCoordinateSystemGridLine* CCoordinateSystemGridLineCollection::GetItem (INT32 index) const
{
    MgCoordinateSystemGridLine* gridLine = static_cast<MgCoordinateSystemGridLine*>(m_GridLineCollection->GetItem(index));
    return gridLine;
}
INT32 CCoordinateSystemGridLineCollection::IndexOf (INT32 gridOrientation,double gridValue) const
{
    INT32 index;
    INT32 rtnValue = -1;
    INT32 count;
    double delta;
    Ptr<MgCoordinateSystemGridLine> gridLine;

    MG_TRY ()
        // The combination of orientation and grid value should be unique.
        // Return is -1 if not found.
        count = m_GridLineCollection->GetCount ();
        for (index = 0;index < count;index += 1)
        {
            gridLine = static_cast<MgCoordinateSystemGridLine*>(m_GridLineCollection->GetItem (index));
            if (gridLine->GetGridOrientation () == gridOrientation)
            {
                delta = fabs (gridLine->GetRealValue () - gridValue);
                if (delta < 1.0E-012)
                {
                    rtnValue = index;
                    break;
                }
            }
        }
    MG_CATCH_AND_THROW(L"CCoordinateSystemGridLineCollection::IndexOf")
    return rtnValue;
}
void CCoordinateSystemGridLineCollection::RemoveAt (INT32 index)
{
    // The MgDisposableCollection object checks the index argument, and throws if appropriate.
    // The MgDisposableCollection object performs the "SAFE_RELEASE" operation.
    m_GridLineCollection->RemoveAt (index);
}
void CCoordinateSystemGridLineCollection::Clear()
{
    // The MgDisposableCollection object performs the "SAFE_RELEASE" operation.
    m_GridLineCollection->Clear ();
}
void CCoordinateSystemGridLineCollection::SetItem (INT32 index,MgCoordinateSystemGridLine* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridLineCollection.SetItem", __LINE__, __WFILE__, NULL, L"", NULL);

    m_GridLineCollection->SetItem (index,value);
}
void CCoordinateSystemGridLineCollection::Add (MgCoordinateSystemGridLine* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridLineCollection.Add", __LINE__, __WFILE__, NULL, L"", NULL);

    // The MgDIsposableCollection object does the "SAFE_ADDREF" operation.
    m_GridLineCollection->Add (value);
}
void CCoordinateSystemGridLineCollection::AddCollection (MgCoordinateSystemGridLineCollection* aGridLineCollection)
{
    INT32 index;
    INT32 toAddCount;
    Ptr<MgCoordinateSystemGridLine> aGridLine;

    // Memory check is done inside Add()
    MG_TRY ()
        toAddCount = aGridLineCollection->GetCount ();
        for (index = 0;index < toAddCount;index += 1)
        {
            aGridLine = aGridLineCollection->GetItem (index);
            this->Add (aGridLine);
        }
    MG_CATCH_AND_THROW(L"CCoordinateSystemGridLineCollection::AddCollection")
    return;
}
void CCoordinateSystemGridLineCollection::Dispose(void)
{
    delete this;
}

//=============================================================================
// A CCoordinateSystemGridRegionCollection is collection of
// CCoordinateSystemGridRegion objects.
CCoordinateSystemGridRegionCollection::CCoordinateSystemGridRegionCollection (INT64 memoryThreshold)
                                         :
                                       MgCoordinateSystemGridRegionCollection (),
                                       m_MemoryThreshold                      (memoryThreshold),
                                       m_GridRegionCollection                 ()
{
    m_GridRegionCollection = new MgDisposableCollection();
}
CCoordinateSystemGridRegionCollection::~CCoordinateSystemGridRegionCollection (void)
{
    Clear ();
}
INT32 CCoordinateSystemGridRegionCollection::GetCount () const
{
    INT32 itemCount;
    itemCount = m_GridRegionCollection->GetCount ();
    return itemCount;
}
MgCoordinateSystemGridRegion* CCoordinateSystemGridRegionCollection::GetItem (INT32 index) const
{
    // The MgDisposableCollection object checks the index argument, and throws if appropriate.
    MgCoordinateSystemGridRegion* itemPtr = static_cast<MgCoordinateSystemGridRegion*>(m_GridRegionCollection->GetItem (index));
    return itemPtr;
}
void CCoordinateSystemGridRegionCollection::RemoveAt (INT32 index)
{
    // The MgDisposableCollection object checks the index argument, and throws if appropriate.
    // The MgDisposableCollection object performs the "SAFE_RELEASE" operation.
    m_GridRegionCollection->RemoveAt (index);
}
void CCoordinateSystemGridRegionCollection::Clear()
{
    m_GridRegionCollection->Clear ();
}
void CCoordinateSystemGridRegionCollection::SetItem (INT32 index, MgCoordinateSystemGridRegion* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridRegionCollection.SetItem", __LINE__, __WFILE__, NULL, L"", NULL);

    m_GridRegionCollection->SetItem (index,value);
}
void CCoordinateSystemGridRegionCollection::Add (MgCoordinateSystemGridRegion* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridRegionCollection.Add", __LINE__, __WFILE__, NULL, L"", NULL);

    m_GridRegionCollection->Add (value);
}
void CCoordinateSystemGridRegionCollection::AddCollection (MgCoordinateSystemGridRegionCollection* aGridRegionCollection)
{
    INT32 index;
    INT32 toAddCount;
    Ptr<MgCoordinateSystemGridRegion> aGridRegion;

    // Memory check is done inside Add()
    MG_TRY ()

        toAddCount = aGridRegionCollection->GetCount ();
        for (index = 0;index < toAddCount;index += 1)
        {
            aGridRegion = aGridRegionCollection->GetItem (index);
            // This Add refers to the "this" object.
            this->Add (aGridRegion);
        }
    MG_CATCH_AND_THROW(L"CCoordinateSystemGridRegionCollection::AddCollection")
    return;
}
void CCoordinateSystemGridRegionCollection::Dispose (void)
{
    // Destructor deletes the contents of the collection.
    delete this;
}

//=============================================================================
// A CCoordinateSystemGridTickCollection is collection of
// CCoordinateSystemGridTick objects.  CCoordinateSystemGridTickCollection
// objects will contain MgCoordinateSystemGridTick objects for the entire
// boundary or for an individual grid line.
CCoordinateSystemGridTickCollection::CCoordinateSystemGridTickCollection (INT64 memoryThreshold)
                                        :
                                     MgCoordinateSystemGridTickCollection (),
                                     m_MemoryThreshold                    (memoryThreshold),
                                     m_GridTickCollection                 ()
{
    m_GridTickCollection = new MgDisposableCollection();
}
CCoordinateSystemGridTickCollection::~CCoordinateSystemGridTickCollection (void)
{
    Clear ();
}
INT32 CCoordinateSystemGridTickCollection::GetCount () const
{
    INT32 itemCount = m_GridTickCollection->GetCount ();
    return itemCount;
}
MgCoordinateSystemGridTick* CCoordinateSystemGridTickCollection::GetItem (INT32 index) const
{
    MgCoordinateSystemGridTick *itemPtr = static_cast<MgCoordinateSystemGridTick*>(m_GridTickCollection->GetItem (index));
    return itemPtr;
}
void CCoordinateSystemGridTickCollection::RemoveAt (INT32 index)
{
    // The MgDisposableCollection object checks the index argument, and throws if appropriate.
    // The MgDisposableCollection object performs the "SAFE_RELEASE" operation.
    m_GridTickCollection->RemoveAt (index);
}
void CCoordinateSystemGridTickCollection::Clear()
{
    m_GridTickCollection->Clear ();
}
void CCoordinateSystemGridTickCollection::SetItem (INT32 index, MgCoordinateSystemGridTick* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridTickCollection.SetItem", __LINE__, __WFILE__, NULL, L"", NULL);

    m_GridTickCollection->SetItem (index,value);
}
void CCoordinateSystemGridTickCollection::Add (MgCoordinateSystemGridTick* value)
{
    if (GetAvailableMemory() < m_MemoryThreshold)
        throw new MgGridDensityException(L"CCoordinateSystemGridTickCollection.Add", __LINE__, __WFILE__, NULL, L"", NULL);

    m_GridTickCollection->Add (value);
}
void CCoordinateSystemGridTickCollection::AddCollection (MgCoordinateSystemGridTickCollection* aGridTickCollection)
{
    INT32 index;
    INT32 toAddCount;
    Ptr<MgCoordinateSystemGridTick> aGridTick;

    // Memory check is done in Add()
    MG_TRY ()
        toAddCount = aGridTickCollection->GetCount ();
        for (index = 0;index < toAddCount;index += 1)
        {
            aGridTick = aGridTickCollection->GetItem (index);
            this->Add (aGridTick);
        }
    MG_CATCH_AND_THROW(L"CCoordinateSystemGridTickCollection::AddCollection")
    return;
}
void CCoordinateSystemGridTickCollection::Dispose (void)
{
    delete this;
}

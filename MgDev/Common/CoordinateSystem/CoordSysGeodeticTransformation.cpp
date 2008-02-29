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

#include "CoordSysEnum.h"                   //for CCoordinateSystemEnum
#include "CoordSysEnumDatum.h"              //for CCoordinateSystemEnumDatum
#include "CoordSysEnumEllipsoid.h"          //for CCoordinateSystemEnumEllipsoid
#include "CoordSysDictionary.h"             //for CCoordinateSystemDictionary
#include "CoordSysDatumDictionary.h"        //for CCoordinateSystemDatumDictionary
#include "CoordSysEllipsoidDictionary.h"    //for CCoordinateSystemEllipsoidDictionary
#include "CoordSysUtil.h"                   //for Convert_Wide_To_Ascii, CsDictionaryOpenMode
#include "CoordSysCategoryDictionary.h"     //for CCoordinateSystemCategoryDictionary
#include "CoordSysCatalog.h"                //for CCoordinateSystemCatalog
#include "CoordSysGeodeticTransformation.h" //for CCoordinateSystemGeodeticTransformation
#include "CoordSysDatum.h"                  //for CCoordinateSystemDatum

using namespace CSLibrary;

//Default datum to use when user doesn't specify
static const wchar_t * const kpWGS84Datum = /*NOXLATE*/L"WGS84";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This will throw an exception if the set up cannot be made
//
CCoordinateSystemGeodeticTransformation::CCoordinateSystemGeodeticTransformation(MgCoordinateSystemCatalog* pCatalog, MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)
{
    SetCatalog(pCatalog);
    SetSourceAndTarget(pSource, pTarget);
}

//-----------------------------------------------------------------------------
CCoordinateSystemGeodeticTransformation::~CCoordinateSystemGeodeticTransformation()
{
}

//MgDisposable
void CCoordinateSystemGeodeticTransformation::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
void CCoordinateSystemGeodeticTransformation::SetCatalog(MgCoordinateSystemCatalog* pCatalog)
{
    m_pCatalog = SAFE_ADDREF(pCatalog);
}

//Initializes the datum shift object with the specified source and
//target datums.
//Either or both of the provided datum pointers may be NULL;
//passing in a null pointer is equivalent to specifying that
//that datum will be WGS84.
//Throws an exception if there's a problem with either of the datums
//(for example, if they haven't been initialized).
//
void CCoordinateSystemGeodeticTransformation::SetSourceAndTarget(MgCoordinateSystemDatum *pSource, MgCoordinateSystemDatum *pTarget)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets a copy of the source datum of the converter (will
//be set to NULL if none has been set).  Caller is responsible for
//freeing the returned pointer via Release().
//
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetSource()
{
    return SAFE_ADDREF(m_pDtSource.p);
}

//Gets a copy of the target datum of the converter (will
//be set to NULL if none has been set).  Caller is responsible for
//freeing the returned pointer via Release().
//
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetTarget()
{
    return SAFE_ADDREF(m_pDtTarget.p);
}

//Shifts the provided latitude/longitude coordinates from source datum
//to destination datum.
//Throws an exception if there's some problem doing the datum shift.  In case of error, the
//source latitude/longitude point will simply be copied to the
//destination (a "null" datum transformation).
//
void CCoordinateSystemGeodeticTransformation::Shift(MgCoordinate* pLonLat)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Shifts the provided latitude/longitude coordinates from source datum
//to destination datum.
//Throws an exception if there's some problem doing the datum shift.  In case of error, the
//source latitude/longitude point will simply be copied to the
//destination (a "null" datum transformation).
//
MgCoordinate* CCoordinateSystemGeodeticTransformation::Shift(double dLongitude, double dLatitude)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Shifts the provided latitude/longitude coordinates from source datum
//to destination datum.
//Throws an exception if there's some problem doing the datum shift.  In case of error, the
//source latitude/longitude point will simply be copied to the
//destination (a "null" datum transformation).
//Throws an exception with a Warning
//if a the shift was performed but may be incorrect (due to missing
//data files, etc.)
//
MgCoordinate* CCoordinateSystemGeodeticTransformation::Shift(double dLongitude, double dLatitude, double dZ)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"", NULL);
}

//-----------------------------------------------------------------------------
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetWGS84()
{
    if (!m_pCatalog)
    {
        return NULL;
    }

    Ptr<MgCoordinateSystemDatumDictionary> pDtDict=m_pCatalog->GetDatumDictionary();
    if (!pDtDict)
    {
        return NULL;
    }
    //get the WGS84
    MgGuardDisposable* pDt=pDtDict->Get(kpWGS84Datum);
    if (!pDt)
    {
        return NULL;
    }
    MgCoordinateSystemDatum* pDtDefinition=dynamic_cast<MgCoordinateSystemDatum*>(pDt);
    return pDtDefinition;
}

//If the provided interface is a null pointer, use WGS84.
//If not return the input value
//
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetWGS84IfNull(MgCoordinateSystemDatum *pDatum)
{
    MgCoordinateSystemDatum* pDtDefinition=pDatum;

    if (!pDtDefinition)
    {
        //None provided; use WGS84.
        pDtDefinition=GetWGS84();
        if (!pDtDefinition)
        {
            return NULL;
        }
    }

    return pDtDefinition;
}

//Gets the X, Y, and Z offset from the WGS-84 datum's geocenter to the
//geocenter of this datum, in meters.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
double CCoordinateSystemGeodeticTransformation::GetOffsetX()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetOffsetX", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetOffsetY()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetOffsetY", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetOffsetZ()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetOffsetZ", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the X, Y, and Z offset from the WGS-84 datum's geocenter to the
//geocenter of this datum, in meters.
//Throws an exception if the caller provides illegal values.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
void CCoordinateSystemGeodeticTransformation::SetOffset(double x, double y, double z)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the Bursa-Wolfe transformation values for the datum.  Rotations are
//in arc seconds.  X rotation is angle from WGS-84 X axis to local geodetic
//system X axis (east positive).  Y rotation is angle from WGS-84 Y axis to
//local geodetic system Y axis (north positive).  Z rotation is angle from
//WGS-84 Z axis to local geodetic system Z axis (use right-hand rule).  Scale
//factor is in parts per million above 1.0.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformRotationX()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationX", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformRotationY()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationY", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformRotationZ()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationZ", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformBwScale()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformBwScale", __LINE__, __WFILE__, NULL, L"", NULL);
}


//Sets the Bursa-Wolfe transformation values for the datum.  (Default is zero
//if not set).  See discussion under GetBursaWolfeTransform() for details.
//Throws an exception if any of the provided numbers are not valid.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
void CCoordinateSystemGeodeticTransformation::SetBursaWolfeTransform(double dRotationX, double dRotationY, double dRotationZ, double dBwScale)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the method which this datum definition will use for transforming
//between itself and WGS84.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
//The value returned is one of the values defined in the class MgCoordinateSystemGeodeticTransformationMethod
//
INT32 CCoordinateSystemGeodeticTransformation::GetGeodeticTransformationMethod()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Sets the method which this datum definition will use for transforming
//between itself and WGS84.  The default for a freshly constructed datum
//definition is Molodensky.
//Throws an exception if the supplied value is not one of the legal
//transformation types.
//
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
//The value used in input is one of the values defined in the class MgCoordinateSystemGeodeticTransformationMethod
//
void CCoordinateSystemGeodeticTransformation::SetGeodeticTransformationMethod(INT32 nGeodeticTransformationMethod)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Gets the max legal value for a datum offset, in meters.
//
double CCoordinateSystemGeodeticTransformation::GetMaxOffset()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetMaxOffset", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified offset value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalOffset(double dOffset)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.IsLegalOffset", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns the max legal value for a datum rotation, in arcseconds.
//
double CCoordinateSystemGeodeticTransformation::GetMaxRotation()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetMaxRotation", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified rotation value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalRotation(double dRotation)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.IsLegalRotation", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Get the maximum and minimum limits for Bursa/Wolfe scale.
//
double CCoordinateSystemGeodeticTransformation::GetBwScaleMin()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBwScaleMin", __LINE__, __WFILE__, NULL, L"", NULL);
}

double CCoordinateSystemGeodeticTransformation::GetBwScaleMax()
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.GetBwScaleMax", __LINE__, __WFILE__, NULL, L"", NULL);
}

//Returns whether the specified Bursa/Wolfe scale value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalBwScale(double dBwScale)
{
    throw new MgNotImplementedException(L"CCoordinateSystemGeodeticTransformation.IsLegalBwScale", __LINE__, __WFILE__, NULL, L"", NULL);
}

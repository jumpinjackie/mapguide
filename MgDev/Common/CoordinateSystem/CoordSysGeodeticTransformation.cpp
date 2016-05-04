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
#include "CoordSysUtil.h"                   //for Convert_Wide_To_UTF8, CsDictionaryOpenMode
#include "MentorUtil.h"                     //for BuildDtDefFromInterface + various utilities

#include "CoordSysGeodeticTransformDef.h"   //for CCoordinateSystemGeodeticTransformDef
#include "CoordSysDatum.h"                  //for CCoordinateSystemDatum
#include "CoordSysGeodeticTransformation.h" //for CCoordinateSystemGeodeticTransformation

using namespace CSLibrary;

//Default datum to use when user doesn't specify
static const wchar_t * const kpWGS84Datum = /*NOXLATE*/L"WGS84";

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//This will throw an exception if the set up cannot be made
//
CCoordinateSystemGeodeticTransformation::CCoordinateSystemGeodeticTransformation(MgCoordinateSystemCatalog* pCatalog, MgCoordinateSystemDatum* pSource, MgCoordinateSystemDatum *pTarget)
:m_pDtcprm(NULL), m_pDtSource(NULL), m_pDtTarget(NULL)
{
    SetCatalog(pCatalog);
    SetSourceAndTarget(pSource, pTarget);
}

//-----------------------------------------------------------------------------
CCoordinateSystemGeodeticTransformation::CCoordinateSystemGeodeticTransformation(MgCoordinateSystemCatalog* pCatalog,
                                                                                 MgCoordinateSystemGeodeticTransformDef* transformationDef,
                                                                                 bool createInversed)
: m_pDtcprm(NULL), m_pDtSource(NULL), m_pDtTarget(NULL)
{
    CHECKARGUMENTNULL(pCatalog, L"CCoordinateSystemGeodeticTransformation.ctor");
    CHECKARGUMENTNULL(transformationDef, L"CCoordinateSystemGeodeticTransformation.ctor");

    //this->Uninitialize(); //not needed - this does release the resourced held by this instance; we haven't set anything yet

    this->SetCatalog(pCatalog);

    //now - setup ourselves from the [MgCoordinateSystemGeodeticTransformDef] we've been passed
    //we'll not pass a source and a target datum to CS Map so it constructs the cs_Dtcprm_ struct
    //but we'll do that because we already have a transformation
    this->SetupFromTransformationDef(transformationDef, createInversed);
}

//-----------------------------------------------------------------------------
CCoordinateSystemGeodeticTransformation::~CCoordinateSystemGeodeticTransformation()
{
    Uninitialize();
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
    MG_TRY()

    //Set up our source datum
    MgCoordinateSystemDatum *pSourceTmp=GetWGS84IfNull(pSource);
    if (!pSourceTmp)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);
    }
    CCoordinateSystemDatum* pDtSourceImp=dynamic_cast<CCoordinateSystemDatum*>(pSourceTmp);
    assert(pDtSourceImp);
    if (!pDtSourceImp)
    {
        //release only if WGS84
        if (!pSource)
        {
            SAFE_RELEASE(pSourceTmp);
        }
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Set up our target datum
    MgCoordinateSystemDatum *pTargetTmp=GetWGS84IfNull(pTarget);
    if (!pTargetTmp)
    {
        //release only if WGS84
        if (!pSource)
        {
            SAFE_RELEASE(pSourceTmp);
        }
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);
    }
    CCoordinateSystemDatum* pDtTargetImp=dynamic_cast<CCoordinateSystemDatum*>(pTargetTmp);
    assert(pDtTargetImp);
    if (!pDtTargetImp)
    {
        //release only if WGS84
        if (!pSource)
        {
            SAFE_RELEASE(pSourceTmp);
        }
        //release only if WGS84
        if (!pTarget)
        {
            SAFE_RELEASE(pTargetTmp);
        }
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemNotFoundException", NULL);
    }

    //Set up our datum conversion struct
    CriticalClass.Enter();
    cs_Dtcprm_ *pDtcprm = CSdtcsu(&pDtSourceImp->m_datum, &pDtTargetImp->m_datum, 0, cs_DTCFLG_BLK_W);
    CriticalClass.Leave();
    assert(pDtcprm);
    if (!pDtcprm)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemGeodeticTransformationSetupException", NULL);
    }

    Uninitialize();
    m_pDtcprm = pDtcprm;
    m_pDtSource=pSourceTmp;
    if (pSource)
    {
        //add ref only if the input is not NULL
        //if NULL, then we have a WGS84 that was extracted from the dictionary and has
        //the correct refcount value
        SAFE_ADDREF(m_pDtSource);
    }
    m_pDtTarget=pTargetTmp;
    if (pTarget)
    {
        //add ref only if the input is not NULL
        //if NULL, then we have a WGS84 that was extracted from the dictionary and has
        //the correct refcount value
        SAFE_ADDREF(m_pDtTarget);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.SetSourceAndTarget")
}

//Gets a copy of the source datum of the converter (will
//be set to NULL if none has been set).  Caller is responsible for
//freeing the returned pointer via Release().
//
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetSource()
{
    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetSource", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return SAFE_ADDREF(m_pDtSource);
}

//Gets a copy of the target datum of the converter (will
//be set to NULL if none has been set).  Caller is responsible for
//freeing the returned pointer via Release().
//
MgCoordinateSystemDatum* CCoordinateSystemGeodeticTransformation::GetTarget()
{
    //Make sure we're initialized
    if (!IsInitialized())
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetTarget", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return SAFE_ADDREF(m_pDtTarget);
}

//Shifts the provided latitude/longitude coordinates from source datum
//to destination datum.
//Throws an exception if there's some problem doing the datum shift.  In case of error, the
//source latitude/longitude point will simply be copied to the
//destination (a "null" datum transformation).
//
void CCoordinateSystemGeodeticTransformation::Shift(MgCoordinate* pLonLat)
{
    MG_TRY()
    //Make sure we've been initialized
    if (!IsInitialized())
    {
        //Hasn't been initialized yet.  Set it up to do a
        //null transformation, assuming default for both source
        //and destination.
        SetSourceAndTarget(NULL, NULL);
    }
    assert(IsInitialized());

    //Do the shift
    double dLongitude=pLonLat->GetX();
    double dLatitude=pLonLat->GetY();
    bool bHasZ = pLonLat->GetDimension() & MgCoordinateDimension::XYZ;
    double dZ=pLonLat->GetZ();
    double *pdZ = (bHasZ ? &dZ : NULL);

    INT32 nResult = GeodeticTransformationPoint(m_pDtcprm, dLongitude, dLatitude, pdZ);
    if (1==nResult)
    {
        // Partial failure - Outside the useful range.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
    }
    else if (-1==nResult)
    {
        // Total failure - Outside the mathematical domain.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }
    pLonLat->SetX(dLongitude);
    pLonLat->SetY(dLatitude);
    if (bHasZ)
    {
        pLonLat->SetZ(*pdZ);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.Shift")
}

//Shifts the provided latitude/longitude coordinates from source datum
//to destination datum.
//Throws an exception if there's some problem doing the datum shift.  In case of error, the
//source latitude/longitude point will simply be copied to the
//destination (a "null" datum transformation).
//
MgCoordinate* CCoordinateSystemGeodeticTransformation::Shift(double dLongitude, double dLatitude)
{
    Ptr<MgCoordinate> pCoord;

    MG_TRY()

    //Make sure we've been initialized
    if (!IsInitialized())
    {
        //Hasn't been initialized yet.  Set it up to do a
        //null transformation, assuming default for both source
        //and destination.
        SetSourceAndTarget(NULL, NULL);
    }
    assert(IsInitialized());

    //Do the shift
    INT32 nResult=GeodeticTransformationPoint(m_pDtcprm, dLongitude, dLatitude, NULL);
    if (1==nResult)
    {
        // Partial failure - Outside the useful range.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
    }
    else if (-1==nResult)
    {
        // Total failure - Outside the mathematical domain.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }

    pCoord=new MgCoordinateXY(dLongitude, dLatitude);
    if (!pCoord)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.Shift")
    return pCoord.Detach();
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
    Ptr<MgCoordinate> pCoord;

    MG_TRY()
    //Make sure we've been initialized
    if (!IsInitialized())
    {
        //Hasn't been initialized yet.  Set it up to do a
        //null transformation, assuming default for both source
        //and destination.
        SetSourceAndTarget(NULL, NULL);
    }
    assert(IsInitialized());

    //Do the shift
    INT32 nResult = GeodeticTransformationPoint(m_pDtcprm, dLongitude, dLatitude, &dZ);
    if (1==nResult)
    {
        // Partial failure - Outside the useful range.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionWarningException", NULL);
    }
    else if (-1==nResult)
    {
        // Total failure - Outside the mathematical domain.
        throw new MgCoordinateSystemConversionFailedException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemConversionExtentException", NULL);
    }

    pCoord=new MgCoordinateXYZ(dLongitude, dLatitude, dZ);
    if (!pCoord)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemGeodeticTransformation.Shift", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.Shift")
    return pCoord.Detach();
}

//Utility function which returns whether the object has
//been initialized or not.
//
bool CCoordinateSystemGeodeticTransformation::IsInitialized() const
{
    return (NULL != m_pDtcprm);
}

//Utility function which uninitializes the data conversion struct.
//
void CCoordinateSystemGeodeticTransformation::Uninitialize()
{
    if (NULL != m_pDtcprm)
    {
        CS_dtcls(m_pDtcprm);
        m_pDtcprm = NULL;
    }
    SAFE_RELEASE(m_pDtSource);
    SAFE_RELEASE(m_pDtTarget);

    assert(!IsInitialized());
}

//Initializes this transformation instance from an [MgCoordinateSystemGeodeticTransformDef] object;
//That is, we don't let CS Map find the appropriate transformation based on a source and a target
//datum but will instruct CS Map to use the transformation defition we pass to it
//
void CCoordinateSystemGeodeticTransformation::SetupFromTransformationDef(MgCoordinateSystemGeodeticTransformDef* transformationDef, bool createInversed)
{
    cs_Dtcprm_* datumTransform = NULL;

    MG_TRY()

    //protect the call to the lib files; the calls to the [datumDictionary] below
    //we also enter the critical section but on the same thread - i.e. no problem;
    //putting the check here saves us from [enter, leave, enter leave]
    SmartCriticalClass criticalSection(true);

    CCoordinateSystemGeodeticTransformDef* transformDefImpl = dynamic_cast<CCoordinateSystemGeodeticTransformDef*>(transformationDef);
    if (NULL == transformDefImpl)
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticTransformation.SetupFromTransformationDef", __LINE__, __WFILE__, NULL, L"", NULL);

    cs_GeodeticTransform_ csMapTransformDef;
    transformDefImpl->CopyTo(csMapTransformDef);

    //ask CS_Map for the transformation
    datumTransform = CSdtcsu2(&csMapTransformDef, createInversed ? cs_DTCDIR_INV : cs_DTCDIR_FWD, cs_DTCFLG_BLK_W);

    if (NULL == datumTransform)
        throw new MgInvalidArgumentException(L"CCoordinateSystemGeodeticTransformation.SetupFromTransformationDef", __LINE__, __WFILE__, NULL, L"", NULL);

    Ptr<MgCoordinateSystemDatumDictionary> datumDictionary = this->m_pCatalog->GetDatumDictionary();
    Ptr<MgCoordinateSystemDatum> srcDatum = datumDictionary->GetDatum(transformationDef->GetSourceDatum());
    Ptr<MgCoordinateSystemDatum> trgDatum = datumDictionary->GetDatum(transformationDef->GetTargetDatum());

    this->m_pDtcprm = datumTransform;
    datumTransform = NULL; //make sure, it's not being released below

    this->m_pDtSource = srcDatum.Detach();  //m_pDtSource is no auto pointer
    this->m_pDtTarget = trgDatum.Detach();  //m_pDtTarget is no auto pointer

    MG_CATCH(L"CCoordinateSystemGeodeticTransformation.SetupFromTransformationDef")

    if (NULL != datumTransform) //has been set to NULL after assignment
        CS_free(datumTransform);

    MG_THROW()
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

//Private method that fails if the target datum is not WGS84
//In this release, this method only works when the target is WGS84 because
//the value is extracted directly from the source datum in Mentor
//and this release of Mentor only deals with datum definitions
//that store datum shift informations for shifting from themselves to WGS84
//So, if the target is not WGS84, we just return false
//
bool CCoordinateSystemGeodeticTransformation::GetDefinitionForGeodeticTransformationParameter(cs_Dtdef_& def)
{
    if (!m_pDtSource || !m_pDtTarget)
    {
        return false;
    }
    //if not WGS84 for the target system, bye bye!
    if (m_pDtTarget->GetDtCode()!=kpWGS84Datum)
    {
        return false;
    }

    bool bResult=BuildDtDefFromInterface(m_pDtSource, def);
    if (!bResult)
    {
        return false;
    }
    return true;
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
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetOffsetX", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }
    dResult=def.delta_X;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetOffsetX")
    return dResult;
}

double CCoordinateSystemGeodeticTransformation::GetOffsetY()
{
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetOffsetY", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }
    dResult=def.delta_Y;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetOffsetY")
    return dResult;
}

double CCoordinateSystemGeodeticTransformation::GetOffsetZ()
{
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetOffsetZ", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }
    dResult=def.delta_Z;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetOffsetZ")
    return dResult;
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
    MG_TRY()
    if (!m_pDtSource || !m_pDtTarget)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    //if not WGS84 for the target system, bye bye!
    if (m_pDtTarget->GetDtCode()!=kpWGS84Datum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    CCoordinateSystemDatum* pDatumImp=dynamic_cast<CCoordinateSystemDatum*>(m_pDtSource);
    assert(pDatumImp);
    if (!pDatumImp)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure this object isn't read-only
    if (m_pDtSource->IsProtected())
    {
        //can't change a read-only object
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    if (!IsLegalDatumOffset(x) ||
        !IsLegalDatumOffset(y) ||
        !IsLegalDatumOffset(z))
    {
        //Caller gave us an illegal value.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemGeodeticTransformation.SetOffset", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pDatumImp->m_DtDef.delta_X = pDatumImp->m_datum.delta_X = x;
    pDatumImp->m_DtDef.delta_Y = pDatumImp->m_datum.delta_Y = y;
    pDatumImp->m_DtDef.delta_Z = pDatumImp->m_datum.delta_Z = z;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.SetOffset")
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
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationX", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    dResult = def.rot_X;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationX")
    return dResult;
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformRotationY()
{
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationY", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    dResult = def.rot_Y;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationY")
    return dResult;
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformRotationZ()
{
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationZ", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    dResult = def.rot_Z;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationZ")
    return dResult;
}

double CCoordinateSystemGeodeticTransformation::GetBursaWolfeTransformBwScale()
{
    double dResult=0.;
    MG_TRY()

    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformBwScale", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    dResult = def.bwscale;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.GetBursaWolfeTransformRotationBwScale")
    return dResult;
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
    MG_TRY()

    if (!m_pDtSource || !m_pDtTarget)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    //if not WGS84 for the target system, bye bye!
    if (m_pDtTarget->GetDtCode()!=kpWGS84Datum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    CCoordinateSystemDatum* pDatumImp=dynamic_cast<CCoordinateSystemDatum*>(m_pDtSource);
    assert(pDatumImp);
    if (!pDatumImp)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure this object isn't read-only
    if (m_pDtSource->IsProtected())
    {
        //can't change a read-only object
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    if (!IsLegalDatumRotation(dRotationX) ||
        !IsLegalDatumRotation(dRotationY) ||
        !IsLegalDatumRotation(dRotationZ) ||
        !::IsLegalBwScale(dBwScale))
    {
        //Caller gave us an illegal value.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pDatumImp->m_DtDef.rot_X = pDatumImp->m_datum.rot_X = dRotationX;
    pDatumImp->m_DtDef.rot_Y = pDatumImp->m_datum.rot_Y = dRotationY;
    pDatumImp->m_DtDef.rot_Z = pDatumImp->m_datum.rot_Z = dRotationZ;
    pDatumImp->m_DtDef.bwscale = pDatumImp->m_datum.bwscale = dBwScale;

    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.SetBursaWolfeTransform")
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
    //if not WGS84 for the target system, bye bye!
    cs_Dtdef_ def;
    if (!GetDefinitionForGeodeticTransformationParameter(def))
    {
        return MgCoordinateSystemGeodeticTransformationMethod::None;
    }

    return static_cast<INT32>(def.to84_via);
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
    MG_TRY()

    if (!m_pDtSource || !m_pDtTarget)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    //if not WGS84 for the target system, bye bye!
    if (m_pDtTarget->GetDtCode()!=kpWGS84Datum)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemMismatchException", NULL);
    }

    CCoordinateSystemDatum* pDatumImp=dynamic_cast<CCoordinateSystemDatum*>(m_pDtSource);
    assert(pDatumImp);
    if (!pDatumImp)
    {
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    //Make sure this object isn't read-only
    if (m_pDtSource->IsProtected())
    {
        //can't change a read-only object
        throw new MgCoordinateSystemInitializationFailedException(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemDatumProtectedException", NULL);
    }

    if (!IsLegalGeodeticTransformationMethod(nGeodeticTransformationMethod))
    {
        //Not one of the known datum conversion techniques.
        throw new MgInvalidArgumentException(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    pDatumImp->m_DtDef.to84_via = pDatumImp->m_datum.to84_via = nGeodeticTransformationMethod;
    MG_CATCH_AND_THROW(L"MgCoordinateSystemGeodeticTransformation.SetGeodeticTransformationMethod")
}

//Gets the max legal value for a datum offset, in meters.
//
double CCoordinateSystemGeodeticTransformation::GetMaxOffset()
{
    return kdMaxDatumOffset;
}

//Returns whether the specified offset value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalOffset(double dOffset)
{
    return IsLegalDatumOffset(dOffset);
}

//Returns the max legal value for a datum rotation, in arcseconds.
//
double CCoordinateSystemGeodeticTransformation::GetMaxRotation()
{
    return kdMaxDatumRotation;
}

//Returns whether the specified rotation value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalRotation(double dRotation)
{
    return IsLegalDatumRotation(dRotation);
}

//Get the maximum and minimum limits for Bursa/Wolfe scale.
//
double CCoordinateSystemGeodeticTransformation::GetBwScaleMin()
{
    return kdMinBwScale;
}

double CCoordinateSystemGeodeticTransformation::GetBwScaleMax()
{
    return kdMaxBwScale;
}

//Returns whether the specified Bursa/Wolfe scale value is legal.
//
bool CCoordinateSystemGeodeticTransformation::IsLegalBwScale(double dBwScale)
{
    return ::IsLegalBwScale(dBwScale);
}

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

#include "ogr_spatialref.h"
#include "cpl_conv.h"
#include "cpl_error.h"
#include "cpl_multiproc.h"
#include "cpl_string.h"
#include "proj_api.h"

#include "CoordSysTransformation.h"

using namespace CSLibrary;

void* CCoordinateSystemTransformation::m_hMutex = NULL;

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system transformation.
///</summary>
CCoordinateSystemTransformation::CCoordinateSystemTransformation()
{
    m_poSRSSource = NULL;
    m_poSRSTarget = NULL;
    m_psPJSource = NULL;
    m_psPJTarget = NULL;

    m_bSourceLatLong = 0;
    m_bTargetLatLong = 0;
    m_dfSourceToRadians = 0.0;
    m_dfTargetToRadians = 0.0;
    m_dfSourceFromRadians = 0.0;
    m_dfTargetFromRadians = 0.0;

    m_nErrorCount = 0;
    m_bDatumShift = false;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
CCoordinateSystemTransformation::~CCoordinateSystemTransformation()
{
    if( m_poSRSSource != NULL )
    {
        if( m_poSRSSource->Dereference() <= 0 )
        {
            delete m_poSRSSource;
            m_poSRSSource = NULL;
        }
    }

    if( m_poSRSTarget != NULL )
    {
        if( m_poSRSTarget->Dereference() <= 0 )
        {
            delete m_poSRSTarget;
            m_poSRSTarget = NULL;
        }
    }

    CPLMutexHolderD( &m_hMutex );

    if( m_psPJSource != NULL )
        pj_free( m_psPJSource );

    if( m_psPJTarget != NULL )
        pj_free( m_psPJTarget );
}

/************************************************************************/
/*                 CreateCoordinateTransformation()                  */
/************************************************************************/

/**
 * Create transformation object.
 *
 * The delete operator, or OCTDestroyCoordinateTransformation() should
 * be used to destroy transformation objects.
 *
 * @param poSource source spatial reference system.
 * @param poTarget target spatial reference system.
 * @param bDatumShift true if you want to use a datum shift for transformations.
 * @return NULL on failure or a ready to use transformation object.
 */

OGRCoordinateTransformation*
CCoordinateSystemTransformation::CreateCoordinateTransformation( OGRSpatialReference *poSource,
                                                                 OGRSpatialReference *poTarget,
                                                                 bool bDatumShift )

{
    CCoordinateSystemTransformation  *poCT;
    poCT = new CCoordinateSystemTransformation();

    if( !poCT->Initialize( poSource, poTarget, bDatumShift ) )
    {
        delete poCT;
        return NULL;
    }
    else
    {
        return poCT;
    }
}

/************************************************************************/
/*                             Initialize()                             */
/************************************************************************/
int CCoordinateSystemTransformation::Initialize( OGRSpatialReference * poSourceIn,
                                                 OGRSpatialReference * poTargetIn,
                                                 bool bDatumShift )

{
    CPLMutexHolderD( &m_hMutex );

    if( poSourceIn == NULL || poTargetIn == NULL )
        return FALSE;

    m_poSRSSource = poSourceIn->Clone();
    m_poSRSTarget = poTargetIn->Clone();

    m_bSourceLatLong = m_poSRSSource->IsGeographic();
    m_bTargetLatLong = m_poSRSTarget->IsGeographic();

/* -------------------------------------------------------------------- */
/*      Setup source and target translations to radians for lat/long    */
/*      systems.                                                        */
/* -------------------------------------------------------------------- */
    m_dfSourceToRadians = DEG_TO_RAD;
    m_dfSourceFromRadians = RAD_TO_DEG;

    if( m_bSourceLatLong )
    {
        OGR_SRSNode *poUNITS = m_poSRSSource->GetAttrNode( "GEOGCS|UNIT" );
        if( poUNITS && poUNITS->GetChildCount() >= 2 )
        {
            m_dfSourceToRadians = atof(poUNITS->GetChild(1)->GetValue());
            if( m_dfSourceToRadians == 0.0 )
                m_dfSourceToRadians = DEG_TO_RAD;
            else
                m_dfSourceFromRadians = 1 / m_dfSourceToRadians;
        }
    }

    m_dfTargetToRadians = DEG_TO_RAD;
    m_dfTargetFromRadians = RAD_TO_DEG;

    if( m_bTargetLatLong )
    {
        OGR_SRSNode *poUNITS = m_poSRSTarget->GetAttrNode( "GEOGCS|UNIT" );
        if( poUNITS && poUNITS->GetChildCount() >= 2 )
        {
            m_dfTargetToRadians = atof(poUNITS->GetChild(1)->GetValue());
            if( m_dfTargetToRadians == 0.0 )
                m_dfTargetToRadians = DEG_TO_RAD;
            else
                m_dfTargetFromRadians = 1 / m_dfTargetToRadians;
        }
    }


/* -------------------------------------------------------------------- */
/*      Establish PROJ.4 handle for source if projection.               */
/* -------------------------------------------------------------------- */
    char        *pszProj4Defn;

    if( m_poSRSSource->exportToProj4( &pszProj4Defn ) != OGRERR_NONE )
        return FALSE;

    m_psPJSource = pj_init_plus( pszProj4Defn );

    if( m_psPJSource == NULL )
    {
        if( pj_get_errno_ref != NULL
            && pj_strerrno != NULL )
        {
            int *p_pj_errno = pj_get_errno_ref();

            CPLError( CE_Failure, CPLE_NotSupported,
                      "Failed to initialize PROJ.4 with `%s'.\n%s",
                      pszProj4Defn, pj_strerrno(*p_pj_errno) );
        }
        else
        {
            CPLError( CE_Failure, CPLE_NotSupported,
                      "Failed to initialize PROJ.4 with `%s'.\n",
                      pszProj4Defn );
        }
    }

    CPLFree( pszProj4Defn );

    if( m_psPJSource == NULL )
        return FALSE;

/* -------------------------------------------------------------------- */
/*      Establish PROJ.4 handle for target if projection.               */
/* -------------------------------------------------------------------- */
    if( m_poSRSTarget->exportToProj4( &pszProj4Defn ) != OGRERR_NONE )
        return FALSE;

    m_psPJTarget = pj_init_plus( pszProj4Defn );

    if( m_psPJTarget == NULL )
        CPLError( CE_Failure, CPLE_NotSupported,
                  "Failed to initialize PROJ.4 with `%s'.",
                  pszProj4Defn );

    CPLFree( pszProj4Defn );

    if( m_psPJTarget == NULL )
        return FALSE;

    return TRUE;
}

/************************************************************************/
/*                            GetSourceCS()                             */
/************************************************************************/
OGRSpatialReference* CCoordinateSystemTransformation::GetSourceCS()
{
    return m_poSRSSource;
}

/************************************************************************/
/*                            GetTargetCS()                             */
/************************************************************************/
OGRSpatialReference* CCoordinateSystemTransformation::GetTargetCS()
{
    return m_poSRSTarget;
}

/************************************************************************/
/*                             Transform()                              */
/*                                                                      */
/*      This is a small wrapper for the extended transform version.     */
/************************************************************************/
int CCoordinateSystemTransformation::Transform( int nCount, double *x, double *y, double *z )
{
    int *pabSuccess = (int *) CPLMalloc(sizeof(int) * nCount );
    int bOverallSuccess, i;

    bOverallSuccess = TransformEx( nCount, x, y, z, pabSuccess );

    for( i = 0; i < nCount; i++ )
    {
        if( !pabSuccess[i] )
        {
            bOverallSuccess = FALSE;
            break;
        }
    }

    CPLFree( pabSuccess );

    return bOverallSuccess;
}

/************************************************************************/
/*                            TransformEx()                             */
/************************************************************************/
int CCoordinateSystemTransformation::TransformEx( int nCount, double *x, double *y, double *z,
                                                  int *pabSuccess )
{
    int   err, i;

/* -------------------------------------------------------------------- */
/*      Potentially transform to radians.                               */
/* -------------------------------------------------------------------- */
    if( m_bSourceLatLong )
    {
        for( i = 0; i < nCount; i++ )
        {
            x[i] *= m_dfSourceToRadians;
            y[i] *= m_dfSourceToRadians;
        }
    }

/* -------------------------------------------------------------------- */
/*      Do the transformation using PROJ.4.                             */
/* -------------------------------------------------------------------- */
    CPLMutexHolderD( &m_hMutex );
    if( m_bDatumShift )
    {
        err = pj_datum_transform( m_psPJSource, m_psPJTarget, nCount, 1, x, y, z );
    }
    else
    {
        err = pj_transform( m_psPJSource, m_psPJTarget, nCount, 1, x, y, z );
    }

/* -------------------------------------------------------------------- */
/*      Try to report an error through CPL.  Get proj.4 error string    */
/*      if possible.  Try to avoid reporting thousands of error         */
/*      ... supress further error reporting on this OGRProj4CT if we    */
/*      have already reported 20 errors.                                */
/* -------------------------------------------------------------------- */
    if( err != 0 )
    {
        if( pabSuccess )
            memset( pabSuccess, 0, sizeof(int) * nCount );

        if( ++m_nErrorCount < 20 )
        {
            const char *pszError = NULL;
            if( pj_strerrno != NULL )
                pszError = pj_strerrno( err );

            if( pszError == NULL )
                CPLError( CE_Failure, CPLE_AppDefined,
                          "Reprojection failed, err = %d",
                          err );
            else
                CPLError( CE_Failure, CPLE_AppDefined, "%s", pszError );
        }
        else if( m_nErrorCount == 20 )
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Reprojection failed, err = %d, further errors will be supressed on the transform object.",
                      err );
        }

        return FALSE;
    }

/* -------------------------------------------------------------------- */
/*      Potentially transform back to degrees.                          */
/* -------------------------------------------------------------------- */
    if( m_bTargetLatLong )
    {
        for( i = 0; i < nCount; i++ )
        {
            if( x[i] != HUGE_VAL && y[i] != HUGE_VAL )
            {
                x[i] *= m_dfTargetFromRadians;
                y[i] *= m_dfTargetFromRadians;
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Establish error information if pabSuccess provided.             */
/* -------------------------------------------------------------------- */
    if( pabSuccess )
    {
        for( i = 0; i < nCount; i++ )
        {
            if( x[i] == HUGE_VAL || y[i] == HUGE_VAL )
                pabSuccess[i] = FALSE;
            else
                pabSuccess[i] = TRUE;
        }
    }

    return TRUE;
}

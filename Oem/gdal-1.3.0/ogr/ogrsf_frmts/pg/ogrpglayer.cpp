/******************************************************************************
 * $Id: ogrpglayer.cpp,v 1.20 2005/08/06 14:49:27 osemykin Exp $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements OGRPGLayer class  which implements shared handling
 *           of feature geometry and so forth needed by OGRPGResultLayer and
 *           OGRPGTableLayer.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log: ogrpglayer.cpp,v $
 * Revision 1.20  2005/08/06 14:49:27  osemykin
 * Added BINARY CURSOR support
 * Use it with 'PGB:dbname=...' instead 'PG:dbname=...'
 *
 * Revision 1.19  2005/07/20 01:45:01  fwarmerdam
 * added PostGIS 8.0 hex geometry format support
 *
 * Revision 1.18  2005/05/05 20:47:52  dron
 * Override GetExtent() method for PostGIS layers with PostGIS standard function
 * extent() (Oleg Semykin <oleg.semykin@gmail.com>
 *
 * Revision 1.17  2005/02/22 12:54:05  fwarmerdam
 * use OGRLayer base spatial filter support
 *
 * Revision 1.16  2005/02/02 20:54:27  fwarmerdam
 * track m_nFeaturesRead
 *
 * Revision 1.15  2004/07/10 04:46:24  warmerda
 * initialize nResultOffset, use soft transactions
 *
 * Revision 1.14  2004/05/08 02:14:49  warmerda
 * added GetFeature() on table, generalize FID support a bit
 *
 * Revision 1.13  2003/05/21 03:59:42  warmerda
 * expand tabs
 *
 * Revision 1.12  2003/02/01 07:55:48  warmerda
 * avoid dependence on libpq-fs.h
 *
 * Revision 1.11  2003/01/08 22:07:14  warmerda
 * Added support for integer and real list field types
 *
 * Revision 1.10  2002/05/09 16:03:19  warmerda
 * major upgrade to support SRS better and add ExecuteSQL
 *
 * Revision 1.9  2001/11/15 21:19:47  warmerda
 * added soft transaction semantics, handle null fields properly
 *
 * Revision 1.8  2001/11/15 16:10:12  warmerda
 * fixed some escaping issues with string field values
 *
 * Revision 1.7  2001/09/28 04:03:52  warmerda
 * partially upraded to PostGIS 0.6
 *
 * Revision 1.6  2001/07/18 04:55:16  warmerda
 * added CPL_CSVID
 *
 * Revision 1.5  2001/06/26 20:59:13  warmerda
 * implement efficient spatial and attribute query support
 *
 * Revision 1.4  2001/06/19 22:29:12  warmerda
 * upgraded to include PostGIS support
 *
 * Revision 1.3  2001/06/19 15:50:23  warmerda
 * added feature attribute query support
 *
 * Revision 1.2  2000/11/23 06:03:35  warmerda
 * added Oid support
 *
 * Revision 1.1  2000/10/17 17:46:51  warmerda
 * New
 *
 */

#include "cpl_conv.h"
#include "ogr_pg.h"
#include "cpl_string.h"

CPL_CVSID("$Id: ogrpglayer.cpp,v 1.20 2005/08/06 14:49:27 osemykin Exp $");

#define CURSOR_PAGE     1

// These originally are defined in libpq-fs.h.

#ifndef INV_WRITE
#define INV_WRITE               0x00020000
#define INV_READ                0x00040000
#endif

/************************************************************************/
/*                           OGRPGLayer()                               */
/************************************************************************/

OGRPGLayer::OGRPGLayer()

{
    poDS = NULL;

    bHasWkb = FALSE;
    bWkbAsOid = FALSE;
    bHasPostGISGeometry = FALSE;
    pszGeomColumn = NULL;
    pszQueryStatement = NULL;

    bHasFid = FALSE;
    pszFIDColumn = NULL;

    iNextShapeId = 0;
    nResultOffset = 0;

    poSRS = NULL;
    nSRSId = -2; // we haven't even queried the database for it yet. 

    /* Eventually we may need to make these a unique name */
    pszCursorName = "OGRPGLayerReader";
    hCursorResult = NULL;
    bCursorActive = FALSE;

    poFeatureDefn = NULL;
}

/************************************************************************/
/*                            ~OGRPGLayer()                             */
/************************************************************************/

OGRPGLayer::~OGRPGLayer()

{
    if( m_nFeaturesRead > 0 && poFeatureDefn != NULL )
    {
        CPLDebug( "PG", "%d features read on layer '%s'.",
                  (int) m_nFeaturesRead, 
                  poFeatureDefn->GetName() );
    }

    ResetReading();

    CPLFree( pszGeomColumn );
    CPLFree( pszFIDColumn );
    CPLFree( pszQueryStatement );

    if( poSRS != NULL )
        poSRS->Dereference();

    if( poFeatureDefn )
        delete poFeatureDefn;
}

/************************************************************************/
/*                            ResetReading()                            */
/************************************************************************/

void OGRPGLayer::ResetReading()

{
    PGconn      *hPGConn = poDS->GetPGConn();
    char        szCommand[1024];

    iNextShapeId = 0;

    if( hCursorResult != NULL )
    {
        PQclear( hCursorResult );

        if( bCursorActive )
        {
            sprintf( szCommand, "CLOSE %s", pszCursorName );
            
            hCursorResult = PQexec(hPGConn, szCommand);
            PQclear( hCursorResult );
        }

        poDS->FlushSoftTransaction();

        hCursorResult = NULL;
    }
}

/************************************************************************/
/*                           GetNextFeature()                           */
/************************************************************************/

OGRFeature *OGRPGLayer::GetNextFeature()

{

    for( ; TRUE; )
    {
        OGRFeature      *poFeature;

        poFeature = GetNextRawFeature();
        if( poFeature == NULL )
            return NULL;

        if( (m_poFilterGeom == NULL
            || bHasPostGISGeometry
            || !FilterGeometry( poFeature->GetGeometryRef() ) )
            && (m_poAttrQuery == NULL
                || m_poAttrQuery->Evaluate( poFeature )) )
            return poFeature;

        delete poFeature;
    }
}
/************************************************************************/
/*                          RecordToFeature()                           */
/*                                                                      */
/*      Convert the indicated record of the current result set into     */
/*      a feature.                                                      */
/************************************************************************/

OGRFeature *OGRPGLayer::RecordToFeature( int iRecord )

{
/* -------------------------------------------------------------------- */
/*      Create a feature from the current result.                       */
/* -------------------------------------------------------------------- */
    int         iField;
    OGRFeature *poFeature = new OGRFeature( poFeatureDefn );

    poFeature->SetFID( iNextShapeId );
    m_nFeaturesRead++;

/* ==================================================================== */
/*      Transfer all result fields we can.                              */
/* ==================================================================== */
    for( iField = 0; 
         iField < PQnfields(hCursorResult);
         iField++ )
    {
        int     iOGRField;

/* -------------------------------------------------------------------- */
/*      Handle FID.                                                     */
/* -------------------------------------------------------------------- */
        if( bHasFid && EQUAL(PQfname(hCursorResult,iField),pszFIDColumn) )
            poFeature->SetFID( atoi(PQgetvalue(hCursorResult,iRecord,iField)));

/* -------------------------------------------------------------------- */
/*      Handle PostGIS geometry                                         */
/* -------------------------------------------------------------------- */
        if( bHasPostGISGeometry
            && (EQUAL(PQfname(hCursorResult,iField),pszGeomColumn)
                || EQUAL(PQfname(hCursorResult,iField),"asEWKT") 
                || EQUAL(PQfname(hCursorResult,iField),"asText") ) )
        {
            char        *pszWKT;
            char        *pszPostSRID;
            OGRGeometry *poGeometry = NULL;
            
            pszWKT = PQgetvalue( hCursorResult, iRecord, iField );
            pszPostSRID = pszWKT;

            // optionally strip off PostGIS SRID identifier.  This
            // happens if we got a raw geometry field.
            if( EQUALN(pszPostSRID,"SRID=",5) )
            {
                while( *pszPostSRID != '\0' && *pszPostSRID != ';' )
                    pszPostSRID++;
                if( *pszPostSRID == ';' )
                    pszPostSRID++;
            }

            if( EQUALN(pszPostSRID,"00",2) || EQUALN(pszPostSRID,"01",2) )
            {
                poGeometry = 
                    HEXToGeometry( 
                        PQgetvalue( hCursorResult, iRecord, iField ) );
            }
            else
                OGRGeometryFactory::createFromWkt( &pszPostSRID, NULL, 
                                                   &poGeometry );
            if( poGeometry != NULL )
                poFeature->SetGeometryDirectly( poGeometry );

            continue;
        }
/* -------------------------------------------------------------------- */
/*      Handle raw binary geometry ... this hasn't been tested in a     */
/*      while.                                                          */
/* -------------------------------------------------------------------- */
        else if( EQUAL(PQfname(hCursorResult,iField),"WKB_GEOMETRY") )
        {
            if( bWkbAsOid )
            {
                poFeature->SetGeometryDirectly( 
                    OIDToGeometry( (Oid) atoi(
                        PQgetvalue( hCursorResult, 
                                    iRecord, iField ) ) ) );
            }
            else
            {
                poFeature->SetGeometryDirectly( 
                    BYTEAToGeometry( 
                        PQgetvalue( hCursorResult, 
                                    iRecord, iField ) ) );
            }
            continue;
        }
        /* Handle binary cursor result */
        else if ( EQUAL(PQfname(hCursorResult,iField),"AsBinary") )
        {
            GByte * pabyWkb = (GByte *)PQgetvalue( hCursorResult,
                                                   iRecord, iField);
            OGRGeometry * poGeom = NULL;
            OGRGeometryFactory::createFromWkb(pabyWkb,NULL,&poGeom);
            poFeature->SetGeometryDirectly( poGeom );
            continue;
        }

/* -------------------------------------------------------------------- */
/*      Transfer regular data fields.                                   */
/* -------------------------------------------------------------------- */
        iOGRField = 
            poFeatureDefn->GetFieldIndex(PQfname(hCursorResult,iField));

        if( iOGRField < 0 )
            continue;

        if( PQgetisnull( hCursorResult, iRecord, iField ) )
            continue;

        if( poFeatureDefn->GetFieldDefn(iOGRField)->GetType() == OFTIntegerList)
        {
            char **papszTokens;
            int *panList, nCount, i;

            papszTokens = CSLTokenizeStringComplex( 
                PQgetvalue( hCursorResult, iRecord, iField ),
                "{,}", FALSE, FALSE );

            nCount = CSLCount(papszTokens);
            panList = (int *) CPLCalloc(sizeof(int),nCount);

            for( i = 0; i < nCount; i++ )
                panList[i] = atoi(papszTokens[i]);
            poFeature->SetField( iOGRField, nCount, panList );
            CPLFree( panList );
            CSLDestroy( papszTokens );
        }
        else if( poFeatureDefn->GetFieldDefn(iOGRField)->GetType() == OFTRealList)
        {
            char **papszTokens;
            int nCount, i;
            double *padfList;

            papszTokens = CSLTokenizeStringComplex( 
                PQgetvalue( hCursorResult, iRecord, iField ),
                "{,}", FALSE, FALSE );

            nCount = CSLCount(papszTokens);
            padfList = (double *) CPLCalloc(sizeof(double),nCount);

            for( i = 0; i < nCount; i++ )
                padfList[i] = atof(papszTokens[i]);
            poFeature->SetField( iOGRField, nCount, padfList );
            CPLFree( padfList );
            CSLDestroy( papszTokens );
        }
        else
        {
            poFeature->SetField( iOGRField, 
                                 PQgetvalue( hCursorResult, 
                                             iRecord, iField ) );
        }
    }

    return poFeature;
}

/************************************************************************/
/*                         GetNextRawFeature()                          */
/************************************************************************/

OGRFeature *OGRPGLayer::GetNextRawFeature()

{
    PGconn      *hPGConn = poDS->GetPGConn();
    char        szCommand[4096];

/* -------------------------------------------------------------------- */
/*      Do we need to establish an initial query?                       */
/* -------------------------------------------------------------------- */
    if( iNextShapeId == 0 && hCursorResult == NULL )
    {
        CPLAssert( pszQueryStatement != NULL );

        poDS->FlushSoftTransaction();
        poDS->SoftStartTransaction();

        if ( poDS->bUseBinaryCursor )
            sprintf( szCommand, "DECLARE %s BINARY CURSOR for %s",
                     pszCursorName, pszQueryStatement );
        else
            sprintf( szCommand, "DECLARE %s CURSOR for %s",
                     pszCursorName, pszQueryStatement );

        CPLDebug( "OGR_PG", "PQexec(%s)", szCommand );

        hCursorResult = PQexec(hPGConn, szCommand );
        PQclear( hCursorResult );

        sprintf( szCommand, "FETCH %d in %s", CURSOR_PAGE, pszCursorName );
        hCursorResult = PQexec(hPGConn, szCommand );

        bCursorActive = TRUE;

        nResultOffset = 0;
    }

/* -------------------------------------------------------------------- */
/*      Are we in some sort of error condition?                         */
/* -------------------------------------------------------------------- */
    if( hCursorResult == NULL 
        || PQresultStatus(hCursorResult) != PGRES_TUPLES_OK )
    {
        iNextShapeId = MAX(1,iNextShapeId);
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Do we need to fetch more records?                               */
/* -------------------------------------------------------------------- */
    if( nResultOffset >= PQntuples(hCursorResult) 
        && bCursorActive )
    {
        PQclear( hCursorResult );

        sprintf( szCommand, "FETCH %d in %s", CURSOR_PAGE, pszCursorName );
        hCursorResult = PQexec(hPGConn, szCommand );

        nResultOffset = 0;
    }

/* -------------------------------------------------------------------- */
/*      Are we out of results?  If so complete the transaction, and     */
/*      cleanup, but don't reset the next shapeid.                      */
/* -------------------------------------------------------------------- */
    if( nResultOffset >= PQntuples(hCursorResult) )
    {
        PQclear( hCursorResult );

        if( bCursorActive )
        {
            sprintf( szCommand, "CLOSE %s", pszCursorName );
            
            hCursorResult = PQexec(hPGConn, szCommand);
            PQclear( hCursorResult );
        }

        poDS->FlushSoftTransaction();

        hCursorResult = NULL;
        bCursorActive = FALSE;

        iNextShapeId = MAX(1,iNextShapeId);

        return NULL;
    }

    
/* -------------------------------------------------------------------- */
/*      Create a feature from the current result.                       */
/* -------------------------------------------------------------------- */
    OGRFeature *poFeature = RecordToFeature( nResultOffset );

    nResultOffset++;
    iNextShapeId++;

    return poFeature;
}

/************************************************************************/
/*                             GetFeature()                             */
/************************************************************************/

OGRFeature *OGRPGLayer::GetFeature( long nFeatureId )

{
    /* This should be implemented! */

    return NULL;
}

/************************************************************************/
/*                           HEXToGeometry()                            */
/************************************************************************/

OGRGeometry *OGRPGLayer::HEXToGeometry( const char *pszBytea )

{
    GByte       *pabyWKB;
    int iSrc=0, iDst=0;
    OGRGeometry *poGeometry;

    if( pszBytea == NULL )
        return NULL;

    pabyWKB = (GByte *) CPLMalloc(strlen(pszBytea)+1);
    while( pszBytea[iSrc] != '\0' )
    {
        if( pszBytea[iSrc] > '0' && pszBytea[iSrc] <= '9' )
            pabyWKB[iDst] = pszBytea[iSrc] - '0';
        else if( pszBytea[iSrc] > 'A' && pszBytea[iSrc] <= 'F' )
            pabyWKB[iDst] = pszBytea[iSrc] - 'A' + 10;
        else if( pszBytea[iSrc] > 'a' && pszBytea[iSrc] <= 'f' )
            pabyWKB[iDst] = pszBytea[iSrc] - 'a' + 10;
        else 
            pabyWKB[iDst] = 0;

        pabyWKB[iDst] *= 16;

        iSrc++;

        if( pszBytea[iSrc] > '0' && pszBytea[iSrc] <= '9' )
            pabyWKB[iDst] += pszBytea[iSrc] - '0';
        else if( pszBytea[iSrc] > 'A' && pszBytea[iSrc] <= 'F' )
            pabyWKB[iDst] += pszBytea[iSrc] - 'A' + 10;
        else if( pszBytea[iSrc] > 'a' && pszBytea[iSrc] <= 'f' )
            pabyWKB[iDst] += pszBytea[iSrc] - 'a' + 10;
        else 
            pabyWKB[iDst] += 0;

        iSrc++;
        iDst++;
    }

    poGeometry = NULL;
    OGRGeometryFactory::createFromWkb( pabyWKB, NULL, &poGeometry, iDst );

    CPLFree( pabyWKB );
    return poGeometry;
}

/************************************************************************/
/*                          BYTEAToGeometry()                           */
/************************************************************************/

OGRGeometry *OGRPGLayer::BYTEAToGeometry( const char *pszBytea )

{
    GByte       *pabyWKB;
    int iSrc=0, iDst=0;
    OGRGeometry *poGeometry;

    if( pszBytea == NULL )
        return NULL;

    pabyWKB = (GByte *) CPLMalloc(strlen(pszBytea));
    while( pszBytea[iSrc] != '\0' )
    {
        if( pszBytea[iSrc] == '\\' )
        {
            if( pszBytea[iSrc+1] >= '0' && pszBytea[iSrc+1] <= '9' )
            {
                pabyWKB[iDst++] = 
                    (pszBytea[iSrc+1] - 48) * 64
                    + (pszBytea[iSrc+2] - 48) * 8
                    + (pszBytea[iSrc+3] - 48) * 1;
                iSrc += 4;
            }
            else
            {
                pabyWKB[iDst++] = pszBytea[iSrc+1];
                iSrc += 2;
            }
        }
        else
        {
            pabyWKB[iDst++] = pszBytea[iSrc++];
        }
    }

    poGeometry = NULL;
    OGRGeometryFactory::createFromWkb( pabyWKB, NULL, &poGeometry, iDst );

    CPLFree( pabyWKB );
    return poGeometry;
}

/************************************************************************/
/*                          GeometryToBYTEA()                           */
/************************************************************************/

char *OGRPGLayer::GeometryToBYTEA( OGRGeometry * poGeometry )

{
    int         nWkbSize = poGeometry->WkbSize();
    GByte       *pabyWKB;
    char        *pszTextBuf, *pszRetBuf;

    pabyWKB = (GByte *) CPLMalloc(nWkbSize);
    if( poGeometry->exportToWkb( wkbNDR, pabyWKB ) != OGRERR_NONE )
        return CPLStrdup("");

    pszTextBuf = (char *) CPLMalloc(nWkbSize*5+1);

    int  iSrc, iDst=0;

    for( iSrc = 0; iSrc < nWkbSize; iSrc++ )
    {
        if( pabyWKB[iSrc] < 40 || pabyWKB[iSrc] > 126
            || pabyWKB[iSrc] == '\\' )
        {
            sprintf( pszTextBuf+iDst, "\\\\%03o", pabyWKB[iSrc] );
            iDst += 5;
        }
        else
            pszTextBuf[iDst++] = pabyWKB[iSrc];
    }
    pszTextBuf[iDst] = '\0';

    pszRetBuf = CPLStrdup( pszTextBuf );
    CPLFree( pszTextBuf );

    return pszRetBuf;
}

/************************************************************************/
/*                          OIDToGeometry()                             */
/************************************************************************/

OGRGeometry *OGRPGLayer::OIDToGeometry( Oid oid )

{
    PGconn      *hPGConn = poDS->GetPGConn();
    GByte       *pabyWKB;
    int         fd, nBytes;
    OGRGeometry *poGeometry;

#define MAX_WKB 500000

    if( oid == 0 )
        return NULL;

    fd = lo_open( hPGConn, oid, INV_READ );
    if( fd < 0 )
        return NULL;

    pabyWKB = (GByte *) CPLMalloc(MAX_WKB);
    nBytes = lo_read( hPGConn, fd, (char *) pabyWKB, MAX_WKB );
    lo_close( hPGConn, fd );

    poGeometry = NULL;
    OGRGeometryFactory::createFromWkb( pabyWKB, NULL, &poGeometry, nBytes );

    CPLFree( pabyWKB );

    return poGeometry;
}

/************************************************************************/
/*                           GeometryToOID()                            */
/************************************************************************/

Oid OGRPGLayer::GeometryToOID( OGRGeometry * poGeometry )

{
    PGconn      *hPGConn = poDS->GetPGConn();
    int         nWkbSize = poGeometry->WkbSize();
    GByte       *pabyWKB;
    Oid         oid;
    int         fd, nBytesWritten;

    pabyWKB = (GByte *) CPLMalloc(nWkbSize);
    if( poGeometry->exportToWkb( wkbNDR, pabyWKB ) != OGRERR_NONE )
        return 0;

    oid = lo_creat( hPGConn, INV_READ|INV_WRITE );
    
    fd = lo_open( hPGConn, oid, INV_WRITE );
    nBytesWritten = lo_write( hPGConn, fd, (char *) pabyWKB, nWkbSize );
    lo_close( hPGConn, fd );

    if( nBytesWritten != nWkbSize )
    {
        CPLDebug( "OGR_PG", 
                  "Only wrote %d bytes of %d intended for (fd=%d,oid=%d).\n",
                  nBytesWritten, nWkbSize, fd, oid );
    }

    CPLFree( pabyWKB );
    
    return oid;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRPGLayer::TestCapability( const char * pszCap )

{
    if( EQUAL(pszCap,OLCRandomRead) )
        return FALSE;

    else if( EQUAL(pszCap,OLCFastFeatureCount) )
        return m_poFilterGeom == NULL || bHasPostGISGeometry;

    else if( EQUAL(pszCap,OLCFastSpatialFilter) )
        return TRUE;

    else if( EQUAL(pszCap,OLCTransactions) )
        return TRUE;

	else if( EQUAL(pszCap,OLCFastGetExtent) )
		return bHasPostGISGeometry;

    else 
        return FALSE;
}

/************************************************************************/
/*                          StartTransaction()                          */
/************************************************************************/

OGRErr OGRPGLayer::StartTransaction()

{
    return poDS->SoftStartTransaction();
}

/************************************************************************/
/*                         CommitTransaction()                          */
/************************************************************************/

OGRErr OGRPGLayer::CommitTransaction()

{
    return poDS->SoftCommit();
}

/************************************************************************/
/*                        RollbackTransaction()                         */
/************************************************************************/

OGRErr OGRPGLayer::RollbackTransaction()

{
    return poDS->SoftRollback();
}

/************************************************************************/
/*                           GetSpatialRef()                            */
/************************************************************************/

OGRSpatialReference *OGRPGLayer::GetSpatialRef()

{
    if( poSRS == NULL && nSRSId > -1 )
    {
        poSRS = poDS->FetchSRS( nSRSId );
        if( poSRS != NULL )
            poSRS->Reference();
        else
            nSRSId = -1;
    }

    return poSRS;
}

/******************************************************************************
 * $Id: s57reader.cpp,v 1.53 2005/08/07 13:58:17 fwarmerdam Exp $
 *
 * Project:  S-57 Translator
 * Purpose:  Implements S57Reader class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1999, 2001, Frank Warmerdam
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
 * $Log: s57reader.cpp,v $
 * Revision 1.53  2005/08/07 13:58:17  fwarmerdam
 * avoid use of std::sprintf in favor of plain sprintf
 *
 * Revision 1.52  2005/08/06 04:41:42  fwarmerdam
 * Incorporated support for finding update files using the catalog file
 * as per bug 840.
 *
 * Revision 1.51  2005/08/06 04:23:51  fwarmerdam
 * Bug 840: allow linestrings with zero vertices between end nodes.
 *
 * Revision 1.50  2005/06/23 18:22:48  fwarmerdam
 * Fixed ReadNextFeature(poTarget=NULL) support.
 *
 * Revision 1.49  2004/08/30 20:10:38  warmerda
 * implemented FetchLine() with optimized access
 *
 * Revision 1.48  2004/08/27 20:23:11  warmerda
 * fixed up a bit of formatting
 *
 * Revision 1.47  2004/06/24 00:37:12  warmerda
 * fixed S57M_OPTIONS logic, support deleting attributes
 *
 * Revision 1.46  2004/06/23 19:46:51  warmerda
 * fixed logic of APPLY updates logic
 *
 * Revision 1.45  2004/06/01 14:51:19  warmerda
 * expand tabs
 *
 * Revision 1.44  2003/11/17 20:10:46  warmerda
 * added support for writing FFPT linkages
 *
 * Revision 1.43  2003/11/12 21:23:40  warmerda
 * updates to new featuredefn generators
 *
 * Revision 1.42  2003/09/15 20:53:06  warmerda
 * fleshed out feature writing
 *
 * Revision 1.41  2003/09/12 21:18:06  warmerda
 * fixed width of AGEN field
 *
 * Revision 1.40  2003/09/09 18:11:17  warmerda
 * capture TOPI field from VRPT
 *
 * Revision 1.39  2003/09/09 16:44:51  warmerda
 * fix sounding support in ReadVector()
 *
 * Revision 1.38  2003/09/05 19:12:05  warmerda
 * added RETURN_PRIMITIVES support to get low level prims
 *
 * Revision 1.37  2003/01/02 21:45:23  warmerda
 * move OGRBuildPolygonsFromEdges into C API
 *
 * Revision 1.36  2002/10/28 22:31:00  warmerda
 * allow wkbMultiPoint25D for SOUNDG
 *
 * Revision 1.35  2002/05/14 20:34:27  warmerda
 * added PRESERVE_EMPTY_NUMBERS support
 *
 * Revision 1.34  2002/03/05 14:25:43  warmerda
 * expanded tabs
 *
 * Revision 1.33  2002/02/22 22:23:38  warmerda
 * added tolerances when assembling polygons
 *
 * Revision 1.32  2001/12/19 22:44:53  warmerda
 * added ADD_SOUNDG_DEPTH support
 *
 * Revision 1.31  2001/12/17 22:36:12  warmerda
 * added readFeature() method
 *
 * Revision 1.30  2001/12/14 19:40:18  warmerda
 * added optimized feature counting, and extents collection
 *
 * Revision 1.29  2001/09/12 17:09:44  warmerda
 * add auto-update support
 *
 * Revision 1.28  2001/09/04 16:21:11  warmerda
 * improved handling of corrupt line geometries
 *
 * Revision 1.27  2001/08/30 21:18:35  warmerda
 * removed debug info
 *
 * Revision 1.26  2001/08/30 21:06:55  warmerda
 * expand tabs
 *
 * Revision 1.25  2001/08/30 21:05:32  warmerda
 * added support for generic object if not recognised
 *
 * Revision 1.24  2001/08/30 14:55:23  warmerda
 * Treat SAT_LIST attributes as strings.
 *
 * Revision 1.23  2001/08/30 03:48:43  warmerda
 * preliminary implementation of S57 Update Support
 *
 * Revision 1.22  2001/07/18 04:55:16  warmerda
 * added CPL_CSVID
 *
 * Revision 1.21  2001/01/22 18:00:03  warmerda
 * Don't completely flip out if there is other than one spatial linkage for
 * in AssemblePointGeometry().  Record 577 of CA49995B.000 (newfiles) has this.
 *
 * Revision 1.20  2000/09/28 16:30:31  warmerda
 * avoid warnings
 *
 * Revision 1.19  2000/09/13 19:19:24  warmerda
 * added checking on NATF:ATTL values
 *
 * Revision 1.18  2000/06/16 18:10:05  warmerda
 * expanded tabs
 *
 * Revision 1.17  2000/06/16 18:00:58  warmerda
 * Fixed AssembleArea() to support multiple FSPT fields
 *
 * Revision 1.16  2000/06/07 21:34:04  warmerda
 * fixed problem with corrupt datasets with an ATTL value of zero
 *
 * Revision 1.15  1999/12/22 15:37:05  warmerda
 * removed abort
 *
 * Revision 1.14  1999/11/26 19:09:29  warmerda
 * Swapped XY for soundings.
 *
 * Revision 1.13  1999/11/26 16:17:58  warmerda
 * added DSNM
 *
 * Revision 1.12  1999/11/26 15:20:54  warmerda
 * Fixed multipoint.
 *
 * Revision 1.11  1999/11/26 15:17:01  warmerda
 * fixed lname to lnam
 *
 * Revision 1.10  1999/11/26 15:08:38  warmerda
 * added setoptions, and LNAM support
 *
 * Revision 1.9  1999/11/26 13:50:34  warmerda
 * added NATF support
 *
 * Revision 1.8  1999/11/26 13:26:03  warmerda
 * fixed up sounding support
 *
 * Revision 1.7  1999/11/25 20:53:49  warmerda
 * added sounding and S57_SPLIT_MULTIPOINT support
 *
 * Revision 1.6  1999/11/18 19:01:25  warmerda
 * expanded tabs
 *
 * Revision 1.5  1999/11/18 18:58:16  warmerda
 * make permissive of missing geometry so that update files work
 *
 * Revision 1.4  1999/11/16 21:47:32  warmerda
 * updated class occurance collection
 *
 * Revision 1.3  1999/11/08 22:23:00  warmerda
 * added object class support
 *
 * Revision 1.2  1999/11/04 21:19:13  warmerda
 * added polygon support
 *
 * Revision 1.1  1999/11/03 22:12:43  warmerda
 * New
 *
 */

#include "s57.h"
#include "ogr_api.h"
#include "cpl_conv.h"
#include "cpl_string.h"

#include <string>
#include <fstream>

CPL_CVSID("$Id: s57reader.cpp,v 1.53 2005/08/07 13:58:17 fwarmerdam Exp $");

/************************************************************************/
/*                             S57Reader()                              */
/************************************************************************/

S57Reader::S57Reader( const char * pszFilename )

{
    pszModuleName = CPLStrdup( pszFilename );
    pszDSNM = NULL;

    poModule = NULL;

    nFDefnCount = 0;
    papoFDefnList = NULL;

    nCOMF = 1000000;
    nSOMF = 10;

    poRegistrar = NULL;
    bFileIngested = FALSE;

    nNextFEIndex = 0;
    nNextVIIndex = 0;
    nNextVCIndex = 0;
    nNextVEIndex = 0;
    nNextVFIndex = 0;

    iPointOffset = 0;
    poMultiPoint = NULL;

    papszOptions = NULL;

    nOptionFlags = S57M_UPDATES;

    bMissingWarningIssued = FALSE;
    bAttrWarningIssued = FALSE;

    memset( apoFDefnByOBJL, 0, sizeof(apoFDefnByOBJL) );
}

/************************************************************************/
/*                             ~S57Reader()                             */
/************************************************************************/

S57Reader::~S57Reader()

{
    Close();
    
    CPLFree( pszModuleName );
    CSLDestroy( papszOptions );

    CPLFree( papoFDefnList );
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

int S57Reader::Open( int bTestOpen )

{
    if( poModule != NULL )
    {
        Rewind();
        return TRUE;
    }

    poModule = new DDFModule();
    if( !poModule->Open( pszModuleName ) )
    {
        // notdef: test bTestOpen.
        delete poModule;
        poModule = NULL;
        return FALSE;
    }

    // note that the following won't work for catalogs.
    if( poModule->FindFieldDefn("DSID") == NULL )
    {
        if( !bTestOpen )
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "%s is an ISO8211 file, but not an S-57 data file.\n",
                      pszModuleName );
        }
        delete poModule;
        poModule = NULL;
        return FALSE;
    }

    // Make sure the FSPT field is marked as repeating.
    DDFFieldDefn *poFSPT = poModule->FindFieldDefn( "FSPT" );
    if( poFSPT != NULL && !poFSPT->IsRepeating() )
    {
        CPLDebug( "S57", "Forcing FSPT field to be repeating." );
        poFSPT->SetRepeatingFlag( TRUE );
    }

    nNextFEIndex = 0;
    nNextVIIndex = 0;
    nNextVCIndex = 0;
    nNextVEIndex = 0;
    nNextVFIndex = 0;
    
    return TRUE;
}

/************************************************************************/
/*                               Close()                                */
/************************************************************************/

void S57Reader::Close()

{
    if( poModule != NULL )
    {
        oVI_Index.Clear();
        oVC_Index.Clear();
        oVE_Index.Clear();
        oVF_Index.Clear();
        oFE_Index.Clear();

        ClearPendingMultiPoint();

        delete poModule;
        poModule = NULL;

        bFileIngested = FALSE;

        CPLFree( pszDSNM );
        pszDSNM = NULL;
    }
}

/************************************************************************/
/*                       ClearPendingMultiPoint()                       */
/************************************************************************/

void S57Reader::ClearPendingMultiPoint()

{
    if( poMultiPoint != NULL )
    {
        delete poMultiPoint;
        poMultiPoint = NULL;
    }
}

/************************************************************************/
/*                       NextPendingMultiPoint()                        */
/************************************************************************/

OGRFeature *S57Reader::NextPendingMultiPoint()

{
    CPLAssert( poMultiPoint != NULL );
    CPLAssert( wkbFlatten(poMultiPoint->GetGeometryRef()->getGeometryType())
                                                        == wkbMultiPoint );

    OGRFeatureDefn *poDefn = poMultiPoint->GetDefnRef();
    OGRFeature  *poPoint = new OGRFeature( poDefn );
    OGRMultiPoint *poMPGeom = (OGRMultiPoint *) poMultiPoint->GetGeometryRef();
    OGRPoint    *poSrcPoint;

    poPoint->SetFID( poMultiPoint->GetFID() );
    
    for( int i = 0; i < poDefn->GetFieldCount(); i++ )
    {
        poPoint->SetField( i, poMultiPoint->GetRawFieldRef(i) );
    }

    poSrcPoint = (OGRPoint *) poMPGeom->getGeometryRef( iPointOffset++ );
    poPoint->SetGeometry( poSrcPoint );

    if( poPoint != NULL )
        poPoint->SetField( "DEPTH", poSrcPoint->getZ() );

    if( iPointOffset >= poMPGeom->getNumGeometries() )
        ClearPendingMultiPoint();

    return poPoint;
}

/************************************************************************/
/*                             SetOptions()                             */
/************************************************************************/

void S57Reader::SetOptions( char ** papszOptionsIn )

{
    const char * pszOptionValue;
    
    CSLDestroy( papszOptions );
    papszOptions = CSLDuplicate( papszOptionsIn );

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_SPLIT_MULTIPOINT );
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_SPLIT_MULTIPOINT;
    else
        nOptionFlags &= ~S57M_SPLIT_MULTIPOINT;

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_ADD_SOUNDG_DEPTH );
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_ADD_SOUNDG_DEPTH;
    else
        nOptionFlags &= ~S57M_ADD_SOUNDG_DEPTH;

    CPLAssert( ! (nOptionFlags & S57M_ADD_SOUNDG_DEPTH)
               || (nOptionFlags & S57M_SPLIT_MULTIPOINT) );

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_LNAM_REFS );
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_LNAM_REFS;
    else
        nOptionFlags &= ~S57M_LNAM_REFS;

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_UPDATES );
    if( pszOptionValue == NULL )
        /* no change */;
    else if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"APPLY") )
        nOptionFlags &= ~S57M_UPDATES;
    else
        nOptionFlags |= S57M_UPDATES;

    pszOptionValue = CSLFetchNameValue(papszOptions, 
                                       S57O_PRESERVE_EMPTY_NUMBERS);
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_PRESERVE_EMPTY_NUMBERS;
    else
        nOptionFlags &= ~S57M_PRESERVE_EMPTY_NUMBERS;

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_RETURN_PRIMITIVES );
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_RETURN_PRIMITIVES;
    else
        nOptionFlags &= ~S57M_RETURN_PRIMITIVES;

    pszOptionValue = CSLFetchNameValue( papszOptions, S57O_RETURN_LINKAGES );
    if( pszOptionValue != NULL && !EQUAL(pszOptionValue,"OFF") )
        nOptionFlags |= S57M_RETURN_LINKAGES;
    else
        nOptionFlags &= ~S57M_RETURN_LINKAGES;
}

/************************************************************************/
/*                           SetClassBased()                            */
/************************************************************************/

void S57Reader::SetClassBased( S57ClassRegistrar * poReg )

{
    poRegistrar = poReg;
}

/************************************************************************/
/*                               Rewind()                               */
/************************************************************************/

void S57Reader::Rewind()

{
    ClearPendingMultiPoint();
    nNextFEIndex = 0;
    nNextVIIndex = 0;
    nNextVCIndex = 0;
    nNextVEIndex = 0;
    nNextVFIndex = 0;
}

/************************************************************************/
/*                               Ingest()                               */
/*                                                                      */
/*      Read all the records into memory, adding to the appropriate     */
/*      indexes.                                                        */
/************************************************************************/

void S57Reader::Ingest()

{
    DDFRecord   *poRecord;
    
    if( poModule == NULL || bFileIngested )
        return;

/* -------------------------------------------------------------------- */
/*      Read all the records in the module, and place them in           */
/*      appropriate indexes.                                            */
/* -------------------------------------------------------------------- */
    while( (poRecord = poModule->ReadRecord()) != NULL )
    {
        DDFField        *poKeyField = poRecord->GetField(1);
        
        if( EQUAL(poKeyField->GetFieldDefn()->GetName(),"VRID") )
        {
            int         nRCNM = poRecord->GetIntSubfield( "VRID",0, "RCNM",0);
            int         nRCID = poRecord->GetIntSubfield( "VRID",0, "RCID",0);

            switch( nRCNM )
            {
              case RCNM_VI:
                oVI_Index.AddRecord( nRCID, poRecord->Clone() );
                break;

              case RCNM_VC:
                oVC_Index.AddRecord( nRCID, poRecord->Clone() );
                break;

              case RCNM_VE:
                oVE_Index.AddRecord( nRCID, poRecord->Clone() );
                break;

              case RCNM_VF:
                oVF_Index.AddRecord( nRCID, poRecord->Clone() );
                break;

              default:
                CPLAssert( FALSE );
                break;
            }
        }

        else if( EQUAL(poKeyField->GetFieldDefn()->GetName(),"DSPM") )
        {
            nCOMF = MAX(1,poRecord->GetIntSubfield( "DSPM",0, "COMF",0));
            nSOMF = MAX(1,poRecord->GetIntSubfield( "DSPM",0, "SOMF",0));
        }

        else if( EQUAL(poKeyField->GetFieldDefn()->GetName(),"FRID") )
        {
            int         nRCID = poRecord->GetIntSubfield( "FRID",0, "RCID",0);
            
            oFE_Index.AddRecord( nRCID, poRecord->Clone() );
        }

        else if( EQUAL(poKeyField->GetFieldDefn()->GetName(),"DSID") )
        {
            CPLFree( pszDSNM );
            pszDSNM =
                CPLStrdup(poRecord->GetStringSubfield( "DSID", 0, "DSNM", 0 ));
        }

        else
        {
            CPLDebug( "S57",
                      "Skipping %s record in S57Reader::Ingest().\n",
                      poKeyField->GetFieldDefn()->GetName() );
        }
    }

    bFileIngested = TRUE;

/* -------------------------------------------------------------------- */
/*      If update support is enabled, read and apply them.              */
/* -------------------------------------------------------------------- */
    if( nOptionFlags & S57M_UPDATES )
        FindAndApplyUpdates();
}

/************************************************************************/
/*                           SetNextFEIndex()                           */
/************************************************************************/

void S57Reader::SetNextFEIndex( int nNewIndex, int nRCNM )

{
    if( nRCNM == RCNM_VI )
        nNextVIIndex = nNewIndex;
    else if( nRCNM == RCNM_VC )
        nNextVCIndex = nNewIndex;
    else if( nRCNM == RCNM_VE )
        nNextVEIndex = nNewIndex;
    else if( nRCNM == RCNM_VF )
        nNextVFIndex = nNewIndex;
    else
    {
        if( nNextFEIndex != nNewIndex )
            ClearPendingMultiPoint();
        
        nNextFEIndex = nNewIndex;
    }
}

/************************************************************************/
/*                           GetNextFEIndex()                           */
/************************************************************************/

int S57Reader::GetNextFEIndex( int nRCNM )

{
    if( nRCNM == RCNM_VI )
        return nNextVIIndex;
    else if( nRCNM == RCNM_VC )
        return nNextVCIndex;
    else if( nRCNM == RCNM_VE )
        return nNextVEIndex;
    else if( nRCNM == RCNM_VF )
        return nNextVFIndex;
    else
        return nNextFEIndex;
}

/************************************************************************/
/*                          ReadNextFeature()                           */
/************************************************************************/

OGRFeature * S57Reader::ReadNextFeature( OGRFeatureDefn * poTarget )

{
    if( !bFileIngested )
        Ingest();

/* -------------------------------------------------------------------- */
/*      Special case for "in progress" multipoints being split up.      */
/* -------------------------------------------------------------------- */
    if( poMultiPoint != NULL )
    {
        if( poTarget == NULL || poTarget == poMultiPoint->GetDefnRef() )
        {
            return NextPendingMultiPoint();
        }
        else
        {
            ClearPendingMultiPoint();
        }
    }

/* -------------------------------------------------------------------- */
/*      Next vector feature?                                            */
/* -------------------------------------------------------------------- */
    if( nOptionFlags & S57M_RETURN_PRIMITIVES )
    {
        int nRCNM = 0;
        int *pnCounter = NULL;

        if( poTarget == NULL )
        {
            if( nNextVIIndex < oVI_Index.GetCount() )
            {
                nRCNM = RCNM_VI;
                pnCounter = &nNextVIIndex;
            }
            else if( nNextVCIndex < oVC_Index.GetCount() )
            {
                nRCNM = RCNM_VC;
                pnCounter = &nNextVCIndex;
            }
            else if( nNextVEIndex < oVE_Index.GetCount() )
            {
                nRCNM = RCNM_VE;
                pnCounter = &nNextVEIndex;
            }
            else if( nNextVFIndex < oVF_Index.GetCount() )
            {
                nRCNM = RCNM_VF;
                pnCounter = &nNextVFIndex;
            }
        }
        else
        {
            if( EQUAL(poTarget->GetName(),OGRN_VI) )
            {
                nRCNM = RCNM_VI;
                pnCounter = &nNextVIIndex;
            }
            else if( EQUAL(poTarget->GetName(),OGRN_VC) )
            {
                nRCNM = RCNM_VC;
                pnCounter = &nNextVCIndex;
            }
            else if( EQUAL(poTarget->GetName(),OGRN_VE) )
            {
                nRCNM = RCNM_VE;
                pnCounter = &nNextVEIndex;
            }
            else if( EQUAL(poTarget->GetName(),OGRN_VF) )
            {
                nRCNM = RCNM_VF;
                pnCounter = &nNextVFIndex;
            }
        }

        if( nRCNM != 0 )
        {
            OGRFeature *poFeature = ReadVector( *pnCounter, nRCNM );
            if( poFeature != NULL )
            {
                *pnCounter += 1;
                return poFeature;
            }
        }
    }
        
/* -------------------------------------------------------------------- */
/*      Next feature.                                                   */
/* -------------------------------------------------------------------- */
    while( nNextFEIndex < oFE_Index.GetCount() )
    {
        OGRFeature      *poFeature;
        OGRFeatureDefn *poFeatureDefn;

        poFeatureDefn = (OGRFeatureDefn *) 
            oFE_Index.GetClientInfoByIndex( nNextFEIndex );

        if( poFeatureDefn == NULL )
        {
            poFeatureDefn = FindFDefn( oFE_Index.GetByIndex( nNextFEIndex ) );
            oFE_Index.SetClientInfoByIndex( nNextFEIndex, poFeatureDefn );
        }

        if( poFeatureDefn != poTarget && poTarget != NULL )
        {
            nNextFEIndex++;
            continue;
        }

        poFeature = ReadFeature( nNextFEIndex++, poTarget );
        if( poFeature != NULL )
        {
            if( (nOptionFlags & S57M_SPLIT_MULTIPOINT)
                && poFeature->GetGeometryRef() != NULL
                && wkbFlatten(poFeature->GetGeometryRef()->getGeometryType())
                                                        == wkbMultiPoint)
            {
                poMultiPoint = poFeature;
                iPointOffset = 0;
                return NextPendingMultiPoint();
            }

            return poFeature;
        }
    }

    return NULL;
}

/************************************************************************/
/*                            ReadFeature()                             */
/*                                                                      */
/*      Read the features who's id is provided.                         */
/************************************************************************/

OGRFeature *S57Reader::ReadFeature( int nFeatureId, OGRFeatureDefn *poTarget )

{
    OGRFeature  *poFeature;

    if( nFeatureId < 0 || nFeatureId >= oFE_Index.GetCount() )
        return NULL;

    poFeature = AssembleFeature( oFE_Index.GetByIndex(nFeatureId),
                                 poTarget );
    if( poFeature != NULL )
        poFeature->SetFID( nFeatureId );

    return poFeature;
}


/************************************************************************/
/*                          AssembleFeature()                           */
/*                                                                      */
/*      Assemble an OGR feature based on a feature record.              */
/************************************************************************/

OGRFeature *S57Reader::AssembleFeature( DDFRecord * poRecord,
                                        OGRFeatureDefn * poTarget )

{
    int         nPRIM, nOBJL;
    OGRFeatureDefn *poFDefn;

/* -------------------------------------------------------------------- */
/*      Find the feature definition to use.  Currently this is based    */
/*      on the primitive, but eventually this should be based on the    */
/*      object class (FRID.OBJL) in some cases, and the primitive in    */
/*      others.                                                         */
/* -------------------------------------------------------------------- */
    poFDefn = FindFDefn( poRecord );
    if( poFDefn == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Does this match our target feature definition?  If not skip     */
/*      this feature.                                                   */
/* -------------------------------------------------------------------- */
    if( poTarget != NULL && poFDefn != poTarget )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Create the new feature object.                                  */
/* -------------------------------------------------------------------- */
    OGRFeature          *poFeature;

    poFeature = new OGRFeature( poFDefn );

/* -------------------------------------------------------------------- */
/*      Assign a few standard feature attribues.                        */
/* -------------------------------------------------------------------- */
    nOBJL = poRecord->GetIntSubfield( "FRID", 0, "OBJL", 0 );
    poFeature->SetField( "OBJL", nOBJL );

    poFeature->SetField( "RCID",
                         poRecord->GetIntSubfield( "FRID", 0, "RCID", 0 ));
    poFeature->SetField( "PRIM",
                         poRecord->GetIntSubfield( "FRID", 0, "PRIM", 0 ));
    poFeature->SetField( "GRUP",
                         poRecord->GetIntSubfield( "FRID", 0, "GRUP", 0 ));
    poFeature->SetField( "RVER",
                         poRecord->GetIntSubfield( "FRID", 0, "RVER", 0 ));
    poFeature->SetField( "AGEN",
                         poRecord->GetIntSubfield( "FOID", 0, "AGEN", 0 ));
    poFeature->SetField( "FIDN",
                         poRecord->GetIntSubfield( "FOID", 0, "FIDN", 0 ));
    poFeature->SetField( "FIDS",
                         poRecord->GetIntSubfield( "FOID", 0, "FIDS", 0 ));

/* -------------------------------------------------------------------- */
/*      Generate long name, if requested.                               */
/* -------------------------------------------------------------------- */
    if( nOptionFlags & S57M_LNAM_REFS )
    {
        GenerateLNAMAndRefs( poRecord, poFeature );
    }

/* -------------------------------------------------------------------- */
/*      Generate primitive references if requested.                     */
/* -------------------------------------------------------------------- */
    if( nOptionFlags & S57M_RETURN_LINKAGES )
        GenerateFSPTAttributes( poRecord, poFeature );

/* -------------------------------------------------------------------- */
/*      Apply object class specific attributes, if supported.           */
/* -------------------------------------------------------------------- */
    if( poRegistrar != NULL )
        ApplyObjectClassAttributes( poRecord, poFeature );

/* -------------------------------------------------------------------- */
/*      Find and assign spatial component.                              */
/* -------------------------------------------------------------------- */
    nPRIM = poRecord->GetIntSubfield( "FRID", 0, "PRIM", 0 );

    if( nPRIM == PRIM_P )
    {
        if( nOBJL == 129 ) /* SOUNDG */
            AssembleSoundingGeometry( poRecord, poFeature );
        else
            AssemblePointGeometry( poRecord, poFeature );
    }
    else if( nPRIM == PRIM_L )
    {
        AssembleLineGeometry( poRecord, poFeature );
    }
    else if( nPRIM == PRIM_A )
    {
        AssembleAreaGeometry( poRecord, poFeature );
    }

    return poFeature;
}

/************************************************************************/
/*                     ApplyObjectClassAttributes()                     */
/************************************************************************/

void S57Reader::ApplyObjectClassAttributes( DDFRecord * poRecord,
                                            OGRFeature * poFeature )

{
/* -------------------------------------------------------------------- */
/*      ATTF Attributes                                                 */
/* -------------------------------------------------------------------- */
    DDFField    *poATTF = poRecord->FindField( "ATTF" );
    int         nAttrCount, iAttr;

    if( poATTF == NULL )
        return;

    nAttrCount = poATTF->GetRepeatCount();
    for( iAttr = 0; iAttr < nAttrCount; iAttr++ )
    {
        int     nAttrId = poRecord->GetIntSubfield("ATTF",0,"ATTL",iAttr);
        const char *pszAcronym;
        
        if( nAttrId < 1 || nAttrId > poRegistrar->GetMaxAttrIndex() 
            || (pszAcronym = poRegistrar->GetAttrAcronym(nAttrId)) == NULL )
        {
            if( !bAttrWarningIssued )
            {
                bAttrWarningIssued = TRUE;
                CPLError( CE_Warning, CPLE_AppDefined,
                          "Illegal feature attribute id (ATTF:ATTL[%d]) of %d\n"
                          "on feature FIDN=%d, FIDS=%d.\n"
                          "Skipping attribute, no more warnings will be issued.",
                          iAttr, nAttrId, 
                          poFeature->GetFieldAsInteger( "FIDN" ),
                          poFeature->GetFieldAsInteger( "FIDS" ) );
            }

            continue;
        }

        /* Fetch the attribute value */
        const char *pszValue;
        pszValue = poRecord->GetStringSubfield("ATTF",0,"ATVL",iAttr);
        
        /* Apply to feature in an appropriate way */
        int iField;
        OGRFieldDefn *poFldDefn;

        iField = poFeature->GetDefnRef()->GetFieldIndex(pszAcronym);
        if( iField < 0 )
        {
            if( !bMissingWarningIssued )
            {
                bMissingWarningIssued = TRUE;
                CPLError( CE_Warning, CPLE_AppDefined, 
                          "Attributes %s ignored, not in expected schema.\n"
                          "No more warnings will be issued for this dataset.", 
                          pszAcronym );
            }
            continue;
        }

        poFldDefn = poFeature->GetDefnRef()->GetFieldDefn( iField );
        if( poFldDefn->GetType() == OFTInteger 
            || poFldDefn->GetType() == OFTReal )
        {
            if( strlen(pszValue) == 0 )
            {
                if( nOptionFlags & S57M_PRESERVE_EMPTY_NUMBERS )
                    poFeature->SetField( iField, EMPTY_NUMBER_MARKER );
                else
                    /* leave as null if value was empty string */;
            }
            else
                poFeature->SetField( iField, pszValue );
        }
        else
            poFeature->SetField( iField, pszValue );
    }
    
/* -------------------------------------------------------------------- */
/*      NATF (national) attributes                                      */
/* -------------------------------------------------------------------- */
    DDFField    *poNATF = poRecord->FindField( "NATF" );

    if( poNATF == NULL )
        return;

    nAttrCount = poNATF->GetRepeatCount();
    for( iAttr = 0; iAttr < nAttrCount; iAttr++ )
    {
        int     nAttrId = poRecord->GetIntSubfield("NATF",0,"ATTL",iAttr);
        const char *pszAcronym;

        if( nAttrId < 1 || nAttrId >= poRegistrar->GetMaxAttrIndex()
            || (pszAcronym = poRegistrar->GetAttrAcronym(nAttrId)) == NULL )
        {
            static int bAttrWarningIssued = FALSE;

            if( !bAttrWarningIssued )
            {
                bAttrWarningIssued = TRUE;
                CPLError( CE_Warning, CPLE_AppDefined,
                          "Illegal feature attribute id (NATF:ATTL[%d]) of %d\n"
                          "on feature FIDN=%d, FIDS=%d.\n"
                          "Skipping attribute, no more warnings will be issued.",
                          iAttr, nAttrId, 
                          poFeature->GetFieldAsInteger( "FIDN" ),
                          poFeature->GetFieldAsInteger( "FIDS" ) );
            }

            continue;
        }
        
        poFeature->SetField( pszAcronym, 
                             poRecord->GetStringSubfield("NATF",0,"ATVL",iAttr) );
    }
}

/************************************************************************/
/*                        GenerateLNAMAndRefs()                         */
/************************************************************************/

void S57Reader::GenerateLNAMAndRefs( DDFRecord * poRecord,
                                     OGRFeature * poFeature )

{
    char        szLNAM[32];
        
/* -------------------------------------------------------------------- */
/*      Apply the LNAM to the object.                                   */
/* -------------------------------------------------------------------- */
    sprintf( szLNAM, "%04X%08X%04X",
             poFeature->GetFieldAsInteger( "AGEN" ),
             poFeature->GetFieldAsInteger( "FIDN" ),
             poFeature->GetFieldAsInteger( "FIDS" ) );
    poFeature->SetField( "LNAM", szLNAM );

/* -------------------------------------------------------------------- */
/*      Do we have references to other features.                        */
/* -------------------------------------------------------------------- */
    DDFField    *poFFPT;

    poFFPT = poRecord->FindField( "FFPT" );

    if( poFFPT == NULL )
        return;

/* -------------------------------------------------------------------- */
/*      Apply references.                                               */
/* -------------------------------------------------------------------- */
    int         nRefCount = poFFPT->GetRepeatCount();
    DDFSubfieldDefn *poLNAM;
    char        **papszRefs = NULL;
    int         *panRIND = (int *) CPLMalloc(sizeof(int) * nRefCount);

    poLNAM = poFFPT->GetFieldDefn()->FindSubfieldDefn( "LNAM" );
    if( poLNAM == NULL )
        return;

    for( int iRef = 0; iRef < nRefCount; iRef++ )
    {
        unsigned char *pabyData;

        pabyData = (unsigned char *)
            poFFPT->GetSubfieldData( poLNAM, NULL, iRef );
        
        sprintf( szLNAM, "%02X%02X%02X%02X%02X%02X%02X%02X",
                 pabyData[1], pabyData[0], /* AGEN */
                 pabyData[5], pabyData[4], pabyData[3], pabyData[2], /* FIDN */
                 pabyData[7], pabyData[6] );

        papszRefs = CSLAddString( papszRefs, szLNAM );

        panRIND[iRef] = pabyData[8];
    }

    poFeature->SetField( "LNAM_REFS", papszRefs );
    CSLDestroy( papszRefs );

    poFeature->SetField( "FFPT_RIND", nRefCount, panRIND );
    CPLFree( panRIND );
}

/************************************************************************/
/*                       GenerateFSPTAttributes()                       */
/************************************************************************/

void S57Reader::GenerateFSPTAttributes( DDFRecord * poRecord,
                                        OGRFeature * poFeature )

{
/* -------------------------------------------------------------------- */
/*      Feature the spatial record containing the point.                */
/* -------------------------------------------------------------------- */
    DDFField    *poFSPT;
    int         nCount, i;

    poFSPT = poRecord->FindField( "FSPT" );
    if( poFSPT == NULL )
        return;
        
    nCount = poFSPT->GetRepeatCount();

/* -------------------------------------------------------------------- */
/*      Allocate working lists of the attributes.                       */
/* -------------------------------------------------------------------- */
    int *panORNT, *panUSAG, *panMASK, *panRCNM, *panRCID;
    
    panORNT = (int *) CPLMalloc( sizeof(int) * nCount );
    panUSAG = (int *) CPLMalloc( sizeof(int) * nCount );
    panMASK = (int *) CPLMalloc( sizeof(int) * nCount );
    panRCNM = (int *) CPLMalloc( sizeof(int) * nCount );
    panRCID = (int *) CPLMalloc( sizeof(int) * nCount );

/* -------------------------------------------------------------------- */
/*      loop over all entries, decoding them.                           */
/* -------------------------------------------------------------------- */
    for( i = 0; i < nCount; i++ )
    {
        panRCID[i] = ParseName( poFSPT, i, panRCNM + i );
        panORNT[i] = poRecord->GetIntSubfield( "FSPT", 0, "ORNT",i);
        panUSAG[i] = poRecord->GetIntSubfield( "FSPT", 0, "USAG",i);
        panMASK[i] = poRecord->GetIntSubfield( "FSPT", 0, "MASK",i);
    }

/* -------------------------------------------------------------------- */
/*      Assign to feature.                                              */
/* -------------------------------------------------------------------- */
    poFeature->SetField( "NAME_RCNM", nCount, panRCNM );
    poFeature->SetField( "NAME_RCID", nCount, panRCID );
    poFeature->SetField( "ORNT", nCount, panORNT );
    poFeature->SetField( "USAG", nCount, panUSAG );
    poFeature->SetField( "MASK", nCount, panMASK );

/* -------------------------------------------------------------------- */
/*      Cleanup.                                                        */
/* -------------------------------------------------------------------- */
    CPLFree( panRCNM );
    CPLFree( panRCID );
    CPLFree( panORNT );
    CPLFree( panUSAG );
    CPLFree( panMASK );
}

/************************************************************************/
/*                             ReadVector()                             */
/*                                                                      */
/*      Read a vector primitive objects based on the type (RCNM_)       */
/*      and index within the related index.                             */
/************************************************************************/

OGRFeature *S57Reader::ReadVector( int nFeatureId, int nRCNM )

{
    DDFRecordIndex *poIndex;
    const char *pszFDName = NULL;

/* -------------------------------------------------------------------- */
/*      What type of vector are we fetching.                            */
/* -------------------------------------------------------------------- */
    switch( nRCNM )
    {
      case RCNM_VI:
        poIndex = &oVI_Index;
        pszFDName = OGRN_VI;
        break;
        
      case RCNM_VC:
        poIndex = &oVC_Index;
        pszFDName = OGRN_VC;
        break;
        
      case RCNM_VE:
        poIndex = &oVE_Index;
        pszFDName = OGRN_VE;
        break;

      case RCNM_VF:
        poIndex = &oVF_Index;
        pszFDName = OGRN_VF;
        break;
        
      default:
        CPLAssert( FALSE );
        return NULL;
    }

    if( nFeatureId < 0 || nFeatureId >= poIndex->GetCount() )
        return NULL;

    DDFRecord *poRecord = poIndex->GetByIndex( nFeatureId );

/* -------------------------------------------------------------------- */
/*      Find the feature definition to use.                             */
/* -------------------------------------------------------------------- */
    OGRFeatureDefn *poFDefn = NULL;

    for( int i = 0; i < nFDefnCount; i++ )
    {
        if( EQUAL(papoFDefnList[i]->GetName(),pszFDName) )              
        {
            poFDefn = papoFDefnList[i];
            break;
        }
    }
    
    if( poFDefn == NULL )
    {
        CPLAssert( FALSE );
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Create feature, and assign standard fields.                     */
/* -------------------------------------------------------------------- */
    OGRFeature *poFeature = new OGRFeature( poFDefn );
    
    poFeature->SetFID( nFeatureId );

    poFeature->SetField( "RCNM", 
                         poRecord->GetIntSubfield( "VRID", 0, "RCNM",0) );
    poFeature->SetField( "RCID", 
                         poRecord->GetIntSubfield( "VRID", 0, "RCID",0) );
    poFeature->SetField( "RVER", 
                         poRecord->GetIntSubfield( "VRID", 0, "RVER",0) );
    poFeature->SetField( "RUIN", 
                         poRecord->GetIntSubfield( "VRID", 0, "RUIN",0) );

/* -------------------------------------------------------------------- */
/*      Collect point geometries.                                       */
/* -------------------------------------------------------------------- */
    if( nRCNM == RCNM_VI || nRCNM == RCNM_VC ) 
    {
        double dfX=0.0, dfY=0.0, dfZ=0.0;

        if( poRecord->FindField( "SG2D" ) != NULL )
        {
            dfX = poRecord->GetIntSubfield("SG2D",0,"XCOO",0) / (double)nCOMF;
            dfY = poRecord->GetIntSubfield("SG2D",0,"YCOO",0) / (double)nCOMF;
            poFeature->SetGeometryDirectly( new OGRPoint( dfX, dfY ) );
        }

        else if( poRecord->FindField( "SG3D" ) != NULL ) /* presume sounding*/
        {
            int i, nVCount = poRecord->FindField("SG3D")->GetRepeatCount();
            if( nVCount == 1 )
            {
                dfX =poRecord->GetIntSubfield("SG3D",0,"XCOO",0)/(double)nCOMF;
                dfY =poRecord->GetIntSubfield("SG3D",0,"YCOO",0)/(double)nCOMF;
                dfZ =poRecord->GetIntSubfield("SG3D",0,"VE3D",0)/(double)nSOMF;
                poFeature->SetGeometryDirectly( new OGRPoint( dfX, dfY, dfZ ));
            }
            else
            {
                OGRMultiPoint *poMP = new OGRMultiPoint();

                for( i = 0; i < nVCount; i++ )
                {
                    dfX = poRecord->GetIntSubfield("SG3D",0,"XCOO",i)
                        / (double)nCOMF;
                    dfY = poRecord->GetIntSubfield("SG3D",0,"YCOO",i)
                        / (double)nCOMF;
                    dfZ = poRecord->GetIntSubfield("SG3D",0,"VE3D",i)
                        / (double)nSOMF;
                    
                    poMP->addGeometryDirectly( new OGRPoint( dfX, dfY, dfZ ) );
                }

                poFeature->SetGeometryDirectly( poMP );
            }
        }

    }

/* -------------------------------------------------------------------- */
/*      Collect an edge geometry.                                       */
/* -------------------------------------------------------------------- */
    else if( nRCNM == RCNM_VE && poRecord->FindField( "SG2D" ) != NULL )
    {
        int i, nVCount = poRecord->FindField("SG2D")->GetRepeatCount();
        OGRLineString *poLine = new OGRLineString();

        poLine->setNumPoints( nVCount );
        
        for( i = 0; i < nVCount; i++ )
        {
            poLine->setPoint( 
                i, 
                poRecord->GetIntSubfield("SG2D",0,"XCOO",i) / (double)nCOMF,
                poRecord->GetIntSubfield("SG2D",0,"YCOO",i) / (double)nCOMF );
        }
        poFeature->SetGeometryDirectly( poLine );
    }

/* -------------------------------------------------------------------- */
/*      Special edge fields.                                            */
/* -------------------------------------------------------------------- */
    DDFField *poVRPT;

    if( nRCNM == RCNM_VE 
        && (poVRPT = poRecord->FindField( "VRPT" )) != NULL )
    {
        poFeature->SetField( "NAME_RCNM_0", RCNM_VC );
        poFeature->SetField( "NAME_RCID_0", ParseName( poVRPT, 0 ) );
        poFeature->SetField( "ORNT_0", 
                             poRecord->GetIntSubfield("VRPT",0,"ORNT",0) );
        poFeature->SetField( "USAG_0", 
                             poRecord->GetIntSubfield("VRPT",0,"USAG",0) );
        poFeature->SetField( "TOPI_0", 
                             poRecord->GetIntSubfield("VRPT",0,"TOPI",0) );
        poFeature->SetField( "MASK_0", 
                             poRecord->GetIntSubfield("VRPT",0,"MASK",0) );
                             
        
        poFeature->SetField( "NAME_RCNM_1", RCNM_VC );
        poFeature->SetField( "NAME_RCID_1", ParseName( poVRPT, 1 ) );
        poFeature->SetField( "ORNT_1", 
                             poRecord->GetIntSubfield("VRPT",0,"ORNT",1) );
        poFeature->SetField( "USAG_1", 
                             poRecord->GetIntSubfield("VRPT",0,"USAG",1) );
        poFeature->SetField( "TOPI_1", 
                             poRecord->GetIntSubfield("VRPT",0,"TOPI",1) );
        poFeature->SetField( "MASK_1", 
                             poRecord->GetIntSubfield("VRPT",0,"MASK",1) );
    }

    return poFeature;
}

/************************************************************************/
/*                             FetchPoint()                             */
/*                                                                      */
/*      Fetch the location of a spatial point object.                   */
/************************************************************************/

int S57Reader::FetchPoint( int nRCNM, int nRCID,
                           double * pdfX, double * pdfY, double * pdfZ )

{
    DDFRecord   *poSRecord;
    
    if( nRCNM == RCNM_VI )
        poSRecord = oVI_Index.FindRecord( nRCID );
    else
        poSRecord = oVC_Index.FindRecord( nRCID );

    if( poSRecord == NULL )
        return FALSE;

    double      dfX = 0.0, dfY = 0.0, dfZ = 0.0;

    if( poSRecord->FindField( "SG2D" ) != NULL )
    {
        dfX = poSRecord->GetIntSubfield("SG2D",0,"XCOO",0) / (double)nCOMF;
        dfY = poSRecord->GetIntSubfield("SG2D",0,"YCOO",0) / (double)nCOMF;
    }
    else if( poSRecord->FindField( "SG3D" ) != NULL )
    {
        dfX = poSRecord->GetIntSubfield("SG3D",0,"XCOO",0) / (double)nCOMF;
        dfY = poSRecord->GetIntSubfield("SG3D",0,"YCOO",0) / (double)nCOMF;
        dfZ = poSRecord->GetIntSubfield("SG3D",0,"VE3D",0) / (double)nSOMF;
    }
    else
        return FALSE;

    if( pdfX != NULL )
        *pdfX = dfX;
    if( pdfY != NULL )
        *pdfY = dfY;
    if( pdfZ != NULL )
        *pdfZ = dfZ;

    return TRUE;
}

/************************************************************************/
/*                             FetchLine()                              */
/************************************************************************/

int S57Reader::FetchLine( DDFRecord *poSRecord, 
                          int iStartVertex, int iDirection,
                          OGRLineString *poLine )

{
    int             nVCount;
    DDFField        *poSG2D = poSRecord->FindField( "SG2D" );
    DDFSubfieldDefn *poXCOO=NULL, *poYCOO=NULL;
    int bStandardFormat = TRUE;

/* -------------------------------------------------------------------- */
/*      Get some basic definitions.                                     */
/* -------------------------------------------------------------------- */
    if( poSG2D != NULL )
    {
        poXCOO = poSG2D->GetFieldDefn()->FindSubfieldDefn("XCOO");
        poYCOO = poSG2D->GetFieldDefn()->FindSubfieldDefn("YCOO");

        if( poXCOO == NULL || poYCOO == NULL )
        {
            CPLDebug( "S57", "XCOO or YCOO are NULL" );
            return FALSE;
        }

        nVCount = poSG2D->GetRepeatCount();
    }
    else
    {
        return TRUE;
    }

/* -------------------------------------------------------------------- */
/*      It is legitimate to have zero vertices for line segments        */
/*      that just have the start and end node (bug 840).                */
/* -------------------------------------------------------------------- */
    if( nVCount == 0 )
        return TRUE;
 
/* -------------------------------------------------------------------- */
/*      Make sure out line is long enough to hold all the vertices      */
/*      we will apply.                                                  */
/* -------------------------------------------------------------------- */
    int nVBase;

    if( iDirection < 0 )
        nVBase = iStartVertex + nVCount;
    else
        nVBase = iStartVertex;

    if( poLine->getNumPoints() < iStartVertex + nVCount )
        poLine->setNumPoints( iStartVertex + nVCount );
        
/* -------------------------------------------------------------------- */
/*      Are the SG2D and XCOO/YCOO definitions in the form we expect?   */
/* -------------------------------------------------------------------- */
    if( poSG2D->GetFieldDefn()->GetSubfieldCount() != 2 )
        bStandardFormat = FALSE;

    if( !EQUAL(poXCOO->GetFormat(),"b24") 
        || !EQUAL(poYCOO->GetFormat(),"b24") )
        bStandardFormat = FALSE;

/* -------------------------------------------------------------------- */
/*      Collect the vertices:                                           */
/*                                                                      */
/*      This approach assumes that the data is LSB organized int32      */
/*      binary data as per the specification.  We avoid lots of         */
/*      extra calls to low level DDF methods as they are quite          */
/*      expensive.                                                      */
/* -------------------------------------------------------------------- */
    if( bStandardFormat )
    {
        const char  *pachData;
        int         nBytesRemaining;

        pachData = poSG2D->GetSubfieldData(poYCOO,&nBytesRemaining,0);
        
        for( int i = 0; i < nVCount; i++ )
        {
            double      dfX, dfY;
            GInt32      nXCOO, nYCOO;

            memcpy( &nYCOO, pachData, 4 );
            pachData += 4;
            memcpy( &nXCOO, pachData, 4 );
            pachData += 4;

#ifdef CPL_MSB
            CPL_SWAP32PTR( &nXCOO );
            CPL_SWAP32PTR( &nYCOO );
#endif
            dfX = nXCOO / (double) nCOMF;
            dfY = nYCOO / (double) nCOMF;

            poLine->setPoint( nVBase, dfX, dfY );

            nVBase += iDirection;
        }
    }

/* -------------------------------------------------------------------- */
/*      Collect the vertices:                                           */
/*                                                                      */
/*      The generic case where we use low level but expensive DDF       */
/*      methods to get the data.  This should work even if some         */
/*      things are changed about the SG2D fields such as making them    */
/*      floating point or a different byte order.                       */
/* -------------------------------------------------------------------- */
    else
    {
        for( int i = 0; i < nVCount; i++ )
        {
            double      dfX, dfY;
            const char  *pachData;
            int         nBytesRemaining;

            pachData = poSG2D->GetSubfieldData(poXCOO,&nBytesRemaining,i);
                
            dfX = poXCOO->ExtractIntData(pachData,nBytesRemaining,NULL)
                / (double) nCOMF;

            pachData = poSG2D->GetSubfieldData(poYCOO,&nBytesRemaining,i);

            dfY = poXCOO->ExtractIntData(pachData,nBytesRemaining,NULL)
                / (double) nCOMF;
                
            poLine->setPoint( nVBase, dfX, dfY );

            nVBase += iDirection;
        }
    }

    return TRUE;
}

/************************************************************************/
/*                       AssemblePointGeometry()                        */
/************************************************************************/

void S57Reader::AssemblePointGeometry( DDFRecord * poFRecord,
                                       OGRFeature * poFeature )

{
    DDFField    *poFSPT;
    int         nRCNM, nRCID;

/* -------------------------------------------------------------------- */
/*      Feature the spatial record containing the point.                */
/* -------------------------------------------------------------------- */
    poFSPT = poFRecord->FindField( "FSPT" );
    if( poFSPT == NULL )
        return;

    if( poFSPT->GetRepeatCount() != 1 )
    {
#ifdef DEBUG
        fprintf( stderr, 
                 "Point features with other than one spatial linkage.\n" );
        poFRecord->Dump( stderr );
#endif
        CPLDebug( "S57", 
           "Point feature encountered with other than one spatial linkage." );
    }
        
    nRCID = ParseName( poFSPT, 0, &nRCNM );

    double      dfX = 0.0, dfY = 0.0, dfZ = 0.0;

    if( !FetchPoint( nRCNM, nRCID, &dfX, &dfY, &dfZ ) )
    {
        CPLAssert( FALSE );
        return;
    }

    poFeature->SetGeometryDirectly( new OGRPoint( dfX, dfY, dfZ ) );
}

/************************************************************************/
/*                      AssembleSoundingGeometry()                      */
/************************************************************************/

void S57Reader::AssembleSoundingGeometry( DDFRecord * poFRecord,
                                          OGRFeature * poFeature )

{
    DDFField    *poFSPT;
    int         nRCNM, nRCID;
    DDFRecord   *poSRecord;
    

/* -------------------------------------------------------------------- */
/*      Feature the spatial record containing the point.                */
/* -------------------------------------------------------------------- */
    poFSPT = poFRecord->FindField( "FSPT" );
    if( poFSPT == NULL )
        return;

    CPLAssert( poFSPT->GetRepeatCount() == 1 );
        
    nRCID = ParseName( poFSPT, 0, &nRCNM );

    if( nRCNM == RCNM_VI )
        poSRecord = oVI_Index.FindRecord( nRCID );
    else
        poSRecord = oVC_Index.FindRecord( nRCID );

    if( poSRecord == NULL )
        return;

/* -------------------------------------------------------------------- */
/*      Extract vertices.                                               */
/* -------------------------------------------------------------------- */
    OGRMultiPoint       *poMP = new OGRMultiPoint();
    DDFField            *poField;
    int                 nPointCount, i, nBytesLeft;
    DDFSubfieldDefn    *poXCOO, *poYCOO, *poVE3D;
    const char         *pachData;

    poField = poSRecord->FindField( "SG2D" );
    if( poField == NULL )
        poField = poSRecord->FindField( "SG3D" );
    if( poField == NULL )
        return;

    poXCOO = poField->GetFieldDefn()->FindSubfieldDefn( "XCOO" );
    poYCOO = poField->GetFieldDefn()->FindSubfieldDefn( "YCOO" );
    poVE3D = poField->GetFieldDefn()->FindSubfieldDefn( "VE3D" );

    nPointCount = poField->GetRepeatCount();

    pachData = poField->GetData();
    nBytesLeft = poField->GetDataSize();

    for( i = 0; i < nPointCount; i++ )
    {
        double          dfX, dfY, dfZ = 0.0;
        int             nBytesConsumed;

        dfY = poYCOO->ExtractIntData( pachData, nBytesLeft,
                                      &nBytesConsumed ) / (double) nCOMF;
        nBytesLeft -= nBytesConsumed;
        pachData += nBytesConsumed;

        dfX = poXCOO->ExtractIntData( pachData, nBytesLeft,
                                      &nBytesConsumed ) / (double) nCOMF;
        nBytesLeft -= nBytesConsumed;
        pachData += nBytesConsumed;
        
        if( poVE3D != NULL )
        {
            dfZ = poYCOO->ExtractIntData( pachData, nBytesLeft,
                                          &nBytesConsumed ) / (double) nSOMF;
            nBytesLeft -= nBytesConsumed;
            pachData += nBytesConsumed;
        }

        poMP->addGeometryDirectly( new OGRPoint( dfX, dfY, dfZ ) );
    }

    poFeature->SetGeometryDirectly( poMP );
}

/************************************************************************/
/*                        AssembleLineGeometry()                        */
/************************************************************************/

void S57Reader::AssembleLineGeometry( DDFRecord * poFRecord,
                                      OGRFeature * poFeature )

{
    DDFField    *poFSPT;
    int         nEdgeCount;
    OGRLineString *poLine = new OGRLineString();

/* -------------------------------------------------------------------- */
/*      Find the FSPT field.                                            */
/* -------------------------------------------------------------------- */
    poFSPT = poFRecord->FindField( "FSPT" );
    if( poFSPT == NULL )
        return;

    nEdgeCount = poFSPT->GetRepeatCount();

/* ==================================================================== */
/*      Loop collecting edges.                                          */
/* ==================================================================== */
    for( int iEdge = 0; iEdge < nEdgeCount; iEdge++ )
    {
        DDFRecord       *poSRecord;
        int             nRCID;

/* -------------------------------------------------------------------- */
/*      Find the spatial record for this edge.                          */
/* -------------------------------------------------------------------- */
        nRCID = ParseName( poFSPT, iEdge );

        poSRecord = oVE_Index.FindRecord( nRCID );
        if( poSRecord == NULL )
        {
            CPLError( CE_Warning, CPLE_AppDefined,
                      "Couldn't find spatial record %d.\n"
                      "Feature OBJL=%s, RCID=%d may have corrupt or"
                      "missing geometry.",
                      nRCID,
                      poFeature->GetDefnRef()->GetName(),
                      poFRecord->GetIntSubfield( "FRID", 0, "RCID", 0 ) );
            continue;
        }
    
/* -------------------------------------------------------------------- */
/*      Establish the number of vertices, and whether we need to        */
/*      reverse or not.                                                 */
/* -------------------------------------------------------------------- */
        int             nVCount;
        int             nStart, nEnd, nInc;
        DDFField        *poSG2D = poSRecord->FindField( "SG2D" );
        DDFSubfieldDefn *poXCOO=NULL, *poYCOO=NULL;

        if( poSG2D != NULL )
        {
            poXCOO = poSG2D->GetFieldDefn()->FindSubfieldDefn("XCOO");
            poYCOO = poSG2D->GetFieldDefn()->FindSubfieldDefn("YCOO");

            nVCount = poSG2D->GetRepeatCount();
        }
        else
            nVCount = 0;

        if( poFRecord->GetIntSubfield( "FSPT", 0, "ORNT", iEdge ) == 2 )
        {
            nStart = nVCount-1;
            nEnd = 0;
            nInc = -1;
        }
        else
        {
            nStart = 0;
            nEnd = nVCount-1;
            nInc = 1;
        }

/* -------------------------------------------------------------------- */
/*      Add the start node, if this is the first edge.                  */
/* -------------------------------------------------------------------- */
        if( iEdge == 0 )
        {
            int         nVC_RCID;
            double      dfX, dfY;
            
            if( nInc == 1 )
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 0 );
            else
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 1 );

            if( FetchPoint( RCNM_VC, nVC_RCID, &dfX, &dfY ) )
                poLine->addPoint( dfX, dfY );
            else
                CPLError( CE_Warning, CPLE_AppDefined, 
                          "Unable to fetch start node RCID%d.\n"
                          "Feature OBJL=%s, RCID=%d may have corrupt or"
                          " missing geometry.", 
                          nVC_RCID, 
                          poFeature->GetDefnRef()->GetName(),
                          poFRecord->GetIntSubfield( "FRID", 0, "RCID", 0 ) );
        }
        
/* -------------------------------------------------------------------- */
/*      Collect the vertices.                                           */
/* -------------------------------------------------------------------- */
        int             nVBase = poLine->getNumPoints();
        
        poLine->setNumPoints( nVCount+nVBase );

        for( int i = nStart; i != nEnd+nInc; i += nInc )
        {
            double      dfX, dfY;
            const char  *pachData;
            int         nBytesRemaining;

            pachData = poSG2D->GetSubfieldData(poXCOO,&nBytesRemaining,i);
                
            dfX = poXCOO->ExtractIntData(pachData,nBytesRemaining,NULL)
                / (double) nCOMF;

            pachData = poSG2D->GetSubfieldData(poYCOO,&nBytesRemaining,i);

            dfY = poXCOO->ExtractIntData(pachData,nBytesRemaining,NULL)
                / (double) nCOMF;
                
            poLine->setPoint( nVBase++, dfX, dfY );
        }

/* -------------------------------------------------------------------- */
/*      Add the end node.                                               */
/* -------------------------------------------------------------------- */
        {
            int         nVC_RCID;
            double      dfX, dfY;
            
            if( nInc == 1 )
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 1 );
            else
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 0 );

            if( FetchPoint( RCNM_VC, nVC_RCID, &dfX, &dfY ) )
                poLine->addPoint( dfX, dfY );
            else
                CPLError( CE_Warning, CPLE_AppDefined, 
                          "Unable to fetch end node RCID=%d.\n"
                          "Feature OBJL=%s, RCID=%d may have corrupt or"
                          " missing geometry.", 
                          nVC_RCID, 
                          poFeature->GetDefnRef()->GetName(),
                          poFRecord->GetIntSubfield( "FRID", 0, "RCID", 0 ) );
        }
    }

    if( poLine->getNumPoints() >= 2 )
        poFeature->SetGeometryDirectly( poLine );
    else
        delete poLine;
}

/************************************************************************/
/*                        AssembleAreaGeometry()                        */
/************************************************************************/

void S57Reader::AssembleAreaGeometry( DDFRecord * poFRecord,
                                         OGRFeature * poFeature )

{
    DDFField    *poFSPT;
    OGRGeometryCollection * poLines = new OGRGeometryCollection();

/* -------------------------------------------------------------------- */
/*      Find the FSPT fields.                                           */
/* -------------------------------------------------------------------- */
    for( int iFSPT = 0; 
         (poFSPT = poFRecord->FindField( "FSPT", iFSPT )) != NULL;
         iFSPT++ )
    {
        int         nEdgeCount;

        nEdgeCount = poFSPT->GetRepeatCount();

/* ==================================================================== */
/*      Loop collecting edges.                                          */
/* ==================================================================== */
        for( int iEdge = 0; iEdge < nEdgeCount; iEdge++ )
        {
            DDFRecord       *poSRecord;
            int             nRCID;

/* -------------------------------------------------------------------- */
/*      Find the spatial record for this edge.                          */
/* -------------------------------------------------------------------- */
            nRCID = ParseName( poFSPT, iEdge );

            poSRecord = oVE_Index.FindRecord( nRCID );
            if( poSRecord == NULL )
            {
                CPLError( CE_Warning, CPLE_AppDefined,
                          "Couldn't find spatial record %d.\n", nRCID );
                continue;
            }
    
/* -------------------------------------------------------------------- */
/*      Create the line string.                                         */
/* -------------------------------------------------------------------- */
            OGRLineString *poLine = new OGRLineString();
        
/* -------------------------------------------------------------------- */
/*      Add the start node.                                             */
/* -------------------------------------------------------------------- */
            {
                int         nVC_RCID;
                double      dfX, dfY;
            
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 0 );

                if( FetchPoint( RCNM_VC, nVC_RCID, &dfX, &dfY ) )
                    poLine->addPoint( dfX, dfY );
            }
        
/* -------------------------------------------------------------------- */
/*      Collect the vertices.                                           */
/* -------------------------------------------------------------------- */
            if( !FetchLine( poSRecord, poLine->getNumPoints(), 1, poLine ) )
            {
                CPLDebug( "S57", "FetchLine() failed in AssembleAreaGeometry()!" );
            }


/* -------------------------------------------------------------------- */
/*      Add the end node.                                               */
/* -------------------------------------------------------------------- */
            {
                int         nVC_RCID;
                double      dfX, dfY;
            
                nVC_RCID = ParseName( poSRecord->FindField( "VRPT" ), 1 );

                if( FetchPoint( RCNM_VC, nVC_RCID, &dfX, &dfY ) )
                    poLine->addPoint( dfX, dfY );
            }

            poLines->addGeometryDirectly( poLine );
        }
    }

/* -------------------------------------------------------------------- */
/*      Build lines into a polygon.                                     */
/* -------------------------------------------------------------------- */
    OGRPolygon  *poPolygon;
    OGRErr      eErr;

    poPolygon = (OGRPolygon *) 
        OGRBuildPolygonFromEdges( (OGRGeometryH) poLines, 
                                  TRUE, FALSE, 0.0, &eErr );
    if( eErr != OGRERR_NONE )
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                  "Polygon assembly has failed for feature FIDN=%d,FIDS=%d.\n"
                  "Geometry may be missing or incomplete.", 
                  poFeature->GetFieldAsInteger( "FIDN" ),
                  poFeature->GetFieldAsInteger( "FIDS" ) );
    }

    delete poLines;

    if( poPolygon != NULL )
        poFeature->SetGeometryDirectly( poPolygon );
}

/************************************************************************/
/*                             FindFDefn()                              */
/*                                                                      */
/*      Find the OGRFeatureDefn corresponding to the passed feature     */
/*      record.  It will search based on geometry class, or object      */
/*      class depending on the bClassBased setting.                     */
/************************************************************************/

OGRFeatureDefn * S57Reader::FindFDefn( DDFRecord * poRecord )

{
    if( poRegistrar != NULL )
    {
        int     nOBJL = poRecord->GetIntSubfield( "FRID", 0, "OBJL", 0 );

        if( apoFDefnByOBJL[nOBJL] != NULL )
            return apoFDefnByOBJL[nOBJL];

        if( !poRegistrar->SelectClass( nOBJL ) )
        {
            for( int i = 0; i < nFDefnCount; i++ )
            {
                if( EQUAL(papoFDefnList[i]->GetName(),"Generic") )
                    return papoFDefnList[i];
            }
            return NULL;
        }

        for( int i = 0; i < nFDefnCount; i++ )
        {
            if( EQUAL(papoFDefnList[i]->GetName(),
                      poRegistrar->GetAcronym()) )
                return papoFDefnList[i];
        }

        return NULL;
    }
    else
    {
        int     nPRIM = poRecord->GetIntSubfield( "FRID", 0, "PRIM", 0 );
        OGRwkbGeometryType eGType;
        
        if( nPRIM == PRIM_P )
            eGType = wkbPoint;
        else if( nPRIM == PRIM_L )
            eGType = wkbLineString;
        else if( nPRIM == PRIM_A )
            eGType = wkbPolygon;
        else
            eGType = wkbNone;

        for( int i = 0; i < nFDefnCount; i++ )
        {
            if( papoFDefnList[i]->GetGeomType() == eGType )
                return papoFDefnList[i];
        }
    }

    return NULL;
}

/************************************************************************/
/*                             ParseName()                              */
/*                                                                      */
/*      Pull the RCNM and RCID values from a NAME field.  The RCID      */
/*      is returned and the RCNM can be gotten via the pnRCNM argument. */
/************************************************************************/

int S57Reader::ParseName( DDFField * poField, int nIndex, int * pnRCNM )

{
    unsigned char       *pabyData;

    pabyData = (unsigned char *)
        poField->GetSubfieldData(
            poField->GetFieldDefn()->FindSubfieldDefn( "NAME" ),
            NULL, nIndex );

    if( pnRCNM != NULL )
        *pnRCNM = pabyData[0];

    return pabyData[1]
         + pabyData[2] * 256
         + pabyData[3] * 256 * 256
         + pabyData[4] * 256 * 256 * 256;
}

/************************************************************************/
/*                           AddFeatureDefn()                           */
/************************************************************************/

void S57Reader::AddFeatureDefn( OGRFeatureDefn * poFDefn )

{
    nFDefnCount++;
    papoFDefnList = (OGRFeatureDefn **)
        CPLRealloc(papoFDefnList, sizeof(OGRFeatureDefn*)*nFDefnCount );

    papoFDefnList[nFDefnCount-1] = poFDefn;

    if( poRegistrar != NULL )
    {
        if( poRegistrar->SelectClass( poFDefn->GetName() ) )
            apoFDefnByOBJL[poRegistrar->GetOBJL()] = poFDefn;
    }
}

/************************************************************************/
/*                          CollectClassList()                          */
/*                                                                      */
/*      Establish the list of classes (unique OBJL values) that         */
/*      occur in this dataset.                                          */
/************************************************************************/

int S57Reader::CollectClassList(int *panClassCount, int nMaxClass )

{
    int         bSuccess = TRUE;

    if( !bFileIngested )
        Ingest();

    for( int iFEIndex = 0; iFEIndex < oFE_Index.GetCount(); iFEIndex++ )
    {
        DDFRecord *poRecord = oFE_Index.GetByIndex( iFEIndex );
        int     nOBJL = poRecord->GetIntSubfield( "FRID", 0, "OBJL", 0 );

        if( nOBJL >= nMaxClass )
            bSuccess = FALSE;
        else
            panClassCount[nOBJL]++;

    }

    return bSuccess;
}

/************************************************************************/
/*                         ApplyRecordUpdate()                          */
/*                                                                      */
/*      Update one target record based on an S-57 update record         */
/*      (RUIN=3).                                                       */
/************************************************************************/

int S57Reader::ApplyRecordUpdate( DDFRecord *poTarget, DDFRecord *poUpdate )

{
    const char *pszKey = poUpdate->GetField(1)->GetFieldDefn()->GetName();

/* -------------------------------------------------------------------- */
/*      Validate versioning.                                            */
/* -------------------------------------------------------------------- */
    if( poTarget->GetIntSubfield( pszKey, 0, "RVER", 0 ) + 1
        != poUpdate->GetIntSubfield( pszKey, 0, "RVER", 0 )  )
    {
        CPLDebug( "S57", 
                  "Mismatched RVER value on RCNM=%d,RCID=%d.\n",
                  poTarget->GetIntSubfield( pszKey, 0, "RCNM", 0 ),
                  poTarget->GetIntSubfield( pszKey, 0, "RCID", 0 ) );

        CPLAssert( FALSE );
        return FALSE;
    }

/* -------------------------------------------------------------------- */
/*      Update the target version.                                      */
/* -------------------------------------------------------------------- */
    unsigned char       *pnRVER;
    DDFField    *poKey = poTarget->FindField( pszKey );
    DDFSubfieldDefn *poRVER_SFD;

    if( poKey == NULL )
    {
        CPLAssert( FALSE );
        return FALSE;
    }

    poRVER_SFD = poKey->GetFieldDefn()->FindSubfieldDefn( "RVER" );
    if( poRVER_SFD == NULL )
        return FALSE;

    pnRVER = (unsigned char *) poKey->GetSubfieldData( poRVER_SFD, NULL, 0 );
 
    *pnRVER += 1;

/* -------------------------------------------------------------------- */
/*      Check for, and apply record record to spatial record pointer    */
/*      updates.                                                        */
/* -------------------------------------------------------------------- */
    if( poUpdate->FindField( "FSPC" ) != NULL )
    {
        int     nFSUI = poUpdate->GetIntSubfield( "FSPC", 0, "FSUI", 0 );
        int     nFSIX = poUpdate->GetIntSubfield( "FSPC", 0, "FSIX", 0 );
        int     nNSPT = poUpdate->GetIntSubfield( "FSPC", 0, "NSPT", 0 );
        DDFField *poSrcFSPT = poUpdate->FindField( "FSPT" );
        DDFField *poDstFSPT = poTarget->FindField( "FSPT" );
        int     nPtrSize;

        if( (poSrcFSPT == NULL && nFSUI != 2) || poDstFSPT == NULL )
        {
            CPLAssert( FALSE );
            return FALSE;
        }

        nPtrSize = poDstFSPT->GetFieldDefn()->GetFixedWidth();

        if( nFSUI == 1 ) /* INSERT */
        {
            char        *pachInsertion;
            int         nInsertionBytes = nPtrSize * nNSPT;

            pachInsertion = (char *) CPLMalloc(nInsertionBytes + nPtrSize);
            memcpy( pachInsertion, poSrcFSPT->GetData(), nInsertionBytes );

            /* 
            ** If we are inserting before an instance that already
            ** exists, we must add it to the end of the data being
            ** inserted.
            */
            if( nFSIX <= poDstFSPT->GetRepeatCount() )
            {
                memcpy( pachInsertion + nInsertionBytes, 
                        poDstFSPT->GetData() + nPtrSize * (nFSIX-1), 
                        nPtrSize );
                nInsertionBytes += nPtrSize;
            }

            poTarget->SetFieldRaw( poDstFSPT, nFSIX - 1, 
                                   pachInsertion, nInsertionBytes );
            CPLFree( pachInsertion );
        }
        else if( nFSUI == 2 ) /* DELETE */
        {
            /* Wipe each deleted coordinate */
            for( int i = nNSPT-1; i >= 0; i-- )
            {
                poTarget->SetFieldRaw( poDstFSPT, i + nFSIX - 1, NULL, 0 );
            }
        }
        else if( nFSUI == 3 ) /* MODIFY */
        {
            /* copy over each ptr */
            for( int i = 0; i < nNSPT; i++ )
            {
                const char *pachRawData;

                pachRawData = poSrcFSPT->GetData() + nPtrSize * i;
                poTarget->SetFieldRaw( poDstFSPT, i + nFSIX - 1, 
                                       pachRawData, nPtrSize );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Check for, and apply vector record to vector record pointer     */
/*      updates.                                                        */
/* -------------------------------------------------------------------- */
    if( poUpdate->FindField( "VRPC" ) != NULL )
    {
        int     nVPUI = poUpdate->GetIntSubfield( "VRPC", 0, "VPUI", 0 );
        int     nVPIX = poUpdate->GetIntSubfield( "VRPC", 0, "VPIX", 0 );
        int     nNVPT = poUpdate->GetIntSubfield( "VRPC", 0, "NVPT", 0 );
        DDFField *poSrcVRPT = poUpdate->FindField( "VRPT" );
        DDFField *poDstVRPT = poTarget->FindField( "VRPT" );
        int     nPtrSize;

        if( (poSrcVRPT == NULL && nVPUI != 2) || poDstVRPT == NULL )
        {
            CPLAssert( FALSE );
            return FALSE;
        }

        nPtrSize = poDstVRPT->GetFieldDefn()->GetFixedWidth();

        if( nVPUI == 1 ) /* INSERT */
        {
            char        *pachInsertion;
            int         nInsertionBytes = nPtrSize * nNVPT;

            pachInsertion = (char *) CPLMalloc(nInsertionBytes + nPtrSize);
            memcpy( pachInsertion, poSrcVRPT->GetData(), nInsertionBytes );

            /* 
            ** If we are inserting before an instance that already
            ** exists, we must add it to the end of the data being
            ** inserted.
            */
            if( nVPIX <= poDstVRPT->GetRepeatCount() )
            {
                memcpy( pachInsertion + nInsertionBytes, 
                        poDstVRPT->GetData() + nPtrSize * (nVPIX-1), 
                        nPtrSize );
                nInsertionBytes += nPtrSize;
            }

            poTarget->SetFieldRaw( poDstVRPT, nVPIX - 1, 
                                   pachInsertion, nInsertionBytes );
            CPLFree( pachInsertion );
        }
        else if( nVPUI == 2 ) /* DELETE */
        {
            /* Wipe each deleted coordinate */
            for( int i = nNVPT-1; i >= 0; i-- )
            {
                poTarget->SetFieldRaw( poDstVRPT, i + nVPIX - 1, NULL, 0 );
            }
        }
        else if( nVPUI == 3 ) /* MODIFY */
        {
            /* copy over each ptr */
            for( int i = 0; i < nNVPT; i++ )
            {
                const char *pachRawData;

                pachRawData = poSrcVRPT->GetData() + nPtrSize * i;

                poTarget->SetFieldRaw( poDstVRPT, i + nVPIX - 1, 
                                       pachRawData, nPtrSize );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Check for, and apply record update to coordinates.              */
/* -------------------------------------------------------------------- */
    if( poUpdate->FindField( "SGCC" ) != NULL )
    {
        int     nCCUI = poUpdate->GetIntSubfield( "SGCC", 0, "CCUI", 0 );
        int     nCCIX = poUpdate->GetIntSubfield( "SGCC", 0, "CCIX", 0 );
        int     nCCNC = poUpdate->GetIntSubfield( "SGCC", 0, "CCNC", 0 );
        DDFField *poSrcSG2D = poUpdate->FindField( "SG2D" );
        DDFField *poDstSG2D = poTarget->FindField( "SG2D" );
        int     nCoordSize;

        /* If we don't have SG2D, check for SG3D */
        if( poDstSG2D == NULL )
        {
            poSrcSG2D = poUpdate->FindField( "SG3D" );
            poDstSG2D = poTarget->FindField( "SG3D" );
        }

        if( (poSrcSG2D == NULL && nCCUI != 2) || poDstSG2D == NULL )
        {
            CPLAssert( FALSE );
            return FALSE;
        }

        nCoordSize = poDstSG2D->GetFieldDefn()->GetFixedWidth();

        if( nCCUI == 1 ) /* INSERT */
        {
            char        *pachInsertion;
            int         nInsertionBytes = nCoordSize * nCCNC;

            pachInsertion = (char *) CPLMalloc(nInsertionBytes + nCoordSize);
            memcpy( pachInsertion, poSrcSG2D->GetData(), nInsertionBytes );

            /* 
            ** If we are inserting before an instance that already
            ** exists, we must add it to the end of the data being
            ** inserted.
            */
            if( nCCIX <= poDstSG2D->GetRepeatCount() )
            {
                memcpy( pachInsertion + nInsertionBytes, 
                        poDstSG2D->GetData() + nCoordSize * (nCCIX-1), 
                        nCoordSize );
                nInsertionBytes += nCoordSize;
            }

            poTarget->SetFieldRaw( poDstSG2D, nCCIX - 1, 
                                   pachInsertion, nInsertionBytes );
            CPLFree( pachInsertion );

        }
        else if( nCCUI == 2 ) /* DELETE */
        {
            /* Wipe each deleted coordinate */
            for( int i = nCCNC-1; i >= 0; i-- )
            {
                poTarget->SetFieldRaw( poDstSG2D, i + nCCIX - 1, NULL, 0 );
            }
        }
        else if( nCCUI == 3 ) /* MODIFY */
        {
            /* copy over each ptr */
            for( int i = 0; i < nCCNC; i++ )
            {
                const char *pachRawData;

                pachRawData = poSrcSG2D->GetData() + nCoordSize * i;

                poTarget->SetFieldRaw( poDstSG2D, i + nCCIX - 1, 
                                       pachRawData, nCoordSize );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      We don't currently handle FFPC (feature to feature linkage)     */
/*      issues, but we will at least report them when debugging.        */
/* -------------------------------------------------------------------- */
    if( poUpdate->FindField( "FFPC" ) != NULL )
    {
        CPLDebug( "S57", "Found FFPC, but not applying it." );
    }

/* -------------------------------------------------------------------- */
/*      Check for and apply changes to attribute lists.                 */
/* -------------------------------------------------------------------- */
    if( poUpdate->FindField( "ATTF" ) != NULL )
    {
        DDFSubfieldDefn *poSrcATVLDefn;
        DDFField *poSrcATTF = poUpdate->FindField( "ATTF" );
        DDFField *poDstATTF = poTarget->FindField( "ATTF" );
        int     nRepeatCount = poSrcATTF->GetRepeatCount();

        poSrcATVLDefn = poSrcATTF->GetFieldDefn()->FindSubfieldDefn( "ATVL" );

        for( int iAtt = 0; iAtt < nRepeatCount; iAtt++ )
        {
            int nATTL = poUpdate->GetIntSubfield( "ATTF", 0, "ATTL", iAtt );
            int iTAtt, nDataBytes;
            const char *pszRawData;

            for( iTAtt = poDstATTF->GetRepeatCount()-1; iTAtt >= 0; iTAtt-- )
            {
                if( poTarget->GetIntSubfield( "ATTF", 0, "ATTL", iTAtt )
                    == nATTL )
                    break;
            }
            if( iTAtt == -1 )
                iTAtt = poDstATTF->GetRepeatCount();

            pszRawData = poSrcATTF->GetInstanceData( iAtt, &nDataBytes );
            if( pszRawData[2] == 0x7f /* delete marker */ )
            {
                poTarget->SetFieldRaw( poDstATTF, iTAtt, NULL, 0 );
            }
            else
            {
                poTarget->SetFieldRaw( poDstATTF, iTAtt, pszRawData, 
                                       nDataBytes );
            }
        }
    }

    return TRUE;
}


/************************************************************************/
/*                            ApplyUpdates()                            */
/*                                                                      */
/*      Read records from an update file, and apply them to the         */
/*      currently loaded index of features.                             */
/************************************************************************/

int S57Reader::ApplyUpdates( DDFModule *poUpdateModule )

{
    DDFRecord   *poRecord;

/* -------------------------------------------------------------------- */
/*      Ensure base file is loaded.                                     */
/* -------------------------------------------------------------------- */
    Ingest();

/* -------------------------------------------------------------------- */
/*      Read records, and apply as updates.                             */
/* -------------------------------------------------------------------- */
    while( (poRecord = poUpdateModule->ReadRecord()) != NULL )
    {
        DDFField        *poKeyField = poRecord->GetField(1);
        const char      *pszKey = poKeyField->GetFieldDefn()->GetName();
        
        if( EQUAL(pszKey,"VRID") || EQUAL(pszKey,"FRID"))
        {
            int         nRCNM = poRecord->GetIntSubfield( pszKey,0, "RCNM",0 );
            int         nRCID = poRecord->GetIntSubfield( pszKey,0, "RCID",0 );
            int         nRVER = poRecord->GetIntSubfield( pszKey,0, "RVER",0 );
            int         nRUIN = poRecord->GetIntSubfield( pszKey,0, "RUIN",0 );
            DDFRecordIndex *poIndex = NULL;

            if( EQUAL(poKeyField->GetFieldDefn()->GetName(),"VRID") )
            {
                switch( nRCNM )
                {
                  case RCNM_VI:
                    poIndex = &oVI_Index;
                    break;

                  case RCNM_VC:
                    poIndex = &oVC_Index;
                    break;

                  case RCNM_VE:
                    poIndex = &oVE_Index;
                    break;

                  case RCNM_VF:
                    poIndex = &oVF_Index;
                    break;

                  default:
                    CPLAssert( FALSE );
                    break;
                }
            }
            else
            {
                poIndex = &oFE_Index;
            }

            if( poIndex != NULL )
            {
                if( nRUIN == 1 )  /* insert */
                {
                    poIndex->AddRecord( nRCID, poRecord->CloneOn(poModule) );
                }
                else if( nRUIN == 2 ) /* delete */
                {
                    DDFRecord   *poTarget;

                    poTarget = poIndex->FindRecord( nRCID );
                    if( poTarget == NULL )
                    {
                        CPLError( CE_Warning, CPLE_AppDefined,
                                  "Can't find RCNM=%d,RCID=%d for delete.\n",
                                  nRCNM, nRCID );
                    }
                    else if( poTarget->GetIntSubfield( pszKey, 0, "RVER", 0 )
                             != nRVER - 1 )
                    {
                        CPLError( CE_Warning, CPLE_AppDefined,
                                  "Mismatched RVER value on RCNM=%d,RCID=%d.\n",
                                  nRCNM, nRCID );
                    }
                    else
                    {
                        poIndex->RemoveRecord( nRCID );
                    }
                }

                else if( nRUIN == 3 ) /* modify in place */
                {
                    DDFRecord   *poTarget;

                    poTarget = poIndex->FindRecord( nRCID );
                    if( poTarget == NULL )
                    {
                        CPLError( CE_Warning, CPLE_AppDefined, 
                                  "Can't find RCNM=%d,RCID=%d for update.\n",
                                  nRCNM, nRCID );
                    }
                    else
                    {
                        if( !ApplyRecordUpdate( poTarget, poRecord ) )
                        {
                            CPLError( CE_Warning, CPLE_AppDefined, 
                                      "An update to RCNM=%d,RCID=%d failed.\n",
                                      nRCNM, nRCID );
                        }
                    }
                }
            }
        }

        else if( EQUAL(pszKey,"DSID") )
        {
            /* ignore */;
        }

        else
        {
            CPLDebug( "S57",
                      "Skipping %s record in S57Reader::ApplyUpdates().\n",
                      pszKey );
        }
    }

    return TRUE;
}

/************************************************************************/
/*                        FindAndApplyUpdates()                         */
/*                                                                      */
/*      Find all update files that would appear to apply to this        */
/*      base file.                                                      */
/************************************************************************/

int S57Reader::FindAndApplyUpdates( const char * pszPath )

{
    int         iUpdate;
    int         bSuccess = TRUE;

    if( pszPath == NULL )
        pszPath = pszModuleName;

    if( !EQUAL(CPLGetExtension(pszPath),"000") )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "Can't apply updates to a base file with a different\n"
                  "extension than .000.\n" );
        return FALSE;
    }

    for( iUpdate = 1; bSuccess; iUpdate++ )
    {
        //Creaing file extension
        std::string extension;
        std::string dirname;
        if( 1 <= iUpdate &&  iUpdate < 10 )
        {
            char buf[2];
            sprintf( buf, "%i", iUpdate );
            extension.append("00");
            extension.append(buf);
            dirname.append(buf);
        }
        else if( 10 <= iUpdate && iUpdate < 100 )
        {
            char buf[3];
            sprintf( buf, "%i", iUpdate );
            extension.append("0");
            extension.append(buf);
            dirname.append(buf);
        }
        else if( 100 <= iUpdate && iUpdate < 1000 )
        {
            char buf[4];
            sprintf( buf, "%i", iUpdate );
            extension.append(buf);
            dirname.append(buf);
        }

        DDFModule oUpdateModule;
          
        //trying current dir first
        char    *pszUpdateFilename = 
            CPLStrdup(CPLResetExtension(pszPath,extension.c_str()));

        std::ifstream file(pszUpdateFilename);
        if( file )
        {
            file.close();
            bSuccess = oUpdateModule.Open( pszUpdateFilename, TRUE );
            if( bSuccess )
                CPLDebug( "S57", "Applying feature updates from %s.", 
                          pszUpdateFilename );
            if( bSuccess )
            {
                if( !ApplyUpdates( &oUpdateModule ) )
                    return FALSE;
            }
        }
        else // file is store on Primar generated cd
        {
            file.close();
            char* pszBaseFileDir = CPLStrdup(CPLGetDirname(pszPath));
            char* pszFileDir = CPLStrdup(CPLGetDirname(pszBaseFileDir));
            std::string remotefile(pszFileDir);
            remotefile.append( "/" );
            remotefile.append( dirname );
            remotefile.append( "/" );
            remotefile.append( CPLGetBasename(pszPath) );
            remotefile.append( "." );
            remotefile.append( extension );
            bSuccess = oUpdateModule.Open( remotefile.c_str(), TRUE );
	
            if( bSuccess )
                CPLDebug( "S57", "Applying feature updates from %s.", 
                          remotefile.c_str() );
            CPLFree( pszBaseFileDir );
            CPLFree( pszFileDir );
            if( bSuccess )
            {
                if( !ApplyUpdates( &oUpdateModule ) )
                    return FALSE;
            }
        }//end for if-else
        CPLFree( pszUpdateFilename );
    }

    return TRUE;
}

/************************************************************************/
/*                             GetExtent()                              */
/*                                                                      */
/*      Scan all the cached records collecting spatial bounds as        */
/*      efficiently as possible for this transfer.                      */
/************************************************************************/

OGRErr S57Reader::GetExtent( OGREnvelope *psExtent, int bForce )

{
#define INDEX_COUNT     4

    DDFRecordIndex      *apoIndex[INDEX_COUNT];

/* -------------------------------------------------------------------- */
/*      If we aren't forced to get the extent say no if we haven't      */
/*      already indexed the iso8211 records.                            */
/* -------------------------------------------------------------------- */
    if( !bForce && !bFileIngested )
        return OGRERR_FAILURE;

    Ingest();

/* -------------------------------------------------------------------- */
/*      We will scan all the low level vector elements for extents      */
/*      coordinates.                                                    */
/* -------------------------------------------------------------------- */
    int         bGotExtents = FALSE;
    int         nXMin=0, nXMax=0, nYMin=0, nYMax=0;

    apoIndex[0] = &oVI_Index;
    apoIndex[1] = &oVC_Index;
    apoIndex[2] = &oVE_Index;
    apoIndex[3] = &oVF_Index;

    for( int iIndex = 0; iIndex < INDEX_COUNT; iIndex++ )
    {
        DDFRecordIndex  *poIndex = apoIndex[iIndex];

        for( int iVIndex = 0; iVIndex < poIndex->GetCount(); iVIndex++ )
        {
            DDFRecord *poRecord = poIndex->GetByIndex( iVIndex );
            DDFField    *poSG3D = poRecord->FindField( "SG3D" );
            DDFField    *poSG2D = poRecord->FindField( "SG2D" );

            if( poSG3D != NULL )
            {
                int     i, nVCount = poSG3D->GetRepeatCount();
                GInt32  *panData, nX, nY;

                panData = (GInt32 *) poSG3D->GetData();
                for( i = 0; i < nVCount; i++ )
                {
                    nX = CPL_LSBWORD32(panData[i*3+1]);
                    nY = CPL_LSBWORD32(panData[i*3+0]);

                    if( bGotExtents )
                    {
                        nXMin = MIN(nXMin,nX);
                        nXMax = MAX(nXMax,nX);
                        nYMin = MIN(nYMin,nY);
                        nYMax = MAX(nYMax,nY);
                    }
                    else
                    {
                        nXMin = nXMax = nX; 
                        nYMin = nYMax = nY;
                        bGotExtents = TRUE;
                    }
                }
            }
            else if( poSG2D != NULL )
            {
                int     i, nVCount = poSG2D->GetRepeatCount();
                GInt32  *panData, nX, nY;

                panData = (GInt32 *) poSG2D->GetData();
                for( i = 0; i < nVCount; i++ )
                {
                    nX = CPL_LSBWORD32(panData[i*2+1]);
                    nY = CPL_LSBWORD32(panData[i*2+0]);

                    if( bGotExtents )
                    {
                        nXMin = MIN(nXMin,nX);
                        nXMax = MAX(nXMax,nX);
                        nYMin = MIN(nYMin,nY);
                        nYMax = MAX(nYMax,nY);
                    }
                    else
                    {
                        nXMin = nXMax = nX; 
                        nYMin = nYMax = nY;
                        bGotExtents = TRUE;
                    }
                }
            }
        }
    }

    if( !bGotExtents )
        return OGRERR_FAILURE;
    else
    {
        psExtent->MinX = nXMin / (double) nCOMF;
        psExtent->MaxX = nXMax / (double) nCOMF;
        psExtent->MinY = nYMin / (double) nCOMF;
        psExtent->MaxY = nYMax / (double) nCOMF;

        return OGRERR_NONE;
    }
}


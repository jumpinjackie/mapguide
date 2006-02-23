/******************************************************************************
 * $Id: gdalpamrasterband.cpp,v 1.8 2005/06/08 14:04:58 fwarmerdam Exp $
 *
 * Project:  GDAL Core
 * Purpose:  Implementation of GDALPamRasterBand, a raster band base class
 *           that knows how to persistently store auxilary metadata in an
 *           external xml file. 
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2005, Frank Warmerdam <warmerdam@pobox.com>
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
 * $Log: gdalpamrasterband.cpp,v $
 * Revision 1.8  2005/06/08 14:04:58  fwarmerdam
 * fixed last fix in handling of histograms
 *
 * Revision 1.7  2005/05/22 08:13:40  fwarmerdam
 * added multidomain metadata support
 *
 * Revision 1.6  2005/05/17 18:30:51  fwarmerdam
 * initialize nodatavalue to -1e10, fix copying
 *
 * Revision 1.5  2005/05/17 15:13:28  fwarmerdam
 * ensure pam info initialize on any set operation
 *
 * Revision 1.4  2005/05/13 18:56:52  fwarmerdam
 * fixed SetDefaultHistogram
 *
 * Revision 1.3  2005/05/13 18:18:46  fwarmerdam
 * reworked histogram support, SetHistogram prototype
 *
 * Revision 1.2  2005/05/11 14:04:08  fwarmerdam
 * added getdefaulthistogram
 *
 * Revision 1.1  2005/04/27 16:27:44  fwarmerdam
 * New
 *
 */

#include "gdal_pam.h"
#include "cpl_string.h"

CPL_CVSID("$Id: gdalpamrasterband.cpp,v 1.8 2005/06/08 14:04:58 fwarmerdam Exp $");

/************************************************************************/
/*                         GDALPamRasterBand()                          */
/************************************************************************/

GDALPamRasterBand::GDALPamRasterBand()

{
    psPam = NULL;
}

/************************************************************************/
/*                         ~GDALPamRasterBand()                         */
/************************************************************************/

GDALPamRasterBand::~GDALPamRasterBand()

{
    PamClear();
}

/************************************************************************/
/*                           SerializeToXML()                           */
/************************************************************************/

CPLXMLNode *GDALPamRasterBand::SerializeToXML( const char *pszVRTPath )

{
    if( psPam == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Setup root node and attributes.                                 */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psTree;

    psTree = CPLCreateXMLNode( NULL, CXT_Element, "PAMRasterBand" );

    if( GetBand() > 0 )
        CPLSetXMLValue( psTree, "#band", CPLSPrintf( "%d", GetBand() ) );

/* -------------------------------------------------------------------- */
/*      Serialize information of interest.                              */
/* -------------------------------------------------------------------- */
    if( strlen(GetDescription()) > 0 )
        CPLSetXMLValue( psTree, "Description", GetDescription() );

    if( psPam->bNoDataValueSet )
        CPLSetXMLValue( psTree, "NoDataValue", 
                        CPLSPrintf( "%.14E", psPam->dfNoDataValue ) );

    if( psPam->pszUnitType != NULL )
        CPLSetXMLValue( psTree, "UnitType", psPam->pszUnitType );

    if( psPam->dfOffset != 0.0 )
        CPLSetXMLValue( psTree, "Offset", 
                        CPLSPrintf( "%.16g", psPam->dfOffset ) );

    if( psPam->dfScale != 1.0 )
        CPLSetXMLValue( psTree, "Scale", 
                        CPLSPrintf( "%.16g", psPam->dfScale ) );

    if( psPam->eColorInterp != GCI_Undefined )
        CPLSetXMLValue( psTree, "ColorInterp", 
                        GDALGetColorInterpretationName( psPam->eColorInterp ));

/* -------------------------------------------------------------------- */
/*      Category names.                                                 */
/* -------------------------------------------------------------------- */
    if( psPam->papszCategoryNames != NULL )
    {
        CPLXMLNode *psCT_XML = CPLCreateXMLNode( psTree, CXT_Element, 
                                                 "CategoryNames" );

        for( int iEntry=0; psPam->papszCategoryNames[iEntry] != NULL; iEntry++)
        {
            CPLCreateXMLElementAndValue( psCT_XML, "Category", 
                                         psPam->papszCategoryNames[iEntry] );
        }
    }

/* -------------------------------------------------------------------- */
/*      Color Table.                                                    */
/* -------------------------------------------------------------------- */
    if( psPam->poColorTable != NULL )
    {
        CPLXMLNode *psCT_XML = CPLCreateXMLNode( psTree, CXT_Element, 
                                                 "ColorTable" );

        for( int iEntry=0; iEntry < psPam->poColorTable->GetColorEntryCount(); 
             iEntry++ )
        {
            GDALColorEntry sEntry;
            CPLXMLNode *psEntry_XML = CPLCreateXMLNode( psCT_XML, CXT_Element, 
                                                        "Entry" );

            psPam->poColorTable->GetColorEntryAsRGB( iEntry, &sEntry );
            
            CPLSetXMLValue( psEntry_XML, "#c1", CPLSPrintf("%d",sEntry.c1) );
            CPLSetXMLValue( psEntry_XML, "#c2", CPLSPrintf("%d",sEntry.c2) );
            CPLSetXMLValue( psEntry_XML, "#c3", CPLSPrintf("%d",sEntry.c3) );
            CPLSetXMLValue( psEntry_XML, "#c4", CPLSPrintf("%d",sEntry.c4) );
        }
    }

/* -------------------------------------------------------------------- */
/*      Min/max.                                                        */
/* -------------------------------------------------------------------- */
    if( psPam->bHaveMinMax )
    {
        CPLSetXMLValue( psTree, "Minimum", 
                        CPLSPrintf( "%.16g", psPam->dfMin ) );
        CPLSetXMLValue( psTree, "Maximum", 
                        CPLSPrintf( "%.16g", psPam->dfMax ) );
    }

/* -------------------------------------------------------------------- */
/*      Statistics                                                      */
/* -------------------------------------------------------------------- */
    if( psPam->bHaveStats )
    {
        CPLSetXMLValue( psTree, "Mean", 
                        CPLSPrintf( "%.16g", psPam->dfMean ) );
        CPLSetXMLValue( psTree, "StandardDeviation", 
                        CPLSPrintf( "%.16g", psPam->dfStdDev ) );
    }

/* -------------------------------------------------------------------- */
/*      Histograms.                                                     */
/* -------------------------------------------------------------------- */
    if( psPam->psSavedHistograms != NULL )
        CPLAddXMLChild( psTree, CPLCloneXMLTree( psPam->psSavedHistograms ) );

/* -------------------------------------------------------------------- */
/*      Metadata.                                                       */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psMD;

    psMD = psPam->oMDMD.Serialize();
    if( psMD != NULL )
        CPLAddXMLChild( psTree, psMD );

/* -------------------------------------------------------------------- */
/*      We don't want to return anything if we had no metadata to       */
/*      attach.                                                         */
/* -------------------------------------------------------------------- */
    if( psTree->psChild == NULL || psTree->psChild->psNext == NULL )
    {
        CPLDestroyXMLNode( psTree );
        psTree = NULL;
    }

    return psTree;
}

/************************************************************************/
/*                           PamInitialize()                            */
/************************************************************************/

void GDALPamRasterBand::PamInitialize()

{
    if( psPam )
        return;

    GDALPamDataset *poParentDS = (GDALPamDataset *) GetDataset();

    if( poParentDS == NULL )
        return;

    poParentDS->PamInitialize();
    if( poParentDS->psPam == NULL )
        return;

    psPam = (GDALRasterBandPamInfo *)
        CPLCalloc(sizeof(GDALRasterBandPamInfo),1);

    psPam->dfScale = 1.0;
    psPam->poParentDS = poParentDS;
    psPam->dfNoDataValue = -1e10;
}

/************************************************************************/
/*                              PamClear()                              */
/************************************************************************/

void GDALPamRasterBand::PamClear()

{
    if( psPam )
    {
        if( psPam->poColorTable )
            delete psPam->poColorTable;
        psPam->poColorTable = NULL;
        
        CPLFree( psPam->pszUnitType );
        CSLDestroy( psPam->papszCategoryNames );

        CPLFree( psPam );
        psPam = NULL;
    }
}

/************************************************************************/
/*                              XMLInit()                               */
/************************************************************************/

CPLErr GDALPamRasterBand::XMLInit( CPLXMLNode *psTree, const char *pszVRTPath )

{
    PamInitialize();

/* -------------------------------------------------------------------- */
/*      Apply any dataset level metadata.                               */
/* -------------------------------------------------------------------- */
    psPam->oMDMD.XMLInit( psTree );

/* -------------------------------------------------------------------- */
/*      Collect various other items of metadata.                        */
/* -------------------------------------------------------------------- */
    SetDescription( CPLGetXMLValue( psTree, "Description", "" ) );
    
    if( CPLGetXMLValue( psTree, "NoDataValue", NULL ) != NULL )
    {
        GDALPamRasterBand::SetNoDataValue( 
            atof(CPLGetXMLValue( psTree, "NoDataValue", "0" )) );
    }

    GDALPamRasterBand::SetOffset( 
        atof(CPLGetXMLValue( psTree, "Offset", "0.0" )) );
    GDALPamRasterBand::SetScale( 
        atof(CPLGetXMLValue( psTree, "Scale", "1.0" )) );

    GDALPamRasterBand::SetUnitType( CPLGetXMLValue( psTree, "UnitType", NULL));

    if( CPLGetXMLValue( psTree, "ColorInterp", NULL ) != NULL )
    {
        const char *pszInterp = CPLGetXMLValue( psTree, "ColorInterp", NULL );
        int        iInterp;
        
        for( iInterp = 0; iInterp < 13; iInterp++ )
        {
            const char *pszCandidate 
                = GDALGetColorInterpretationName( (GDALColorInterp) iInterp );

            if( pszCandidate != NULL && EQUAL(pszCandidate,pszInterp) )
            {
                GDALPamRasterBand::SetColorInterpretation( (GDALColorInterp) iInterp );
                break;
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      Category names.                                                 */
/* -------------------------------------------------------------------- */
    if( CPLGetXMLNode( psTree, "CategoryNames" ) != NULL )
    {
        CPLXMLNode *psEntry;
        char **papszCategoryNames = NULL;

        for( psEntry = CPLGetXMLNode( psTree, "CategoryNames" )->psChild;
             psEntry != NULL; psEntry = psEntry->psNext )
        {
            if( psEntry->eType != CXT_Element 
                || !EQUAL(psEntry->pszValue,"Category") 
                || psEntry->psChild == NULL 
                || psEntry->psChild->eType != CXT_Text )
                continue;
            
            papszCategoryNames = CSLAddString( papszCategoryNames, 
                                               psEntry->psChild->pszValue );
        }
        
        GDALPamRasterBand::SetCategoryNames( papszCategoryNames );
    }

/* -------------------------------------------------------------------- */
/*      Collect a color table.                                          */
/* -------------------------------------------------------------------- */
    if( CPLGetXMLNode( psTree, "ColorTable" ) != NULL )
    {
        CPLXMLNode *psEntry;
        GDALColorTable oTable;
        int        iEntry = 0;

        for( psEntry = CPLGetXMLNode( psTree, "ColorTable" )->psChild;
             psEntry != NULL; psEntry = psEntry->psNext )
        {
            GDALColorEntry sCEntry;

            sCEntry.c1 = (short) atoi(CPLGetXMLValue( psEntry, "c1", "0" ));
            sCEntry.c2 = (short) atoi(CPLGetXMLValue( psEntry, "c2", "0" ));
            sCEntry.c3 = (short) atoi(CPLGetXMLValue( psEntry, "c3", "0" ));
            sCEntry.c4 = (short) atoi(CPLGetXMLValue( psEntry, "c4", "255" ));

            oTable.SetColorEntry( iEntry++, &sCEntry );
        }
        
        GDALPamRasterBand::SetColorTable( &oTable );
    }

/* -------------------------------------------------------------------- */
/*      Do we have a complete set of stats?                             */
/* -------------------------------------------------------------------- */
    if( CPLGetXMLNode( psTree, "Minimum" ) != NULL 
        && CPLGetXMLNode( psTree, "Maximum" ) != NULL )
    {
        psPam->bHaveMinMax = TRUE;
        psPam->dfMin = atof(CPLGetXMLValue(psTree, "Minimum","0"));
        psPam->dfMax = atof(CPLGetXMLValue(psTree, "Maximum","0"));
    }

    if( CPLGetXMLNode( psTree, "Mean" ) != NULL 
        && CPLGetXMLNode( psTree, "StandardDeviation" ) != NULL )
    {
        psPam->bHaveStats = TRUE;
        psPam->dfMean = atof(CPLGetXMLValue(psTree, "Mean","0"));
        psPam->dfStdDev = atof(CPLGetXMLValue(psTree,"StandardDeviation","0"));
    }

/* -------------------------------------------------------------------- */
/*      Histograms                                                      */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psHist = CPLGetXMLNode( psTree, "Histograms" );
    if( psHist != NULL )
    {
        CPLXMLNode *psNext = psHist->psNext;
        psHist->psNext = NULL;

        psPam->psSavedHistograms = CPLCloneXMLTree( psHist );
        psHist = psNext;
    }

    return CE_None;
}

/************************************************************************/
/*                             CloneInfo()                              */
/************************************************************************/

CPLErr GDALPamRasterBand::CloneInfo( GDALRasterBand *poSrcBand, 
                                     int nCloneFlags )

{
    int bOnlyIfMissing = nCloneFlags & GCIF_ONLY_IF_MISSING;
    int bSuccess;

    PamInitialize();

/* -------------------------------------------------------------------- */
/*      Metadata                                                        */
/* -------------------------------------------------------------------- */
    if( nCloneFlags & GCIF_BAND_METADATA )
    {
        if( poSrcBand->GetMetadata() != NULL )
        {
            if( !bOnlyIfMissing 
             || CSLCount(GetMetadata()) != CSLCount(poSrcBand->GetMetadata()) )
            {
                SetMetadata( poSrcBand->GetMetadata() );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      NODATA                                                          */
/* -------------------------------------------------------------------- */
    if( nCloneFlags & GCIF_NODATA )
    {
        double dfNoData = poSrcBand->GetNoDataValue( &bSuccess );
        
        if( bSuccess )
        {
            if( !bOnlyIfMissing 
                || GetNoDataValue( &bSuccess ) != dfNoData 
                || !bSuccess )
                GDALPamRasterBand::SetNoDataValue( dfNoData );
        }
    }

/* -------------------------------------------------------------------- */
/*      Offset/scale                                                    */
/* -------------------------------------------------------------------- */
    if( nCloneFlags & GCIF_SCALEOFFSET )
    {
        double dfOffset = poSrcBand->GetOffset( &bSuccess );
        
        if( bSuccess )
        {
            if( !bOnlyIfMissing || GetOffset() != dfOffset )
                GDALPamRasterBand::SetOffset( dfOffset );
        }

        double dfScale = poSrcBand->GetScale( &bSuccess );
        
        if( bSuccess )
        {
            if( !bOnlyIfMissing || GetScale() != dfScale )
                GDALPamRasterBand::SetScale( dfScale );
        }
    }

/* -------------------------------------------------------------------- */
/*      Unittype.                                                       */
/* -------------------------------------------------------------------- */
    if( nCloneFlags & GCIF_UNITTYPE )
    {
        if( strlen(poSrcBand->GetUnitType()) > 0 )
        {
            if( !bOnlyIfMissing 
                || !EQUAL(GetUnitType(),poSrcBand->GetUnitType()) )
            {
                GDALPamRasterBand::SetUnitType( poSrcBand->GetUnitType() );
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      ColorInterp                                                     */
/* -------------------------------------------------------------------- */
    if( nCloneFlags & GCIF_COLORINTERP )
    {
        if( poSrcBand->GetColorInterpretation() != GCI_Undefined )
        {
            if( !bOnlyIfMissing
                || poSrcBand->GetColorInterpretation() 
                != GetColorInterpretation() )
                GDALPamRasterBand::SetColorInterpretation( 
                    poSrcBand->GetColorInterpretation() );
        }
    }

/* -------------------------------------------------------------------- */
/*      color table.                                                    */
/* -------------------------------------------------------------------- */
    if( nCloneFlags && GCIF_COLORTABLE )
    {
        if( poSrcBand->GetColorTable() != NULL )
        {
            if( !bOnlyIfMissing || GetColorTable() != NULL )
            {
                GDALPamRasterBand::SetColorTable( 
                    poSrcBand->GetColorTable() );
            }
        }
    }

    return CE_None;
}

/************************************************************************/
/*                            GetMetadata()                             */
/************************************************************************/

char **GDALPamRasterBand::GetMetadata( const char *pszDomain )

{
    if( psPam )
        return psPam->oMDMD.GetMetadata( pszDomain );
    else
        return GDALRasterBand::GetMetadata( pszDomain );
}

/************************************************************************/
/*                            SetMetadata()                             */
/************************************************************************/

CPLErr GDALPamRasterBand::SetMetadata( char **papszMetadata, 
                                    const char *pszDomain )

{
    PamInitialize();

    if( psPam )
    {
        psPam->poParentDS->MarkPamDirty();
        return psPam->oMDMD.SetMetadata( papszMetadata, pszDomain );
    }
    else
    {
        return GDALRasterBand::SetMetadata( papszMetadata, pszDomain );
    }
}

/************************************************************************/
/*                          GetMetadataItem()                           */
/************************************************************************/

const char *GDALPamRasterBand::GetMetadataItem( const char *pszName, 
                                             const char *pszDomain )

{
    PamInitialize();

    if( psPam )
    {
        return psPam->oMDMD.GetMetadataItem( pszName, pszDomain );
    }
    else
    {
        return GDALRasterBand::GetMetadataItem( pszName, pszDomain );
    }
}

/************************************************************************/
/*                          SetMetadataItem()                           */
/************************************************************************/

CPLErr GDALPamRasterBand::SetMetadataItem( const char *pszName, 
                                        const char *pszValue, 
                                        const char *pszDomain )

{
    PamInitialize();

    if( psPam )
    {
        psPam->poParentDS->MarkPamDirty();
        return psPam->oMDMD.SetMetadataItem( pszName, pszValue, pszDomain );
    }
    else
    {
        return GDALRasterBand::SetMetadataItem( pszName, pszValue, pszDomain );
    }
}

/************************************************************************/
/*                           SetNoDataValue()                           */
/************************************************************************/

CPLErr GDALPamRasterBand::SetNoDataValue( double dfNewValue )

{
    PamInitialize();

    if( psPam )
    {
        psPam->bNoDataValueSet = TRUE;
        psPam->dfNoDataValue = dfNewValue;
        psPam->poParentDS->MarkPamDirty();
        return CE_None;
    }
    else
        return GDALRasterBand::SetNoDataValue( dfNewValue );
}

/************************************************************************/
/*                           GetNoDataValue()                           */
/************************************************************************/

double GDALPamRasterBand::GetNoDataValue( int *pbSuccess )

{
    if( psPam != NULL )
    {
        if( pbSuccess )
            *pbSuccess = psPam->bNoDataValueSet;

        return psPam->dfNoDataValue;
    }
    else
        return GDALRasterBand::GetNoDataValue( pbSuccess );
}

/************************************************************************/
/*                             GetOffset()                              */
/************************************************************************/

double GDALPamRasterBand::GetOffset( int *pbSuccess )

{
    if( psPam )
    {
        if( pbSuccess != NULL )
            *pbSuccess = TRUE;
        
        return psPam->dfOffset;
    }
    else
        return GDALRasterBand::GetOffset( pbSuccess );
}

/************************************************************************/
/*                             SetOffset()                              */
/************************************************************************/

CPLErr GDALPamRasterBand::SetOffset( double dfNewOffset )

{
    PamInitialize();

    if( psPam != NULL )
    {
        psPam->dfOffset = dfNewOffset;
        psPam->poParentDS->MarkPamDirty();

        return CE_None;
    }
    else
        return GDALRasterBand::SetOffset( dfNewOffset );
}

/************************************************************************/
/*                              GetScale()                              */
/************************************************************************/

double GDALPamRasterBand::GetScale( int *pbSuccess )

{
    if( psPam )
    {
        if( pbSuccess != NULL )
            *pbSuccess = TRUE;
        
        return psPam->dfScale;
    }
    else
        return GDALRasterBand::GetScale( pbSuccess );
}

/************************************************************************/
/*                              SetScale()                              */
/************************************************************************/

CPLErr GDALPamRasterBand::SetScale( double dfNewScale )

{
    PamInitialize();

    if( psPam != NULL )
    {
        psPam->dfScale = dfNewScale;
        psPam->poParentDS->MarkPamDirty();
        return CE_None;
    }
    else
        return GDALRasterBand::SetScale( dfNewScale );
}

/************************************************************************/
/*                            GetUnitType()                             */
/************************************************************************/

const char *GDALPamRasterBand::GetUnitType()

{
    if( psPam != NULL )
    {
        if( psPam->pszUnitType == NULL )
            return "";
        else
            return psPam->pszUnitType;
    }
    else
        return GDALRasterBand::GetUnitType();
}

/************************************************************************/
/*                            SetUnitType()                             */
/************************************************************************/

CPLErr GDALPamRasterBand::SetUnitType( const char *pszNewValue )

{
    PamInitialize();

    if( psPam )
    {
        CPLFree( psPam->pszUnitType );
        
        if( pszNewValue == NULL )
            psPam->pszUnitType = NULL;
        else
            psPam->pszUnitType = CPLStrdup(pszNewValue);

        return CE_None;
    }
    else
        return GDALRasterBand::SetUnitType( pszNewValue );
}

/************************************************************************/
/*                          GetCategoryNames()                          */
/************************************************************************/

char **GDALPamRasterBand::GetCategoryNames()

{
    if( psPam )
        return psPam->papszCategoryNames;
    else
        return GDALRasterBand::GetCategoryNames();
}

/************************************************************************/
/*                          SetCategoryNames()                          */
/************************************************************************/

CPLErr GDALPamRasterBand::SetCategoryNames( char ** papszNewNames )

{
    PamInitialize();

    if( psPam ) 
    {
        CSLDestroy( psPam->papszCategoryNames );
        psPam->papszCategoryNames = CSLDuplicate( papszNewNames );
        psPam->poParentDS->MarkPamDirty();
        return CE_None;
    }
    else 
        return GDALRasterBand::SetCategoryNames( papszNewNames );

}


/************************************************************************/
/*                           GetColorTable()                            */
/************************************************************************/

GDALColorTable *GDALPamRasterBand::GetColorTable()

{
    if( psPam )
        return psPam->poColorTable;
    else
        return GDALRasterBand::GetColorTable();
}

/************************************************************************/
/*                           SetColorTable()                            */
/************************************************************************/

CPLErr GDALPamRasterBand::SetColorTable( GDALColorTable *poTableIn )

{
    PamInitialize();

    if( psPam )
    {
        if( psPam->poColorTable != NULL )
        {
            delete psPam->poColorTable;
            psPam->poColorTable = NULL;
        }
        
        if( poTableIn )
        {
            psPam->poColorTable = poTableIn->Clone();
            psPam->eColorInterp = GCI_PaletteIndex;
        }

        psPam->poParentDS->MarkPamDirty();

        return CE_None;
    }
    else
        return GDALRasterBand::SetColorTable( poTableIn );

}

/************************************************************************/
/*                       SetColorInterpretation()                       */
/************************************************************************/

CPLErr GDALPamRasterBand::SetColorInterpretation( GDALColorInterp eInterpIn )

{
    PamInitialize();

    if( psPam )
    {
        psPam->poParentDS->MarkPamDirty();
        
        psPam->eColorInterp = eInterpIn;

        return CE_None;
    }
    else
        return GDALRasterBand::SetColorInterpretation( eInterpIn );
}

/************************************************************************/
/*                       GetColorInterpretation()                       */
/************************************************************************/

GDALColorInterp GDALPamRasterBand::GetColorInterpretation()

{
    if( psPam )
        return psPam->eColorInterp;
    else
        return GDALRasterBand::GetColorInterpretation();
}

#ifdef notdef
/************************************************************************/
/*                           GetStatistics()                            */
/************************************************************************/

CPLErr GDALPamRasterBand::GetStatistics( int bApproxOK, int bForce,
                                         double *pdfMin, double *pdfMax, 
                                         double *pdfMean, double *pdfStdDev )

{
    if( psPam )
    {
        // We have needed stats, just return them. 
        if( ((pdfMean == NULL && pdfStdDev == NULL) || psPam->bHaveStats)
            && psPam->bHaveMinMax )
        {
            if( pdfMin != NULL )
                *pdfMin = psPam->dfMin;
            if( pdfMax != NULL )
                *pdfMax = psPam->dfMax;
            if( pdfMean != NULL )
                *pdfMean = psPam->dfMean;
            if( pdfStdDev != NULL )
                *pdfStdDev = psPam->dfStdDev;

            return CE_None;
        }

        // We need to compute the stats, but can then record them. 
        else
        {
            CPLErr eErr;

            eErr = GDALRasterBand::GetStatistics( bApproxOK, bForce, 
                                                  pdfMin, pdfMax, 
                                                  pdfMean, pdfStdDev);
            if( eErr == CE_None && pdfMin != NULL && pdfMax != NULL
                && pdfMean != NULL && pdfStdDev != NULL )
            {
                GDALPamRasterBand::SetStatistics( *pdfMin, *pdfMax, 
                                                  *pdfMean, *pdfStdDev );
            }

            return eErr;
        }
    }
    else
        return GDALRasterBand::GetStatistics( bApproxOK, bForce,
                                              pdfMin, pdfMax, 
                                              pdfMean, pdfStdDev);
}

/************************************************************************/
/*                           SetStatistics()                            */
/************************************************************************/

CPLErr GDALPamRasterBand::SetStatistics( double dfMin, double dfMax, 
                                         double dfMean, double dfStdDev )

{
    PamInitialize();

    if( psPam )
    {
        psPam->bHaveMinMax = TRUE;
        psPam->dfMin = dfMin;
        psPam->dfMax = dfMax;

        psPam->bHaveStats = TRUE;
        psPam->dfMean = dfMean;
        psPam->dfStdDev = dfStdDev;
        
        psPam->poParentDS->MarkPamDirty();

        return CE_None;
    }
    else
        return GDALRasterBand::SetStatistics( dfMin, dfMax, dfMean, dfStdDev);
}

/************************************************************************/
/*                             GetMinimum()                             */
/************************************************************************/

double GDALPamRasterBand::GetMinimum( int *pbSuccess )

{
    if( psPam && psPam->bHaveMinMax )
        return psPam->dfMin;
    else
        return GDALRasterBand::GetMinimum( pbSuccess );
}

/************************************************************************/
/*                             GetMaximum()                             */
/************************************************************************/

double GDALPamRasterBand::GetMaximum( int *pbSuccess )

{
    if( psPam && psPam->bHaveMinMax )
        return psPam->dfMax;
    else
        return GDALRasterBand::GetMaximum( pbSuccess );
}
#endif

/************************************************************************/
/*                         PamParseHistogram()                          */
/************************************************************************/

static int 
PamParseHistogram( CPLXMLNode *psHistItem, 
                   double *pdfMin, double *pdfMax, 
                   int *pnBuckets, int **ppanHistogram, 
                   int *pbIncludeOutOfRange, int *pbApproxOK )

{
    if( psHistItem == NULL )
        return FALSE;

    *pdfMin = atof(CPLGetXMLValue( psHistItem, "HistMin", "0"));
    *pdfMax = atof(CPLGetXMLValue( psHistItem, "HistMax", "1"));
    *pnBuckets = atoi(CPLGetXMLValue( psHistItem, "BucketCount","2"));

    if( ppanHistogram == NULL )
        return TRUE;

    // Fetch the histogram and use it. 
    int iBucket;
    const char *pszHistCounts = CPLGetXMLValue( psHistItem, 
                                                "HistCounts", "" );

    *ppanHistogram = (int *) CPLCalloc(sizeof(int),*pnBuckets);

    for( iBucket = 0; iBucket < *pnBuckets; iBucket++ )
    {
        (*ppanHistogram)[iBucket] = atoi(pszHistCounts);
        
        // skip to next number.
        while( *pszHistCounts != '\0' && *pszHistCounts != '|' )
            pszHistCounts++;
        if( *pszHistCounts == '|' )
            pszHistCounts++;
    }
    
    return TRUE;
}

/************************************************************************/
/*                      PamFindMatchingHistogram()                      */
/************************************************************************/
static CPLXMLNode *
PamFindMatchingHistogram( GDALRasterBandPamInfo *psPam, 
                          double dfMin, double dfMax, int nBuckets, 
                          int bIncludeOutOfRange, int bApproxOK )

{
    if( psPam == NULL || psPam->psSavedHistograms == NULL )
        return NULL;

    CPLXMLNode *psXMLHist;
    for( psXMLHist = psPam->psSavedHistograms->psChild;
         psXMLHist != NULL; psXMLHist = psXMLHist->psNext )
    {
        if( psXMLHist->eType != CXT_Element
            || !EQUAL(psXMLHist->pszValue,"HistItem") )
            continue;

        // should try and make min/max test a bit fuzzy.

        if( atof(CPLGetXMLValue( psXMLHist, "HistMin", "0")) != dfMin 
            || atof(CPLGetXMLValue( psXMLHist, "HistMax", "0")) != dfMax
            || atoi(CPLGetXMLValue( psXMLHist, "BucketCount","0")) 
            != nBuckets)
            continue;

        return psXMLHist;
    }

    return NULL;
}

/************************************************************************/
/*                       PamHistogramToXMLTree()                        */
/************************************************************************/

static CPLXMLNode *
PamHistogramToXMLTree( double dfMin, double dfMax,
                       int nBuckets, int * panHistogram,
                       int bIncludeOutOfRange, int bApprox )

{
    char *pszHistCounts = (char *) CPLMalloc(8 * nBuckets + 10);
    int iBucket, iHistOffset;
    CPLXMLNode *psXMLHist;

    psXMLHist = CPLCreateXMLNode( NULL, CXT_Element, "HistItem" );

    CPLSetXMLValue( psXMLHist, "HistMin", 
                    CPLSPrintf( "%.16g", dfMin ));
    CPLSetXMLValue( psXMLHist, "HistMax", 
                    CPLSPrintf( "%.16g", dfMax ));
    CPLSetXMLValue( psXMLHist, "BucketCount", 
                    CPLSPrintf( "%d", nBuckets ));
    CPLSetXMLValue( psXMLHist, "IncludeOutOfRange", 
                    CPLSPrintf( "%d", bIncludeOutOfRange ));
    CPLSetXMLValue( psXMLHist, "Approximate", 
                    CPLSPrintf( "%d", bApprox ));

    iHistOffset = 0;
    pszHistCounts[0] = '\0';
    for( iBucket = 0; iBucket < nBuckets; iBucket++ )
    {
        sprintf( pszHistCounts + iHistOffset, "%d", panHistogram[iBucket] );
        if( iBucket < nBuckets-1 )
            strcat( pszHistCounts + iHistOffset, "|" );
        iHistOffset += strlen(pszHistCounts+iHistOffset);
    }
        
    CPLSetXMLValue( psXMLHist, "HistCounts", pszHistCounts );
    CPLFree( pszHistCounts );

    return psXMLHist;
}

/************************************************************************/
/*                            GetHistogram()                            */
/************************************************************************/

CPLErr GDALPamRasterBand::GetHistogram( double dfMin, double dfMax,
                                        int nBuckets, int * panHistogram,
                                        int bIncludeOutOfRange, int bApproxOK,
                                        GDALProgressFunc pfnProgress, 
                                        void *pProgressData )

{
    PamInitialize();

    if( psPam == NULL )
        return GDALRasterBand::GetHistogram( dfMin, dfMax, 
                                             nBuckets, panHistogram, 
                                             bIncludeOutOfRange, bApproxOK,
                                             pfnProgress, pProgressData );

/* -------------------------------------------------------------------- */
/*      Check if we have a matching histogram.                          */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psHistItem;

    psHistItem = PamFindMatchingHistogram( psPam, dfMin, dfMax, nBuckets, 
                                           bIncludeOutOfRange, bApproxOK );
    if( psHistItem != NULL )
    {
        int *panTempHist = NULL;

        if( PamParseHistogram( psHistItem, &dfMin, &dfMax, &nBuckets, 
                               &panTempHist,
                               &bIncludeOutOfRange, &bApproxOK ) )
        {
            memcpy( panHistogram, panTempHist, sizeof(int) * nBuckets );
            CPLFree( panTempHist );
            return CE_None;
        }
    }

/* -------------------------------------------------------------------- */
/*      We don't have an existing histogram matching the request, so    */
/*      generate one manually.                                          */
/* -------------------------------------------------------------------- */
    CPLErr eErr;

    eErr = GDALRasterBand::GetHistogram( dfMin, dfMax, 
                                         nBuckets, panHistogram, 
                                         bIncludeOutOfRange, bApproxOK,
                                         pfnProgress, pProgressData );

/* -------------------------------------------------------------------- */
/*      Save an XML description of this histogram.                      */
/* -------------------------------------------------------------------- */
    if( eErr == CE_None )
    {
        CPLXMLNode *psXMLHist;

        psXMLHist = PamHistogramToXMLTree( dfMin, dfMax, nBuckets, 
                                           panHistogram, 
                                           bIncludeOutOfRange, bApproxOK );
        if( psXMLHist != NULL )
        {
            psPam->poParentDS->MarkPamDirty();

            if( psPam->psSavedHistograms == NULL )
                psPam->psSavedHistograms = CPLCreateXMLNode( NULL, CXT_Element,
                                                             "Histograms" );
            
            CPLAddXMLChild( psPam->psSavedHistograms, psXMLHist );
        }
    }

    return eErr;
}

/************************************************************************/
/*                        SetDefaultHistogram()                         */
/************************************************************************/

CPLErr GDALPamRasterBand::SetDefaultHistogram( double dfMin, double dfMax, 
                                               int nBuckets, int *panHistogram)

{
    CPLXMLNode *psNode;

    PamInitialize();

    if( psPam == NULL )
        return GDALRasterBand::SetDefaultHistogram( dfMin, dfMax, 
                                                    nBuckets, panHistogram );

/* -------------------------------------------------------------------- */
/*      Do we have a matching histogram we should replace?              */
/* -------------------------------------------------------------------- */
    psNode = PamFindMatchingHistogram( psPam, dfMin, dfMax, nBuckets,
                                       TRUE, TRUE );
    if( psNode != NULL )
    {
        /* blow this one away */
        CPLRemoveXMLChild( psPam->psSavedHistograms, psNode );
        CPLDestroyXMLNode( psNode );
    }

/* -------------------------------------------------------------------- */
/*      Translate into a histogram XML tree.                            */
/* -------------------------------------------------------------------- */
    CPLXMLNode *psHistItem;

    psHistItem = PamHistogramToXMLTree( dfMin, dfMax, nBuckets, 
                                        panHistogram, TRUE, FALSE );

/* -------------------------------------------------------------------- */
/*      Insert our new default histogram at the front of the            */
/*      histogram list so that it will be the default histogram.        */
/* -------------------------------------------------------------------- */
    psPam->poParentDS->MarkPamDirty();

    if( psPam->psSavedHistograms == NULL )
        psPam->psSavedHistograms = CPLCreateXMLNode( NULL, CXT_Element,
                                                     "Histograms" );
            
    psHistItem->psNext = psPam->psSavedHistograms->psChild;
    psPam->psSavedHistograms->psChild = psHistItem;
    
    return CE_None;
}

/************************************************************************/
/*                        GetDefaultHistogram()                         */
/************************************************************************/

CPLErr 
GDALPamRasterBand::GetDefaultHistogram( double *pdfMin, double *pdfMax, 
                                        int *pnBuckets, int **ppanHistogram, 
                                        int bForce,
                                        GDALProgressFunc pfnProgress, 
                                        void *pProgressData )
    
{
    if( psPam && psPam->psSavedHistograms != NULL )
    {
        CPLXMLNode *psXMLHist;

        for( psXMLHist = psPam->psSavedHistograms->psChild;
             psXMLHist != NULL; psXMLHist = psXMLHist->psNext )
        {
            int bApprox, bIncludeOutOfRange;

            if( psXMLHist->eType != CXT_Element
                || !EQUAL(psXMLHist->pszValue,"HistItem") )
                continue;

            if( PamParseHistogram( psXMLHist, pdfMin, pdfMax, pnBuckets, 
                                   ppanHistogram, &bIncludeOutOfRange,
                                   &bApprox ) )
                return CE_None;
            else
                return CE_Failure;
        }
    }

    return GDALRasterBand::GetDefaultHistogram( pdfMin, pdfMax, pnBuckets, 
                                                ppanHistogram, bForce, 
                                                pfnProgress, pProgressData );
}


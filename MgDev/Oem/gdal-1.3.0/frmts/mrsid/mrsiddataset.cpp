/******************************************************************************
 * $Id: mrsiddataset.cpp,v 1.38 2005/07/22 15:50:10 fwarmerdam Exp $
 *
 * Project:  Multi-resolution Seamless Image Database (MrSID)
 * Purpose:  Read/write LizardTech's MrSID file format - Version 4 SDK.
 * Author:   Andrey Kiselev, dron@remotesensing.org
 *
 ******************************************************************************
 * Copyright (c) 2003, Andrey Kiselev <dron@remotesensing.org>
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
 * $Log: mrsiddataset.cpp,v $
 * Revision 1.38  2005/07/22 15:50:10  fwarmerdam
 * Use blocked io for very small request buffers as well as small windows.
 *
 * Revision 1.37  2005/07/21 17:30:13  fwarmerdam
 * Added preliminary worldfile support.
 *
 * Revision 1.36  2005/07/14 23:40:31  fwarmerdam
 * getNumLevels() is the number of overviews
 *
 * Revision 1.35  2005/06/24 15:54:30  dron
 * Properly fetch NODATA taking in account data type of the value.
 *
 * Revision 1.34  2005/05/13 01:12:25  fwarmerdam
 * Fixed bug with block oriented access to 16bit data.
 *
 * Revision 1.33  2005/05/05 15:54:49  fwarmerdam
 * PAM Enabled
 *
 * Revision 1.32  2005/05/04 17:21:00  fwarmerdam
 * Avoid 32bit overflow when testing xsize*ysize.
 *
 * Revision 1.31  2005/04/21 16:43:26  fwarmerdam
 * Fixed bug with RasterIO() at overview levels higher than is available
 * in the file.  Now the code limits itself to available overview levels.
 *
 * Revision 1.30  2005/04/11 17:03:07  fwarmerdam
 * Improved initial open checking to ensure it works properly with no header
 * data at all.
 *
 * Revision 1.29  2005/04/11 14:41:14  fwarmerdam
 * Only call getWKT() if HAVE_MRSID_GETWKT is defined.
 *
 * Revision 1.28  2005/04/06 22:03:17  kmelero
 * Updated for CPLStrdup and mod to pointers for large file support per
 * LizardTech Mrsid Team.  (kmelero@sanz.com)
 *
 * Revision 1.27  2005/04/02 19:26:08  kmelero
 * Added read support for WKT.  (kmelero@sanz.com)
 *
 * Revision 1.26  2005/04/02 19:04:00  kmelero
 * Added setting WKT in MrSID file.  (kmelero@sanz.com)
 *
 * Revision 1.25  2005/03/26 19:29:21  kmelero
 * Added flag to set 64-bit fwrite in support of large MrSID files.  (kmelero@sanz.com)
 *
 * Revision 1.24  2005/03/25 15:02:22  kmelero
 * Removed misplaced < character.  Was causing compiler error. (kmelero@sanz.com)
 *
 * Revision 1.23  2005/02/17 22:17:36  fwarmerdam
 * some adjustments to deciding whether to use direct io or not
 *
 * Revision 1.22  2005/02/16 22:08:21  fwarmerdam
 * split out v3 driver, added dataset::IRasterIO
 *
 * Revision 1.21  2005/02/12 17:52:28  kmelero
 * Rewrote CreateCopy to support v2 and v3 of MrSID.  (kmelero@sanz.com)
 *
 * Revision 1.20  2005/02/06 18:40:05  dron
 * Fixes in colorspace handling when the new file creating; use WORLDFILE option.
 *
 * Revision 1.19  2005/02/02 14:58:46  fwarmerdam
 * Added a few debug statements.
 *
 * Revision 1.18  2004/11/14 21:18:22  dron
 * Initial support for MrSID Encoding SDK.
 *
 * Revision 1.17  2004/09/09 19:20:18  dron
 * Use VSIFClose instead of VSIFCloseL in MrSIDDataset::Open().
 *
 * Revision 1.16  2004/09/09 18:13:27  dron
 * Fixed bug when getting NoData value in MrSIDRasterBand().
 *
 * Revision 1.15  2004/08/29 10:11:43  dron
 * Implement caching for the case of sequental block reading.
 *
 * Revision 1.14  2004/08/20 19:03:58  dron
 * Fixed problem with wrong sign in georeference calculation.
 *
 * Revision 1.13  2004/08/07 10:25:12  dron
 * Added support for overviews in new version; fixed problem with the last
 * strip decoding; support for DSDK 4.0.9.713.
 *
 * Revision 1.12  2004/07/04 06:26:30  dron
 * Added support for decoding JPEG2000 format using MrSID DSDK.
 *
 * Revision 1.11  2004/05/02 15:47:26  dron
 * Updated to DSDK 4.0.7.646.
 *
 * Revision 1.10  2004/04/27 08:45:59  dron
 * Read strip height from image and use it as BlockYSize.
 *
 * Revision 1.9  2004/04/23 09:49:38  dron
 * Added support for DSDK v.4.0.x; significant changes in the driver structure.
 *
 * Revision 1.8  2004/03/19 12:19:48  dron
 * Few improvements; several memory leaks fixed.
 *
 * Revision 1.7  2004/02/08 14:29:43  dron
 * Support for DSDK v.3.2.x.
 *
 * Revision 1.6  2004/02/03 20:56:45  dron
 * Fixes in coordinate handling.
 *
 * Revision 1.5  2003/07/24 09:33:07  dron
 * Added MrSIDRasterBand::IRasterIO().
 *
 * Revision 1.4  2003/05/27 21:08:38  warmerda
 * avoid int/short casting warnings
 *
 * Revision 1.3  2003/05/25 15:24:49  dron
 * Added georeferencing.
 *
 * Revision 1.2  2003/05/07 10:26:02  dron
 * Added multidimensional metadata handling.
 *
 * Revision 1.1  2003/04/23 12:21:56  dron
 * New.
 *
 */

#include "gdal_pam.h"
#include "ogr_spatialref.h"
#include "cpl_string.h"

#include <geo_normalize.h>
#include <geovalues.h>

CPL_CVSID("$Id: mrsiddataset.cpp,v 1.38 2005/07/22 15:50:10 fwarmerdam Exp $");

CPL_C_START
void    GDALRegister_MrSID(void);
double GTIFAngleToDD( double dfAngle, int nUOMAngle );
CPL_C_END

#include "lt_types.h"
#include "lt_base.h"
#include "lt_fileSpec.h"
#include "lt_ioFileStream.h"
#include "lt_utilStatusStrings.h"
#include "lti_geoCoord.h"
#include "lti_pixel.h"
#include "lti_navigator.h"
#include "lti_sceneBuffer.h"
#include "lti_metadataDatabase.h"
#include "lti_metadataRecord.h"
#include "lti_utils.h"
#include "MrSIDImageReader.h"
//#include "J2KImageReader.h"

#ifdef MRSID_ESDK_VERSION_40
# include "MG3ImageWriter.h"
# include "MG3WriterParams.h"
# include "MG2ImageWriter.h"
# include "MG2WriterParams.h"
#endif /* MRSID_ESDK_VERSION_40 */

LT_USE_NAMESPACE(LizardTech)

/************************************************************************/
/* ==================================================================== */
/*                              MrSIDDataset                            */
/* ==================================================================== */
/************************************************************************/

class MrSIDDataset : public GDALPamDataset
{
    friend class MrSIDRasterBand;

    LTIImageReader      *poImageReader;
#ifdef MRSID_ESDK_VERSION_40
    LTIMeteredImageWriter *poImageWriter;
#endif /* MRSID_ESDK_VERSION_40 */
    LTINavigator        *poLTINav;
    LTIMetadataDatabase *poMetadata;
    const LTIPixel      *poNDPixel;

    LTISceneBuffer      *poBuffer;
    int                 nBlockXSize, nBlockYSize;
    int                 bPrevBlockRead;
    int                 nPrevBlockXOff, nPrevBlockYOff;

    LTIDataType         eSampleType;
    GDALDataType        eDataType;
    LTIColorSpace       eColorSpace;

    double              dfCurrentMag;

    int                 bHasGeoTransform;
    double              adfGeoTransform[6];
    char                *pszProjection;
    GTIFDefn            *psDefn;

    int                 bIsOverview;
    int                 nOverviewCount;
    MrSIDDataset        **papoOverviewDS;

    CPLErr              OpenZoomLevel( lt_int32 iZoom );
    char                *SerializeMetadataRec( const LTIMetadataRecord* );
    int                 GetMetadataElement( const char *, void *, int=0 );
    void                FetchProjParms();
    void                GetGTIFDefn();
    char                *GetOGISDefn( GTIFDefn * );

    virtual CPLErr      IRasterIO( GDALRWFlag, int, int, int, int, void *,
                                   int, int, GDALDataType, int, int *,int,
                                   int, int );

  public:
                MrSIDDataset();
                ~MrSIDDataset();

    static GDALDataset  *Open( GDALOpenInfo * );
    virtual CPLErr      GetGeoTransform( double * padfTransform );
    const char          *GetProjectionRef();

#ifdef MRSID_ESDK_VERSION_40
    static GDALDataset  *Create( const char * pszFilename,
                                 int nXSize, int nYSize, int nBands,
                                 GDALDataType eType, char ** papszParmList );
    virtual void        FlushCache( void );
#endif /* MRSID_ESDK_VERSION_40 */
};

#include "mrsidcommon.h"

/************************************************************************/
/* ==================================================================== */
/*                           MrSIDRasterBand                            */
/* ==================================================================== */
/************************************************************************/

class MrSIDRasterBand : public GDALPamRasterBand
{
    friend class MrSIDDataset;

    LTIPixel        *poPixel;

    int             nBlockSize;

    int             bNoDataSet;
    double          dfNoDataValue;

  public:

                MrSIDRasterBand( MrSIDDataset *, int );
                ~MrSIDRasterBand();

    virtual CPLErr IRasterIO( GDALRWFlag, int, int, int, int,
                              void *, int, int, GDALDataType,
                              int, int );

    virtual CPLErr          IReadBlock( int, int, void * );
    virtual GDALColorInterp GetColorInterpretation();
    virtual double	    GetNoDataValue( int * );
    virtual int             GetOverviewCount();
    virtual GDALRasterBand  *GetOverview( int );

#ifdef MRSID_ESDK_VERSION_40
    virtual CPLErr          IWriteBlock( int, int, void * );
#endif /* MRSID_ESDK_VERSION_40 */
};

/************************************************************************/
/*                           MrSIDRasterBand()                          */
/************************************************************************/

MrSIDRasterBand::MrSIDRasterBand( MrSIDDataset *poDS, int nBand )
{
    this->poDS = poDS;
    this->nBand = nBand;
    this->eDataType = poDS->eDataType;

/* -------------------------------------------------------------------- */
/*      Set the block sizes and buffer parameters.                      */
/* -------------------------------------------------------------------- */
    nBlockXSize = poDS->nBlockXSize;
    nBlockYSize = poDS->nBlockYSize;
//#ifdef notdef
    if( poDS->GetRasterXSize() > 2048 )
        nBlockXSize = 1024;
    if( poDS->GetRasterYSize() > 128 )
        nBlockYSize = 128;
    else
        nBlockYSize = poDS->GetRasterYSize();
//#endif

    nBlockSize = nBlockXSize * nBlockYSize;
    poPixel = new LTIPixel( poDS->eColorSpace, poDS->nBands,
                            poDS->eSampleType );


/* -------------------------------------------------------------------- */
/*      Set NoData values.                                              */
/* -------------------------------------------------------------------- */
     if ( poDS->poNDPixel )
     {
         switch( poDS->eSampleType )
         {
             case LTI_DATATYPE_UINT8:
             case LTI_DATATYPE_SINT8:
                 dfNoDataValue = (double)
                     poDS->poNDPixel->getSampleValueUint8( nBand - 1 );
                 break;
             case LTI_DATATYPE_UINT16:
                 dfNoDataValue = (double)
                     poDS->poNDPixel->getSampleValueUint16( nBand - 1 );
                 break;
             case LTI_DATATYPE_FLOAT32:
                 dfNoDataValue =
                     poDS->poNDPixel->getSampleValueFloat32( nBand - 1 );
                 break;
             case LTI_DATATYPE_SINT16:
                 dfNoDataValue = (double)
                     *(GInt16 *)poDS->poNDPixel->getSampleValueAddr( nBand - 1 );
                 break;
             case LTI_DATATYPE_UINT32:
                 dfNoDataValue = (double)
                     *(GUInt32 *)poDS->poNDPixel->getSampleValueAddr( nBand - 1 );
                 break;
             case LTI_DATATYPE_SINT32:
                 dfNoDataValue = (double)
                     *(GInt32 *)poDS->poNDPixel->getSampleValueAddr( nBand - 1 );
                 break;
             case LTI_DATATYPE_FLOAT64:
                 dfNoDataValue =
                     *(double *)poDS->poNDPixel->getSampleValueAddr( nBand - 1 );
                 break;

             case LTI_DATATYPE_INVALID:
                 CPLAssert( FALSE );
                 break;
         }
	 bNoDataSet = TRUE;
     }
     else
     {
	dfNoDataValue = 0.0;
        bNoDataSet = FALSE;
     }
}

/************************************************************************/
/*                            ~MrSIDRasterBand()                        */
/************************************************************************/

MrSIDRasterBand::~MrSIDRasterBand()
{
    if ( poPixel )
        delete poPixel;
}

/************************************************************************/
/*                             IReadBlock()                             */
/************************************************************************/

CPLErr MrSIDRasterBand::IReadBlock( int nBlockXOff, int nBlockYOff,
                                    void * pImage )
{
    MrSIDDataset    *poGDS = (MrSIDDataset *)poDS;

#ifdef MRSID_ESDK_VERSION_40
    if( poGDS->eAccess == GA_Update )
    {
        CPLDebug( "MrSID", 
                  "IReadBlock() - DSDK 4.0 - read on updatable file fails." );
        memset( pImage, 0, nBlockSize * GDALGetDataTypeSize(eDataType) / 8 );
        return CE_None;
    }
#endif /* MRSID_ESDK_VERSION_40 */

    CPLDebug( "MrSID", "IReadBlock(%d,%d)", nBlockXOff, nBlockYOff );

    if ( !poGDS->bPrevBlockRead
         || poGDS->nPrevBlockXOff != nBlockXOff
         || poGDS->nPrevBlockYOff != nBlockYOff )
    {
        GInt32      nLine = nBlockYOff * nBlockYSize;
        GInt32      nCol = nBlockXOff * nBlockXSize;

        // XXX: The scene, passed to LTIImageStage::read() call must be
        // inside the image boundaries. So we should detect the last strip and
        // form the scene properly.
        CPLDebug( "MrSID", 
                  "IReadBlock - read() %dx%d block at %d,%d.", 
                  nBlockXSize, nBlockYSize, nCol, nLine );
                  
        if(!LT_SUCCESS( poGDS->poLTINav->setSceneAsULWH(
                            nCol, nLine,
                            (nCol+nBlockXSize>poGDS->GetRasterXSize())?
                            (poGDS->GetRasterXSize()-nCol):nBlockXSize,
                            (nLine+nBlockYSize>poGDS->GetRasterYSize())?
                            (poGDS->GetRasterYSize()-nLine):nBlockYSize,
                            poGDS->dfCurrentMag) ))
            
        {
            CPLError( CE_Failure, CPLE_AppDefined,
            "MrSIDRasterBand::IReadBlock(): Failed to set scene position." );
            return CE_Failure;
        }

        if ( !poGDS->poBuffer )
        {
            poGDS->poBuffer =
                new LTISceneBuffer( *poPixel, nBlockXSize, nBlockYSize, NULL );
        }

        if(!LT_SUCCESS(poGDS->poImageReader->read(poGDS->poLTINav->getScene(),
                                                  *poGDS->poBuffer)))
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "MrSIDRasterBand::IReadBlock(): Failed to load image." );
            return CE_Failure;
        }

        poGDS->bPrevBlockRead = TRUE;
        poGDS->nPrevBlockXOff = nBlockXOff;
        poGDS->nPrevBlockYOff = nBlockYOff;
    }

    memcpy( pImage, poGDS->poBuffer->getTotalBandData(nBand - 1), 
            nBlockSize * (GDALGetDataTypeSize(poGDS->eDataType) / 8) );

    return CE_None;
}

#ifdef MRSID_ESDK_VERSION_40

/************************************************************************/
/*                            IWriteBlock()                             */
/************************************************************************/

CPLErr MrSIDRasterBand::IWriteBlock( int nBlockXOff, int nBlockYOff,
                                     void * pImage )
{
    MrSIDDataset    *poGDS = (MrSIDDataset *)poDS;

    CPLAssert( poGDS != NULL
               && nBlockXOff >= 0
               && nBlockYOff >= 0
               && pImage != NULL );

#if DEBUG
    CPLDebug( "MrSID", "IWriteBlock(): nBlockXOff=%d, nBlockYOff=%d",
              nBlockXOff, nBlockYOff );
#endif

    LTIScene        oScene( nBlockXOff * nBlockXSize,
                            nBlockYOff * nBlockYSize,
                            nBlockXSize, nBlockYSize, 1.0);
    LTISceneBuffer  oSceneBuf( *poPixel, poGDS->nBlockXSize,
                               poGDS->nBlockYSize, &pImage );

    if( !LT_SUCCESS(poGDS->poImageWriter->writeBegin(oScene)) )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDRasterBand::IWriteBlock(): writeBegin failed." );
        return CE_Failure;
    }

    if( !LT_SUCCESS(poGDS->poImageWriter->writeStrip(oSceneBuf, oScene)) )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDRasterBand::IWriteBlock(): writeStrip failed." );
        return CE_Failure;
    }

    if( !LT_SUCCESS(poGDS->poImageWriter->writeEnd()) )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDRasterBand::IWriteBlock(): writeEnd failed." );
        return CE_Failure;
    }

    return CE_None;
}

#endif /* MRSID_ESDK_VERSION_40 */

/************************************************************************/
/*                             IRasterIO()                              */
/************************************************************************/

CPLErr MrSIDRasterBand::IRasterIO( GDALRWFlag eRWFlag,
                                   int nXOff, int nYOff, int nXSize, int nYSize,
                                   void * pData, int nBufXSize, int nBufYSize,
                                   GDALDataType eBufType,
                                   int nPixelSpace, int nLineSpace )
    
{
    MrSIDDataset *poGDS = (MrSIDDataset *) poDS;

/* -------------------------------------------------------------------- */
/*      Fallback to default implementation if the whole scanline        */
/*      without subsampling requested.                                  */
/* -------------------------------------------------------------------- */
    if ( nXSize == poGDS->GetRasterXSize()
         && nXSize == nBufXSize
         && nYSize == nBufYSize )
    {
        return GDALRasterBand::IRasterIO( eRWFlag, nXOff, nYOff,
                                          nXSize, nYSize, pData,
                                          nBufXSize, nBufYSize, eBufType,
                                          nPixelSpace, nLineSpace );
    }

/* -------------------------------------------------------------------- */
/*      Handle via the dataset level IRasterIO()                        */
/* -------------------------------------------------------------------- */
    return poGDS->IRasterIO( eRWFlag, nXOff, nYOff, nXSize, nYSize, pData, 
                             nBufXSize, nBufYSize, eBufType, 
                             1, &nBand, nPixelSpace, nLineSpace, 0 );
}

/************************************************************************/
/*                       GetColorInterpretation()                       */
/************************************************************************/

GDALColorInterp MrSIDRasterBand::GetColorInterpretation()
{
    MrSIDDataset      *poGDS = (MrSIDDataset *) poDS;

    switch( poGDS->eColorSpace )
    {
        case LTI_COLORSPACE_RGB:
            if( nBand == 1 )
                return GCI_RedBand;
            else if( nBand == 2 )
                return GCI_GreenBand;
            else if( nBand == 3 )
                return GCI_BlueBand;
            else
                return GCI_Undefined;
        case LTI_COLORSPACE_RGBK:
            if( nBand == 1 )
                return GCI_RedBand;
            else if( nBand == 2 )
                return GCI_GreenBand;
            else if( nBand == 3 )
                return GCI_BlueBand;
            else if( nBand == 4 )
                return GCI_AlphaBand;
            else
                return GCI_Undefined;
        case LTI_COLORSPACE_CMYK:
            if( nBand == 1 )
                return GCI_CyanBand;
            else if( nBand == 2 )
                return GCI_MagentaBand;
            else if( nBand == 3 )
                return GCI_YellowBand;
            else if( nBand == 4 )
                return GCI_BlackBand;
            else
                return GCI_Undefined;
        case LTI_COLORSPACE_GRAYSCALE:
            return GCI_GrayIndex;
        default:
            return GCI_Undefined;
    }

}

/************************************************************************/
/*                           GetNoDataValue()                           */
/************************************************************************/

double MrSIDRasterBand::GetNoDataValue( int * pbSuccess )

{
    if( pbSuccess )
        *pbSuccess = bNoDataSet;

    return dfNoDataValue;
}

/************************************************************************/
/*                          GetOverviewCount()                          */
/************************************************************************/

int MrSIDRasterBand::GetOverviewCount()

{
    MrSIDDataset        *poGDS = (MrSIDDataset *) poDS;

    return poGDS->nOverviewCount;
}

/************************************************************************/
/*                            GetOverview()                             */
/************************************************************************/

GDALRasterBand *MrSIDRasterBand::GetOverview( int i )

{
    MrSIDDataset        *poGDS = (MrSIDDataset *) poDS;

    if( i < 0 || i >= poGDS->nOverviewCount )
        return NULL;
    else
        return poGDS->papoOverviewDS[i]->GetRasterBand( nBand );
}

/************************************************************************/
/*                           MrSIDDataset()                             */
/************************************************************************/

MrSIDDataset::MrSIDDataset()
{
    poImageReader = NULL;
#ifdef MRSID_ESDK_VERSION_40
    poImageWriter = NULL;
#endif /* MRSID_ESDK_VERSION_40 */
    poLTINav = NULL;
    poMetadata = NULL;
    poNDPixel = NULL;
    eSampleType = LTI_DATATYPE_UINT8;
    nBands = 0;
    eDataType = GDT_Byte;
    
    poBuffer = NULL;
    bPrevBlockRead = FALSE;
    nPrevBlockXOff = 0;
    nPrevBlockYOff = 0;
    
    pszProjection = CPLStrdup( "" );
    bHasGeoTransform = FALSE;
    adfGeoTransform[0] = 0.0;
    adfGeoTransform[1] = 1.0;
    adfGeoTransform[2] = 0.0;
    adfGeoTransform[3] = 0.0;
    adfGeoTransform[4] = 0.0;
    adfGeoTransform[5] = 1.0;
    psDefn = NULL;
    
    dfCurrentMag = 1.0;
    bIsOverview = FALSE;
    nOverviewCount = 0;
    papoOverviewDS = NULL;
}

/************************************************************************/
/*                            ~MrSIDDataset()                           */
/************************************************************************/

MrSIDDataset::~MrSIDDataset()
{
    FlushCache();
#ifdef MRSID_ESDK_VERSION_40

    if ( poImageWriter )
        delete poImageWriter;
#endif /* MRSID_ESDK_VERSION_40 */

    if ( poImageReader && !bIsOverview )
        delete poImageReader;
    if ( poLTINav )
        delete poLTINav;
    if ( poBuffer )
        delete poBuffer;
    if ( poMetadata )
        delete poMetadata;
    if ( pszProjection )
        CPLFree( pszProjection );
    if ( psDefn )
        delete psDefn;
    if ( papoOverviewDS )
    {
        for( int i = 0; i < nOverviewCount; i++ )
            delete papoOverviewDS[i];
        CPLFree( papoOverviewDS );
    }
}

/************************************************************************/
/*                             IRasterIO()                              */
/************************************************************************/

CPLErr MrSIDDataset::IRasterIO( GDALRWFlag eRWFlag,
                                int nXOff, int nYOff, int nXSize, int nYSize,
                                void * pData, int nBufXSize, int nBufYSize,
                                GDALDataType eBufType, 
                                int nBandCount, int *panBandMap,
                                int nPixelSpace, int nLineSpace, int nBandSpace )

{
/* -------------------------------------------------------------------- */
/*      We need various criteria to skip out to block based methods.    */
/* -------------------------------------------------------------------- */
    int bUseBlockedIO = bForceCachedIO;

    if( nYSize == 1 || nXSize * ((double) nYSize) < 100.0 )
        bUseBlockedIO = TRUE;

    if( nBufYSize == 1 || nBufXSize * ((double) nBufYSize) < 100.0 )
        bUseBlockedIO = TRUE;

    if( CSLTestBoolean( CPLGetConfigOption( "GDAL_ONE_BIG_READ", "NO") ) )
        bUseBlockedIO = FALSE;

    if( bUseBlockedIO )
        return GDALDataset::BlockBasedRasterIO( 
            eRWFlag, nXOff, nYOff, nXSize, nYSize,
            pData, nBufXSize, nBufYSize, eBufType, 
            nBandCount, panBandMap, nPixelSpace, nLineSpace, nBandSpace );

    CPLDebug( "MrSID", "RasterIO() - using optimized dataset level IO." );
    
/* -------------------------------------------------------------------- */
/*      What is our requested window relative to the base dataset.      */
/*      We want to operate from here on as if we were operating on      */
/*      the full res band.                                              */
/* -------------------------------------------------------------------- */
    int nZoomMag = (int) (dfCurrentMag * 1.0001);

    nXOff *= nZoomMag;
    nYOff *= nZoomMag;
    nXSize *= nZoomMag;
    nYSize *= nZoomMag;

/* -------------------------------------------------------------------- */
/*      We need to figure out the best zoom level to use for this       */
/*      request.  We apply a small fudge factor to make sure that       */
/*      request just very, very slightly larger than a zoom level do    */
/*      not force us to the next level.                                 */
/* -------------------------------------------------------------------- */
    int iOverview = 0;
    double dfZoomMag = MIN((nXSize / (double)nBufXSize), 
                           (nYSize / (double)nBufYSize));

    for( nZoomMag = 1; 
         nZoomMag * 2 < (dfZoomMag + 0.1) && iOverview < nOverviewCount; 
         nZoomMag *= 2, iOverview++ ) {}

/* -------------------------------------------------------------------- */
/*      Work out the size of the temporary buffer and allocate it.      */
/*      The temporary buffer will generally be at a moderately          */
/*      higher resolution than the buffer of data requested.            */
/* -------------------------------------------------------------------- */
    int nTmpXSize, nTmpYSize, nTmpPixelSize;
    LTIPixel       oPixel( eColorSpace, nBands, eSampleType );
    
    nTmpXSize = (int) floor( nXSize / (double) nZoomMag + 0.99 );
    nTmpYSize = (int) floor( nYSize / (double) nZoomMag + 0.99 );

    LTISceneBuffer oLTIBuffer( oPixel, nTmpXSize, nTmpYSize, NULL );

    nTmpPixelSize = GDALGetDataTypeSize( eDataType ) / 8;

/* -------------------------------------------------------------------- */
/*      Create navigator, and move to the requested scene area.         */
/* -------------------------------------------------------------------- */
    LTINavigator oNav( *poImageReader );
    
    if( !LT_SUCCESS(oNav.setSceneAsULWH( nXOff / nZoomMag, nYOff / nZoomMag, 
                                         nXSize / nZoomMag, nYSize / nZoomMag, 
                                         1.0 / nZoomMag )) )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDDataset::IRasterIO(): Failed to set scene position." );

        return CE_Failure;
    }

    CPLDebug( "MrSID", 
              "Dataset:IRasterIO(%d,%d %dx%d -> %dx%d -> %dx%d, zoom=%d)",
              nXOff, nYOff, nXSize, nYSize, 
              nTmpXSize, nTmpYSize,
              nBufXSize, nBufYSize, 
              nZoomMag );

    if( !oNav.isSceneValid() )
        CPLDebug( "MrSID", "LTINavigator in invalid state." );

#ifdef notdef
    {
        lt_uint32 iWidth, iHeight;
        
        poImageReader->getDimsAtMag( dfCurrentMag, iWidth, iHeight );
    }
#endif

/* -------------------------------------------------------------------- */
/*      Read into the buffer.                                           */
/* -------------------------------------------------------------------- */
    LT_STATUS eLTStatus;

    eLTStatus = poImageReader->read(oNav.getScene(),oLTIBuffer);
    if(!LT_SUCCESS(eLTStatus) )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDRasterBand::IRasterIO(): Failed to load image.\n%s",
                  getLastStatusString( eLTStatus ) );
        return CE_Failure;
    }
    
/* -------------------------------------------------------------------- */
/*      Manually resample to our target buffer.                         */
/* -------------------------------------------------------------------- */
    int         iBufLine, iBufPixel;

    for( iBufLine = 0; iBufLine < nBufYSize; iBufLine++ )
    {
        int iTmpLine = (int) floor(((iBufLine+0.5) / nBufYSize) * nTmpYSize);

        for( iBufPixel = 0; iBufPixel < nBufXSize; iBufPixel++ )
        {
            int iTmpPixel = (int) 
                floor(((iBufPixel+0.5) / nBufXSize) * nTmpXSize);

            for( int iBand = 0; iBand < nBandCount; iBand++ )
            {
                GByte *pabySrc, *pabyDst;

                pabyDst = ((GByte *) pData) 
                    + nPixelSpace * iBufPixel
                    + nLineSpace * iBufLine
                    + nBandSpace * iBand;

                pabySrc = (GByte *) oLTIBuffer.getTotalBandData( 
                    panBandMap[iBand] - 1 );
                pabySrc += (iTmpLine * nTmpXSize + iTmpPixel) * nTmpPixelSize;

                if( eDataType == eBufType )
                    memcpy( pabyDst, pabySrc, nTmpPixelSize );
                else
                    GDALCopyWords( pabySrc, eDataType, 0, 
                                   pabyDst, eBufType, 0, 1 );
            }
        }
    }

    return CE_None;
}

/************************************************************************/
/*                          GetGeoTransform()                           */
/************************************************************************/

CPLErr MrSIDDataset::GetGeoTransform( double * padfTransform )
{
    if( bHasGeoTransform )
    {
        memcpy( padfTransform, adfGeoTransform, sizeof(adfGeoTransform[0]) * 6 );
        return CE_None;
    }
    else
        return GDALPamDataset::GetGeoTransform( padfTransform );
}

/************************************************************************/
/*                          GetProjectionRef()                          */
/************************************************************************/

const char *MrSIDDataset::GetProjectionRef()
{
    if( pszProjection )
        return pszProjection;
    else
        return GDALPamDataset::GetProjectionRef();
}

/************************************************************************/
/*                        SerializeMetadataRec()                        */
/************************************************************************/

char *MrSIDDataset::SerializeMetadataRec( const LTIMetadataRecord *poMetadataRec )
{
    GUInt32  iNumDims = 0;
    const GUInt32  *paiDims = NULL;
    const void     *pData = poMetadataRec->getArrayData( iNumDims, paiDims );
    GUInt32        i, j, k = 0, iLength;
    const char     *pszTemp = NULL;
    char           *pszMetadata = CPLStrdup( "" );

    for ( i = 0; i < iNumDims; i++ )
        for ( j = 0; j < paiDims[i]; j++ )
        {
            switch( poMetadataRec->getDataType() )
            {
                case LTI_METADATA_DATATYPE_UINT8:
                case LTI_METADATA_DATATYPE_SINT8:
                    pszTemp = CPLSPrintf( "%d", ((GByte *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_UINT16:
                    pszTemp = CPLSPrintf( "%u", ((GUInt16 *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_SINT16:
                    pszTemp = CPLSPrintf( "%d", ((GInt16 *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_UINT32:
                    pszTemp = CPLSPrintf( "%lu", ((unsigned long *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_SINT32:
                    pszTemp = CPLSPrintf( "%ld", ((long *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_FLOAT32:
                    pszTemp = CPLSPrintf( "%f", ((float *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_FLOAT64:
                    pszTemp = CPLSPrintf( "%lf", ((double *)pData)[k++] );
                    break;
                case LTI_METADATA_DATATYPE_ASCII:
                    pszTemp = ((const char **)pData)[k++];
                    break;
                default:
                    pszTemp = "";
                    break;
            }

            iLength = strlen(pszMetadata) + strlen(pszTemp) + 2;

            pszMetadata = (char *)CPLRealloc( pszMetadata, iLength );
            if ( !EQUAL( pszMetadata, "" ) )
                strncat( pszMetadata, ",", 1 );
            strncat( pszMetadata, pszTemp, iLength );
        }

    return pszMetadata;
}

/************************************************************************/
/*                          GetMetadataElement()                        */
/************************************************************************/

int MrSIDDataset::GetMetadataElement( const char *pszKey, void *pValue,
                                      int iLength )
{
    if ( !poMetadata->has( pszKey ) )
        return FALSE;

    const LTIMetadataRecord *poMetadataRec = NULL;
    poMetadata->get( pszKey, poMetadataRec );

    if ( !poMetadataRec->isScalar() )
	return FALSE;

    // XXX: return FALSE if we have more than one element in metadata record
    int iSize;
    switch( poMetadataRec->getDataType() )
    {
         case LTI_METADATA_DATATYPE_UINT8:
         case LTI_METADATA_DATATYPE_SINT8:
	     iSize = 1;
	     break;
         case LTI_METADATA_DATATYPE_UINT16:
         case LTI_METADATA_DATATYPE_SINT16:
             iSize = 2;
             break;
         case LTI_METADATA_DATATYPE_UINT32:
         case LTI_METADATA_DATATYPE_SINT32:
         case LTI_METADATA_DATATYPE_FLOAT32:
             iSize = 4;
             break;
         case LTI_METADATA_DATATYPE_FLOAT64:
             iSize = 8;
             break;
	 case LTI_METADATA_DATATYPE_ASCII:
	     iSize = iLength;
	     break;
         default:
             iSize = 0;
             break;
    }

    if ( poMetadataRec->getDataType() == LTI_METADATA_DATATYPE_ASCII )
    {
	strncpy( (char *)pValue,
		 ((const char**)poMetadataRec->getScalarData())[0], iSize );
        ((char *)pValue)[iSize - 1] = '\0';
    }
    else
        memcpy( pValue, poMetadataRec->getScalarData(), iSize );

    return TRUE;
}

/************************************************************************/
/*                             OpenZoomLevel()                          */
/************************************************************************/

CPLErr MrSIDDataset::OpenZoomLevel( lt_int32 iZoom )
{
/* -------------------------------------------------------------------- */
/*      Get image geometry.                                            */
/* -------------------------------------------------------------------- */
    if ( iZoom != 0 )
    {
        lt_uint32 iWidth, iHeight;
        dfCurrentMag = LTIUtils::levelToMag( iZoom );
        poImageReader->getDimsAtMag( dfCurrentMag, iWidth, iHeight );
	nRasterXSize = iWidth;
	nRasterYSize = iHeight;
    }
    else
    {
        dfCurrentMag = 1.0;
        nRasterXSize = poImageReader->getWidth();
        nRasterYSize = poImageReader->getHeight();
    }

    nBands = poImageReader->getNumBands();
    nBlockXSize = nRasterXSize;
    nBlockYSize = poImageReader->getStripHeight();

    CPLDebug( "MrSID", "Opened zoom level %d with size %dx%d.\n",
              iZoom, nRasterXSize, nRasterYSize );

    try
    {
        poLTINav = new LTINavigator( *poImageReader );
    }
    catch ( ... )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDDataset::OpenZoomLevel(): "
                  "Failed to create LTINavigator object." );
        return CE_Failure;
    }

/* -------------------------------------------------------------------- */
/*  Handle sample type and color space.                                 */
/* -------------------------------------------------------------------- */
    eColorSpace = poImageReader->getColorSpace();
    eSampleType = poImageReader->getDataType();
    switch ( eSampleType )
    {
      case LTI_DATATYPE_UINT16:
        eDataType = GDT_UInt16;
        break;
      case LTI_DATATYPE_SINT16:
        eDataType = GDT_Int16;
        break;
      case LTI_DATATYPE_UINT32:
        eDataType = GDT_UInt32;
        break;
      case LTI_DATATYPE_SINT32:
        eDataType = GDT_Int32;
        break;
      case LTI_DATATYPE_FLOAT32:
        eDataType = GDT_Float32;
        break;
      case LTI_DATATYPE_FLOAT64:
        eDataType = GDT_Float64;
        break;
      case LTI_DATATYPE_UINT8:
      case LTI_DATATYPE_SINT8:
      default:
        eDataType = GDT_Byte;
        break;
    }

/* -------------------------------------------------------------------- */
/*      Read georeferencing.                                            */
/* -------------------------------------------------------------------- */
    if ( !poImageReader->isGeoCoordImplicit() )
    {
        const LTIGeoCoord& oGeo = poImageReader->getGeoCoord();
        oGeo.get( adfGeoTransform[0], adfGeoTransform[3],
	          adfGeoTransform[1], adfGeoTransform[5],
	          adfGeoTransform[2], adfGeoTransform[4] );
        
        adfGeoTransform[0] = adfGeoTransform[0] - adfGeoTransform[1] / 2;
        adfGeoTransform[3] = adfGeoTransform[3] - adfGeoTransform[5] / 2;
	bHasGeoTransform = TRUE;
    }
    else if( iZoom == 0 )
    {
        bHasGeoTransform = 
            GDALReadWorldFile( GetDescription(), ".sdw",  adfGeoTransform )
            || GDALReadWorldFile( GetDescription(), ".sidw", 
                                  adfGeoTransform )
            || GDALReadWorldFile( GetDescription(), ".wld", 
                                  adfGeoTransform );
    }
    
/* -------------------------------------------------------------------- */
/*      Read wkt.                                                       */
/* -------------------------------------------------------------------- */
#ifdef HAVE_MRSID_GETWKT
    if ( !poImageReader->isGeoCoordImplicit() )
    {
	const LTIGeoCoord& oGeo = poImageReader->getGeoCoord();
	
	if( oGeo.getWKT() )
            pszProjection =  CPLStrdup( oGeo.getWKT() );
    }
#endif // HAVE_MRSID_GETWKT

/* -------------------------------------------------------------------- */
/*      Read NoData value.                                              */
/* -------------------------------------------------------------------- */
    poNDPixel = poImageReader->getNoDataPixel();

/* -------------------------------------------------------------------- */
/*      Create band information objects.                                */
/* -------------------------------------------------------------------- */
    int             iBand;

    for( iBand = 1; iBand <= nBands; iBand++ )
        SetBand( iBand, new MrSIDRasterBand( this, iBand ) );

    return CE_None;
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

GDALDataset *MrSIDDataset::Open( GDALOpenInfo * poOpenInfo )
{
    int     bIsJP2 = FALSE;

/* -------------------------------------------------------------------- */
/*      Is this a mrsid or jpeg 2000 file?                              */
/* -------------------------------------------------------------------- */
    if( poOpenInfo->nHeaderBytes < 32 )
        return NULL;

    if( EQUALN((const char *) poOpenInfo->pabyHeader + 4, "jP  ", 4) )
        bIsJP2 = TRUE;
    else if ( !EQUALN((const char *) poOpenInfo->pabyHeader, "msid", 4) )
        return NULL;

    if(poOpenInfo->fp)
    {
        VSIFClose( poOpenInfo->fp );
        poOpenInfo->fp = NULL;
    }

/* -------------------------------------------------------------------- */
/*      Create a corresponding GDALDataset.                             */
/* -------------------------------------------------------------------- */
    MrSIDDataset        *poDS;
    const LTFileSpec    oFileSpec( poOpenInfo->pszFilename );

    poDS = new MrSIDDataset();
    //    if ( bIsJP2 )
    //    poDS->poImageReader = new J2KImageReader( oFileSpec, true );
    //else
        poDS->poImageReader = new MrSIDImageReader( oFileSpec );
    if ( !LT_SUCCESS( poDS->poImageReader->initialize() ) )
    {
        delete poDS;
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDDataset::Open(): Failed to open file %s",
                  poOpenInfo->pszFilename );
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Read metadata.                                                  */
/* -------------------------------------------------------------------- */
    poDS->poMetadata =
        new LTIMetadataDatabase( poDS->poImageReader->getMetadata() );
    const GUInt32       iNumRecs = poDS->poMetadata->getIndexCount();
    GUInt32             i;

    for ( i = 0; i < iNumRecs; i++ )
    {
        const LTIMetadataRecord *poMetadataRec = NULL;
        if ( LT_SUCCESS(poDS->poMetadata->getDataByIndex(i, poMetadataRec)) )
	{
            char    *pszElement = poDS->SerializeMetadataRec( poMetadataRec );
            char    *pszKey = CPLStrdup( poMetadataRec->getTagName() );
            char    *pszTemp = pszKey;

            // GDAL metadata keys should not contain ':' and '=' characters.
            // We will replace them with '_'.
            do
            {
                if ( *pszTemp == ':' || *pszTemp == '=' )
                    *pszTemp = '_';
            }
            while ( *++pszTemp );

            poDS->SetMetadataItem( pszKey, pszElement );

            CPLFree( pszElement );
            CPLFree( pszKey );
	}
    }

    poDS->GetGTIFDefn();
    
/* -------------------------------------------------------------------- */
/*      Get number of resolution levels (we will use them as overviews).*/
/* -------------------------------------------------------------------- */
    poDS->nOverviewCount
        = ((MrSIDImageReader *) (poDS->poImageReader))->getNumLevels();

    if ( poDS->nOverviewCount > 0 )
    {
        lt_int32        i;

        poDS->papoOverviewDS = (MrSIDDataset **)
            CPLMalloc( poDS->nOverviewCount * (sizeof(void*)) );

        for ( i = 0; i < poDS->nOverviewCount; i++ )
        {
            poDS->papoOverviewDS[i] = new MrSIDDataset();
            poDS->papoOverviewDS[i]->poImageReader = poDS->poImageReader;
            poDS->papoOverviewDS[i]->OpenZoomLevel( i + 1 );
            poDS->papoOverviewDS[i]->bIsOverview = TRUE;
            
        }
    }

/* -------------------------------------------------------------------- */
/*      Create object for the whole image.                              */
/* -------------------------------------------------------------------- */
    poDS->OpenZoomLevel( 0 );

    CPLDebug( "MrSID",
              "Opened image: width %d, height %d, bands %d",
              poDS->nRasterXSize, poDS->nRasterYSize, poDS->nBands );

/* -------------------------------------------------------------------- */
/*      Initialize any PAM information.                                 */
/* -------------------------------------------------------------------- */
    poDS->SetDescription( poOpenInfo->pszFilename );
    poDS->TryLoadXML();

    return( poDS );
}

#ifdef MRSID_ESDK_VERSION_40

/************************************************************************/
/* ==================================================================== */
/*                        MrSIDDummyImageReader                         */
/*                                                                      */
/*  This is a helper class to wrap GDAL calls in MrSID interface.       */
/* ==================================================================== */
/************************************************************************/

class MrSIDDummyImageReader : public LTIImageReader
{
  public:

    MrSIDDummyImageReader( GDALDataset *poSrcDS );
    ~MrSIDDummyImageReader();
    LT_STATUS           initialize();
    lt_int64            getPhysicalFileSize(void) const { return 0; };

  private:
    GDALDataset         *poDS;
    GDALDataType        eDataType;
    LTIDataType         eSampleType;
    const LTIPixel      *poPixel;
    
    double              adfGeoTransform[6];

    virtual LT_STATUS   decodeStrip( LTISceneBuffer& stripBuffer,
                                     const LTIScene& stripScene );
    virtual LT_STATUS   decodeBegin( const LTIScene& scene )
                            { return LT_STS_Success; };
    virtual LT_STATUS   decodeEnd() { return LT_STS_Success; };
};

/************************************************************************/
/*                        MrSIDDummyImageReader()                       */
/************************************************************************/

MrSIDDummyImageReader::MrSIDDummyImageReader( GDALDataset *poSrcDS ) :
                                            LTIImageReader(), poDS(poSrcDS)
{
    poPixel = NULL;
}

/************************************************************************/
/*                        ~MrSIDDummyImageReader()                      */
/************************************************************************/

MrSIDDummyImageReader::~MrSIDDummyImageReader()
{
    if ( poPixel )
        delete poPixel;
}

/************************************************************************/
/*                             initialize()                             */
/************************************************************************/

LT_STATUS MrSIDDummyImageReader::initialize()
{
    if ( !LT_SUCCESS(LTIImageReader::initialize()) )
        return LT_STS_Failure;
    
    lt_uint16 nBands = (lt_uint16)poDS->GetRasterCount();
    LTIColorSpace eColorSpase = LTI_COLORSPACE_RGB;
    switch ( nBands )
    {
        case 1:
            eColorSpase = LTI_COLORSPACE_GRAYSCALE;
            break;
        case 3:
            eColorSpase = LTI_COLORSPACE_RGB;
            break;
        default:
            eColorSpase = LTI_COLORSPACE_RGB;
            break;
    }
    
    eDataType = poDS->GetRasterBand(1)->GetRasterDataType();
    switch ( eDataType )
    {
        case GDT_UInt16:
            eSampleType = LTI_DATATYPE_UINT16;
            break;
        case GDT_Int16:
            eSampleType = LTI_DATATYPE_SINT16;
            break;
        case GDT_UInt32:
            eSampleType = LTI_DATATYPE_UINT32;
            break;
        case GDT_Int32:
            eSampleType = LTI_DATATYPE_SINT32;
            break;
        case GDT_Float32:
            eSampleType = LTI_DATATYPE_FLOAT32;
            break;
        case GDT_Float64:
            eSampleType = LTI_DATATYPE_FLOAT64;
            break;
        case GDT_Byte:
        default:
            eSampleType = LTI_DATATYPE_UINT8;
            break;
    }

    poPixel = new LTIPixel( eColorSpase, nBands, eSampleType );
    if ( !LT_SUCCESS(setPixelProps(*poPixel)) )
        return LT_STS_Failure;

    if ( !LT_SUCCESS(setDimensions(poDS->GetRasterXSize(),
                                   poDS->GetRasterYSize())) )
        return LT_STS_Failure;

    if ( poDS->GetGeoTransform( adfGeoTransform ) == CE_None )
    {
        LTIGeoCoord oGeo( adfGeoTransform[0] + adfGeoTransform[1] / 2,
                          adfGeoTransform[3] + adfGeoTransform[5] / 2,
                          adfGeoTransform[1], adfGeoTransform[5],
                          adfGeoTransform[2], adfGeoTransform[4], NULL,
                          poDS->GetProjectionRef() );
        if ( !LT_SUCCESS(setGeoCoord( oGeo )) )
            return LT_STS_Failure;
    }

    /*int     bSuccess;
    double  dfNoDataValue = poDS->GetNoDataValue( &bSuccess );
    if ( bSuccess )
    {
        LTIPixel    oNoDataPixel( *poPixel );
        lt_uint16   iBand;

        for (iBand = 0; iBand < (lt_uint16)poDS->GetRasterCount(); iBand++)
            oNoDataPixel.setSampleValueFloat32( iBand, dfNoDataValue );
        if ( !LT_SUCCESS(setNoDataPixel( &oNoDataPixel )) )
            return LT_STS_Failure;
    }*/

    setDefaultDynamicRange();
    setClassicalMetadata();

    return LT_STS_Success;
}

/************************************************************************/
/*                             decodeStrip()                            */
/************************************************************************/

LT_STATUS MrSIDDummyImageReader::decodeStrip(LTISceneBuffer& stripData,
                                             const LTIScene& stripScene)
{
    const lt_int32  nXOff = stripScene.getUpperLeftCol();
    const lt_int32  nYOff = stripScene.getUpperLeftRow();
    const lt_int32  nBufXSize = stripScene.getNumCols();
    const lt_int32  nBufYSize = stripScene.getNumRows();
    const lt_uint16 nBands = poPixel->getNumBands();

    void    *pData = CPLMalloc(nBufXSize * nBufYSize * poPixel->getNumBytes());
    if ( !pData )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "MrSIDDummyImageReader::decodeStrip(): "
                  "Cannot allocate enough space for scene buffer" );
        return LT_STS_Failure;
    }

    poDS->RasterIO( GF_Read, nXOff, nYOff, nBufXSize, nBufYSize, 
                    pData, nBufXSize, nBufYSize, eDataType, nBands, NULL, 
                    0, 0, 0 );
    stripData.importDataBSQ( pData );

    CPLFree( pData );

    return LT_STS_Success;
}


/************************************************************************/
/*                             FlushCache()                             */
/************************************************************************/

void MrSIDDataset::FlushCache()

{
    GDALDataset::FlushCache();
}

/************************************************************************/
/*                             CreateCopy()                             */
/************************************************************************/

static GDALDataset *
CreateCopy( const char * pszFilename, GDALDataset *poSrcDS,
            int bStrict, char ** papszOptions, 
            GDALProgressFunc pfnProgress, void * pProgressData )

{ 
  const char* pszVerison = CSLFetchNameValue(papszOptions, "VERSION");

  // Output Mrsid Version 2 file.
  if( pszVerison && atoi(pszVerison) == 2 )
    {
      int nXSize = poSrcDS->GetRasterXSize();
      int nYSize = poSrcDS->GetRasterYSize();
      
      if( !pfnProgress( 0.0, NULL, pProgressData ) )
        return NULL;
      
      // Create the file.                                               
      MrSIDDummyImageReader oImageReader( poSrcDS );
      oImageReader.initialize();
      
      MG2ImageWriter oImageWriter(&oImageReader);
      oImageWriter.initialize();

      oImageWriter.setUsageMeterEnabled(true);
   
      // set output filename
      oImageWriter.setOutputFileSpec( pszFilename );

      // Set defaults
      oImageWriter.params().setBlockSize(oImageWriter.params().getBlockSize());
      oImageWriter.setStripHeight(oImageWriter.getStripHeight());

      // check for compression option
      const char* pszValue = CSLFetchNameValue(papszOptions, "COMPRESSION");
      if( pszValue != NULL )
	oImageWriter.params().setCompressionRatio( atof(pszValue) );

      // set MrSID world file
      if( CSLFetchNameValue(papszOptions, "WORLDFILE") != NULL )
	oImageWriter.setWorldFileSupport( true );
      
      // write the scene
      const LTIScene oScene( 0, 0, nXSize, nYSize, 1.0 );
      oImageWriter.write( oScene );
    }
  // Output Mrsid Version 3 file.
  else
    {
      int nXSize = poSrcDS->GetRasterXSize();
      int nYSize = poSrcDS->GetRasterYSize();
      
      if( !pfnProgress( 0.0, NULL, pProgressData ) )
        return NULL;
      
      // Create the file.   
      MrSIDDummyImageReader oImageReader( poSrcDS );
      oImageReader.initialize();
      
      MG3ImageWriter oImageWriter(&oImageReader);
      oImageWriter.initialize();
      
      // Set 64-bit Interface for large files.
      oImageWriter.setFileStream64(true);

      oImageWriter.setUsageMeterEnabled(true);
      
      // set output filename
      oImageWriter.setOutputFileSpec( pszFilename );

      // Set defaults
      oImageWriter.setStripHeight(oImageWriter.getStripHeight());

      // set 2 pass optimizer option
      if( CSLFetchNameValue(papszOptions, "TWOPASS") != NULL )
	oImageWriter.params().setTwoPassOptimizer( true );

      // set MrSID world file
      if( CSLFetchNameValue(papszOptions, "WORLDFILE") != NULL )
	oImageWriter.setWorldFileSupport( true );
      
      const char* pszValue;
      
      // set filesize in KB
      pszValue = CSLFetchNameValue(papszOptions, "FILESIZE");
      if( pszValue != NULL )
	oImageWriter.params().setTargetFilesize( atoi(pszValue) );
	
      // write the scene
      const LTIScene oScene( 0, 0, nXSize, nYSize, 1.0 );
      oImageWriter.write( oScene );
    }
  
/* -------------------------------------------------------------------- */
/*      Re-open dataset, and copy any auxilary pam information.         */
/* -------------------------------------------------------------------- */
    GDALPamDataset *poDS = (GDALPamDataset *) 
        GDALOpen( pszFilename, GA_ReadOnly );

    if( poDS )
        poDS->CloneInfo( poSrcDS, GCIF_PAM_DEFAULT );

    return poDS;
}
#endif /* MRSID_ESDK_VERSION_40 */

/************************************************************************/
/*                        GDALRegister_MrSID()                          */
/************************************************************************/

void GDALRegister_MrSID()

{
    GDALDriver  *poDriver;

    if( GDALGetDriverByName( "MrSID" ) == NULL )
    {
        poDriver = new GDALDriver();

        poDriver->SetDescription( "MrSID" );
        poDriver->SetMetadataItem( GDAL_DMD_LONGNAME,
                        "Multi-resolution Seamless Image Database (MrSID)" );
        poDriver->SetMetadataItem( GDAL_DMD_HELPTOPIC, "frmt_mrsid.html" );
        poDriver->SetMetadataItem( GDAL_DMD_EXTENSION, "sid" );

#ifdef MRSID_ESDK_VERSION_40
        poDriver->SetMetadataItem( GDAL_DMD_CREATIONDATATYPES, "Byte" );
        poDriver->SetMetadataItem( GDAL_DMD_CREATIONOPTIONLIST,
"<CreationOptionList>"
// Version 2 Options
"   <Option name='COMPRESSION' type='double' description='Set compression ratio (0.0 default is meant to be lossless)'/>"
// Version 3 Options
"   <Option name='TWOPASS' type='int' description='Use twopass optimizer algorithm'/>"
"   <Option name='FILESIZE' type='int' description='Set target file size (0 implies lossless compression)'/>"
// Version 2 and 3 Option
"   <Option name='WORLDFILE' type='boolean' description='Write out world file'/>"
// Version Type
"   <Option name='VERSION' type='int' description='Valid versions are 2 and 3, default = 3'/>"
"</CreationOptionList>" );

        poDriver->pfnCreateCopy = CreateCopy;
#endif /* MRSID_ESDK_VERSION_40 */

        poDriver->pfnOpen = MrSIDDataset::Open;

        GetGDALDriverManager()->RegisterDriver( poDriver );
    }
}


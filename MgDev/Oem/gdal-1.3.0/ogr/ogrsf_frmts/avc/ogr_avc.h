/******************************************************************************
 * $Id: ogr_avc.h,v 1.4 2005/02/22 12:55:03 fwarmerdam Exp $
 *
 * Project:  Arc/Info Coverage (E00 & Binary) Reader
 * Purpose:  Declarations for OGR wrapper classes for coverage access.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2002, Frank Warmerdam
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
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER 
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************
 *
 * $Log: ogr_avc.h,v $
 * Revision 1.4  2005/02/22 12:55:03  fwarmerdam
 * use OGRLayer base spatial filter support
 *
 * Revision 1.3  2002/02/18 20:36:27  warmerda
 * added spatial query, nNextFID, nTableAttrIndex
 *
 * Revision 1.2  2002/02/14 23:01:04  warmerda
 * added region and attribute support
 *
 * Revision 1.1  2002/02/13 20:48:18  warmerda
 * New
 *
 */

#ifndef _OGR_AVC_H_INCLUDED
#define _OGR_AVC_H_INCLUDED

#include "ogrsf_frmts.h"
#include "avc.h"

class OGRAVCDataSource;

/************************************************************************/
/*                             OGRAVCLayer                              */
/************************************************************************/

class OGRAVCLayer : public OGRLayer
{
  protected:
    OGRFeatureDefn      *poFeatureDefn;

    OGRAVCDataSource    *poDS;
    
    AVCFileType         eSectionType;

    int                 SetupFeatureDefinition( const char *pszName ); 
    int                 AppendTableDefinition( AVCTableDef *psTableDef );

    int                 MatchesSpatialFilter( void * );
    OGRFeature          *TranslateFeature( void * );

    int                 TranslateTableFields( OGRFeature *poFeature, 
                                              int nFieldBase, 
                                              AVCTableDef *psTableDef,
                                              AVCField *pasFields );

  public:
                        OGRAVCLayer( AVCFileType eSectionType, 
                                     OGRAVCDataSource *poDS );
    			~OGRAVCLayer();

    OGRFeatureDefn *	GetLayerDefn() { return poFeatureDefn; }

    virtual OGRSpatialReference *GetSpatialRef();
    
    virtual int         TestCapability( const char * );
};

/************************************************************************/
/*                         OGRAVCDataSource                             */
/************************************************************************/

class OGRAVCDataSource : public OGRDataSource
{
  protected:
    OGRSpatialReference *poSRS;
    char		*pszCoverageName;

  public:
		        OGRAVCDataSource();
    			~OGRAVCDataSource();

    OGRSpatialReference *GetSpatialRef();

    const char          *GetCoverageName();
};

/* ==================================================================== */
/*      Binary Coverage Classes                                         */
/* ==================================================================== */

class OGRAVCBinDataSource;

/************************************************************************/
/*                            OGRAVCBinLayer                            */
/************************************************************************/

class OGRAVCBinLayer : public OGRAVCLayer
{
    AVCE00Section       *psSection;
    AVCBinFile          *hFile;

    OGRAVCBinLayer      *poArcLayer;
    int                 bNeedReset;

    char		szTableName[128];
    AVCBinFile          *hTable;
    int                 nTableBaseField;
    int                 nTableAttrIndex;

    int                 nNextFID;

    OGRSpatialReference *poSRS;

    int                 FormPolygonGeometry( OGRFeature *poFeature,
                                             AVCPal *psPAL );

    int                 CheckSetupTable();
    int                 AppendTableFields( OGRFeature *poFeature );

  public:
                        OGRAVCBinLayer( OGRAVCBinDataSource *poDS,
                                        AVCE00Section *psSectionIn );

    			~OGRAVCBinLayer();

    void		ResetReading();
    OGRFeature *	GetNextFeature();
    OGRFeature *	GetFeature( long nFID );

    int                 TestCapability( const char * );
};

/************************************************************************/
/*                         OGRAVCBinDataSource                          */
/************************************************************************/

class OGRAVCBinDataSource : public OGRAVCDataSource
{
    OGRLayer            **papoLayers;
    int			nLayers;
    
    char		*pszName;

    AVCE00ReadPtr       psAVC;
    
  public:
    			OGRAVCBinDataSource();
    			~OGRAVCBinDataSource();

    int			Open( const char *, int bTestOpen );

    const char	        *GetName() { return pszName; }
    int			GetLayerCount() { return nLayers; }
    OGRLayer		*GetLayer( int );

    int                 TestCapability( const char * );

    AVCE00ReadPtr       GetInfo() { return psAVC; }
};

/************************************************************************/
/*                             OGRAVCDriver                             */
/************************************************************************/

class OGRAVCBinDriver : public OGRSFDriver
{
  public:
    		~OGRAVCBinDriver();
                
    const char *GetName();
    OGRDataSource *Open( const char *, int );

    int                 TestCapability( const char * );
};

/* ==================================================================== */
/*      E00 (ASCII) Coverage Classes                                    */
/* ==================================================================== */

/************************************************************************/
/*                            OGRAVCE00Layer                            */
/************************************************************************/
#ifdef notdef
class OGRAVCE00Layer : public OGRAVCLayer
{
    AVCE00Section       *psSection;
    AVCBinFile          *hFile;

  public:
                        OGRAVCE00Layer( OGRAVCDataSource *poDS,
                                        AVCE00Section *psSectionIn );

    			~OGRAVCE00Layer();

    void		ResetReading();
    OGRFeature *	GetNextFeature();
};
#endif

#endif /* _OGR_AVC_H_INCLUDED */

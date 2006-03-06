/******************************************************************************
 * $Id: shape2ogr.cpp,v 1.35 2005/08/04 15:31:09 fwarmerdam Exp $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Implements translation of Shapefile shapes into OGR
 *           representation.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 1999,  Les Technologies SoftMap Inc.
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
 * $Log: shape2ogr.cpp,v $
 * Revision 1.35  2005/08/04 15:31:09  fwarmerdam
 * Added additional patch for computing ring direction in cases of
 * vertices that are very near or coincident.
 *
 * Revision 1.34  2005/08/04 15:10:46  fwarmerdam
 * Committed fix to avoid infinite loop on degenerate shapefiles.  Patch
 * provided by Baumann Konstantin.  See Var2_STRASSE.shp.
 *
 * Revision 1.33  2005/07/20 01:44:25  fwarmerdam
 * try and preserve geometry dimension properly
 *
 * Revision 1.32  2004/07/06 19:35:51  warmerda
 * Default to using FTString for unrecognised field types like logical.
 *
 * Revision 1.31  2004/01/16 22:40:40  warmerda
 * any inner rings not assigned to a polygo are promoted to be outer rings
 *
 * Revision 1.30  2003/12/11 19:19:40  warmerda
 * Fixed leak of geometry objects when building multipolygons.
 *
 * Revision 1.29  2003/11/09 18:54:53  warmerda
 * added auto-FID field creation if new file has no fields on first feature wrt
 *
 * Revision 1.28  2003/10/18 19:01:10  warmerda
 * added Radims patch to recognise multipolygons on read properly - bug 213
 *
 * Revision 1.27  2003/06/10 14:44:11  warmerda
 * Added support for writing shapes with NULL geometries.
 *
 * Revision 1.26  2003/05/27 21:39:53  warmerda
 * added support for writing MULTILINESTRINGs as ARCs
 *
 * Revision 1.25  2003/05/21 04:03:54  warmerda
 * expand tabs
 *
 * Revision 1.24  2003/02/25 14:35:11  warmerda
 * Incorporated more correct support for reading multi-part arcs as
 * MultiLineStrings.
 *
 * Revision 1.23  2002/10/04 04:31:40  warmerda
 * Fixes for writing 3D multipoints.
 *
 * Revision 1.22  2002/10/04 04:30:46  warmerda
 * Fixed some bugs in 3D support for writing polygons.
 *
 * Revision 1.21  2002/08/15 15:10:01  warmerda
 * Fixed problem when GetFeature() called for a non-existant id.
 * http://bugzilla.remotesensing.org/show_bug.cgi?id=175
 *
 * Revision 1.20  2002/08/14 23:27:25  warmerda
 * Fixed reading of Z coordinate for SHPT_ARCZ type.
 *
 * Revision 1.19  2002/04/17 21:49:02  warmerda
 * Fixed a bug writing arcs with Z coordinates.
 *
 * Revision 1.18  2002/04/17 20:03:27  warmerda
 * Try to preserve Z values on read.
 *
 * Revision 1.17  2002/04/17 20:01:39  warmerda
 * Ensure Z coordinate preserved on read.
 *
 * Revision 1.16  2002/04/16 16:21:28  warmerda
 * Ensure polygons written with correct winding.
 *
 * Revision 1.15  2002/03/27 21:04:38  warmerda
 * Added support for reading, and creating lone .dbf files for wkbNone geometry
 * layers.  Added support for creating a single .shp file instead of a directory
 * if a path ending in .shp is passed to the data source create method.
 *
 * Revision 1.14  2001/12/06 18:14:22  warmerda
 * handle case where there is no DBF file
 *
 * Revision 1.13  2001/09/27 14:53:31  warmerda
 * added proper 3D support for all element types
 *
 * Revision 1.12  2001/09/04 15:39:07  warmerda
 * tightened up NULL attribute handling
 *
 * Revision 1.11  2001/07/18 04:55:16  warmerda
 * added CPL_CSVID
 *
 * Revision 1.10  2001/02/06 14:30:15  warmerda
 * don't crash on attempt to write geometryless features
 *
 * Revision 1.9  2000/11/29 14:09:36  warmerda
 * Extended shapefile OGR driver to support MULTIPOLYGON objects.
 *
 * Revision 1.8  2000/01/26 22:05:45  warmerda
 * fixed bug with 2d/3d arcs
 *
 * Revision 1.7  1999/12/23 14:51:07  warmerda
 * Improved support in face of 3D geometries.
 *
 * Revision 1.6  1999/11/04 21:18:01  warmerda
 * fix bug with shapeid for new shapes
 *
 * Revision 1.5  1999/07/27 01:52:04  warmerda
 * added support for writing polygon/multipoint/arc files
 *
 * Revision 1.4  1999/07/27 00:52:40  warmerda
 * added write methods
 *
 * Revision 1.3  1999/07/26 13:59:25  warmerda
 * added feature writing api
 *
 * Revision 1.2  1999/07/12 15:39:18  warmerda
 * added setting of shape geometry
 *
 * Revision 1.1  1999/07/05 18:58:07  warmerda
 * New
 *
 * Revision 1.3  1999/06/11 19:22:13  warmerda
 * added OGRFeature and OGRFeatureDefn related translation
 *
 * Revision 1.2  1999/05/31 20:41:49  warmerda
 * added multipoint support
 *
 * Revision 1.1  1999/05/31 19:26:03  warmerda
 * New
 */

#include "ogrshape.h"
#include "cpl_conv.h"

CPL_CVSID("$Id: shape2ogr.cpp,v 1.35 2005/08/04 15:31:09 fwarmerdam Exp $");

static const double EPSILON = 1E-5;

/************************************************************************/
/*                            epsilonEqual()                            */
/************************************************************************/

static inline bool epsilonEqual(double a, double b, double eps) 
{
    return (::fabs(a - b) < eps);
}

/************************************************************************/
/*                        RingStartEnd                                  */
/*        set first and last vertex for given ring                      */
/************************************************************************/
void RingStartEnd ( SHPObject *psShape, int ring, int *start, int *end )
{
    if( psShape->panPartStart == NULL ) {
	*start = 0;
        *end = psShape->nVertices - 1;
    } else {
        if( ring == psShape->nParts - 1 )
            *end = psShape->nVertices - 1;
        else
            *end = psShape->panPartStart[ring+1] - 1;

        *start = psShape->panPartStart[ring];
    }
}
    
/************************************************************************/
/*                          RingDirection()                             */
/*                                                                      */
/*      return: 1 CW (clockwise)                                        */
/*             -1 CCW (counterclockwise)                                */
/*              0 error                                                 */
/************************************************************************/

int RingDirection ( SHPObject *Shape, int ring ) 
{
    int    i, start, end, v, next;
    double *x, *y, dx0, dy0, dx1, dy1, area2; 

    /* Note: first and last coordinate MUST be identical according to shapefile specification */
    if ( ring < 0 || ring >= Shape->nParts ) return ( 0 );

    x = Shape->padfX;
    y = Shape->padfY;

    RingStartEnd ( Shape, ring, &start, &end );

    if ( start == end ) return ( 0 ); // empty ring!?!?

    /* Find the lowest rightmost vertex */
    v = start;  
    for ( i = start + 1; i < end; i++ ) { /* => v < end */
	if ( y[i] < y[v] || ( y[i] == y[v] && x[i] > x[v] ) ) {
	    v = i;
	}
    }
    
    /* Vertices may be duplicate, we have to go to nearest different in each direction */
    /* preceding */
    next = v - 1;
    while ( 1 ) {
	if ( next < start ) 
	    next = end - 1; 

        if( !epsilonEqual(x[next], x[v], EPSILON) 
            || !epsilonEqual(y[next], y[v], EPSILON) )
	    break;

	if ( next == v ) /* So we cannot get into endless loop */
	    break;

	next--;
    }
	    
    dx0 = x[next] - x[v];
    dy0 = y[next] - y[v];
    
    
    /* following */
    next = v + 1;
    while ( 1 ) {
	if ( next >= end ) 
	    next = start; 

        if ( !epsilonEqual(x[next], x[v], EPSILON) 
             || !epsilonEqual(y[next], y[v], EPSILON) )
	    break;

	if ( next == v ) /* So we cannot get into endless loop */
	    break;

	next++;
    }
    dx1 = x[next] - x[v];
    dy1 = y[next] - y[v];

    area2 = dx1 * dy0 - dx0 * dy1;
    
    if ( area2 > 0 )      /* CCW */
	return -1;
    else if ( area2 < 0 )  /* CW */
	return 1;
    
    return 0; /* error */
}

/************************************************************************/
/*                          PointInRing()                               */
/*                                                                      */
/*      return: 1 point is inside the ring                              */
/*              0 point is outside the ring                             */
/*                                                                      */
/*              for point exactly on the boundary it returns 0 or 1     */
/************************************************************************/

int PointInRing ( SHPObject *Shape, int ring, double x, double y ) 
{
    int    i, start, end, c = 0;
    double *xp, *yp; 

    if ( ring < 0 || ring >= Shape->nParts ) return ( 0 );

    xp = Shape->padfX;
    yp = Shape->padfY;

    RingStartEnd ( Shape, ring, &start, &end );

    /* This code was originaly written by Randolph Franklin, here it is slightly modified. */
    for (i = start; i < end; i++) {
        if ( ( ( ( yp[i] <= y ) && ( y < yp[i+1] ) ) || 
               ( ( yp[i+1] <= y ) && ( y < yp[i] ) ) ) &&
             ( x < (xp[i+1] - xp[i]) * (y - yp[i]) / (yp[i+1] - yp[i]) + xp[i] )
	     ) 
	{
            c = !c;
	}
    }
    return c;
}
    
/************************************************************************/
/*                        CreateLinearRing                              */
/*                                                                      */
/************************************************************************/
OGRLinearRing * CreateLinearRing ( SHPObject *psShape, int ring )
{
    OGRLinearRing *poRing;
    int nRingStart, nRingEnd, nRingPoints;

    poRing = new OGRLinearRing();

    RingStartEnd ( psShape, ring, &nRingStart, &nRingEnd );

    nRingPoints = nRingEnd - nRingStart + 1;

    poRing->setPoints( nRingPoints, psShape->padfX + nRingStart, 
	       psShape->padfY + nRingStart, psShape->padfZ + nRingStart );

    return ( poRing );
}
    
/************************************************************************/
/*                          SHPReadOGRObject()                          */
/*                                                                      */
/*      Read an item in a shapefile, and translate to OGR geometry      */
/*      representation.                                                 */
/************************************************************************/

OGRGeometry *SHPReadOGRObject( SHPHandle hSHP, int iShape )

{
    SHPObject   *psShape;
    OGRGeometry *poOGR = NULL;

    psShape = SHPReadObject( hSHP, iShape );

    if( psShape == NULL )
        return NULL;

/* -------------------------------------------------------------------- */
/*      Point.                                                          */
/* -------------------------------------------------------------------- */
    else if( psShape->nSHPType == SHPT_POINT
             || psShape->nSHPType == SHPT_POINTM
             || psShape->nSHPType == SHPT_POINTZ )
    {
        poOGR = new OGRPoint( psShape->padfX[0], psShape->padfY[0],
                              psShape->padfZ[0] );
        if( psShape->nSHPType == SHPT_POINT )
            poOGR->setCoordinateDimension( 2 );
    }

/* -------------------------------------------------------------------- */
/*      Multipoint.                                                     */
/* -------------------------------------------------------------------- */
    else if( psShape->nSHPType == SHPT_MULTIPOINT
             || psShape->nSHPType == SHPT_MULTIPOINTM
             || psShape->nSHPType == SHPT_MULTIPOINTZ )
    {
        OGRMultiPoint *poOGRMPoint = new OGRMultiPoint();
        int             i;

        for( i = 0; i < psShape->nVertices; i++ )
        {
            OGRPoint    *poPoint;

            poPoint = new OGRPoint( psShape->padfX[i], psShape->padfY[i],
                                    psShape->padfZ[i] );
            
            poOGRMPoint->addGeometry( poPoint );

            delete poPoint;
        }
        
        poOGR = poOGRMPoint;

        if( psShape->nSHPType == SHPT_MULTIPOINT )
            poOGR->setCoordinateDimension( 2 );
    }

/* -------------------------------------------------------------------- */
/*      Arc (LineString)                                                */
/*                                                                      */
/*      I am ignoring parts though they can apply to arcs as well.      */
/* -------------------------------------------------------------------- */
    else if( psShape->nSHPType == SHPT_ARC
             || psShape->nSHPType == SHPT_ARCM
             || psShape->nSHPType == SHPT_ARCZ )
    {
        if( psShape->nParts == 1 )
        {
            OGRLineString *poOGRLine = new OGRLineString();

            poOGRLine->setPoints( psShape->nVertices,
                                  psShape->padfX, psShape->padfY, psShape->padfZ );
        
            poOGR = poOGRLine;
        }
        else
        {
            int iRing;
            OGRMultiLineString *poOGRMulti;
        
            poOGR = poOGRMulti = new OGRMultiLineString();
            
            for( iRing = 0; iRing < psShape->nParts; iRing++ )
            {
                OGRLineString   *poLine;
                int     nRingPoints;
                int     nRingStart;

                poLine = new OGRLineString();

                if( psShape->panPartStart == NULL )
                {
                    nRingPoints = psShape->nVertices;
                    nRingStart = 0;
                }
                else
                {

                    if( iRing == psShape->nParts - 1 )
                        nRingPoints =
                            psShape->nVertices - psShape->panPartStart[iRing];
                    else
                        nRingPoints = psShape->panPartStart[iRing+1]
                            - psShape->panPartStart[iRing];
                    nRingStart = psShape->panPartStart[iRing];
                }
            
                poLine->setPoints( nRingPoints, 
                                   psShape->padfX + nRingStart,
                                   psShape->padfY + nRingStart,
                                   psShape->padfZ + nRingStart );

                poOGRMulti->addGeometryDirectly( poLine );
            }
        }

        if( psShape->nSHPType == SHPT_ARC )
            poOGR->setCoordinateDimension( 2 );
    }

/* -------------------------------------------------------------------- */
/*      Polygon                                                         */
/*                                                                      */
/*      For now we assume the first ring is an outer ring, and          */
/*      everything else is an inner ring.  This must smarten up in      */
/*      the future.                                                     */
/* -------------------------------------------------------------------- */
    else if( psShape->nSHPType == SHPT_POLYGON
             || psShape->nSHPType == SHPT_POLYGONM
             || psShape->nSHPType == SHPT_POLYGONZ )
    {
        int     iRing;
        
	if ( psShape->nParts == 1 ) { /* Surely outer ring */
            OGRPolygon *poOGRPoly;
	    OGRLinearRing       *poRing;

	    poOGR = poOGRPoly = new OGRPolygon();
	    poRing = CreateLinearRing ( psShape, 0 );
	    poOGRPoly->addRingDirectly( poRing );
	} else {
	    int nOuter = 0; /* Number of outer rings */ 
	    int *direction; /* ring direction (1 CW,outer, -1 CCW, inner) */ 
	    int *outer;     /* list of outer rings */ 
	    int *outside;   /* outer ring index for inner rings, -1 for outer rings */ 

	    direction = (int *) CPLMalloc( psShape->nParts * sizeof(int) );
	    outer = (int *) CPLMalloc( psShape->nParts * sizeof(int) );
	    outside = (int *) CPLMalloc( psShape->nParts * sizeof(int) );
	    
	    /* First distinguish outer from inner rings */
	    for( iRing = 0; iRing < psShape->nParts; iRing++ ) {
		direction[iRing] = RingDirection ( psShape, iRing);
		if ( direction[iRing] == 1 ) {
		    outer[nOuter] = iRing;
		    nOuter++;
		}
		    
		outside[iRing] = -1;
	    }
	    
	    /* Find for each inner ring outer ring it is within */

	    /* Note: According to specification, rings can touch by one vertex, but not along segment. */
	    /* It may happen that inner ring touches the outer and PointInRing() retuirns 0 (outside) */
	    /* because of representation error. Because of that, cycle through all vertices of inner */
	    /* ring is used, until outer ring is found. Of course, this may fail as well, but probability */
	    /* is lower. We could use also points on inner segments or centroid. */ 
	    /* In theory, it may also happen, that vertex touching outer ring falls outside its outer */
	    /* ring and inside another (incorrect). */ 
	    /* The problem is how to find a point on inner ring which doesn't lie on outer ring. */
	    for( iRing = 0; iRing < psShape->nParts; iRing++ ) { /* cycle through inner rings */
		int  start, end;
		
		if ( direction[iRing] != -1 ) /* is not inner ring */
		    continue;
		
		RingStartEnd ( psShape, iRing, &start, &end );

		for ( int vert = start; vert < end; vert++ ) { /* cycle through inner ring's vertices */
		    double x, y;
		    
		    x = psShape->padfX[vert];
		    y = psShape->padfY[vert];

		    for( int oRing = 0; oRing < psShape->nParts; oRing++ ) { /* outer rings */
			int inside;
			    
			if ( direction[oRing] != 1 ) /* not outer */ 
			    continue;
			    
			inside = PointInRing ( psShape, oRing, x, y);

			if ( inside ) {
			    outside[iRing] = oRing;
			    break;
			}
		    }
		    if ( outside[iRing] >= 0 ) /* outer ring found */
			break;
	        }
	    }

            /* promote any unassigned inner rings to be outside rings */
            
            for ( iRing = 0; iRing < psShape->nParts; iRing++ ) { /* outer rings */
                if( direction[iRing] != 1 && outside[iRing] == -1 )
                {
                    direction[iRing] = 1; /* this isn't exactly true! */
                    outer[nOuter++] = iRing;
                }
            }

	    if ( nOuter == 1 ) { /* One outer ring and more inner rings */
		OGRPolygon    *poOGRPoly;
	        OGRLinearRing *poRing;

		/* Outer */
	        poOGR = poOGRPoly = new OGRPolygon();
		poRing = CreateLinearRing ( psShape, outer[0] );
	        poOGRPoly->addRingDirectly( poRing );

		/* Inner */
		for( iRing = 0; iRing < psShape->nParts; iRing++ ) {
		    if ( direction[iRing] == -1 ) {
			poRing = CreateLinearRing ( psShape, iRing );
			poOGRPoly->addRingDirectly( poRing );
		    }
		}
	    } else { 
		OGRGeometryCollection *poOGRGeCo;

		poOGR = poOGRGeCo = new OGRMultiPolygon();

	        for ( iRing = 0; iRing < nOuter; iRing++ ) { /* outer rings */
		    int oRing; 

		    OGRPolygon    *poOGRPoly;
		    OGRLinearRing *poRing;

		    oRing = outer[iRing];

		    /* Outer */
		    poOGRPoly = new OGRPolygon();
		    poRing = CreateLinearRing ( psShape, oRing );
		    poOGRPoly->addRingDirectly( poRing );

		    /* Inner */
		    for( int iRing2 = 0; iRing2 < psShape->nParts; iRing2++ ) {
			if ( outside[iRing2] == oRing ) {
			    poRing = CreateLinearRing ( psShape, iRing2 );
			    poOGRPoly->addRingDirectly( poRing );
			}
		    }
                    poOGRGeCo->addGeometryDirectly (poOGRPoly);
		}
	    }
            CPLFree( direction );
            CPLFree( outer );
            CPLFree( outside );
	}

        if( psShape->nSHPType == SHPT_POLYGON )
            poOGR->setCoordinateDimension( 2 );
    }

/* -------------------------------------------------------------------- */
/*      Otherwise for now we just ignore the object.  Eventually we     */
/*      should implement multipatch.                                    */
/* -------------------------------------------------------------------- */
    else
    {
        if( psShape->nSHPType != SHPT_NULL )
            CPLDebug( "OGR", "Unsupported shape type in SHPReadOGRObject()" );
        /* nothing returned */
    }
    
/* -------------------------------------------------------------------- */
/*      Cleanup shape, and set feature id.                              */
/* -------------------------------------------------------------------- */
    SHPDestroyObject( psShape );

    return poOGR;
}

/************************************************************************/
/*                         SHPWriteOGRObject()                          */
/************************************************************************/

OGRErr SHPWriteOGRObject( SHPHandle hSHP, int iShape, OGRGeometry *poGeom )

{
/* ==================================================================== */
/*      Write "shape" with no geometry.                                 */
/* ==================================================================== */
    if( poGeom == NULL )
    {
        SHPObject       *psShape;

        psShape = SHPCreateSimpleObject( SHPT_NULL, 0, NULL, NULL, NULL );
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );
    }

/* ==================================================================== */
/*      Write point geometry.                                           */
/* ==================================================================== */
    else if( hSHP->nShapeType == SHPT_POINT
             || hSHP->nShapeType == SHPT_POINTM
             || hSHP->nShapeType == SHPT_POINTZ )
    {
        SHPObject       *psShape;
        OGRPoint        *poPoint = (OGRPoint *) poGeom;
        double          dfX, dfY, dfZ = 0;

        if( poGeom->getGeometryType() != wkbPoint
            && poGeom->getGeometryType() != wkbPoint25D )        
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to write non-point (%s) geometry to"
                      " point shapefile.",
                      poGeom->getGeometryName() );

            return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
        }

        dfX = poPoint->getX();
        dfY = poPoint->getY();
        dfZ = poPoint->getZ();
        
        psShape = SHPCreateSimpleObject( hSHP->nShapeType, 1,
                                         &dfX, &dfY, &dfZ );
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );
    }
/* ==================================================================== */
/*      MultiPoint.                                                     */
/* ==================================================================== */
    else if( hSHP->nShapeType == SHPT_MULTIPOINT
             || hSHP->nShapeType == SHPT_MULTIPOINTM
             || hSHP->nShapeType == SHPT_MULTIPOINTZ )
    {
        OGRMultiPoint   *poMP = (OGRMultiPoint *) poGeom;
        double          *padfX, *padfY, *padfZ;
        int             iPoint;
        SHPObject       *psShape;

        if( wkbFlatten(poGeom->getGeometryType()) != wkbMultiPoint )
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to write non-multipoint (%s) geometry to "
                      "multipoint shapefile.",
                      poGeom->getGeometryName() );

            return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
        }

        padfX = (double *) CPLMalloc(sizeof(double)*poMP->getNumGeometries());
        padfY = (double *) CPLMalloc(sizeof(double)*poMP->getNumGeometries());
        padfZ = (double *) CPLCalloc(sizeof(double),poMP->getNumGeometries());

        for( iPoint = 0; iPoint < poMP->getNumGeometries(); iPoint++ )
        {
            OGRPoint    *poPoint = (OGRPoint *) poMP->getGeometryRef(iPoint);
            
            padfX[iPoint] = poPoint->getX();
            padfY[iPoint] = poPoint->getY();
            padfZ[iPoint] = poPoint->getZ();
        }

        psShape = SHPCreateSimpleObject( hSHP->nShapeType,
                                         poMP->getNumGeometries(),
                                         padfX, padfY, padfZ );
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );
        
        CPLFree( padfX );
        CPLFree( padfY );
        CPLFree( padfZ );
    }

/* ==================================================================== */
/*      Arcs from simple line strings.                                  */
/* ==================================================================== */
    else if( (hSHP->nShapeType == SHPT_ARC
              || hSHP->nShapeType == SHPT_ARCM
              || hSHP->nShapeType == SHPT_ARCZ)
             && wkbFlatten(poGeom->getGeometryType()) == wkbLineString )
    {
        OGRLineString   *poArc = (OGRLineString *) poGeom;
        double          *padfX, *padfY, *padfZ;
        int             iPoint;
        SHPObject       *psShape;

        if( poGeom->getGeometryType() != wkbLineString
            && poGeom->getGeometryType() != wkbLineString25D )
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to write non-linestring (%s) geometry to "
                      "ARC type shapefile.",
                      poGeom->getGeometryName() );

            return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
        }

        padfX = (double *) CPLMalloc(sizeof(double)*poArc->getNumPoints());
        padfY = (double *) CPLMalloc(sizeof(double)*poArc->getNumPoints());
        padfZ = (double *) CPLCalloc(sizeof(double),poArc->getNumPoints());

        for( iPoint = 0; iPoint < poArc->getNumPoints(); iPoint++ )
        {
            padfX[iPoint] = poArc->getX( iPoint );
            padfY[iPoint] = poArc->getY( iPoint );
            padfZ[iPoint] = poArc->getZ( iPoint );
        }

        psShape = SHPCreateSimpleObject( hSHP->nShapeType,
                                         poArc->getNumPoints(),
                                         padfX, padfY, padfZ );
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );
        
        CPLFree( padfX );
        CPLFree( padfY );
        CPLFree( padfZ );
    }
/* ==================================================================== */
/*      Arcs - Try to treat as MultiLineString.                         */
/* ==================================================================== */
    else if( hSHP->nShapeType == SHPT_ARC
             || hSHP->nShapeType == SHPT_ARCM
             || hSHP->nShapeType == SHPT_ARCZ )
    {
        OGRMultiLineString *poML;
        double          *padfX=NULL, *padfY=NULL, *padfZ=NULL;
        int             iGeom, iPoint, nPointCount = 0;
        SHPObject       *psShape;
        int             *panRingStart;

        poML = (OGRMultiLineString *) 
            OGRGeometryFactory::forceToMultiLineString( poGeom->clone() );

        if( wkbFlatten(poML->getGeometryType()) != wkbMultiLineString )
        {
            delete poML;
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to write non-linestring (%s) geometry to "
                      "ARC type shapefile.",
                      poGeom->getGeometryName() );

            return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
        }

        panRingStart = (int *) 
            CPLMalloc(sizeof(int) * poML->getNumGeometries());

        for( iGeom = 0; iGeom < poML->getNumGeometries(); iGeom++ )
        {
            OGRLineString *poArc = (OGRLineString *)
                poML->getGeometryRef(iGeom);
            int nNewPoints = poArc->getNumPoints();

            panRingStart[iGeom] = nPointCount;

            padfX = (double *) 
                CPLRealloc( padfX, sizeof(double)*(nNewPoints+nPointCount) );
            padfY = (double *) 
                CPLRealloc( padfY, sizeof(double)*(nNewPoints+nPointCount) );
            padfZ = (double *) 
                CPLRealloc( padfZ, sizeof(double)*(nNewPoints+nPointCount) );

            for( iPoint = 0; iPoint < nNewPoints; iPoint++ )
            {
                padfX[nPointCount] = poArc->getX( iPoint );
                padfY[nPointCount] = poArc->getY( iPoint );
                padfZ[nPointCount] = poArc->getZ( iPoint );
                nPointCount++;
            }
        }
        
        psShape = SHPCreateObject( hSHP->nShapeType, iShape, 
                                   poML->getNumGeometries(), 
                                   panRingStart, NULL,
                                   nPointCount, padfX, padfY, padfZ, NULL);
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );

        CPLFree( padfX );
        CPLFree( padfY );
        CPLFree( padfZ );

        delete poML;
    }

/* ==================================================================== */
/*      Polygons/MultiPolygons                                          */
/* ==================================================================== */
    else if( hSHP->nShapeType == SHPT_POLYGON
             || hSHP->nShapeType == SHPT_POLYGONM
             || hSHP->nShapeType == SHPT_POLYGONZ )
    {
        OGRPolygon      *poPoly;
        OGRLinearRing   *poRing, **papoRings=NULL;
        double          *padfX=NULL, *padfY=NULL, *padfZ=NULL;
        int             iPoint, iRing, nRings, nVertex=0, *panRingStart;
        SHPObject       *psShape;

        /* Collect list of rings */

        if( wkbFlatten(poGeom->getGeometryType()) == wkbPolygon )
        {
            poPoly =  (OGRPolygon *) poGeom;

            nRings = poPoly->getNumInteriorRings()+1;
            papoRings = (OGRLinearRing **) CPLMalloc(sizeof(void*)*nRings);
            for( iRing = 0; iRing < nRings; iRing++ )
            {
                if( iRing == 0 )
                    papoRings[iRing] = poPoly->getExteriorRing();
                else
                    papoRings[iRing] = poPoly->getInteriorRing( iRing-1 );
            }
        }
        else if( wkbFlatten(poGeom->getGeometryType()) == wkbMultiPolygon
                 || wkbFlatten(poGeom->getGeometryType()) 
                                                == wkbGeometryCollection )
        {
            OGRGeometryCollection *poGC = (OGRGeometryCollection *) poGeom;
            int         iGeom;

            nRings = 0;
            for( iGeom=0; iGeom < poGC->getNumGeometries(); iGeom++ )
            {
                poPoly =  (OGRPolygon *) poGC->getGeometryRef( iGeom );

                if( wkbFlatten(poPoly->getGeometryType()) != wkbPolygon )
                {
                    CPLFree( papoRings );
                    CPLError( CE_Failure, CPLE_AppDefined,
                              "Attempt to write non-polygon (%s) geometry to "
                              " type shapefile.",
                              poGeom->getGeometryName() );

                    return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
                }

                papoRings = (OGRLinearRing **) CPLRealloc(papoRings, 
                     sizeof(void*) * (nRings+poPoly->getNumInteriorRings()+1));
                for( iRing = 0; 
                     iRing < poPoly->getNumInteriorRings()+1; 
                     iRing++ )
                {
                    if( iRing == 0 )
                        papoRings[nRings+iRing] = poPoly->getExteriorRing();
                    else
                        papoRings[nRings+iRing] = 
                            poPoly->getInteriorRing( iRing-1 );
                }
                nRings += poPoly->getNumInteriorRings()+1;
            }
        }
        else 
        {
            CPLError( CE_Failure, CPLE_AppDefined,
                      "Attempt to write non-polygon (%s) geometry to "
                      " type shapefile.",
                      poGeom->getGeometryName() );

            return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
        }

        /* count vertices */
        nVertex = 0;
        for( iRing = 0; iRing < nRings; iRing++ )
            nVertex += papoRings[iRing]->getNumPoints();

        panRingStart = (int *) CPLMalloc(sizeof(int) * nRings);
        padfX = (double *) CPLMalloc(sizeof(double)*nVertex);
        padfY = (double *) CPLMalloc(sizeof(double)*nVertex);
        padfZ = (double *) CPLMalloc(sizeof(double)*nVertex);

        /* collect vertices */
        nVertex = 0;
        for( iRing = 0; iRing < nRings; iRing++ )
        {
            poRing = papoRings[iRing];
            panRingStart[iRing] = nVertex;

            for( iPoint = 0; iPoint < poRing->getNumPoints(); iPoint++ )
            {
                padfX[nVertex] = poRing->getX( iPoint );
                padfY[nVertex] = poRing->getY( iPoint );
                padfZ[nVertex] = poRing->getZ( iPoint );
                nVertex++;
            }
        }

        psShape = SHPCreateObject( hSHP->nShapeType, iShape, nRings,
                                   panRingStart, NULL,
                                   nVertex, padfX, padfY, padfZ, NULL );
        SHPRewindObject( hSHP, psShape );
        SHPWriteObject( hSHP, iShape, psShape );
        SHPDestroyObject( psShape );
        
        CPLFree( papoRings );
        CPLFree( panRingStart );
        CPLFree( padfX );
        CPLFree( padfY );
        CPLFree( padfZ );
    }
    else
    {
        /* do nothing for multipatch */
        return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
    }

    return OGRERR_NONE;
}

/************************************************************************/
/*                       SHPReadOGRFeatureDefn()                        */
/************************************************************************/

OGRFeatureDefn *SHPReadOGRFeatureDefn( const char * pszName,
                                       SHPHandle hSHP, DBFHandle hDBF )

{
    OGRFeatureDefn      *poDefn = new OGRFeatureDefn( pszName );
    int                 iField;

    for( iField = 0; 
         hDBF != NULL && iField < DBFGetFieldCount( hDBF ); 
         iField++ )
    {
        char            szFieldName[20];
        int             nWidth, nPrecision;
        DBFFieldType    eDBFType;
        OGRFieldDefn    oField("", OFTInteger);

        eDBFType = DBFGetFieldInfo( hDBF, iField, szFieldName,
                                    &nWidth, &nPrecision );

        oField.SetName( szFieldName );
        oField.SetWidth( nWidth );
        oField.SetPrecision( nPrecision );

        if( eDBFType == FTDouble )
            oField.SetType( OFTReal );
        else if( eDBFType == FTInteger )
            oField.SetType( OFTInteger );
        else
            oField.SetType( OFTString );

        poDefn->AddFieldDefn( &oField );
    }

    if( hSHP == NULL )
        poDefn->SetGeomType( wkbNone );
    else
    {
        switch( hSHP->nShapeType )
        {
          case SHPT_POINT:
          case SHPT_POINTM:
            poDefn->SetGeomType( wkbPoint );
            break;

          case SHPT_POINTZ:
            poDefn->SetGeomType( wkbPoint25D );
            break;

          case SHPT_ARC:
          case SHPT_ARCM:
            poDefn->SetGeomType( wkbLineString );
            break;

          case SHPT_ARCZ:
            poDefn->SetGeomType( wkbLineString25D );
            break;

          case SHPT_MULTIPOINT:
          case SHPT_MULTIPOINTM:
            poDefn->SetGeomType( wkbMultiPoint );
            break;

          case SHPT_MULTIPOINTZ:
            poDefn->SetGeomType( wkbMultiPoint25D );
            break;

          case SHPT_POLYGON:
          case SHPT_POLYGONM:
            poDefn->SetGeomType( wkbPolygon );
            break;

          case SHPT_POLYGONZ:
            poDefn->SetGeomType( wkbPolygon25D );
            break;
            
        }
    }

    return poDefn;
}

/************************************************************************/
/*                         SHPReadOGRFeature()                          */
/************************************************************************/

OGRFeature *SHPReadOGRFeature( SHPHandle hSHP, DBFHandle hDBF,
                               OGRFeatureDefn * poDefn, int iShape )

{
    if( iShape < 0 
        || (hSHP != NULL && iShape >= hSHP->nRecords)
        || (hDBF != NULL && iShape >= hDBF->nRecords) )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "Attempt to read shape with feature id (%d) out of available"
                  " range.", iShape );
        return NULL;
    }

    OGRFeature  *poFeature = new OGRFeature( poDefn );

    if( hSHP != NULL )
        poFeature->SetGeometryDirectly( SHPReadOGRObject( hSHP, iShape ) );

    for( int iField = 0; iField < poDefn->GetFieldCount(); iField++ )
    {
        // Skip null fields.
        if( DBFIsAttributeNULL( hDBF, iShape, iField ) )
            continue;

        switch( poDefn->GetFieldDefn(iField)->GetType() )
        {
          case OFTString:
            poFeature->SetField( iField,
                                 DBFReadStringAttribute( hDBF, iShape,
                                                         iField ) );
            break;

          case OFTInteger:
            poFeature->SetField( iField,
                                 DBFReadIntegerAttribute( hDBF, iShape,
                                                          iField ) );
            break;

          case OFTReal:
            poFeature->SetField( iField,
                                 DBFReadDoubleAttribute( hDBF, iShape,
                                                         iField ) );
            break;

          default:
            CPLAssert( FALSE );
        }
    }

    if( poFeature != NULL )
        poFeature->SetFID( iShape );

    return( poFeature );
}

/************************************************************************/
/*                         SHPWriteOGRFeature()                         */
/*                                                                      */
/*      Write to an existing feature in a shapefile, or create a new    */
/*      feature.                                                        */
/************************************************************************/

OGRErr SHPWriteOGRFeature( SHPHandle hSHP, DBFHandle hDBF,
                           OGRFeatureDefn * poDefn, 
                           OGRFeature * poFeature )

{
#ifdef notdef
/* -------------------------------------------------------------------- */
/*      Don't write objects with missing geometry.                      */
/* -------------------------------------------------------------------- */
    if( poFeature->GetGeometryRef() == NULL && hSHP != NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "Attempt to write feature without geometry not supported"
                  " for shapefile driver." );
        
        return OGRERR_UNSUPPORTED_GEOMETRY_TYPE;
    }
#endif

/* -------------------------------------------------------------------- */
/*      Write the geometry.                                             */
/* -------------------------------------------------------------------- */
    OGRErr      eErr;

    if( hSHP != NULL )
    {
        eErr = SHPWriteOGRObject( hSHP, poFeature->GetFID(),
                                  poFeature->GetGeometryRef() );
        if( eErr != OGRERR_NONE )
            return eErr;
    }
    
/* -------------------------------------------------------------------- */
/*      If this is a new feature, establish it's feature id.            */
/* -------------------------------------------------------------------- */
    if( poFeature->GetFID() == OGRNullFID )
        poFeature->SetFID( DBFGetRecordCount( hDBF ) );

/* -------------------------------------------------------------------- */
/*      If this is the first feature to be written, verify that we      */
/*      have at least one attribute in the DBF file.  If not, create    */
/*      a dummy FID attribute to satisfy the requirement that there     */
/*      be at least one attribute.                                      */
/* -------------------------------------------------------------------- */
    if( DBFGetRecordCount( hDBF ) == 0 && DBFGetFieldCount( hDBF ) == 0 )
    {
        CPLDebug( "OGR", 
               "Created dummy FID field for shapefile since schema is empty.");
        DBFAddField( hDBF, "FID", FTInteger, 11, 0 );
    }

/* -------------------------------------------------------------------- */
/*      Write out dummy field value if it exists.                       */
/* -------------------------------------------------------------------- */
    if( DBFGetFieldCount( hDBF ) == 1 && poDefn->GetFieldCount() == 0 )
    {
        DBFWriteIntegerAttribute( hDBF, poFeature->GetFID(), 0, 
                                  poFeature->GetFID() );
    }

/* -------------------------------------------------------------------- */
/*      Write all the fields.                                           */
/* -------------------------------------------------------------------- */
    for( int iField = 0; iField < poDefn->GetFieldCount(); iField++ )
    {
        if( !poFeature->IsFieldSet( iField ) )
        {
            DBFWriteNULLAttribute( hDBF, poFeature->GetFID(), iField );
            continue;
        }

        switch( poDefn->GetFieldDefn(iField)->GetType() )
        {
          case OFTString:
            DBFWriteStringAttribute( hDBF, poFeature->GetFID(), iField, 
                                     poFeature->GetFieldAsString( iField ));
            break;

          case OFTInteger:
            DBFWriteIntegerAttribute( hDBF, poFeature->GetFID(), iField, 
                                      poFeature->GetFieldAsInteger(iField) );
            break;

          case OFTReal:
            DBFWriteDoubleAttribute( hDBF, poFeature->GetFID(), iField, 
                                     poFeature->GetFieldAsDouble(iField) );
            break;

          default:
            CPLAssert( FALSE );
        }
    }

    return OGRERR_NONE;
}


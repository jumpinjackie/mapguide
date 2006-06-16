/**********************************************************************
 * $Id: gmlhandler.cpp,v 1.6 2003/06/26 19:42:31 warmerda Exp $
 *
 * Project:  GML Reader
 * Purpose:  Implementation of GMLHandler class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 **********************************************************************
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
 **********************************************************************
 *
 * $Log: gmlhandler.cpp,v $
 * Revision 1.6  2003/06/26 19:42:31  warmerda
 * Was starting a new feature in the middle of geometry, see place.xml
 *
 * Revision 1.5  2003/05/21 03:48:35  warmerda
 * Expand tabs
 *
 * Revision 1.4  2002/04/01 17:51:33  warmerda
 * allow geometry tags without gml: qualifier
 *
 * Revision 1.3  2002/01/25 20:38:49  warmerda
 * report fatal errors, add to geometry type list
 *
 * Revision 1.2  2002/01/24 17:37:57  warmerda
 * added geometry support, rewrite TrString use
 *
 * Revision 1.1  2002/01/04 19:46:30  warmerda
 * New
 *
 *
 **********************************************************************/

#include <ctype.h>
#include "gmlreaderp.h"
#include "cpl_conv.h"

#define MAX_TOKEN_SIZE  1000

/************************************************************************/
/*                             GMLHandler()                             */
/************************************************************************/

GMLHandler::GMLHandler( GMLReader *poReader )

{
    m_poReader = poReader;
    m_pszCurField = NULL;
    m_pszGeometry = NULL;
}

/************************************************************************/
/*                            ~GMLHandler()                             */
/************************************************************************/

GMLHandler::~GMLHandler()

{
    CPLFree( m_pszCurField );
    CPLFree( m_pszGeometry );
}


/************************************************************************/
/*                            startElement()                            */
/************************************************************************/

void GMLHandler::startElement(const XMLCh* const    uri,
                              const XMLCh* const    localname,
                              const XMLCh* const    qname,
                              const Attributes& attrs )

{
    char        szElementName[MAX_TOKEN_SIZE];
    GMLReadState *poState = m_poReader->GetState();

    tr_strcpy( szElementName, qname );

/* -------------------------------------------------------------------- */
/*      If we are in the midst of collecting a feature attribute        */
/*      value, then this must be a complex attribute which we don't     */
/*      try to collect for now, so just terminate the field             */
/*      collection.                                                     */
/* -------------------------------------------------------------------- */
    if( m_pszCurField != NULL )
    {
        CPLFree( m_pszCurField );
        m_pszCurField = NULL;
    }

/* -------------------------------------------------------------------- */
/*      If we are collecting geometry, or if we determine this is a     */
/*      geometry element then append to the geometry info.              */
/* -------------------------------------------------------------------- */
    if( m_pszGeometry != NULL 
        || IsGeometryElement( szElementName ) )
    {
        int nLNLen = tr_strlen( localname );
        int nGLen;

        /* should save attributes too! */

        if( m_pszGeometry == NULL )
        {
            m_pszGeometry = (char *) CPLCalloc(1,nLNLen+4);
            m_nGeometryDepth = poState->m_nPathLength;
            nGLen = 0;
        }
        else    
        {
            nGLen = strlen(m_pszGeometry);
            m_pszGeometry = (char *) 
                CPLRealloc( m_pszGeometry, nGLen+nLNLen+4);
        }

        strcat( m_pszGeometry+nGLen, "<" );
        tr_strcpy( m_pszGeometry+nGLen+1, localname );
        strcat( m_pszGeometry+nGLen+nLNLen, ">" );
    }
    
/* -------------------------------------------------------------------- */
/*      Is it a feature?  If so push a whole new state, and return.     */
/* -------------------------------------------------------------------- */
    else if( m_poReader->IsFeatureElement( szElementName ) )
    {
        m_poReader->PushFeature( szElementName, attrs );
        return;
    }

/* -------------------------------------------------------------------- */
/*      If it is (or at least potentially is) a simple attribute,       */
/*      then start collecting it.                                       */
/* -------------------------------------------------------------------- */
    else if( m_poReader->IsAttributeElement( szElementName ) )
    {
        CPLFree( m_pszCurField );
        m_pszCurField = CPLStrdup("");
    }

/* -------------------------------------------------------------------- */
/*      Push the element onto the current state's path.                 */
/* -------------------------------------------------------------------- */
    poState->PushPath( szElementName );
}

/************************************************************************/
/*                             endElement()                             */
/************************************************************************/
void GMLHandler::endElement(const   XMLCh* const    uri,
                            const   XMLCh* const    localname,
                            const   XMLCh* const    qname )

{
    char        szElementName[MAX_TOKEN_SIZE];
    GMLReadState *poState = m_poReader->GetState();

    tr_strcpy( szElementName, qname );

/* -------------------------------------------------------------------- */
/*      Is this closing off an attribute value?  We assume so if        */
/*      we are collecting an attribute value and got to this point.     */
/*      We don't bother validating that the closing tag matches the     */
/*      opening tag.                                                    */
/* -------------------------------------------------------------------- */
    if( m_pszCurField != NULL )
    {
        CPLAssert( poState->m_poFeature != NULL );
        
        m_poReader->SetFeatureProperty( szElementName, m_pszCurField );
        CPLFree( m_pszCurField );
        m_pszCurField = NULL;
    }

/* -------------------------------------------------------------------- */
/*      If we are collecting Geometry than store it, and consider if    */
/*      this is the end of the geometry.                                */
/* -------------------------------------------------------------------- */
    if( m_pszGeometry != NULL )
    {
        int nLNLen = tr_strlen( localname );
        int nGLen;

        nGLen = strlen(m_pszGeometry);
        m_pszGeometry = (char *) 
            CPLRealloc( m_pszGeometry, nGLen+nLNLen+4);

        strcat( m_pszGeometry+nGLen, "</" );
        tr_strcpy( m_pszGeometry+nGLen+2, localname );
        strcat( m_pszGeometry+nGLen+nLNLen, ">" );

        if( poState->m_nPathLength == m_nGeometryDepth+1 )
        {
            if( poState->m_poFeature != NULL )
                poState->m_poFeature->SetGeometryDirectly( m_pszGeometry );
            else
                CPLFree( m_pszGeometry );

            m_pszGeometry = NULL;
        }
    }

/* -------------------------------------------------------------------- */
/*      If we are collecting a feature, and this element tag matches    */
/*      element name for the class, then we have finished the           */
/*      feature, and we pop the feature read state.                     */
/* -------------------------------------------------------------------- */
    if( poState->m_poFeature != NULL
        && EQUAL(szElementName,
                 poState->m_poFeature->GetClass()->GetElementName()) )
    {
        m_poReader->PopState();
    }

/* -------------------------------------------------------------------- */
/*      Otherwise, we just pop the element off the local read states    */
/*      element stack.                                                  */
/* -------------------------------------------------------------------- */
    else
    {
        if( EQUAL(szElementName,poState->GetLastComponent()) )
            poState->PopPath();
        else
        {
            CPLAssert( FALSE );
        }
    }
}

/************************************************************************/
/*                             characters()                             */
/************************************************************************/

void GMLHandler::characters(const XMLCh* const chars_in,
                                const unsigned int length )

{
    const XMLCh *chars = chars_in;

    if( m_pszCurField != NULL )
    {
        int     nCurFieldLength = strlen(m_pszCurField);

        while( *chars == ' ' || *chars == 10 || *chars == 13 || *chars == '\t')
            chars++;
        
        m_pszCurField = (char *) 
            CPLRealloc( m_pszCurField, 
                        nCurFieldLength+tr_strlen(chars)+1 );
        tr_strcpy( m_pszCurField + nCurFieldLength, chars );
    }
    else if( m_pszGeometry != NULL )
    {
        int     nCurLength = strlen(m_pszGeometry);

        while( *chars == ' ' || *chars == 10 || *chars == 13 || *chars == '\t')
            chars++;
        
        m_pszGeometry = (char *) 
            CPLRealloc( m_pszGeometry, nCurLength+tr_strlen(chars)+1 );
        
        tr_strcpy( m_pszGeometry+nCurLength, chars );
    }
}

/************************************************************************/
/*                             fatalError()                             */
/************************************************************************/

void GMLHandler::fatalError( const SAXParseException &exception)

{
    char *pszErrorMessage;

    pszErrorMessage = tr_strdup( exception.getMessage() );
    CPLError( CE_Failure, CPLE_AppDefined, 
              "XML Parsing Error: %s\n", 
              pszErrorMessage );

    CPLFree( pszErrorMessage );
}

/************************************************************************/
/*                         IsGeometryElement()                          */
/************************************************************************/

int GMLHandler::IsGeometryElement( const char *pszElement )

{
    if( EQUALN(pszElement,"gml:",4) )
        pszElement += 4;

    return EQUAL(pszElement,"Polygon") 
        || EQUAL(pszElement,"MultiPolygon") 
        || EQUAL(pszElement,"MultiPoint") 
        || EQUAL(pszElement,"MultiLineString") 
        || EQUAL(pszElement,"GeometryCollection") 
        || EQUAL(pszElement,"Point") 
        || EQUAL(pszElement,"LineString");
}

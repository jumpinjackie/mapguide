/******************************************************************************
 * $Id: ogravcdatasource.cpp,v 1.1 2002/02/14 23:01:09 warmerda Exp $
 *
 * Project:  OGR
 * Purpose:  Implements OGRAVCDataSource class.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2002, Frank Warmerdam <warmerdam@pobox.com>
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
 * $Log: ogravcdatasource.cpp,v $
 * Revision 1.1  2002/02/14 23:01:09  warmerda
 * New
 *
 */

#include "ogr_avc.h"

CPL_CVSID("$Id: ogravcdatasource.cpp,v 1.1 2002/02/14 23:01:09 warmerda Exp $");

/************************************************************************/
/*                          OGRAVCDataSource()                          */
/************************************************************************/

OGRAVCDataSource::OGRAVCDataSource()

{
    poSRS = NULL;
}

/************************************************************************/
/*                         ~OGRAVCDataSource()                          */
/************************************************************************/

OGRAVCDataSource::~OGRAVCDataSource()

{
    if( poSRS )
        delete poSRS;
}

/************************************************************************/
/*                           GetSpatialRef()                            */
/************************************************************************/

OGRSpatialReference *OGRAVCDataSource::GetSpatialRef()

{
    return poSRS;
}

/************************************************************************/
/*                          GetCoverageName()                           */
/************************************************************************/

const char *OGRAVCDataSource::GetCoverageName()

{
    if( pszCoverageName == NULL )
        return "";
    else
        return pszCoverageName;
}

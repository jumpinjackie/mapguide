/******************************************************************************
 * $Id: ogravcbindriver.cpp,v 1.2 2002/02/14 14:36:05 warmerda Exp $
 *
 * Project:  OGR
 * Purpose:  OGRAVCBinDriver implementation (Arc/Info Binary Coverages)
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
 * $Log: ogravcbindriver.cpp,v $
 * Revision 1.2  2002/02/14 14:36:05  warmerda
 * don't indicate create ability
 *
 * Revision 1.1  2002/02/13 20:48:18  warmerda
 * New
 *
 */

#include "ogr_avc.h"

CPL_CVSID("$Id: ogravcbindriver.cpp,v 1.2 2002/02/14 14:36:05 warmerda Exp $");

/************************************************************************/
/*                          ~OGRAVCBinDriver()                          */
/************************************************************************/

OGRAVCBinDriver::~OGRAVCBinDriver()

{
}

/************************************************************************/
/*                              GetName()                               */
/************************************************************************/

const char *OGRAVCBinDriver::GetName()

{
    return "AVCBin";
}

/************************************************************************/
/*                                Open()                                */
/************************************************************************/

OGRDataSource *OGRAVCBinDriver::Open( const char * pszFilename,
                                      int bUpdate )

{
    OGRAVCBinDataSource	*poDS;

    if( bUpdate )
        return NULL;

    poDS = new OGRAVCBinDataSource();

    if( !poDS->Open( pszFilename, TRUE )
        || poDS->GetLayerCount() == 0 )
    {
        delete poDS;
        return NULL;
    }
    else
        return poDS;
}

/************************************************************************/
/*                           TestCapability()                           */
/************************************************************************/

int OGRAVCBinDriver::TestCapability( const char * pszCap )

{
    return FALSE;
}

/************************************************************************/
/*                           RegisterOGRAVC()                           */
/************************************************************************/

void RegisterOGRAVCBin()

{
    OGRSFDriverRegistrar::GetRegistrar()->RegisterDriver( 
        new OGRAVCBinDriver );
}

/******************************************************************************
 * $Id: ogrcomgeom_server.cpp,v 1.4 1999/05/20 14:54:55 warmerda Exp $
 *
 * Project:  OpenGIS Simple Features Reference Implementation
 * Purpose:  Geometry services COM infrastructure.
 * Author:   Frank Warmerdam, warmerda@home.com
 *
 ******************************************************************************
 * Copyright (c) 1999, Frank Warmerdam
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
 * $Log: ogrcomgeom_server.cpp,v $
 * Revision 1.4  1999/05/20 14:54:55  warmerda
 * started work on automation
 *
 * Revision 1.3  1999/05/17 14:43:10  warmerda
 * Added Polygon, linestring and curve support.  Changed IGeometryTmpl to
 * also include COM interface class as an argument.
 *
 * Revision 1.2  1999/05/14 13:28:38  warmerda
 * client and service now working for IPoint
 *
 * Revision 1.1  1999/05/13 19:49:01  warmerda
 * New
 *
 */

#define INITGUID
#define DBINITCONSTANTS

#include "ogrcomgeometry.h"
#include "sfclsid.h"
#include "sfiiddef.h"

/************************************************************************/
/*                         DllGetClassObject()                          */
/************************************************************************/

//
// Function exported from in-process server DLL that is called by COM to
// obtain an interface pointer to a particular class factory.
//
// __declspec(dllexport)

STDAPI
DllGetClassObject(REFCLSID rClsId,
                  REFIID rIID,
                  void **ppvInterface)
{
   HRESULT                  hResult;
   OGRComClassFactory       *pFactory;

   OGRComDebug( "startup", "In DllGetClassObject\n" );

   // Make sure we're not being asked for a class factory that we don't know about
   if (rClsId != CLSID_OGRComClassFactory) {
       OGRComDebug("failure",
           "COM is requesting a class other than CLSID_OGRCOMClassFactory\n");
      return E_FAIL;
   }

   // Instantiate the new class factory
   pFactory = new OGRComClassFactory;

   // Now obtain the requested interface
   hResult = pFactory->QueryInterface(rIID, ppvInterface);

   // Destroy the factory if the desired interface couldn't be obtained
   if (FAILED(hResult)) {
      delete pFactory;
      return hResult;
   }

   return NOERROR;
}


/************************************************************************/
/*                          DllCanUnloadNow()                           */
/************************************************************************/

//
// Function exported from in-process server DLL that is called by COM to
// determine if the server can be unloaded from memory.
//

STDAPI DllCanUnloadNow()
{
#ifdef notdef
   string strTemp;
   VerboseMsg("In DllGetClassObject\n");

   // Be verbose -- let people know what's going on
   strTemp = "   Server Lock Count:   ";
   strTemp += ULongToStr(Counters::GetLockCount());
   strTemp += "\n";
   VerboseMsg(strTemp);
   strTemp = "   Server Object Count:   ";
   strTemp += ULongToStr(Counters::GetObjectCount());
   strTemp += "\n";
   VerboseMsg(strTemp);

   // If the server lock count and the object count are both zero, then
   // the server can be unloaded
   return ((Counters::GetLockCount() == 0L &&
            Counters::GetObjectCount() == 0L) ? S_OK : S_FALSE);
#endif
   return S_FALSE;
}


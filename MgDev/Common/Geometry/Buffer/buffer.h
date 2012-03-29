//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: buffer.h.
//
// PURPOSE: Master include file for buffering library. This is the only header
//          file an application needs to explicitly include, and is designed to
//          facilitate the use of precompiled headers.
//
//------------------------------------------------------------------------------

#ifndef _BUFFER_H_
#define _BUFFER_H_

#define BOOL bool

#ifndef _WIN32
    #define FALSE false
    #define TRUE true
#endif

#include <math.h>

#include "GeometryCommon.h"


#include "oobject.h"
#include "opoint.h"
#include "oextent.h"
#include "ogeomtry.h"
#include "opsprimitive.h"
#include "opolygnp.h"
#include "olinep.h"
#include "opolygon.h"
#include "oppolygn.h"
#include "opline.h"
#include "oppolyln.h"
#include "ortree.h"
#include "oarray.h"
#include "bufraloc.h"
#include "progcbck.h"
#include "bufrsort.h"
#include "wingedge.h"
#include "vertaloc.h"
#include "wedgaloc.h"
#include "worklist.h"
#include "xordtree.h"
#include "sweepedg.h"
#include "sedgaloc.h"
#include "wedgearr.h"
#include "interprc.h"
#include "interlst.h"
#include "tuplearr.h"
#include "eventint.h"
#include "eventarr.h"
#include "orpplygn.h"
#include "bufrutil.h"
#include "planeswp.h"
#include "pobjbufr.h"
#include "plinbufr.h"
#include "plgnbufr.h"
#include "plgnsbak.h"
#include "ppgunion.h"
#include "BorderWalker.h"
#include "LatLonBorderWalker.h"
#include "GreatCircleBufferUtil.h"
#include "MgBuffer.h"
#include "progcbck.h"
#include "FloatTransform.h"

#endif

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

#ifndef _MGGEOMETRYCONSOLETEST_H_
#define _MGGEOMETRYCONSOLETEST_H_

#include <stdio.h>
#include <iostream>
#include <fstream>

#include "GctTestParameters.h"
#include "GctTestSupport.h"
#include "GctOsGeoTestFile.h"

/// \defgroup MgGeometryConsoleTest MgGeometryConsoleTest
/// \ingroup Geometry_UnitTest_classes
/// \{

INT32 GctTest1 (MgGctTestParameters& cmdLineParms);
INT32 GctTest2 (MgGctTestParameters& cmdLineParms);
INT32 GctTest3 (MgGctTestParameters& cmdLineParms);
INT32 GctTest4 (MgGctTestParameters& cmdLineParms);
INT32 GctTest5 (MgGctTestParameters& cmdLineParms);
INT32 GctTest6 (MgGctTestParameters& cmdLineParms);
INT32 GctTest7 (MgGctTestParameters& cmdLineParms);
INT32 GctTest8 (MgGctTestParameters& cmdLineParms);
INT32 GctTest9 (MgGctTestParameters& cmdLineParms);
INT32 GctTestA (MgGctTestParameters& cmdLineParms);
INT32 GctTestT (MgGctTestParameters& cmdLineParms);

// A structure used in the dictionary tests.  Essentially, this structure is
// used to hard code a list of dictionary entries by name.  The entry is
// typically fetched, deleted, and added back in to the dictionary.
struct MgDictionaryTestList
{
	wchar_t name [24];
	wchar_t cloneName [24];
	MgGuardDisposable* pointer;
};

/// \}


#endif //_MGGEOMETRYCONSOLETEST_H_


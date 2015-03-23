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

#include "GeometryCommon.h"
#include "Spatial/MathUtility.h"
#include "Spatial/SpatialUtility.h"
#include "GeometryConsoleTest.h"

//=========================================================================
// Construction, Destruction, and Assignment
MgOsGeoTestFile::MgOsGeoTestFile (const wchar_t* dataFilePath)
											:
									TcsCsvFileBase (false,14,18),
									Ok             (false),
									Status         ()
{
	bool firstIsLabels (false);

	std::wifstream inStrm (dataFilePath,std::ios_base::in);
	if (inStrm.is_open ())
	{
		Ok = ReadFromStream (inStrm,firstIsLabels,Status);        
		inStrm.close ();
	}
}
MgOsGeoTestFile::MgOsGeoTestFile (const char* dataFilePath)
											:
									TcsCsvFileBase (false,14,18),
									Ok             (false),
									Status         ()
{
	bool firstIsLabels (false);

	std::wifstream inStrm (dataFilePath,std::ios_base::in);
	if (inStrm.is_open ())
	{
		Ok = ReadFromStream (inStrm,firstIsLabels,Status);        
		inStrm.close ();
	}
}
MgOsGeoTestFile::MgOsGeoTestFile (const MgOsGeoTestFile& source)
											:
									TcsCsvFileBase (source),
									Ok             (source.Ok),
									Status         (source.Status)
{
}
MgOsGeoTestFile::~MgOsGeoTestFile (void)
{
	// Nothing to do here.
}
MgOsGeoTestFile& MgOsGeoTestFile::operator= (const MgOsGeoTestFile& rhs)
{
	if (&rhs != this)
	{
		TcsCsvFileBase::operator= (rhs);
		Ok     = rhs.Ok;
		Status = rhs.Status;
	}
	return *this;
}
//=========================================================================
// Operator Overrides
//=========================================================================
// Public Named Member Functions
bool MgOsGeoTestFile::IsComment (unsigned recordNbr)
{
	bool ok;
	bool isComment (false);

	const wchar_t* wcPtr;
	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,static_cast<short>(0),Status);
	if (ok)
	{
		wcPtr = fieldData.c_str ();
		isComment = (*wcPtr == L'#');
	}
	return isComment;
}
bool MgOsGeoTestFile::GetTestName (std::wstring& testName,unsigned recordNbr)
{
	bool ok (false);

	ok = GetField (testName,recordNbr,static_cast<short>(0),Status);
	return ok;
}
EcsTestMethod MgOsGeoTestFile::GetTestMethod (unsigned recordNbr)
{
	bool ok (false);
	EcsTestMethod rtnValue (testMthUnknown);

	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,1,Status);
	if (ok)
	{
		if (!CS_wcsicmp (fieldData.c_str (),L"CRS2D"))
		{
			rtnValue = testMthCrs2D;
		}
		else if (!CS_wcsicmp (fieldData.c_str (),L"CRS3D"))
		{
			rtnValue = testMthCrs3D;
		}
		else if (!CS_wcsicmp (fieldData.c_str (),L"GEOID"))
		{
			rtnValue = testMthGeoid;
		}
	}
	return rtnValue;
}
EcsCrsAuthority MgOsGeoTestFile::GetCrsKey (unsigned recordNbr,bool target)
{
	bool ok (false);
	short fldNbr = (target) ? 4 : 2;
	EcsCrsAuthority rtnValue (crsAuthUnknown);

	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,fldNbr,Status);
	if (ok)
	{
		if (!CS_wcsicmp (fieldData.c_str (),L"CsMap"))
		{
			rtnValue = crsAuthCsMap;
		}
		else if (!CS_wcsicmp (fieldData.c_str (),L"EPSG"))
		{
			rtnValue = crsAuthEpsg;
		}
		else if (!CS_wcsicmp (fieldData.c_str (),L"Proj4"))
		{
			rtnValue = crsAuthProj4;
		}
	}
	return rtnValue;
}
EcsCrsAuthority MgOsGeoTestFile::GetSrcCrsKey (std::wstring& srcKey,unsigned recordNbr)
{
	bool ok (false);
	EcsCrsAuthority rtnValue (crsAuthUnknown);

	rtnValue = GetCrsKey (recordNbr,false);
	if (rtnValue != crsAuthUnknown)
	{
		ok = GetField (srcKey,recordNbr,3,Status);
		if (!ok)
		{
			rtnValue = crsAuthUnknown;
		}
	}
	return rtnValue;   
}
EcsCrsAuthority MgOsGeoTestFile::GetTrgCrsKey (std::wstring& trgKey,unsigned recordNbr)
{
	bool ok (false);
	EcsCrsAuthority rtnValue;

	rtnValue = GetCrsKey (recordNbr,true);
	if (rtnValue != crsAuthUnknown)
	{
		ok = GetField (trgKey,recordNbr,5,Status);
		if (!ok)
		{
			rtnValue = crsAuthUnknown;
		}
	}
	return rtnValue;
}
bool MgOsGeoTestFile::GetSourceCoordinates (double sourceCoord [3],unsigned recordNbr)
{
	bool ok;
	wchar_t* wcPtr;
	double realX (0.0);
	double realY (0.0);
	double realZ (0.0);
	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,6,Status);
	if (ok)
	{
		realX = wcstod (fieldData.c_str (),&wcPtr);
		ok = (fabs (realX) < HUGE_VAL) ;
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,7,Status);
		if (ok)
		{
			realY = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realY) < HUGE_VAL;
		}
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,8,Status);
		if (ok)
		{
			realZ = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realZ) < HUGE_VAL;
		}
	}
	if (ok)
	{
		sourceCoord [0] = realX;
		sourceCoord [1] = realY;
		sourceCoord [2] = realZ;
	}
	return ok;
}
bool MgOsGeoTestFile::GetTargetCoordinates (double targetCoord [3],unsigned recordNbr)
{
	bool ok;
	wchar_t* wcPtr;
	double realX (0.0);
	double realY (0.0);
	double realZ (0.0);
	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,9,Status);
	if (ok)
	{
		realX = wcstod (fieldData.c_str (),&wcPtr);
		ok = (fabs (realX) < HUGE_VAL) ;
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,10,Status);
		if (ok)
		{
			realY = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realY) < HUGE_VAL;
		}
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,11,Status);
		if (ok)
		{
			realZ = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realZ) < HUGE_VAL;
		}
	}
	if (ok)
	{
		targetCoord [0] = realX;
		targetCoord [1] = realY;
		targetCoord [2] = realZ;
	}
	return ok;
}
bool MgOsGeoTestFile::GetTolerances (double tolerance [3],unsigned recordNbr)
{
	bool ok;
	wchar_t* wcPtr;
	double realX (0.0);
	double realY (0.0);
	double realZ (0.0);
	std::wstring fieldData;

	ok = GetField (fieldData,recordNbr,12,Status);
	if (ok)
	{
		realX = wcstod (fieldData.c_str (),&wcPtr);
		ok = (fabs (realX) < HUGE_VAL) ;
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,13,Status);
		if (ok)
		{
			realY = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realY) < HUGE_VAL;
		}
	}
	if (ok)
	{
		ok = GetField (fieldData,recordNbr,14,Status);
		if (ok)
		{
			realZ = wcstod (fieldData.c_str (),&wcPtr);
			ok = fabs (realZ) < HUGE_VAL;
		}
	}
	if (ok)
	{
		tolerance [0] = realX;
		tolerance [1] = realY;
		tolerance [2] = realZ;
	}
	return ok;
}
bool MgOsGeoTestFile::GetDataSource (std::wstring& dataSource,unsigned recordNbr)
{
	bool ok = GetField (dataSource,recordNbr,13,Status);
	return ok;
}
bool MgOsGeoTestFile::GetDataComment (std::wstring& dataComment,unsigned recordNbr)
{
	bool ok = GetField (dataComment,recordNbr,13,Status);
	return ok;
}
bool MgOsGeoTestFile::CompareResults (double results [3],unsigned recordNbr,double& dd)
{
	bool ok;
	double delta [3];
	double targetCoords [3];
	double tolerances [3];

	dd = 0.0;

	EcsTestMethod testMethod = GetTestMethod (recordNbr);
	ok = testMethod != testMthUnknown;
	if (ok)
	{
		ok = GetTargetCoordinates (targetCoords,recordNbr);
	}
	if (ok)
	{
		ok = GetTolerances (tolerances,recordNbr);
	}
	if (ok && testMethod == testMthCrs2D)
	{
		delta [0] = results [0] - targetCoords [0];
		delta [1] = results [1] - targetCoords [1];
		
		ok  = (fabs (delta [0]) <= tolerances [0]);
		ok &= (fabs (delta [1]) <= tolerances [1]);

		delta [0] *= 111300000.0;
		delta [1] *= 111300000.0;
		dd = sqrt (delta [0] * delta [0] + delta [1] * delta [1]);
	}
	else if (ok && testMethod == testMthCrs3D)
	{
		delta [0] = results [0] - targetCoords [0];
		delta [1] = results [1] - targetCoords [1];
		delta [2] = results [2] - targetCoords [2];
		
		ok  = (fabs (delta [0]) <= tolerances [0]);
		ok &= (fabs (delta [1]) <= tolerances [1]);
		ok &= (fabs (delta [2]) <= tolerances [2]);

		delta [0] *= 111300000.0;
		delta [1] *= 111300000.0;
		delta [2] *= 1000.0;
		dd = sqrt (delta [0] * delta [0] + delta [1] * delta [1]);
	}
	else if (ok && testMethod == testMthGeoid)
	{
		delta [0] = results [0] - targetCoords [0];
		ok  = (fabs (delta [0]) <= tolerances [0]);
		
		dd = delta [0];
	}
	return ok;
}


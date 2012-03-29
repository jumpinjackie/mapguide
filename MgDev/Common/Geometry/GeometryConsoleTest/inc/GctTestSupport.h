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

#ifndef _MGGCTTESTSUPPORT_H_
#define _MGGCTTESTSUPPORT_H_

bool GctDblCmp (double first,double second,double tolerance);
MgCoordinateCollection* CreateCoordinateCollection (unsigned seed,MgCoordinateSystem* crs,
                                                                  INT32 pointCount = 2048,
                                                                  bool threeD = false);
MgLineString* CreateLineString (MgCoordinateSystem* crs,INT32 pointCount);
MgPolygon* CreatePolygon (MgCoordinateSystem* crs,INT32 pointCount);

class MgCrsThreadTestDataEntry
{
    static unsigned m_RandomSeed;
    static unsigned SetSeed (unsigned newSeed);
public:
    MgCrsThreadTestDataEntry (CREFSTRING srcCrs,CREFSTRING trgCrs,INT32 pointCount = 1024,
                                                                  bool threeD = false);
    virtual ~MgCrsThreadTestDataEntry (void);

    void Initialize (void);
    STRING GetTransformName (void);

    MgCoordinateSystemTransform* GetForwardTransform (void);
    MgCoordinateSystemTransform* GetInverseTransform (void);
    MgCoordinateCollection* GetSrcPointsCopy (void);
    bool VerifyResults (MgCoordinateCollection* resultCollection);

private:
    bool m_bThreeD;
    double m_dToleranceXY;
    double m_dToleranceZ;
    STRING m_pSrcCrsCode;
    STRING m_pTrgCrsCode;
    STRING m_sTransformName;
    INT32 m_nPointCount;
    MgCoordinateSystem* m_pSrcCRS;
    MgCoordinateSystem* m_pTrgCRS;
    MgCoordinateSystemTransform* m_pTransformFwd;
    MgCoordinateSystemTransform* m_pTransformInv;

    MgCoordinateCollection* m_pSrcPoints;

    // Intentionally omitted, for now anyway.
    MgCrsThreadTestDataEntry (const MgCrsThreadTestDataEntry& source);
    MgCrsThreadTestDataEntry operator= (const MgCrsThreadTestDataEntry& rhs);
};

typedef std::vector<MgCrsThreadTestDataEntry*> MgCrsThreadTestDataVector;
typedef std::vector<MgCrsThreadTestDataEntry*>::iterator MgCrsThreadTestDataItr;

class MgCoordinateSystemTestThreadParams
{
public:
    MgCrsThreadTestDataVector* m_pTestData;
    MgGctTestParameters* m_cmdLineParms;
};

#endif    //_MGGCTTESTSUPPORT_H_

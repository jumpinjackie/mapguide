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

#ifndef _CCOORDINATESYSTEMGRIDGENERIC_H_
#define _CCOORDINATESYSTEMGRIDGENERIC_H_

namespace CSLibrary
{

class CCoordinateSystemOneGrid;

class CCoordinateSystemGridGeneric : public MgCoordinateSystemGridGeneric
{
    static const INT32 m_GridLineExceptionLevelK;
    static const INT32 m_GridRegionExceptionLevelK;
    static const INT32 m_GridTickExceptionLevelK;
public:
    CCoordinateSystemGridGeneric(bool bSetExceptionsOn);
    CCoordinateSystemGridGeneric(MgCoordinateSystem* pSourceCs,MgCoordinateSystem* pTargetCs,bool bSetExceptionsOn);
    virtual ~CCoordinateSystemGridGeneric();

    INT32 GetSpecializationType ();
    void SetBoundary (MgCoordinateSystemGridBoundary* pFrameBoundary);
    MgCoordinateSystemGridBoundary* GetBoundary(void);
    MgCoordinateSystemGridLineCollection* GetGridLines (MgCoordinateSystemGridSpecification* specification);
    MgCoordinateSystemGridRegionCollection* GetGridRegions (MgCoordinateSystemGridSpecification* specification);
    MgCoordinateSystemGridTickCollection* GetGridTicks (MgCoordinateSystemGridSpecification* specification);

    double GetConvergenceAngle (MgCoordinate* location);
    double GetProjectiveGridScale (MgCoordinate* location);

    INT32 ApproxGridLineMemoryUsage (MgCoordinateSystemGridSpecification* specification);
    INT32 ApproxGridRegionMemoryUsage (MgCoordinateSystemGridSpecification* specification);
    INT32 ApproxGridTickMemoryUsage (MgCoordinateSystemGridSpecification* specification);

    INT32 SetGridLineExceptionLevel (INT32 memoryUseMax);
    INT32 SetGridRegionExceptionLevel (INT32 memoryUseMax);
    INT32 SetGridTickExceptionLevel (INT32 memoryUseMax);

    INT32 GetLastError();
    void ResetLastError();
    bool AreExceptionsOn();
    void SetExceptionsOn(bool bOn);

protected:
    //from MgDisposable
    void Dispose();

    // Data Memebers -- The m_GridRegionExcpetionLevel member is not really
    // applicable to generic grids.  However, since the ApproxGridRegionMemoryUsage
    // and SetGridRegionExceptionLevel functions are pure virtual functions in the
    // interface, the functions need to exist.  So, for now we _pretend_ that
    // the 'Region' memory limits are applicable to generic grids.
    bool m_bExceptionsOn;
    INT32 m_GridLineExceptionLevel;
    INT32 m_GridRegionExceptionLevel;
    INT32 m_GridTickExceptionLevel;
    INT64 m_GridLineMemoryThreshold;
    INT64 m_GridRegionMemoryThreshold;
    INT64 m_GridTickMemoryThreshold;
    INT32 m_nLastError;
    Ptr<MgCoordinateSystem> m_pCsSource;
    Ptr<MgCoordinateSystem> m_pCsTarget;
    Ptr<MgCoordinateSystemGridBoundary> m_FrameBoundary;
    CCoordinateSystemOneGrid* m_TheGrid;
private:
    CCoordinateSystemGridGeneric();
    CCoordinateSystemGridGeneric(const CCoordinateSystemGridGeneric&);
    CCoordinateSystemGridGeneric& operator=(const CCoordinateSystemGridGeneric&);
};

} // End of namespace

#endif //_CCOORDINATESYSTEMGRIDGENERIC_H_

//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

    INT32 GetLastError();
    void ResetLastError();
    bool AreExceptionsOn();
    void SetExceptionsOn(bool bOn);

protected:
    //from MgDisposable
    void Dispose();

    // Data Memebrs
    bool m_bExceptionsOn;
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

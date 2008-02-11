//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMTRANSFORMATION_H_
#define _CCOORDINATESYSTEMTRANSFORMATION_H_

namespace CSLibrary
{
class CCoordinateSystemTransformation : public OGRCoordinateTransformation
{
private:
    OGRSpatialReference* m_poSRSSource;
    void*       m_psPJSource;
    int         m_bSourceLatLong;
    double      m_dfSourceToRadians;
    double      m_dfSourceFromRadians;


    OGRSpatialReference* m_poSRSTarget;
    void*       m_psPJTarget;
    int         m_bTargetLatLong;
    double      m_dfTargetToRadians;
    double      m_dfTargetFromRadians;

    int         m_nErrorCount;

    static void* m_hMutex;
    bool        m_bDatumShift;

public:
                CCoordinateSystemTransformation();
    virtual     ~CCoordinateSystemTransformation();

    int         Initialize( OGRSpatialReference *poSource, OGRSpatialReference *poTarget, bool bDatumShift = false );

    virtual OGRSpatialReference *GetSourceCS();
    virtual OGRSpatialReference *GetTargetCS();
    virtual int Transform( int nCount, double *x, double *y, double *z = NULL );
    virtual int TransformEx( int nCount, double *x, double *y, double *z = NULL, int *panSuccess = NULL );

    static OGRCoordinateTransformation* CreateCoordinateTransformation( OGRSpatialReference *poSource,
                                                                        OGRSpatialReference *poTarget,
                                                                        bool bDatumShift = false );
};

} // End of namespace

#endif //_CCOORDINATESYSTEMTRANSFORMATION_H_

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

#ifndef _MGCSTRANS_H_
#define _MGCSTRANS_H_

#include "ServerFeatureDllExport.h"
#include "CSysTransformer.h"

class MG_SERVER_FEATURE_API MgCSTrans : public CSysTransformer
{
public:
    MgCSTrans(MgCoordinateSystem* srcCs, MgCoordinateSystem* dstCs);
    virtual ~MgCSTrans();

    virtual void TransformPoint(double& x, double& y);
    virtual void TransformPoints(int numPts, double* x, double* y);
    virtual void TransformExtent(double& minX, double& minY, double& maxX, double& maxY);
    virtual double GetLinearScale();

private:
    Ptr<MgCoordinateSystemTransform> m_trans;
    double m_dLinearScale;
};

#endif

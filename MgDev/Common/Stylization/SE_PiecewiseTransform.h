//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SE_PIECEWISETRANSFORM_H
#define SE_PIECEWISETRANSFORM_H

#include "Bounds.h"

struct SE_TransformInfo
{
    double dXdx;
    double dYdx;
    double dXdy;
    double dYdy;
    double d2Xdx2;
    double d2Ydx2;
    double d2Xdy2;
    double d2Ydy2;
    double d2Xdxdy;
    double d2Ydxdy;
};

class SE_PiecewiseTransform
{
public:
    /* Returned in the format pt00, pt01, pt10, pt11 etc. where each pair of points represents
     * the endpoints of a line along which the the transform is not smooth */
    virtual RS_F_Point* GetDiscontinuities(int &length) = 0;
    /* Regions that are not transformed (other than rotation) */
    virtual RS_Bounds* GetIdentityRegions(int &length) = 0;
    /* Regions where simple linear of transformed points is insufficient */
    virtual RS_Bounds* GetNonlinearRegions(int &length) = 0;
    virtual RS_Bounds& GetTransformedBounds() = 0;

    /* Applying the chop is left to the caller */
    virtual double GetXChop(bool &chopEnd) = 0;

    /* The transform function is not aware of the chop value */
    virtual void Transform(double& x, double& y, SE_TransformInfo* info) = 0;
};

#endif // SE_PIECEWISETRANSFORM_H

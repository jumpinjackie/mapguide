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

#ifndef SE_IDENTITYJOIN_H
#define SE_IDENTITYJOIN_H

#include "SE_PiecewiseTransform.h"
#include "SE_Matrix.h"

class SE_IdentityJoin : public SE_PiecewiseTransform
{
public:
    /* bounds:  The bounds of the symbol.
     * offset:  The distance along the line in pixels from pt0 to the beginning of the symbol
     * pt0:     The start point of the line segment
     * pt1:     The end point of the line segment
     */
    SE_IdentityJoin(RS_Bounds& bounds, double offset, const RS_F_Point& pt0, const RS_F_Point& pt1);

    virtual RS_F_Point* GetDiscontinuities(int &length);
    virtual RS_Bounds& GetTransformedBounds();

    virtual void GetXChop(double& startx, double& endx);

    virtual RS_F_Point* Transform(const RS_F_Point& pt0, const RS_F_Point& pt1, int& length);
    virtual void Transform(SE_LineStorage* src, SE_LineStorage* dst, bool closed);


private:
    double m_chop_start;
    double m_chop_end;

    RS_F_Point m_points[2];

    SE_Matrix m_xform;

    RS_Bounds m_xf_bounds;
    RS_Bounds m_bounds;
};

#endif // SE_IDENTITYJOIN_H

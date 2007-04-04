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
#include <float.h>

class SE_IdentityJoin : public SE_PiecewiseTransform
{
public:
    SE_IdentityJoin() { }
    
    /* bounds:   The bounds of the symbol.
     * offset:   The distance along the line in pixels from pt0 to the beginning of the symbol
     * pt0:      The start point of the line segment
     * pt1:      The end point of the line segment
     * prechop:  The offset from pt0 before which to truncate the symbol
     * postchop: The offset from pt0 after which to truncate the symbol
     */
    SE_IdentityJoin(RS_Bounds& bounds, double offset, const RS_F_Point& pt0, const RS_F_Point& pt1, double prechop = -DBL_MAX, double postchop = DBL_MAX);

    virtual RS_F_Point* GetDiscontinuities(int &length);
    virtual RS_Bounds& GetTransformedBounds();

    virtual void GetXChop(double& startx, double& endx);
    virtual void GetXRadius(double& pre, double& post);
    
    virtual void ApplyPreTransform(SE_Matrix& prexf);
    virtual void Transform(SE_LineStorage* src, SE_LineStorage* dst, int contour, int ncntrs, bool closed);


private:
    double m_chop_start;
    double m_chop_end;

    RS_F_Point m_points[2];

    SE_Matrix m_xform;

    RS_Bounds m_xf_bounds;
    RS_Bounds m_bounds;
};

#endif // SE_IDENTITYJOIN_H

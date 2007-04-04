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

#ifndef SE_MITERJOIN_H
#define SE_MITERJOIN_H

#include "SE_PiecewiseTransform.h"
#include "SE_Matrix.h"
#include <float.h>
#include <vector>

class SE_MiterJoin : public SE_PiecewiseTransform
{
public:
    SE_MiterJoin() { }

    SE_MiterJoin(double limit,                  /* The miter limit of the join */
                 RS_Bounds& bounds,             /* The bounds of the unoriented symbol in pixel units */
                 double vertexOffset,           /* The distance along the line (in pixels) from the 
                                                   beginning of the symbol to the vertex of the join 
                                                   (the value will be negative if the symbol starts before
                                                   the join, and positive if it starts after the join) */
                 const RS_F_Point& prev,        /* The point preceeding the vertex of the join in screen space */
                 const RS_F_Point& vert,        /* The vertex of the join in screen space */
                 const RS_F_Point& next,        /* The point following the vertex of the join in screen space */
                 bool noTransform = true);      /* True if the symbol is already oriented appropriately along the leading segment */

    virtual RS_F_Point* GetDiscontinuities(int &length);
    virtual RS_Bounds& GetTransformedBounds();

    virtual void GetXChop(double& startx, double& endx);
    virtual void GetXRadius(double& pre, double& post);

    virtual void ApplyPreTransform(SE_Matrix& prexf);
    /* Transform will apply the join transform, as well as the appropriate orientation in screen space */
    virtual void Transform(SE_LineStorage* src, SE_LineStorage* dst, int contour, int ncntrs, bool closed);

private:
    void _Transform(RS_F_Point& pt);
    void _Explode(const RS_F_Point& head, const RS_F_Point& tail, int& length);

    double m_height;      /* Height on each side from the center (ignoring bevel) in join space */
    double m_bevel;       /* Height of the bevel in join space */
    double m_width;       /* Distance from the center on each side that is transformed in join space*/
    double m_bevel_width; /* Maximum transformed size of m_width */

    double m_bevel_scale;
    double m_miter_scale;

    SE_Matrix m_w2j;      /* World space to join space transform */
    SE_Matrix m_j2w;      /* Join space to world space transform */

    RS_F_Point m_points[6];
    
    int m_n_discontinuities;
    RS_F_Point m_discontinuities[10];

    RS_Bounds m_bounds;
    RS_Bounds m_j_bounds;
    RS_Bounds m_xf_bounds;

    SE_Matrix m_sRot;
    SE_Matrix m_eRot;
};

#endif // SE_MITERJOIN_H

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

class SE_MiterJoin : public SE_PiecewiseTransform
{
public:

    SE_MiterJoin(double limit,                  /* The miter limit of the join. */
                 RS_Bounds& bounds,             /* The bounds of the unoriented symbol in pixel units. */
                 double vertexOffset,           /* The distance along the line (in pixels) from the 
                                                   beginning of the symbol to the vertex of the join 
                                                   (the value will be negative if the symbol starts before
                                                   the join, and positive if it starts after the join). */
                 double sAng,                   /* The angle of the first line segment. */
                 double eAng,                   /* The angle of the second line segment. */
                 double dx,                     /* The x offset of the origin of the symbol */
                 double dy,                     /* The y offset of the origin of the symbol */
                 double chopOffset = DBL_MAX);  /* The distance after the vertex at which to truncate the symbol */

    virtual RS_F_Point* GetDiscontinuities(int &length);
    virtual RS_Bounds* GetIdentityRegions(int &length);
    virtual RS_Bounds* GetNonlinearRegions(int &length);
    virtual RS_Bounds& GetTransformedBounds();

    virtual double GetXChop(bool &chopEnd);

    /* Transform will apply the join transform, as well as the appropriate rotation */
    virtual void Transform(double& x, double& y, SE_TransformInfo* info);

private:
    double m_y_max;
    double m_y_max_len;
    double m_y_mid;
    double m_y_mid_len;
    double m_y_min;
    double m_y_min_len;

    double m_x_vert;
    double m_miter_scale;
    double m_bevel_scale;
    double m_sAng;
    double m_eAng;
    
    int m_n_discontinuities;
    RS_F_Point* m_first_discontinuity;
    RS_F_Point m_discontinuities[6];

    int m_n_identity_regions;
    RS_Bounds* m_first_identity_region;
    RS_Bounds m_identity_region[2];
    
    RS_Bounds m_bounds;
    RS_Bounds m_xf_bounds;

    SE_Matrix m_sRot;
    SE_Matrix m_eRot;

    double m_chop;
};

#endif // SE_MITERJOIN_H

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
#include "SE_Matrix.h"

class SE_LineStorage;
class SE_LineBufferStorage;


class SE_PiecewiseTransform
{
public:
    /* Returned in the format pt00, pt01, pt10, pt11 etc. where each pair of points represents
     * the endpoints of a line along which the the transform is not smooth */
    virtual RS_F_Point* GetDiscontinuities(int &length) = 0;
    /* Transformed bounds take chop into account */
    virtual RS_Bounds& GetTransformedBounds() = 0;

    /* Applying the chop is left to the caller.  Chop the symbol before startx and after endx. */
    virtual void GetXChop(double& startx, double& endx) = 0;

    /* The nonnegative distance along the line, before and after the vertex, that the join affects */
    virtual void GetXRadius(double& pre, double& post) = 0;

    /* Line-Space transform (i.e. translation by x is along the line) applied to all arguments
     * to Transform before join computation (multiple calls are cumulative)*/
    virtual void ApplyPreTransform(SE_Matrix& prexf) = 0;

    /* The transform function handles the chop as well */
    virtual void Transform(SE_LineStorage* src, SE_LineStorage* dst, int contour, int ncntrs, bool closed) = 0;
};

#endif // SE_PIECEWISETRANSFORM_H

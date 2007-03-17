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

class SE_IdentityJoin : public SE_PiecewiseTransform
{
public:
    /* bounds:  The bounds of the symbol.
     * chop:    The amount to remove in pixel units
     * end:     The symbol is truncated backwards from maxx if true, or forwards from minx if not
     */
    SE_IdentityJoin(RS_Bounds& bounds, double chop, bool end);

    virtual RS_F_Point* GetDiscontinuities(int &length);
    virtual RS_Bounds* GetIdentityRegions(int &length);
    virtual RS_Bounds* GetNonlinearRegions(int &length);
    virtual RS_Bounds& GetTransformedBounds();

    virtual double GetXChop(bool &chopEnd);

    virtual void Transform(double& x, double &y, SE_TransformInfo* info);
private:
    double m_chop;
    bool m_end;
    RS_Bounds m_identity_region;
    RS_Bounds m_xf_bounds;
};

#endif // SE_IDENTITYJOIN_H

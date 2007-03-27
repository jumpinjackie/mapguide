//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef SYMBOLTRANS_H
#define SYMBOLTRANS_H

#include "CSysTransformer.h"
#include "Bounds.h"

//TODO: currently the symbol library uses a max coordinate of
//2000000. This should be changed to INT_MAX
#define SYMBOL_MAX 2000000

///<summary>
/// Transformer used for point symbol stylization. It does point specific
/// things like rotation and reference point.
///</summary>
class SymbolTrans : public CSysTransformer
{
public:
    STYLIZATION_API SymbolTrans();

    STYLIZATION_API SymbolTrans(RS_Bounds& src, RS_Bounds& dst, double refX, double refY, double angle);

    STYLIZATION_API virtual ~SymbolTrans();

    STYLIZATION_API virtual void TransformPoint(double& x, double& y);
    STYLIZATION_API virtual void TransformPoints(int numPts, double* x, double* y);

    STYLIZATION_API virtual double GetLinearScale();

    STYLIZATION_API double GetAngle() { return m_angle; }
    STYLIZATION_API double GetRefX() { return m_refX; }
    STYLIZATION_API double GetRefY() { return m_refY; }
    STYLIZATION_API RS_Bounds GetSrcBounds() { return m_src; }
    STYLIZATION_API RS_Bounds GetDstBounds() { return m_dst; }

    STYLIZATION_API void SetAngle(double angle);
    STYLIZATION_API void SetRefX(double refX);
    STYLIZATION_API void SetRefY(double refY);
    STYLIZATION_API void SetSrcBounds(const RS_Bounds& src);
    STYLIZATION_API void SetDstBounds(const RS_Bounds& dst);

private:
    RS_Bounds m_src;
    RS_Bounds m_dst;
    double m_refX;
    double m_refY;
    double m_angle;

    //cached common subexpressions
    double m_angleCos;
    double m_angleSin;
    double m_widthScale;
    double m_heightScale;
};

#endif

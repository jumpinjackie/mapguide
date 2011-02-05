//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "Renderers.h"
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
    SymbolTrans();
    SymbolTrans(RS_Bounds& src, RS_Bounds& dst, double refX, double refY, double angleRad);
    virtual ~SymbolTrans();

    virtual void TransformPoint(double& x, double& y);
    virtual void TransformPoints(int numPts, double* x, double* y);

    virtual double GetLinearScale();

    double GetAngle() { return m_angleRad; }
    double GetRefX() { return m_refX; }
    double GetRefY() { return m_refY; }
    RS_Bounds GetSrcBounds() { return m_src; }
    RS_Bounds GetDstBounds() { return m_dst; }

    void SetAngle(double angleRad);
    void SetRefX(double refX);
    void SetRefY(double refY);
    void SetSrcBounds(const RS_Bounds& src);
    void SetDstBounds(const RS_Bounds& dst);

private:
    RS_Bounds m_src;
    RS_Bounds m_dst;
    double m_refX;
    double m_refY;
    double m_angleRad;

    // cached common subexpressions
    double m_angleCos;
    double m_angleSin;
    double m_widthScale;
    double m_heightScale;
};

#endif

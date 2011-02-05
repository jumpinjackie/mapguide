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

#include "stdafx.h"
#include "SymbolTrans.h"


SymbolTrans::SymbolTrans()
: m_src(0.0, 0.0, 1.0, 1.0),
  m_dst(0.0, 0.0, 1.0, 1.0),
  m_refX(-0.5),
  m_refY(-0.5),
  m_angleRad(0.0),
  m_angleCos(1.0),
  m_angleSin(0.0),
  m_widthScale(1.0),
  m_heightScale(1.0)
{
}


SymbolTrans::SymbolTrans(RS_Bounds& src, RS_Bounds& dst, double refX, double refY, double angleRad)
: m_src(src),
  m_dst(dst),
  m_refX(refX),
  m_refY(refY),
  m_angleRad(angleRad)
{
    if (angleRad != 0.0)
    {
        m_angleCos = cos(angleRad);
        m_angleSin = sin(angleRad);
    }
    else
    {
        m_angleCos = 1.0;
        m_angleSin = 0.0;
    }

    m_widthScale = m_dst.width() / m_src.width();
    m_heightScale = m_dst.height() / m_src.height();
}


SymbolTrans::~SymbolTrans()
{
}


void SymbolTrans::TransformPoint(double& x, double& y)
{
    double sx = (x - m_src.minx) * m_widthScale;
    double sy = (y - m_src.miny) * m_heightScale;

    //relative distance from insertion point
    double dx = sx - m_refX * m_dst.width();
    double dy = sy - m_refY * m_dst.height();

    //rotate insertion vector around angle
    double rx = dx * m_angleCos - dy * m_angleSin;
    double ry = dx * m_angleSin + dy * m_angleCos;

    //now move to absolute position
    x = rx + m_dst.minx;
    y = ry + m_dst.miny;
}


void SymbolTrans::TransformPoints(int numPts, double* x, double* y)
{
    for (int i=0; i<numPts; i++)
        TransformPoint(x[i], y[i]);
}


double SymbolTrans::GetLinearScale()
{
    return m_heightScale;
}


void SymbolTrans::SetAngle(double angleRad)
{
    m_angleRad = angleRad;

    if (angleRad != 0.0)
    {
        m_angleCos = cos(angleRad);
        m_angleSin = sin(angleRad);
    }
    else
    {
        m_angleCos = 1.0;
        m_angleSin = 0.0;
    }
}


void SymbolTrans::SetRefX(double refX)
{
    m_refX = refX;
}


void SymbolTrans::SetRefY(double refY)
{
    m_refY = refY;
}


void SymbolTrans::SetSrcBounds(const RS_Bounds& src)
{
    m_src = src;

    m_widthScale  = m_dst.width()  / m_src.width();
    m_heightScale = m_dst.height() / m_src.height();
}


void SymbolTrans::SetDstBounds(const RS_Bounds& dst)
{
    m_dst = dst;

    m_widthScale  = m_dst.width()  / m_src.width();
    m_heightScale = m_dst.height() / m_src.height();
}

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

#include "KmlPolyStyle.h"

KmlPolyStyle::KmlPolyStyle(int lineColor, double lineWidth, int fillColor):
    m_lineColor(lineColor),
    m_lineWidth(lineWidth),
    m_fillColor(fillColor)
{
}


bool KmlPolyStyle::operator<(const KmlPolyStyle& other) const
{
    return (other.m_lineColor < m_lineColor ||
        (other.m_lineColor == m_lineColor &&
        (other.m_lineWidth < m_lineWidth ||
        (other.m_lineWidth == m_lineWidth && other.m_fillColor < m_fillColor))));
}

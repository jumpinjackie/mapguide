//
//  Copyright (C) 2007-2008 by Autodesk, Inc.
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

#ifndef SE_GEOMETRYOPERATIONS_H_
#define SE_GEOMETRYOPERATIONS_H_

#include "SE_Matrix.h"
#include "SE_Tuple.h"
#include "LineBuffer.h"

/*
 * Transforms the geometry in src by tx, and stores the result in dst.
 * The bounds property of dst will not be computed unless updateBounds
 * is true.
 */
void TransformLB(LineBuffer* src,
                 LineBuffer* dst,
                 const SE_Matrix& tx,
                 bool updateBounds);

/*
 * Computes the intersection of (A0, A1) with (B0, B1).  If an
 * intersection exists, returns true and stores the uv intersection
 * point (in units of A1-A0, B1-B0) in T, and the true intersection
 * point in isection.
 */
bool Intersects(const SE_Tuple& A0, const SE_Tuple& A1,
                const SE_Tuple& B0, const SE_Tuple& B1,
                SE_Tuple& T, SE_Tuple& isection);

#endif // SE_GEOMETRYOPERATIONS_H_

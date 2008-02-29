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

#include "SE_LinearClip.h"

VerticalClip::VerticalClip(double clipx)
{
    m_clip = clipx;
    m_perturbation = m_clip == 0.0 ? 1.0e-10 : m_clip + m_clip * 1.0e-10;
}

bool VerticalClip::intersect(const SE_Tuple& start,
                             const SE_Tuple& end,
                             SE_Tuple& isect) const
{
    if ((start.x > m_clip && end.x > m_clip) ||
        (start.x < m_clip && end.x < m_clip))
        return false;

    double sx = start.x;
    double ex = end.x;

    if (sx == m_clip || ex == m_clip)
    {

        if (sx == m_clip)
            sx = m_perturbation;
        if (ex == m_clip)
            ex = m_perturbation;

        if ((sx > m_clip && ex > m_clip) ||
            (sx < m_clip && ex < m_clip))
            return false;
    }

    isect = start + (end - start) * ((m_clip - sx) / (ex - sx));
    return true;
}


bool VerticalClip::clockwise(const SE_Tuple& point) const
{
    return point.x == m_clip ? m_perturbation > m_clip : point.x > m_clip;
}


bool VerticalClip::operator()(const ClipNode* left, const ClipNode* right) const
{
    return left->intersection.y > right->intersection.y;
}


GeneralClip::GeneralClip(const SE_Tuple& clipbase, const SE_Tuple& clipvector)
{
    _ASSERT(clipvector.x != 0.0 || clipvector.y != 0.0);

    m_clippt = clipbase;
    m_clipln = clipvector.normalize();
    m_perturbation = SE_Tuple(-clipvector.y, clipvector.x) * (clipbase.length() * 1.0e-10);
}

bool GeneralClip::intersect(const SE_Tuple& start,
                            const SE_Tuple& end,
                            SE_Tuple& isect) const
{
    double sx = m_clipln.cross(start - m_clippt);
    double ex = m_clipln.cross(end - m_clippt);

    SE_Tuple ns = sx == 0.0 ? start + m_perturbation : start;
    SE_Tuple ne = ex == 0.0 ? end + m_perturbation : end;

    SE_Tuple dv  = ne - ns;
    SE_Tuple vd = ns - m_clippt;
    double icp = 1.0 / m_clipln.cross(dv);

    SE_Tuple T = SE_Tuple(vd.cross(dv) * icp, vd.cross(m_clipln) * icp);

    if (T.y >= 0.0 && T.y <= 1.0)
    {
        isect = m_clippt + (m_clipln * T.x);
        return true;
    }

    return false;
}


bool GeneralClip::clockwise(const SE_Tuple& point) const
{
    /* Conveniently enough, in the case of a point on the line (cross product is zero),
     * we perturb the point in the counter-clockwise direction--the cross product will
     * be less than zero. */
    return  m_clipln.cross(point - m_clippt) > 0.0;
}

bool GeneralClip::operator()(const ClipNode* left, const ClipNode* right) const
{
    /* Since this function can only be called on points that actually intersect the
     * clipline and the square root function is monotonic, we have a really easy time
     * with this calculation. */
    return (left->intersection - m_clippt).lengthSquared() >
           (right->intersection - m_clippt).lengthSquared();
}

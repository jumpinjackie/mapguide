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

#include "SE_MiterJoin.h"
#include "SE_LineBuffer.h"
#include "stdafx.h"


SE_MiterJoin::SE_MiterJoin
                         ( double limit, 
                           RS_Bounds& bounds, 
                           double vertexOffset, 
                           const RS_F_Point& prev,
                           const RS_F_Point& vert,
                           const RS_F_Point& next,
                           bool noTransform ) :
    m_sRot(),
    m_eRot(),
    m_j_bounds(),
    m_xf_bounds(DBL_MAX, DBL_MAX, -DBL_MAX, -DBL_MAX)
{
    double V;           /* The location of the vertex in symbol coordinates */
    double wp;          /* The height of the positive portion of the symbol (above the centerline) */
    double wn;          /* The height of the negative portion of the symbol (below the centerline) */
    double wb;          /* The height of the beveled portion of the symbol */
    double w;           /* The height of the join above or below the line (symmetrical) */
    double jo;          /* The x distance from the vertex to the end of the outside join */
    double ji;          /* The x distance from the vertex to the end of the inside join */
    double t;           /* The tangent of the half-angle */
    double ca;          /* The cosine of the angle */
    double ch;          /* The cosine of the half-angle */
    double sa;          /* The sine of the angle */
    double mmax;        /* The length of the miter before the miter limit is applied */
    double mmin;        /* The minimum miter limit (at which a flat bevel is possible) */
    double mlen;        /* The miter length (the miter limit times the outside line thickness) */
    double movr;        /* The length of the portion of the miter that exceeds the miter length */
    double imprev;      /* The inverse magnitude of the (vert - prev) vector */
    double imnext;      /* The invserse magnitude of the (next - vert) vector */
    RS_F_Point nprev;   /* The normalized (vert - prev) vector */
    RS_F_Point nnext;   /* The normalized (next - vert) vector */
    bool clockwise;     /* If true, then the positive portion of the symbol is on the inside of the join */
    bool rtl;           /* If true, then the line moves negatively in x */
    
    V = bounds.minx - vertexOffset;
    clockwise = PointLeft(prev.x, prev.y, vert.x, vert.y, next.x, next.y);
    rtl = next.x < prev.x;
   
    nnext.x = next.x - vert.x;
    nnext.y = next.y - vert.y;
    imnext = 1.0/sqrt(nnext.x*nnext.x + nprev.y*nprev.y);
    nnext.x *= imnext;
    nnext.y *= imnext;

    nprev.x = vert.x - prev.x;
    nprev.y = vert.y - prev.y;
    imprev = 1.0/sqrt(nprev.x*nprev.x + nprev.y*nprev.y);
    nprev.x *= imprev;
    nprev.y *= imprev;

    m_j_bounds.minx = vertexOffset; /* bounds.minx - V */
    m_j_bounds.maxx = bounds.maxx - V;

    if (noTransform)
    {
        m_eRot.translate(-vert.x, -vert.y);
        m_eRot.rotate(-nprev.y, nprev.x); /* Undo leading segment rotation */

        m_w2j.translate(-vert.x, -vert.y);
        m_w2j.rotate(-nprev.y, nprev.x); /* rotate by -angle */
        if (clockwise)
        {   /* Flip across the line so we don't have to worry about this stuff */
            m_w2j.scaleY(-1.0);
            m_j2w.scaleY(-1.0);
            m_j_bounds.miny = -bounds.maxy;
            m_j_bounds.maxy = -bounds.miny;
        }
        else
        {
            m_j_bounds.miny = bounds.miny;
            m_j_bounds.maxy = bounds.maxy;
        }
        if (rtl)
        {
            m_w2j.scaleX(-1.0);
            m_j2w.scaleX(-1.0);
        }
        m_j2w.rotate(nprev.y, nprev.x);
        m_j2w.translate(vert.x, vert.y);
    }
    else
    {
        m_sRot.translateX(-V); /* translate by vert-(V,0), then by -vert */
        m_sRot.rotate(nprev.y, nprev.x);
        m_sRot.translate(vert.x, vert.y);
        m_w2j.translateX(-V);
        if (clockwise)
        {
            m_w2j.scaleY(-1.0);
            m_j2w.scaleY(-1.0);
        }
        if (rtl)
        {
            m_w2j.scaleX(-1.0);
            m_j2w.scaleX(-1.0);
        }
        m_w2j.translateX(V);
        m_eRot.translateX(-V);
    }

    /* Rotation should occur about the vertex */
    m_eRot.rotate(nnext.y, nnext.x);
    m_eRot.translate(vert.x, vert.y);
    
    m_eRot.postmultiply(m_j2w);
    m_sRot.postmultiply(m_j2w);

    wn = bounds.miny < 0.0 ? -bounds.miny : 0.0;
    wp = bounds.maxy > 0.0 ? bounds.maxy : 0.0;
    double hw = (wn > wp) ? wn : wp;
    w = 2.0 * hw;

    ca = -nprev.x*nnext.x - nprev.y*nnext.y; /* <-nprev, nnext> */
    sa = fabs(-nprev.x*nnext.y + nprev.y*nnext.x); /* |-nprev X nnext| */
    t = (1 - ca)/sa; /* From half angle identity: tan(x/2) = (1 - cos(x))/sin(x) */

    /* the tangent of the half angle is equal to w/j */
    jo = ji = hw/t;
    mmax = sqrt(jo*jo + hw*hw);

    /* Derivation of mmin:
     * Let  A                                   be the area of the minimum bevel filled in between segments with no join
     * Let  theta                               be the angle between the two segments
     *
     * A = w*w*sin(pi - theta)                  from the sine formula for the area of a triangle
     * A = w*w*sin(theta)                       from identity sin(pi - theta) = sin(theta)
     * 
     * A/2 = w*mmin*sin(pi/2 - theta/2)         from applying the same formula to one half of the (isoceles) triangle
     * A/2 = w*mmin*cos(theta/2)                from identity sin(pi/2 - theta) = cos(theta)
     * A/2 = w*mmin*sqrt((1 + cos(theta))/2)    from cos(theta/2) = +/- sqrt((1 + cos(theta)/2), and theta/2 < pi/2, so positive
     * w*w*sin(theta)/2 = w*wmin*sqrt((1 + cos(theta))/2)
     * w*sa/2 = wmin*sqrt((1+ca)/2)
     * wmin = hw*sa/sqrt((1+ca)/2)
     */
    ch = sqrt((1 + ca) / 2.0);
    mmin = sa / ch;
    mlen = hw * (limit < mmin ? mmin : limit);
    movr = mmax - mlen;

    if (movr > 0)
    {
        /* Todo: limit cannot go to zero */
        double reduction = movr/mmax;
        jo *= 1.0 - reduction;
        wb = hw * reduction;
        m_bevel_width = mlen * ch;
        m_bevel_scale = (m_bevel_width - jo) / wb;
    }
    else
    {
        wb = 0.0;
        m_bevel_width = jo;
    }
 
    m_height = hw;
    m_width = ji;
    m_top_width = jo;
    m_bevel = hw - wb;
    m_miter_scale = m_width * (m_bevel_width + m_width) / (m_bevel + m_height);
    
    m_n_discontinuities = -1;
}

RS_F_Point* SE_MiterJoin::GetDiscontinuities(int &length)
{
    if (m_n_discontinuities == -1) /* Lazily initialize */
    {
        m_n_discontinuities = 0;
        int index = 0;

        if (m_j_bounds.minx < -m_width)
        {
            m_n_discontinuities++;
            m_discontinuities[index].x = -m_width;
            m_discontinuities[index].y = -m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
            m_discontinuities[index].x = -m_width;
            m_discontinuities[index].y = m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
        }

        if (m_bevel > 0.0 && m_j_bounds.maxy > m_bevel)
        {
            m_n_discontinuities++;
            m_discontinuities[index].x = -m_width;
            m_discontinuities[index].y = m_bevel;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
            m_discontinuities[index].x = 0.0;
            m_discontinuities[index].y = m_bevel;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;

            m_n_discontinuities++;
            m_discontinuities[index].x = 0.0;
            m_discontinuities[index].y = m_bevel;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
            m_discontinuities[index].x = m_width;
            m_discontinuities[index].y = m_bevel;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
        }

        if (m_j_bounds.minx < 0 && m_j_bounds.maxx > 0)
        {
            m_n_discontinuities++;
            m_discontinuities[index].x = 0;
            m_discontinuities[index].y = -m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
            m_discontinuities[index].x = 0;
            m_discontinuities[index].y = m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
        }

        if (m_j_bounds.maxx > m_width)
        {
            m_n_discontinuities++;
            m_discontinuities[index].x = m_width;
            m_discontinuities[index].y = -m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
            index++;
            m_discontinuities[index].x = m_width;
            m_discontinuities[index].y = m_height;
            m_j2w.transform(m_discontinuities[index].x, m_discontinuities[index].y);
        }
    }

    length = m_n_discontinuities;
    return m_discontinuities;
}

RS_Bounds& SE_MiterJoin::GetTransformedBounds()
{
    if (!m_xf_bounds.IsValid())
    {
        RS_F_Point xfpt;

        xfpt.x = m_j_bounds.minx;
        xfpt.y = m_j_bounds.miny;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_j_bounds.maxx;
        xfpt.y = m_j_bounds.maxy;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = 0.0;
        xfpt.y = m_j_bounds.maxy;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = 0.0;
        xfpt.y = m_j_bounds.miny;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_j_bounds.minx;
        xfpt.y = m_j_bounds.maxy;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);

        xfpt.x = m_j_bounds.maxx;
        xfpt.y = m_j_bounds.miny;
        _Transform(xfpt);
        m_xf_bounds.add_point(xfpt);
    }

    return m_xf_bounds;
}

void SE_MiterJoin::GetXChop(double& startx, double& endx)
{
    startx = -DBL_MAX;
    endx = DBL_MAX;
}

RS_F_Point* SE_MiterJoin::Transform(const RS_F_Point& pt0, const RS_F_Point& pt1, int& length)
{
    RS_F_Point head, tail;

    m_w2j.transform(pt0.x, pt0.y, head.x, head.y);
    m_w2j.transform(pt1.x, pt1.y, tail.x, tail.y);

    if (head.x > tail.x) /* Swap 'em--the head comes first! */
    {
        m_points[0] = tail;
        tail = head;
        head = m_points[0];
    }

    _Explode(head, tail, length);

    for (int i = 0; i < length; i++)
        _Transform(m_points[i]);
    return m_points;
}

void SE_MiterJoin::Transform(SE_LineStorage* src, SE_LineStorage* dst, bool /*closed*/)
{
    int n_cntrs = src->cntr_count();
    int* contours = src->cntrs();
    double* pts = src->points();
    RS_F_Point head, tail;
    int length;

    for (int i = 0; i < n_cntrs; i++)
    {
        double x, y, lx, ly;
        double* last = pts + 2*contours[i];
        dst->EnsureContours(1);
        lx = *pts++;
        ly = *pts++;
        m_w2j.transform(lx, ly);
        bool first = true;
        bool noswap;

        while (pts < last)
        {
            x = *pts++;
            y = *pts++;
            m_w2j.transform(x, y);

            head.x = x;
            head.y = y;
            tail.x = lx;
            tail.y = ly;

            if (noswap = head.x > tail.x) /* Swap 'em--the head comes first! */
            {
                m_points[0] = tail;
                tail = head;
                head = m_points[0];
            }

            _Explode(head, tail, length);
            dst->EnsurePoints(length);
            
            int index, inc;
            if (noswap)
            {
                index = 0;
                inc = 1;
            }
            else
            {
                index = length - 1;
                inc = -1;
            }

            if (first)
            {
                first = false;
                _Transform(m_points[index]);
                dst->_MoveToNoChop(m_points[index].x, m_points[index].y);
            }

            index += inc;

            for (i = 1; i < length; i++)
            {
                RS_F_Point& pt = m_points[index];
                index += inc;
                _Transform(pt);
                dst->_LineToNoChop(pt.x, pt.y);
            }
            lx = x;
            ly = y;
        }
    }
}

void SE_MiterJoin::_Explode(const RS_F_Point& head, const RS_F_Point& tail, int& length)
{
    RS_F_Point bevel, zero;
    bool addBevel = false, addZero = false;

    length = 0;
    double slope = (tail.y - head.y) / (tail.x - head.x);

    m_points[length++] = head;

    if (head.x < -m_width && tail.x > -m_width) /* Line crosses the beginning of the transform */
    {
        m_points[length].x = -m_width;
        m_points[length++].y = head.y - slope * (m_width + head.x);
    }

    if ( (head.y < m_bevel && tail.y > m_bevel) || (head.y > m_bevel && tail.y < m_bevel) )
    {
        bevel.x = head.x + (m_bevel - head.y) / slope;
        bevel.y = m_bevel;
        addBevel = true;
    }

    if (head.x < 0.0 && tail.x > 0.0)
    {
        zero.y = head.y - slope * head.x;
        zero.x = 0.0;
        /* If, by some chance, the line crosses the bevel at x=0, don't add the point twice */
        addZero = !addBevel || zero.x != bevel.x || zero.y != bevel.y;
    }

    if (addBevel && addZero)
    {
        if (bevel.x < zero.x)
        {
            m_points[length++] = bevel;
            m_points[length++] = zero;
        }
        else
        {
            m_points[length++] = zero;
            m_points[length++] = bevel;
        }
    }
    else if (addBevel)
        m_points[length++] = bevel;
    else if (addZero)
        m_points[length++] = zero;

    if (head.x < m_width && tail.x > m_width) /* Line crosses the end of the transform */
    {
        m_points[length].x = m_width;
        m_points[length++].y = head.y + slope * (m_width - head.x);
    }

    m_points[length++] = tail;
}

/* Argument: point in join space
 * Returns:  point in input space
 */
void SE_MiterJoin::_Transform(RS_F_Point& pt)
{
    double x_min;
    SE_Matrix* postxf;
    
    if (pt.x <= 0)
    {
        x_min = -m_width;
        postxf = &m_sRot;
    }
    else
    {
        x_min = m_width;
        postxf = &m_eRot;
    }

    if (fabs(pt.x) <= m_width)
    {
        if (pt.y > m_bevel && pt.y <= m_height)
        {
        /* The bevel transform can be described by the following function:
         * X(x,y) = x_min + (x - x_min) * (m_top_width + (m_height - y) * (m_bevel_width - m_top_width) / (m_height - m_bevel)
         *                                                          |------------- m_bevel_scale -------------|
         * where:
         * dX/dx = m_top_width + (m_height - y) * scale
         * dX/dy = - (x - x_min) * scale
         * d2X/dx2 = 0
         * d2X/dy2 = 0
         * d2X/dxdy = -scale
         * 
         * The transform is constant over Y, so
         * Y(x,y) = y
         */
            double dy = m_height - pt.y;
            double dx = pt.x - x_min;

            pt.x = x_min + dx * (m_top_width + dy * m_bevel_scale);
        }
        else if (pt.y <= m_bevel && pt.y >= -m_height)
        {

        /* The miter transform can be described by the following function:
         * X(x,y) = x_min + (x - x_min) * (y + m_height) * m_width * (m_bevel_width + m_width) / (m_bevel + m_height)
         *                                             |------------------- m_miter_scale -------------------|
         * where:
         * dX/dx = (y + m_height) * scale
         * dX/dy = (x - x_min) * scale
         * d2X/dx2 = 0
         * d2X/dy2 = 0
         * d2X/dxdy = scale
         * 
         * The transform is constant over Y, so
         * Y(x,y) = y
         */
            double dx = pt.x - x_min;
            double dy = pt.y + m_height;

            pt.x = x_min + dx * dy * m_miter_scale;
        }
    }

    postxf->transform(pt.x, pt.y);
}

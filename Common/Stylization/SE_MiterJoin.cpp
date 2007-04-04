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
#include "SE_LineStorage.h"
#include "stdafx.h"

/* TODO: replace this with something based on the change in the join */
#define POINT_DELTA 1.0

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
    double hw;          /* The symmetrical height, halved */
    double jo;          /* The x distance from the vertex to the end of the outside join */
    double ji;          /* The x distance from the vertex to the end of the inside join */
    double t;           /* The tangent of the half-angle */
    double ca;          /* The cosine of the angle */
    double ch;          /* The cosine of the half-angle */
    double sh;          /* The sine of the half-angle */
    double sa;          /* The sine of the angle */
    double mmax;        /* The length of the miter before the miter limit is applied */
    double mmin;        /* The minimum miter limit (at which a flat bevel is possible) */
    double mlen;        /* The miter length (the miter limit times the outside line thickness) */
    double imprev;      /* The inverse magnitude of the (vert - prev) vector */
    double imnext;      /* The invserse magnitude of the (next - vert) vector */
    RS_F_Point nprev;   /* The normalized (vert - prev) vector */
    RS_F_Point nnext;   /* The normalized (next - vert) vector */
    bool clockwise;     /* If true, then the positive portion of the symbol is on the inside of the join */
    
    V = bounds.minx - vertexOffset;
    clockwise = PointLeft(prev.x, prev.y, vert.x, vert.y, next.x, next.y);
   
    nnext.x = next.x - vert.x;
    nnext.y = next.y - vert.y;
    imnext = 1.0/sqrt(nnext.x*nnext.x + nnext.y*nnext.y);
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
            m_j_bounds.miny = -bounds.maxy;
            m_j_bounds.maxy = -bounds.miny;
        }
        else
        {
            m_j_bounds.miny = bounds.miny;
            m_j_bounds.maxy = bounds.maxy;
        }
        m_j2w.translateX(V);
        m_eRot.translateX(-V);
    }

    /* Rotation should occur about the vertex */
    m_eRot.rotate(nnext.y, nnext.x);
    m_eRot.translate(vert.x, vert.y);
    
    m_eRot.postmultiply(m_j2w);
    m_sRot.postmultiply(m_j2w);

    wn = bounds.miny < 0.0 ? -bounds.miny : 0.0;
    wp = bounds.maxy > 0.0 ? bounds.maxy : 0.0;
    hw = wn > wp ? wn : wp;

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
     * A = hw*hw*sin(pi - theta)                from the sine formula for the area of a triangle
     * A = hw*hw*sin(theta)                     from identity sin(pi - theta) = sin(theta)
     * 
     * A/2 = hw*mmin*sin(pi/2 - theta/2)        from applying the same formula to one half of the (isoceles) triangle
     * A/2 = hw*mmin*cos(theta/2)               from identity sin(pi/2 - theta) = cos(theta)
     * A/2 = hw*mmin*sqrt((1 + cos(theta))/2)   from cos(theta/2) = +/- sqrt((1 + cos(theta)/2), and theta/2 < pi/2, so positive
     * hw*hw*sin(theta)/2 = wp*wmin*sqrt((1 + cos(theta))/2)
     * hw*sa/2 = wmin*sqrt((1+ca)/2)
     * wmin = hw*sa/2/sqrt((1+ca)/2)
     */
    ch = sqrt((1 + ca) / 2.0);
    sh = t * ch;
    mmin = sa / 2.0 / ch;
    mlen = hw * (limit < mmin ? mmin : limit);

    m_height = hw;
    m_width = ji;

    if (mmax > mlen)
    {
        m_bevel_width = mlen * ch;
        m_bevel = mlen * sh;
        m_bevel_scale = m_bevel_width / (m_width *(m_height - m_bevel));
    }
    else
    {
        m_bevel_width = jo;
        m_bevel = hw;
    }
 
    m_miter_scale = (m_bevel_width + m_width) / ((m_bevel + m_height) * m_width);
    
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

void SE_MiterJoin::GetXRadius(double& pre, double& post)
{
    pre = post = m_width;
}

void SE_MiterJoin::ApplyPreTransform(SE_Matrix& prexf)
{
    m_w2j.premultiply(prexf);
}

void SE_MiterJoin::Transform(SE_LineStorage* src, SE_LineStorage* dst, int contour, int ncntrs, bool /* closed */)
{
    int* contours = src->cntrs();
    double* pts = src->points();
    RS_F_Point point;

    for (int i = contour; i < ncntrs; i++)
    {
        double x, y, lx, ly;
        double* last = pts + 2*contours[i];
        dst->EnsureContours(1);
        dst->EnsurePoints(1);
        lx = *pts++;
        ly = *pts++;
        m_w2j.transform(lx, ly);
        point.x = lx;
        point.y = ly;
        _Transform(point);
        dst->_MoveToNoChop(point.x, point.y);

        while (pts < last)
        {
            x = *pts++;
            y = *pts++;
            m_w2j.transform(x, y);

            double dx = x - lx;
            double dy = y - ly;
            double len = sqrt(dx*dx + dy*dy);
            int segs = (int)(len / POINT_DELTA) + 1;
            
            if (segs == 1)
            {
                dst->EnsurePoints(1);
                point.x = x;
                point.y = y;
                _Transform(point);
                dst->_LineToNoChop(point.x, point.y);
            }
            else
            {
                double invsegs = 1.0 / (double)segs;
                dx *= invsegs;
                dy *= invsegs;
                dst->EnsurePoints(segs);
                x = lx;
                y = ly;
                for (int i = 0; i < segs; i++)
                {
                    x += dx;
                    y += dy;
                    point.x = x;
                    point.y = y;
                    _Transform(point);
                    dst->_LineToNoChop(point.x, point.y);
                }
            }

            lx = x;
            ly = y;
        }
    }    
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
        if (m_bevel != m_height && pt.y > m_bevel /*&& pt.y <= m_height*/ )
        {
        /* The bevel transform can be described by the following function:
         * X(x,y) = x + (x - x_min) * (m_height - y) / (m_height - m_bevel) * m_bevel_width / m_width
         *                                            |---------------- m_bevel_scale ----------------|
         * where:
         * dX/dx = 1.0 + (m_height - y) * scale
         * dX/dy = -(x - x_min) * scale
         * d2X/dx2 = 0
         * d2X/dy2 = 0
         * d2X/dxdy = -scale
         * 
         * The transform is constant over Y, so
         * Y(x,y) = y
         */
            double dy = m_height - pt.y;
            double dx = pt.x - x_min;

            pt.x += dx * dy * m_bevel_scale;
        }
        else /* if (pt.y <= m_bevel /*&& pt.y >= -m_height) */
        {

        /* The miter transform can be described by the following function:
         * X(x,y) = x_min + (x - x_min) * (y + m_height) * (m_bevel_width + m_width) / (m_bevel + m_height) / m_width
         *                                             |--------------------- m_miter_scale ---------------------|
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

//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef LINEBUFFER_H
#define LINEBUFFER_H

#include "Stylization.h"
#include <vector>
#include "Bounds.h"
#include "DataValueStack.h"

#define PI2 6.283185307179586476925286766559    //2*pi

//defines how many iterations to use when tesselating
//qudratics, cubics and spirals. We use up to 100 iterations
#define TESSELATION_ITERATIONS 100
#define INV_TESSELATION_ITERATIONS 0.01

class LineBufferPool;
class CSysTransformer;
class RS_OutputStream;

///<summary>
/// Utility class used as intermediary between WKB and DWF entities.
/// The DWF Renderer know how to work on LineBuffers
///</summary>
class LineBuffer
{
public:
    //LineBuffer segment types
    //some day we might actually store curve segments instead of tesselating them
    enum SegType
    {
        stMoveTo = 0,
        stLineTo = 1
        //stQuadTo,
        //stCubicTo
    };

    enum GeomOperationType
    {
        ctNone = 0,
        ctLine = 1,
        ctArea = 2,
        ctPoint = 3,
        ctAGF = 4
    };

public:

    STYLIZATION_API LineBuffer(int size);
    STYLIZATION_API virtual ~LineBuffer();

    //the basic stuff
    STYLIZATION_API void MoveTo(double x, double y);
    STYLIZATION_API void LineTo(double x, double y);
    STYLIZATION_API void Close();

    //the fancy stuff
    STYLIZATION_API void CircularArcTo(double midx, double midy, double endx, double endy);
    STYLIZATION_API void ArcTo(double cx, double cy, double a, double b, double startRad, double endRad);
    //STYLIZATION_API void QuadTo(double x2, double y2, double x3, double y3);
    //STYLIZATION_API void CubicTo(double x2, double y2, double x3, double y3, double x4, double y4);
    //STYLIZATION_API void AddEllipse(double cx, double cy, double a, double b);

    //the ugly stuff
    STYLIZATION_API void LoadFromAgf(unsigned char* RESTRICT data, int sz, CSysTransformer* xformer);
    STYLIZATION_API void ToAgf(RS_OutputStream* os);

    //the cool stuff
    STYLIZATION_API LineBuffer* Optimize(double drawingScale, LineBufferPool* lbp);
    STYLIZATION_API LineBuffer* Clip(RS_Bounds& b, GeomOperationType clipType, LineBufferPool* lbp);
    STYLIZATION_API void Centroid(GeomOperationType type, double* x, double * y, double* slope);

    //clears the buffer for reuse
    STYLIZATION_API void Reset();
    STYLIZATION_API void SetGeometryType(int geomType);

    //the inline stuff
    inline unsigned char* types()       { return m_types; }
    inline double*        points()      { return m_pts; }
    inline int            point_count() { return m_cur_types; }
    inline int            geom_type()   { return m_geom_type; }
    inline int*           cntrs()       { return m_cntrs; }
    inline int            cntr_count()  { return m_cur_cntr + 1; }
    inline const RS_Bounds& bounds()    { return m_bounds; }

private:

    unsigned char* m_types;
    double* m_pts;
    int* m_cntrs;

    double* m_xformbuf;
    int m_xformbuf_len;

    int m_cur_pts;
    int m_cur_types;
    int m_cur_cntr;

    int m_types_len;
    int m_pts_len;
    int m_cntrs_len;

    void Resize();
    void ResizeContours();

    inline void AddToBounds(double x, double y);

    double m_last_x, m_last_y;

    RS_Bounds m_bounds;

    int m_geom_type;

    LineBuffer* ClipPolygon(RS_Bounds& b, LineBuffer* dst);
    LineBuffer* ClipPolyline(RS_Bounds& b, LineBuffer* dst);
    LineBuffer* ClipPoints(RS_Bounds& b, LineBuffer* dst);
    int ClipLine(RS_Bounds& clipRect, double* line, double* RESTRICT ret);
    static int ClipCode(RS_Bounds& b, double x, double y);
    static void AppendLBClipVertex(RS_Bounds& clipRect, double x, double y, LineBuffer* lb, bool move);

    void PolygonCentroid(double* cx, double* cy);
    void PolylineCentroid(double* cx, double* cy, double* slope);
    void MultiPointCentroid(double* cx, double* cy);
    void MultiPolygonCentroid(double* cx, double* cy);
    void MultiPolylineCentroid(double* cx, double* cy, double* slope);

    double CubicApproxParameter(double halfAngle);
    void TesselateCubicTo(double x2, double y2, double x3, double y3, double x4, double y4);
    void TesselateQuadTo(double x2, double y2, double x3, double y3);
};


//---------------------------------------------
// Object pool for line buffers
//---------------------------------------------

class LineBufferPool
{
public:
    STYLIZATION_API virtual ~LineBufferPool();
    STYLIZATION_API LineBuffer* NewLineBuffer(int requestSize);
    STYLIZATION_API void FreeLineBuffer(LineBuffer*);

private:

    DataValueStack<LineBuffer> m_pool;
};

#endif

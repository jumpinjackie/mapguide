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

#ifndef LINEBUFFER_H_
#define LINEBUFFER_H_

#include "StylizationAPI.h"
#include "StylizationDefs.h"
#include "Bounds.h"
#include "DataValueStack.h"
#include "Matrix3D.h"

#ifndef RESTRICT
#ifdef _WIN32
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif
#endif

// defines how many segments to use when tessellating arcs
const int ARC_TESSELLATION_SEGMENTS = 100;

class LineBufferPool;
class CSysTransformer;
class RS_OutputStream;

// A LineBuffer consists of multiple geometries, each geometry
// consists of one or more contours, each contour consists of
// one or more points.
// Points are stored in an array of x,y,z triplets.
// Contours are stored as an array of point counts - i.e.
// contour consisting of a single line segment would have a count of 2
// Geometries are likewise stored as an array of contour counts
// The following is an example of two geometries. The first consists
// of two line segments, and the second a single line segment:
//
// Point Data:     {0,0,0},{1,1,0},{2,2,0},{3,3,0},{4,4,0},{5,5,0}
// Contour Data:   {2, 2, 1}
// Geometry Data:  {2, 1}
//
// If there are arc segments in the geometry that were tessellated,
// then the m_arcs_sp array contains one pair for each tessellated
// arc segment.  The pair contains the vertex indices (into the
// m_pts array) of the start and end segments of the tessellated arc.
//
// If arcs are present in a curve polygon or curve multi-polygon, and
// an extra line segment was added from the end-point of the last arc
// segment of a contour to the first vertex of the contour in order to
// close it, then the index of this segment is added to a "close"
// segment array.  A "close" segment may occur once per contour.  These
// "close" segment indices are added to the m_closeseg array.
class LineBuffer
{
public:
    // LineBuffer segment types.  Some day we might actually store
    // curve segments instead of tessellating them
    enum SegType
    {
        stMoveTo  = 0,
        stLineTo  = 1
//      stQuadTo  = 2,
//      stCubicTo = 3
    };

    enum GeomOperationType
    {
        ctNone  = 0,
        ctLine  = 1,
        ctArea  = 2,
        ctPoint = 3,
        ctAGF   = 4
    };

    //Matches the FGF enumeration FdoDimensionality
    enum Dimensionality
    {
        /// X and Y dimensions are present.
        Dimensionality_XY = 0,
        /// Z dimension is present.
        Dimensionality_Z  = 1,
        /// M ('measure') dimension is present.
        Dimensionality_M  = 2
    };

    //Matches the FGF enumeration FdoGeometryType
    enum GeometryType
    {
        GeometryType_None = 0,
        GeometryType_Point = 1,
        GeometryType_LineString = 2,
        GeometryType_Polygon = 3,
        GeometryType_MultiPoint = 4,
        GeometryType_MultiLineString = 5,
        GeometryType_MultiPolygon = 6,
        GeometryType_MultiGeometry = 7,
        GeometryType_CurveString = 10,
        GeometryType_CurvePolygon = 11,
        GeometryType_MultiCurveString = 12,
        GeometryType_MultiCurvePolygon = 13
    };

    //Matches FdoGeometryComponentType
    enum GeometryComponentType
    {
        GeometryComponentType_LinearRing = 129,
        GeometryComponentType_CircularArcSegment = 130,
        GeometryComponentType_LineStringSegment = 131,
        GeometryComponentType_Ring = 132
    };

    STYLIZATION_API LineBuffer(int size, int dimensionality = Dimensionality_XY, bool bIgnoreZ = true);
    STYLIZATION_API virtual ~LineBuffer();

    // rudimentary stuff
    STYLIZATION_API LineBuffer& operator=(const LineBuffer& src);
    STYLIZATION_API LineBuffer& operator+=(LineBuffer& other);

    // the basic stuff
    STYLIZATION_API void MoveTo(double x, double y, double z=0.0);
    STYLIZATION_API void LineTo(double x, double y, double z=0.0);
    STYLIZATION_API void Close();

    // the fancy stuff
    STYLIZATION_API void CircularArcTo(double midx, double midy, double endx, double endy);
    STYLIZATION_API void CircularArcTo(double midx, double midy, double midz, double endx, double endy, double endz);
    STYLIZATION_API void ArcTo(double cx, double cy, double a, double b, double startRad, double endRad, double rotRad = 0.0);
//  STYLIZATION_API void QuadTo(double x2, double y2, double x3, double y3);
//  STYLIZATION_API void CubicTo(double x2, double y2, double x3, double y3, double x4, double y4);
//  STYLIZATION_API void AddEllipse(double cx, double cy, double a, double b);

    // the ugly stuff
    STYLIZATION_API void LoadFromAgf(unsigned char* RESTRICT data, int sz, CSysTransformer* xformer);
    STYLIZATION_API void ToAgf(RS_OutputStream* os);

    // the cool stuff
    STYLIZATION_API LineBuffer* Optimize(double drawingScale, LineBufferPool* lbp);
    STYLIZATION_API LineBuffer* Clip(RS_Bounds& b, GeomOperationType clipType, LineBufferPool* lbp);
    STYLIZATION_API void Centroid(GeomOperationType type, double* x, double * y, double* slope) const;

    // clears the buffer for reuse
    STYLIZATION_API void Reset(int dimensionality = Dimensionality_XY, bool bIgnoreZ = true);
    STYLIZATION_API void SetGeometryType(int geomType);

    // computes the bounds of the line buffer's geometry
    STYLIZATION_API void ComputeBounds(RS_Bounds& bounds);

    // attributes
    STYLIZATION_API int dimensionality() const;
    STYLIZATION_API bool hasZ() const;
    STYLIZATION_API bool ignoreZ() const;

    // start a new geometry
    STYLIZATION_API void NewGeometry();

    // checks for a point in any contour
    STYLIZATION_API bool PointInPolygon(double& x, double& y) const;
    STYLIZATION_API bool PointInPolygon(int cntr, double& x, double& y) const; // point in specific contour

    // sets the drawing scale (used for arc tessellation)
    STYLIZATION_API void SetDrawingScale(double drawingScale);

    STYLIZATION_API double PolygonArea(int cntr) const;
    STYLIZATION_API double PolygonSignedArea(int cntr) const;
    STYLIZATION_API double PolylineLength(int cntr) const;
    STYLIZATION_API double PolylineLengthSqr(int cntr) const;

    // the inline stuff
    inline unsigned char point_type(int n) const;
    inline int point_count() const;         // number of points in buffer
    inline int point_capacity() const;      // max number of points buffer could hold
    inline int geom_type() const;
    inline int* cntrs() const;
    inline int cntr_size(int cntr) const;
    inline int cntr_count() const;
    inline int* geoms() const;
    inline int geom_count() const;
    inline int geom_size(int geom) const;
    inline const RS_Bounds& bounds() const;
    inline void EnsurePoints(int n);
    inline void EnsureContours(int n);
    inline void EnsureArcsSpArray(int n);
    inline void EnsureCloseSegArray(int n);
    inline void StoreArcStartIndex();
    inline void StoreArcEndIndex();
    inline int contour_start_point(int contour) const;
    inline int contour_end_point(int contour) const;
    inline int arcs_sp_length() const;
    inline int* arcs_sp_array() const;
    inline int closeseg_length() const;
    inline int* closeseg_array() const;
    inline void get_point(int n, double&x, double&y, double& z) const;
    inline void get_point(int n, double&x, double&y) const;
    inline double& x_coord(int n) const;
    inline double& y_coord(int n) const;
    inline double& z_coord(int n) const;
    inline bool contour_closed(int cntr) const;

    // Adds point without checking for available space, updating the bounds, or applying
    // the 3D transform. Thus, Ensure{Points, Contours}, and possibly ComputeBounds must
    // be called (although not NewGeometry).
    inline void UnsafeMoveTo(double x, double y, double z=0.0);
    inline void UnsafeLineTo(double x, double y, double z=0.0);
    inline void UnsafeClose();

    inline void last_point(double& x, double&y, double& z);

protected:
    RS_Bounds m_bounds;

    // empty constructor for use by inheriting classes
    LineBuffer();
    inline void append_segment(SegType type, const double& x, const double& y, const double& z);
    inline void increment_contour();
    inline void increment_contour_pts();

protected:
    unsigned char* m_types;     // segment types array (SegType)
    double (*m_pts)[3];         // raw point arry - triplets x, y, z
    int* m_cntrs;               // contour array
    int* m_csp;                 // contour start points
    int m_cur_types;
    int m_cur_cntr;
    int m_types_len;
    int m_cntrs_len;
    int m_geom_type;
    bool m_bTransform2DPoints;
    Matrix3D m_T;
    int* m_num_geomcntrs;
    int m_num_geomcntrs_len;
    int m_cur_geom;
    bool m_bIgnoreZ;
    bool m_bProcessZ;
    int m_dimensionality;
    double m_drawingScale;
    int m_arcs_sp_len;          // length of m_arcs_sp array
    int m_cur_arcs_sp;          // current index into m_arcs_sp
    int* m_arcs_sp;             // arc start point indices array
    int m_closeseg_len;         // length of m_closeseg array
    int m_cur_closeseg;         // current index into m_closeseg;
    int* m_closeseg;            // closed segment indices array

    void Resize();
    void ResizeContours();
    void ResizeNumGeomContours(int size);
    void ResizeArcsSpArray();
    void ResizeCloseSegArray();

    void AddToBounds(double x, double y, double z = 0.0);

    void CircularArcTo2D(double startx, double starty, double midx, double midy, double endx, double endy);
    void CircularArcTo3D(double startx, double starty, double startz, double midx, double midy, double midz, double endx, double endy, double endz);
    void AdjustArcEndPoint(double x, double y, double z = 0.0);
    void ClipPolygon(RS_Bounds& b, LineBuffer* dst);
    void ClipPolyline(RS_Bounds& b, LineBuffer* dst);
    void ClipPoints(RS_Bounds& b, LineBuffer* dst);
    int ClipLine(RS_Bounds& clipRect, double* line, double* RESTRICT ret);
    static int ClipCode(RS_Bounds& b, double x, double y);
    static void AppendLBClipVertex(RS_Bounds& clipRect, double x, double y, LineBuffer* lb, bool move);
    void FinalizeContour();

    void PolygonCentroid(int cntr, double* cx, double* cy) const; // centroid of specific contour
    void PolygonCentroidTAW(int cntr, double* cx, double* cy) const;
    void PolygonCentroidBVM(int cntr, double* cx, double* cy) const;
    void PolygonCentroidWMC(int cntr, double* cx, double* cy) const;
    void PolylineCentroid(int cntr, double* cx, double* cy, double* slope) const;
    void MultiPointCentroid(double* cx, double* cy) const;
    void MultiPolygonCentroid(double* cx, double* cy) const;
    void MultiPolylineCentroid(double* cx, double* cy, double* slope) const;


    void ResizePoints(int n);    // new size of array # of points
    void ResizeContours(int n);
    void ResizeArcsSpArray(int n);
    void ResizeCloseSegArray(int n);
};


//---------------------------------------------
// Object pool for line buffers
//---------------------------------------------

class LineBufferPool
{
public:
    STYLIZATION_API LineBufferPool();
    STYLIZATION_API virtual ~LineBufferPool();

    STYLIZATION_API static LineBuffer* NewLineBuffer(LineBufferPool* pool, int requestSize, int dimensionality = LineBuffer::Dimensionality_XY, bool bIgnoreZ = true);
    STYLIZATION_API static void FreeLineBuffer(LineBufferPool* pool, LineBuffer* lb);

private:
    DataValueStack<LineBuffer> m_pool;
};


//---------------------------------------------
// inline methods
//---------------------------------------------


unsigned char LineBuffer::point_type(int n) const
{
    return m_types[n];
}


int LineBuffer::point_count() const
{
    return m_cur_types; // number of points in buffer
}


int LineBuffer::point_capacity() const
{
    return m_types_len; // max number of points buffer could hold
}


int LineBuffer::geom_type() const
{
    return m_geom_type;
}


int* LineBuffer::cntrs() const
{
    return m_cntrs;
}


int LineBuffer::cntr_size(int cntr) const
{
    return m_cntrs[cntr];
}


int LineBuffer::cntr_count() const
{
    return m_cur_cntr + 1;
}


int* LineBuffer::geoms() const
{
    return m_num_geomcntrs;
}


int LineBuffer::geom_count() const
{
    return m_cur_geom + 1;
}


int LineBuffer::geom_size(int geom) const
{
    return m_num_geomcntrs[geom];
}


const RS_Bounds& LineBuffer::bounds() const
{
    return m_bounds;
}


void LineBuffer::EnsurePoints(int n)
{
    // need to have space for n additional points
    int needed = point_count() + n;
    // existing array not large enough
    if (needed > m_types_len)
        ResizePoints(2 * needed);
}


void LineBuffer::EnsureContours(int n)
{
    // need to have space for n additional contours
    int needed = cntr_count() + n;
    // existing array not large enough
    if (needed > m_cntrs_len)
        ResizeContours(2 * needed);
}


void LineBuffer::EnsureArcsSpArray(int n)
{
    // need to have space for n additional arc start point indices
    int needed = arcs_sp_length() + n;
    // existing array not large enough
    if (needed > m_arcs_sp_len)
        ResizeArcsSpArray(2 * needed);
}


void LineBuffer::EnsureCloseSegArray(int n)
{
    // need to have space for n additional close segment indices
    int needed = closeseg_length() + n;
    // existing array not large enough
    if (needed > m_closeseg_len)
        ResizeCloseSegArray(2 * needed);
}


// Store off arc start point index, consistent with ArcTo().
void LineBuffer::StoreArcStartIndex()
{
    EnsureArcsSpArray(1);
    m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 1;
}


// Store off arc end point index (want index of start point of last segment),
// consistent with ArcTo().
void LineBuffer::StoreArcEndIndex()
{
    EnsureArcsSpArray(1);
    m_arcs_sp[++m_cur_arcs_sp] = m_cur_types - 2;
}


void LineBuffer::append_segment(SegType type, const double& x, const double& y, const double& z)
{
    m_pts[m_cur_types][0] = x;
    m_pts[m_cur_types][1] = y;
    m_pts[m_cur_types][2] = z;
    m_types[m_cur_types++] = (unsigned char)type;
}


void LineBuffer::increment_contour()
{
    // this routine is always called after the first point of the new contour
    // has been added via append_segment
    m_cntrs[++m_cur_cntr] = 1; //increment to next contour and count the point
    m_csp[m_cur_cntr] = m_cur_types-1; // record contour start point
}


void LineBuffer::increment_contour_pts()
{
    m_cntrs[m_cur_cntr]++;
}


void LineBuffer::last_point(double& x, double&y, double& z)
{
    x = m_pts[m_cur_types-1][0];
    y = m_pts[m_cur_types-1][1];
    z = m_pts[m_cur_types-1][2];
}


int LineBuffer::contour_start_point(int contour) const
{
    return m_csp[contour];
}


int LineBuffer::contour_end_point(int contour) const
{
    return m_csp[contour] + m_cntrs[contour] - 1;
}


int LineBuffer::arcs_sp_length() const
{
    return m_cur_arcs_sp + 1;
}


int* LineBuffer::arcs_sp_array() const
{
    return m_arcs_sp;
}


int LineBuffer::closeseg_length() const
{
    return m_cur_closeseg + 1;
}


int* LineBuffer::closeseg_array() const
{
    return m_closeseg;
}


void LineBuffer::get_point(int n, double&x, double&y, double& z) const
{
    x = m_pts[n][0];
    y = m_pts[n][1];
    z = m_pts[n][2];
}


void LineBuffer::get_point(int n, double&x, double&y) const
{
    x = m_pts[n][0];
    y = m_pts[n][1];
}


double& LineBuffer::x_coord(int n) const
{
    return m_pts[n][0];
}


double& LineBuffer::y_coord(int n) const
{
    return m_pts[n][1];
}


double& LineBuffer::z_coord(int n) const
{
    return m_pts[n][2];
}


bool LineBuffer::contour_closed(int cntr) const
{
    int first(contour_start_point(cntr));
    int last(contour_end_point(cntr));
    return (x_coord(first) == x_coord(last)) &&
           (y_coord(first) == y_coord(last)) &&
           (!m_bProcessZ || (z_coord(first) == z_coord(last))) ;
}


void LineBuffer::UnsafeMoveTo(double x, double y, double z)
{
    append_segment(stMoveTo, x, y, z);
    increment_contour();

    if (m_cur_geom < 0)
        NewGeometry();

    m_num_geomcntrs[m_cur_geom] += 1;

    // this may be unsafe, but in the debug build, you won't err unknowingly
    _ASSERT(m_cur_types <= m_types_len);
    _ASSERT(m_cur_cntr < m_cntrs_len);
    _ASSERT(m_cur_geom < m_num_geomcntrs_len);
}


void LineBuffer::UnsafeLineTo(double x, double y, double z)
{
    append_segment(stLineTo, x, y, z);
    increment_contour_pts();

    // this may be unsafe, but in the debug build, you won't err unknowingly
    _ASSERT(m_cur_types <= m_types_len);
}


void LineBuffer::UnsafeClose()
{
    // find out if it's already closed
    if (contour_closed(m_cur_cntr))
        return;

    // store off close segment index
    m_closeseg[++m_cur_closeseg] = m_cur_types - 1;

    int cntr_start = m_csp[m_cur_cntr];
    UnsafeLineTo(x_coord(cntr_start), y_coord(cntr_start), z_coord(cntr_start));

    // this may be unsafe, but in the debug build, you won't err unknowingly
    _ASSERT(m_cur_closeseg <= m_closeseg_len);
}

#endif

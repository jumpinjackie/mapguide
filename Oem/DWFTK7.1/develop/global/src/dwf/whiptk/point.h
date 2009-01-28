//
//  Copyright (C) 1996-2009 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//

#if !defined POINT_HEADER
#define POINT_HEADER

/// 3D point (using doubles) used in 4x4 matrix calculations.
/** \sa WT_Matrix */
class WHIPTK_API WT_Point3D
{
public:
    double        m_x; /**< Magnitude of point along the X-axis. */
    double        m_y; /**< Magnitude of point along the Y-axis. */
    double        m_z; /**< Magnitude of point along the Z-axis. */

    /// Constructs a WT_Point3D object.
    WT_Point3D() {}

    /// Constructs a WT_Point3D object with the given coordinates.
    WT_Point3D(double x, double y, double z = 0.0)
        : m_x (x)
        , m_y (y)
        , m_z (z)
    { }

    /// Returns WD_True if the two points are equal.
    WT_Boolean operator== (WT_Point3D const & p3d) const
    {
        if (m_x == p3d.m_x && m_y == p3d.m_y && m_z == p3d.m_z)
            return WD_True;
        else
            return WD_False;
    }
};

/// 2D point (using doubles) used in 3x3 matrix calculations.
/** \sa WT_Matrix2D */
class WHIPTK_API WT_Point2D
{
public:
    double        m_x; /**< Magnitude of point along the X-axis. */
    double        m_y; /**< Magnitude of point along the Y-axis. */

    /// Constructs a WT_Point2D object.
    WT_Point2D() {}

    /// Constructs a WT_Point2D object with the given coordinates.
    WT_Point2D(double x, double y)
        : m_x (x)
        , m_y (y)
    { }

    /// Returns WD_True if the two points are equal.
    WT_Boolean operator== (WT_Point2D const & p3d) const
    {
        if (m_x == p3d.m_x && m_y == p3d.m_y )
            return WD_True;
        else
            return WD_False;
    }
};

#endif // POINT_HEADER

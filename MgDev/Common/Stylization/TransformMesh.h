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

#ifndef TRANSFORMMESH_H_
#define TRANSFORMMESH_H_

#include "StylizationAPI.h"
#include "RendererStyles.h"

#ifndef RESTRICT
#ifdef _WIN32
#define RESTRICT __restrict
#else
#define RESTRICT __restrict__
#endif
#endif

class CSysTransformer;
struct RS_Bounds;

struct MeshPoint
{
    RS_F_Point pt_src;
    RS_F_Point pt_dest;
};


// A TransformMesh...
//
class TransformMesh
{
public:
    STYLIZATION_API TransformMesh();
    STYLIZATION_API TransformMesh(int gridSize, int minGridSize, double gridSizeOverrideRatio,
        RS_Bounds& srcExt, int srcW, int srcH,
        RS_Bounds& destExt, int destW, int destH,
        CSysTransformer* srcToDestTransform, bool invertYAxis = true);

    STYLIZATION_API virtual ~TransformMesh();

    STYLIZATION_API void Initialize(int gridSize, int minGridSize, double gridSizeOverrideRatio,
        RS_Bounds& srcExt, int srcW, int srcH,
        RS_Bounds& destExt, int destW, int destH,
        CSysTransformer* srcToDestTransform, bool invertYAxis = true);

    STYLIZATION_API MeshPoint GetMeshPoint(int index);

    // the inline stuff
    STYLIZATION_API int GetTotalPoints() const;            // number of points
    STYLIZATION_API int GetTotalVerticalPoints() const;    // number of vertical points (incl origin and end point)
    STYLIZATION_API int GetTotalHorizontalPoints() const;  // number of horizontal points (incl origin and end point)
    STYLIZATION_API bool IsYAxisInverted() const;          // returns true if y axis is inverted
    STYLIZATION_API int GetGridSizeHeight() const;         // grid size height used in calculations
    STYLIZATION_API int GetGridSizeWidth() const;          // grid size width used in calculations

private:
    int m_numVerticalPoints;
    int m_numHorizontalPoints;
    int m_gridSizeHeight;
    int m_gridSizeWidth;
    int m_minGridSize;
    double m_gridSizeOverrideRatio;
    bool m_yAxisInverted;

    std::vector<MeshPoint> m_meshPoints;
};


//---------------------------------------------
// inline methods
//---------------------------------------------

inline int TransformMesh::GetTotalPoints() const
{
    return (int)m_meshPoints.size();
}

inline int TransformMesh::GetTotalVerticalPoints() const
{
    return m_numVerticalPoints;
}

inline int TransformMesh::GetTotalHorizontalPoints() const
{
    return m_numHorizontalPoints;
}

inline bool TransformMesh::IsYAxisInverted() const
{
    return m_yAxisInverted;
}

inline int TransformMesh::GetGridSizeHeight() const
{
    return m_gridSizeHeight;
}

inline int TransformMesh::GetGridSizeWidth() const
{
    return m_gridSizeWidth;
}
#endif

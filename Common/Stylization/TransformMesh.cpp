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
#include "StylizationDefs.h"
#include "Bounds.h"
#include "TransformMesh.h"
#include "CSysTransformer.h"
#include <float.h>


// Default grid size in device space pixels
const int DEFAULT_GRID_SIZE = 100;

// Default minimum grid size in device space pixels
const int DEFAULT_MIN_GRID_SIZE = 10;

// Default, max, and min grid size override ratios
// The grid size must be less than the specified ratio of the image height/width
const double DEFAULT_GRID_SIZE_OVERRIDE_RATIO = 0.25;
const double MAX_GRID_SIZE_OVERRIDE_RATIO = 1.0;
const double MIN_GRID_SIZE_OVERRIDE_RATIO = 0.0;

// The maximum amount we will stretch a rectangle in the grid to allow it to
// snap to the edge of the extents
const double MAX_GRID_EXPANSION_FACTOR = 0.2; // i.e. 20%

#define PRINT_RS_BOUNDS(ext) \
    printf("      minx = %6.4f miny = %6.4f maxx = %6.4f maxy = %6.4f\n", ext.minx, ext.miny, ext.maxx, ext.maxy); \
    printf("      width = %6.4f height = %6.4f\n", ext.width(), ext.height());

TransformMesh::TransformMesh()
: m_numVerticalPoints(0), m_numHorizontalPoints(0), m_gridSizeHeight(DEFAULT_GRID_SIZE), m_gridSizeWidth(DEFAULT_GRID_SIZE),
m_minGridSize(DEFAULT_MIN_GRID_SIZE), m_gridSizeOverrideRatio(DEFAULT_GRID_SIZE_OVERRIDE_RATIO), m_yAxisInverted(true)
{
}

TransformMesh::TransformMesh(int gridSize, int minGridSize, double gridSizeOverrideRatio,
                             RS_Bounds& srcExt, int srcW, int srcH,
                             RS_Bounds& destExt, int destW, int destH,
                             CSysTransformer* srcToDestTransform,
                             bool invertYaxis)
: m_numVerticalPoints(0), m_numHorizontalPoints(0)
{
    Initialize(gridSize, minGridSize, gridSizeOverrideRatio,
        srcExt, srcW, srcH,
        destExt, destW, destH,
        srcToDestTransform, invertYaxis);
}

void TransformMesh::Initialize(int gridSize, int minGridSize, double gridSizeOverrideRatio,
                               RS_Bounds& srcExt, int srcW, int srcH,
                               RS_Bounds& destExt, int destW, int destH,
                               CSysTransformer* xformer,
                               bool invertYaxis)
{
    _ASSERT(xformer != NULL);
    _ASSERT(gridSize > 0 && minGridSize > 0 && srcW > 0 && srcH > 0 && destW > 0 && destH > 0);
    _ASSERT(gridSizeOverrideRatio >= MIN_GRID_SIZE_OVERRIDE_RATIO && gridSizeOverrideRatio <= MAX_GRID_SIZE_OVERRIDE_RATIO);

    // initialize members
    m_minGridSize = minGridSize;
    m_gridSizeOverrideRatio = gridSizeOverrideRatio;

    int calculatedGridSize = gridSize;

    // if the override is enabled (0 < ratio < 1), then adjust if necessary
    if (gridSizeOverrideRatio < MAX_GRID_SIZE_OVERRIDE_RATIO
        && gridSizeOverrideRatio > MIN_GRID_SIZE_OVERRIDE_RATIO)
    {
        // the calculated grid size is the minimum of the passed in grid size and the overrided grid size
        calculatedGridSize = rs_min(gridSize, (int)(rs_min(srcH, srcW) * m_gridSizeOverrideRatio));
    }

    // we check to make sure the minGridSize is less that gridSize
    // -- in case we are given a minGridSize that is greater than the gridSize
    // ensure grid size is not less than the minimum
    if (minGridSize < gridSize)
    {
        calculatedGridSize = rs_max(minGridSize, calculatedGridSize);
    }

    // ensure grid size is not bigger than the source image's height and width
    m_gridSizeHeight = rs_min(calculatedGridSize, srcH);
    m_gridSizeWidth = rs_min(calculatedGridSize, srcW);


    m_yAxisInverted = invertYaxis;
    m_meshPoints.empty();
    m_numVerticalPoints = 0;
    m_numHorizontalPoints = 0;

    double pixPerSrcUnitX = (double)srcW / srcExt.width();
    double pixPerSrcUnitY = (double)srcH / srcExt.height();
    double pixPerDestUnitX = (double)destW / destExt.width();
    double pixPerDestUnitY = (double)destH / destExt.height();

    // Create a grid in pixel space that covers the whole src extent
    for (int gridX = 0; gridX < srcW + m_gridSizeWidth; gridX += m_gridSizeWidth)
    {
        ++m_numHorizontalPoints;

        // this sets the x-coordinate to the end pt if it gets close
        if (gridX + MAX_GRID_EXPANSION_FACTOR * m_gridSizeWidth > srcW || gridX > srcW)
        {
            gridX = srcW;
        }

        for (int gridY = 0; gridY < srcH + m_gridSizeHeight; gridY += m_gridSizeHeight)
        {
            if (gridX == 0)
                ++m_numVerticalPoints;

            // this sets the y-coordinate to the end pt if it gets close
            if (gridY + MAX_GRID_EXPANSION_FACTOR * m_gridSizeHeight > srcH || gridY > srcH)
                gridY = srcH;

            // The point in pixel coordinates in the src image
            RS_F_Point src_pt0(gridX, gridY);

            // Convert from screen coordinates to src CS coordinates (note that the screen coords are inverted because the
            // invert y axis based upon the setting
            RS_F_Point srcCS_pt0(src_pt0.x / pixPerSrcUnitX + srcExt.minx,
                (m_yAxisInverted ? (srcH - src_pt0.y) : src_pt0.y) / pixPerSrcUnitY + srcExt.miny);

            // Convert from src cs to dest cs coordinates
            RS_F_Point destCS_pt0(srcCS_pt0.x, srcCS_pt0.y);
            xformer->TransformPoint(destCS_pt0.x, destCS_pt0.y);

            // Convert from dest CS coordinates to dest pixel coordinates.
            RS_F_Point dest_pt0((destCS_pt0.x - destExt.minx) * pixPerDestUnitX,
                (destCS_pt0.y - destExt.miny) * pixPerDestUnitY);

            // Store the point mapping in the transform mesh
            MeshPoint mesh_pt;
            mesh_pt.pt_src = src_pt0;
            mesh_pt.pt_dest = dest_pt0;
            m_meshPoints.push_back(mesh_pt);
        }
    }
}

TransformMesh::~TransformMesh()
{
}

MeshPoint TransformMesh::GetMeshPoint(int index)
{
    return m_meshPoints[index];
}

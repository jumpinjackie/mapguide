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

#ifndef OBSERVATIONMESH_H
#define OBSERVATIONMESH_H

#include "Renderers.h"

//disable conditional expression is constant warning
#ifdef _WIN32
#pragma warning(disable : 4127)
#endif

class CSysTransformer;
struct RS_Bounds;
class RS_OutputStream;


struct ObsPoint
{
    double x;
    double y;
    double ox;
    double oy;
};


struct ObsFace
{
    //indices into points array for the 3 points of
    //this face
    int p[3];

    //indices of neighboring faces in the face list
    int n[3];

    double cx;
    double cy;
    double sqradius;

    ObsFace()
    {
        p[0] = p[1] = p[2] = n[0] = n[1] = n[2] = -1;
        cx = cy = 0.0;
        sqradius = 0.0;
    }

    bool has_point(int index)
    {
        if (p[0] == index || p[1] == index || p[2] == index)
            return true;

        return false;
    }
};


struct HullEdge
{
    HullEdge(int p, int f)
    {
        pt = p;
        face = f;
    }

    HullEdge()
    {
        pt = -1;
        face = -1;
    }

    int pt;
    int face;
    int outface;
};


/////////////////////////////////////////////////////////////////////////
// Utility class used for generation ov so called "observation meshes"
// We use these thingies to provide the DWF Viewer with a way to transform
// from mapping space coordinates to LatLon coordinates. The observation
// mesh is essentially a triangulated irregular network (TIN) with extra
// data stored for each x,y point.
/////////////////////////////////////////////////////////////////////////
class ObservationMesh
{
public:
    ObservationMesh(RS_Bounds& mapExtent, CSysTransformer* xformer);
    virtual ~ObservationMesh();

    void ProcessPoint(double x, double y);

    void WriteXml(RS_OutputStream* os);

private:
    bool PointInTriangle(double px, double py, ObsFace& f);

    void ComputeCircle(ObsFace& f);

    void SplitFaces(std::vector<int>& face_indices, int ptindex);

    void ComputeNewFaces(std::vector<int>& face_indices);

    int FindNeighborOnEdge(ObsFace& f, int p0, int p1);

    void AddPoint(double x, double y);
    void AddFace(int i1, int i2, int i3, int ni1, int ni2, int ni3);
    void CollectFaces(ObsFace& startFace, int faceIndex, std::vector<ObsFace>);

    std::vector<ObsPoint> m_vPts;
    std::vector<ObsFace>  m_vFaces;

    CSysTransformer* m_xformer;
};

#endif

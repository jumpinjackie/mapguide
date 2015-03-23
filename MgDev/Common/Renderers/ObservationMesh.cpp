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
#include "Bounds.h"
#include "ObservationMesh.h"
#include "CSysTransformer.h"
#include "RS_OutputStream.h"
#include <map>
#include <cstring>
#include <cstdio>

//desired observation mesh accuracy (in degrees)
#define OM_TOLERANCE 1e-5
#define OM_TOLERANCE_SQ 1e-10

#define MAX_POINTS 1000


ObservationMesh::ObservationMesh(RS_Bounds& mapExtent, CSysTransformer* xformer)
{
    //remember the mapping space -> latlon tranformer
    _ASSERT(xformer != NULL);
    m_xformer = xformer;

    m_vPts.reserve(8);
    m_vFaces.reserve(8);

    //now start by adding the 4 corners of the map to the point list
    AddPoint(mapExtent.minx, mapExtent.miny);
    AddPoint(mapExtent.maxx, mapExtent.miny);
    AddPoint(mapExtent.maxx, mapExtent.maxy);

    //shift last point a little to avoid coincident circumscribed circles
    //which confuse the DeLaunay triangulation
    AddPoint(mapExtent.minx, mapExtent.maxy + mapExtent.height() * 1e-3);

    //make two faces
    AddFace(0, 1, 2, -1, -1, 1); //only one neighbor for this face, the other two are -1 (undefined)
    AddFace(2, 3, 0, -1, -1, 0); //one neighbor -> the face we added first
}

ObservationMesh::~ObservationMesh()
{
}

//Add this point to the Mesh.
void ObservationMesh::ProcessPoint(double x, double y)
{
    //limit size of mesh
    if (m_vPts.size() >= MAX_POINTS)
        return;


    //find DeLaunay circles that this point is inside of
    //do a simple linear search through the list of faces.
    //This can be made faster by use of an R-Tree or a DAG (with edge weights)
    //apparently, according to some googling I did

    std::vector<int> intersecting_faces;
    int inside_face = -1;

    for (unsigned i=0; i<m_vFaces.size(); i++)
    {
        ObsFace f = m_vFaces[i];

        double dx = f.cx - x;
        double dy = f.cy - y;
        double distsq = dx*dx + dy*dy;

        if (distsq <= f.sqradius)
        {
            intersecting_faces.push_back(i);

            //also check if it is inside the triangle
            //so that we can see how close the interpolation is
            if (inside_face < 0)
            {
                if (PointInTriangle(x, y, f))
                    inside_face = i;
            }
        }
    }

    //now interpolate and see how close we are to the actual value
    //using the current interpolation mesh
    if (inside_face >= 0)
    {
        ObsFace face = m_vFaces[inside_face];

        ObsPoint pt0 = m_vPts[face.p[0]];
        ObsPoint pt1 = m_vPts[face.p[1]];
        ObsPoint pt2 = m_vPts[face.p[2]];

        //compute barycentric coordinates
        //see http://www.library.uu.nl/digiarchief/dip/diss/2003-1028-125323/appd.pdf for details
        //Basically inlined solution to a system of linear equations to find the
        //barycentric coordinates a0, a1 and a2. The system is
        //
        //       N
        //       --
        //       \   ai (pi - p0) = p - p0
        //       /
        //       --
        //       i=1
        //
        // For N=2 and where p is the observation point and pi are the triangle points
        //
        double dx10 = pt1.x - pt0.x;
        double dxp0 = x - pt0.x;
        double dx20 = pt2.x - pt0.x;

        double dy10 = pt1.y - pt0.y;
        double dyp0 = y - pt0.y;
        double dy20 = pt2.y - pt0.y;

        double denom = (dy20 * dx10 - dy10 * dx20);

        double a2 = 0.0;

        if (denom != 0.0)
            a2 = (dyp0 * dx10 - dy10 * dxp0) / denom;
        else
        {
            double denomx = (dx20 * dy10 - dx10 * dy20);

            if (denomx != 0.0)
                a2 = (dxp0 * dy10 - dx10 * dyp0) / denomx;
            else
                a2 = 0.0;
        }

        double a1 = 0.0;
        if (dx10 != 0.0)
            a1 = (dxp0 - a2 * dx20) / dx10;
        else if (dy10 != 0.0)
            a1 = (dyp0 - a2 * dy20) / dy10;
        else
            a1 = 0.0;

        double a0 = 1.0 - a1 - a2;

        //get interpolated lat lon value
        double int_x = a0 * pt0.ox + a1 * pt1.ox + a2 * pt2.ox;
        double int_y = a0 * pt0.oy + a1 * pt1.oy + a2 * pt2.oy;

        ObsPoint pt;
        pt.x = pt.ox = x;
        pt.y = pt.oy = y;

        if (m_xformer)
            m_xformer->TransformPoint(pt.ox, pt.oy);

        double dxll = int_x - pt.ox;
        double dyll = int_y - pt.oy;

        double sqrdist = dxll*dxll + dyll*dyll;

        //add the point to the mesh if the approximation given by the mesh
        //is unsatisfactory
        if (sqrdist > OM_TOLERANCE_SQ)
        {
            AddPoint(x, y);
            ComputeNewFaces(intersecting_faces);
        }
    }
}


void ObservationMesh::ComputeNewFaces(std::vector<int>& face_indices)
{
    //huh, this could be more efficient
    std::map<int, std::vector<HullEdge> > edges;

    //find edges that will form the outside edges
    //we will then connect each of them to the new point
    //to form a new triangle
    for (unsigned i=0; i<face_indices.size(); i++)
    {
        int face_index = face_indices[i];
        ObsFace f = m_vFaces[face_index];

        for (int k=0; k<3; k++)
        {
            int ptindex0 = f.p[k % 3];
            int ptindex1 = f.p[(k+1)%3];

            //find outside neighbor face for this edge
            //if we set neighbors intelligently we might
            //not have to look through all 3 neighbors
            //but for now we do
            int outface = -1;
            for (int k=0; k<3; k++)
            {
                if (f.n[k] >=0)
                {
                    ObsFace nei = m_vFaces[f.n[k]];
                    if (nei.has_point(ptindex0) && nei.has_point(ptindex1))
                        outface = f.n[k];
                }
            }

            //add forward edge
            std::vector<HullEdge>& ep0 = edges[ptindex0];
            unsigned j;
            for (j=0; j<ep0.size(); j++)
            {
                if (ep0[j].pt == ptindex1)
                {
                    ep0[j].pt = -ptindex1 - 2;
                    break;
                }
            }

            if (j==ep0.size())
            {
                HullEdge edge;
                edge.pt = ptindex1;
                edge.face = face_index;
                edge.outface = outface;
                ep0.push_back(edge);
            }

            //add reverse edge
            //int j;
            std::vector<HullEdge>& ep1 = edges[ptindex1];
            for (j=0; j<ep1.size(); j++)
            {
                if (ep1[j].pt == ptindex0)
                {
                    ep1[j].pt = -ptindex0 - 2;
                    break;
                }
            }

            if (j==ep1.size())
            {
                HullEdge edge;
                edge.pt = ptindex0;
                edge.face = face_index;
                edge.outface = outface;
                ep1.push_back(edge);
            }
        }
    }


    //find an edge we can start with generating new faces
    int p0i=-1;

    for (std::map<int, std::vector<HullEdge> >::iterator iter = edges.begin();
        iter != edges.end(); ++iter)
    {
        std::vector<HullEdge>& ei = iter->second;
        for (unsigned j=0; j<ei.size(); j++)
        {
            if (ei[j].pt >= 0)
            {
                p0i = iter->first;
                goto done;
            }
        }
    }

done:

    int prev_face_index = -1;
    int first_face_index = -1;

    //iterate over all the edges we are using for
    //new triangles and for each one make a triangle that
    //goes through the new point
    while (true)
    {
        HullEdge edge;

        unsigned j;

        std::vector<HullEdge>& ep0i = edges[p0i];

        for (j=0; j<ep0i.size(); j++)
        {
            if (ep0i[j].pt >= 0)
            {
                edge = ep0i[j];
                break;
            }
        }

        //are we done
        if (j==ep0i.size())
        {
            //set first neighbor of first face to equal the
            //last face we processed
            m_vFaces[first_face_index].n[0] = prev_face_index;

            //set the last neighbor of the last face to equal
            //the first face
            m_vFaces[prev_face_index].n[2] = first_face_index;

            //and then we are done
            break;
        }

        //mark edge as processed
        ep0i[j].face = -1;
        ep0i[j].pt = -1;

        int p1i = edge.pt;

        std::vector<HullEdge>& ep1i = edges[p1i];

        //mark reverse edge as processed also so that we don't hit it again?
        for (j = 0; j<ep1i.size(); j++)
        {
            if (ep1i[j].pt == p0i)
            {
                ep1i[j].pt = -1;
                ep1i[j].face = -1;
                break;
            }
        }

        if (first_face_index < 0)
            first_face_index = edge.face;

        ObsFace face = m_vFaces[edge.face];

        //has face already been updated? Then need to add to back of face list
        if (face.has_point((int)m_vPts.size()-1))
        {
            face.p[0] = p0i;
            face.p[1] = p1i;
            face.p[2] = (int)m_vPts.size() - 1;

            //set neighbors...
            int this_face_index = (int)m_vFaces.size();

            //in this case we also need to update outside neighbor
            //face since the index of this face changes
            if (edge.outface >= 0)
            {
                ObsFace& f = m_vFaces[edge.outface];
                for (int r=0; r<3; r++)
                    if (f.n[r] == edge.face)
                        f.n[r] = this_face_index;
            }


            face.n[0] = prev_face_index;
            face.n[1] = edge.outface;

            if (prev_face_index >=0)
            {
                //set last face index of previously processed face
                m_vFaces[prev_face_index].n[2] = this_face_index;
            }

            //add to back of face list
            ComputeCircle(face);
            prev_face_index = this_face_index;
            m_vFaces.push_back(face);

        }
        else
        {
            for (int k=0; k<3; k++)
            {
                if (face.p[k] != p0i && face.p[k] != p1i)
                    face.p[k] = (int)m_vPts.size()-1;
            }

            //set neighbors...
            //in this case we do not have to update the outside neighbor
            //since the index of this face does not change
            int this_face_index = edge.face;

            face.n[0] = prev_face_index;
            face.n[1] = edge.outface;

            if (prev_face_index >=0)
            {
                //set last face index of previously processed face
                m_vFaces[prev_face_index].n[2] = this_face_index;
            }

            //just update face in situ
            ComputeCircle(face);
            m_vFaces[edge.face] = face;
            prev_face_index = this_face_index;
        }

        p0i = p1i;
    }

    //there is a chance that some faces need to be completely removed
    //from the list instead of updates. we look for them as an
    //afterstep here

    //first find the face indices to remove
    //not that since face_indices is sorted,
    //so is toremove
    std::vector<int> toremove;
    for (unsigned i=0; i<face_indices.size(); i++)
    {
        int face_index = face_indices[i];

        ObsFace face = m_vFaces[face_index];

        if (!face.has_point((int)m_vPts.size() - 1))
            toremove.push_back(face_index);
    }


    //then go through and do it, by moving faces
    //from the end of the face list into their positions
    //go in reverse order
    for (int p=(int)toremove.size()-1; p>=0; --p)
    {
        int dead_face_index = toremove[p];
        if (dead_face_index == (int)m_vFaces.size()-1)
        {
            //simple case where we are removing the last face
            //in the face list
            m_vFaces.pop_back();
        }
        else
        {
            //otherwise move the last face into the dead one
            ObsFace last_face = m_vFaces.back();
            m_vFaces.pop_back();

            //update neighbors of face we are moving
            for (int k=0; k<3; k++)
            {
                if (last_face.n[k] >= 0)
                {
                    ObsFace& f = m_vFaces[last_face.n[k]];
                    for (int l=0; l<3; l++)
                        if (f.n[l] == (int)m_vFaces.size())
                            f.n[l] = dead_face_index;
                }
            }

            //now move the face to a new location
            m_vFaces[dead_face_index] = last_face;
        }
    }
}

void ObservationMesh::AddPoint(double x, double y)
{
    ObsPoint pt;
    pt.x = pt.ox = x;
    pt.y = pt.oy = y;

    if (m_xformer)
        m_xformer->TransformPoint(pt.ox, pt.oy);

    m_vPts.push_back(pt);
}

void ObservationMesh::AddFace(int i1, int i2, int i3, int ni1, int ni2, int ni3)
{
    ObsFace f;
    f.p[0] = i1;
    f.p[1] = i2;
    f.p[2] = i3;

    f.n[0] = ni1;
    f.n[1] = ni2;
    f.n[2] = ni3;

    ComputeCircle(f);

    m_vFaces.push_back(f);
}

void ObservationMesh::WriteXml(RS_OutputStream* os)
{
    //TODO: xml serialization should eventually move to the DWF EMap toolkit.

    char* header = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<observation from=\"emap:mcs\" to=\"lat,lon:EPSG:4326\" coverage=\"barycentric\">\r\n";
    os->write(header, strlen(header));

    char buf[256];

    sprintf(buf, "<points>\r\n");
    os->write(buf, strlen(buf));

    for (size_t i=0; i<m_vPts.size(); i++)
    {
        ObsPoint pt = m_vPts[i];
        sprintf(buf, "\t<point x=\"%lf\" y=\"%lf\">%lf, %lf</point>\r\n", pt.x, pt.y, pt.ox, pt.oy);
        os->write(buf, strlen(buf));
    }

    sprintf(buf, "</points>\r\n");
    os->write(buf, strlen(buf));

    sprintf(buf, "<surface>\r\n");
    os->write(buf, strlen(buf));

    for (size_t i=0; i<m_vFaces.size(); i++)
    {
        ObsFace face = m_vFaces[i];
        //note face indices are 1-based for observation mesh so we have to add 1
        sprintf(buf, "\t<trianglestrip>%d %d %d</trianglestrip>\r\n", face.p[0]+1, face.p[1]+1, face.p[2]+1);
        os->write(buf, strlen(buf));
    }

    sprintf(buf, "</surface>\r\n</observation>\r\n");
    os->write(buf, strlen(buf));
}

void ObservationMesh::ComputeCircle(ObsFace& f)
{
    ObsPoint p0 = m_vPts[f.p[0]];
    ObsPoint p1 = m_vPts[f.p[1]];
    ObsPoint p2 = m_vPts[f.p[2]];

    //now we can compute the circle that those 3 points describe
    double dx1 = p1.x - p0.x;
    double dy1 = p1.y - p0.y;
    double dx2 = p2.x - p0.x;
    double dy2 = p2.y - p0.y;

    double area = 0.5 * (dx1 * dy2 - dx2 * dy1);

    if (area < 0.0)
    {
        //_ASSERT(false);
        //TODO do we actually need to do this?:
        //if CCW, reverse d1 and d2
    }

    double sqLen10 = dx1 * dx1 + dy1 * dy1;
    double sqLen20 = dx2 * dx2 + dy2 * dy2;
    double invArea = 1.0 / area;

    //find center point and radius of circuscribing circle using
    //formulas from Geometric Tools (Schneider & Eberly)

    double cx = p0.x + 0.25 * invArea * (dy2 * sqLen10 - dy1 * sqLen20);
    double cy = p0.y + 0.25 * invArea * (dx1 * sqLen20 - dx2 * sqLen10);

    double rdx = cx - p0.x;
    double rdy = cy - p0.y;

    f.cx = cx;
    f.cy = cy;
    f.sqradius = rdx*rdx + rdy*rdy;
}


bool ObservationMesh::PointInTriangle(double px, double py, ObsFace& f)
{
    ObsPoint pt0 = m_vPts[f.p[0]];
    ObsPoint pt1 = m_vPts[f.p[1]];
    ObsPoint pt2 = m_vPts[f.p[2]];

    bool code1 = (pt1.x - pt0.x)*(py - pt0.y) - (pt1.y - pt0.y)*(px - pt0.x) >= 0.0;
    bool code2 = (pt2.x - pt1.x)*(py - pt1.y) - (pt2.y - pt1.y)*(px - pt1.x) >= 0.0;

    if (code1 ^ code2)
        return false;

    bool code3 = (pt0.x - pt2.x)*(py - pt2.y) - (pt0.y - pt2.y)*(px - pt2.x) >= 0.0;

    if (code1 ^ code3)
        return false;

    return true;
}

//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

class LineBuffer;

class Centroid
{

public:

    static bool PointInPolygon(double* pointData, int numPointData, int* contourSizes, int numContourSizes, double x, double y );
    static double PolylineLength(double* pts, int numpts);
    static double PolygonArea(double* pts, int numpts);
    static void PolylineCentroid(double* pts, int numpts, double* cx, double* cy, double* slope);
    static void PolygonCentroid(double* pts, int numpts, double* cx, double* cy);
    static void PolygonCentroidTAW(double* pts, int len, double* cx, double* cy);
    static void PolygonCentroidBVM(double* pts, int len, double* cx, double* cy);
    static void PolygonCentroidWMC(double* pts, int len, double* cx, double* cy);
};

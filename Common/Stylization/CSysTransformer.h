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

#ifndef CSYSTRANSFORMER_H_
#define CSYSTRANSFORMER_H_

///<summary>
///defines a simple interface for a 2D tranformation function.
///Used for coordinate system transformations API.
///</summary>
class CSysTransformer
{
public:
    //x and y coordinates of point to be transformed.
    //Result will be stored in inputs which will be overwirtten
    virtual void TransformPoint(double& x, double& y) = 0;

    virtual void TransformPoints(int numPts, double* x, double* y) = 0;

    // We provide a simple, default implementation of TransformExtent
    virtual void TransformExtent(double& minX, double& minY, double& maxX, double& maxY)
    {
        double xValues[4];
        double yValues[4];

        // Lower left
        xValues[0] = minX;
        yValues[0] = minY;

        // Lower right
        xValues[1] = maxX;
        yValues[1] = minY;

        // Upper left
        xValues[2] = minX;
        yValues[2] = maxY;

        // Upper right
        xValues[3] = maxX;
        yValues[3] = maxY;

        // Transform the four corners
        TransformPoints(4, xValues, yValues);

        // Determine the new bounding box
        minX = maxX = xValues[0];
        minY = maxY = yValues[0];
        for(int i = 1; i < 4; i++)
        {
            if(xValues[i] < minX)
            {
                minX = xValues[i];
            }
            if(xValues[i] > maxX)
            {
                maxX = xValues[i];
            }
            if(yValues[i] < minY)
            {
                minY = yValues[i];
            }
            if(yValues[i] > maxY)
            {
                maxY = yValues[i];
            }
        }
    }

    //returns the linear unit scale factor
    virtual double GetLinearScale() = 0;
};

#endif

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

#ifndef CSYSTRANSFORMER_H
#define CSYSTRANSFORMER_H

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

    //returns the linear unit scale factor
    virtual double GetLinearScale() = 0;
};

#endif

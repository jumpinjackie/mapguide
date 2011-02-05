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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: olinep.h.
//
// PURPOSE: Declaration of the OpsLinePrimitive class. All line-type
//          primitives in the OPS++ class hierarchy are dervived from
//          this class.
//
//------------------------------------------------------------------------------

#ifndef _OLINEP_H_
#define _OLINEP_H_

class OpsLinePrimitive : public OpsPrimitive
{
public:
    /// method to get the length of the line primitive

    virtual double GetLength() const = 0;
    /// method to clip the line primitive against a specified clip rectangle
    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint outVerts[],
        int nPolyVerts[], int &nPolylines) const = 0;
};

#endif

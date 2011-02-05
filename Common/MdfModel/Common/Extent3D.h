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

#ifndef EXTENT3D_H_
#define EXTENT3D_H_

#include "../MdfModel.h"
#include "Point3D.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The Extent3D class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API Extent3D
{
public:
    // Constructor(s)/Destructor
    Extent3D();
    Extent3D(const Point3D& minPt, const Point3D& maxPt);
    virtual ~Extent3D();

    // Operations
    Point3D* GetMinimumPoint();

    Point3D* GetMaximumPoint();

private:
    // Data Members
    Point3D m_minPt;
    Point3D m_maxPt;
};

// Inline Methods
inline Point3D* Extent3D::GetMinimumPoint()
{
    return &m_minPt;
}

inline Point3D* Extent3D::GetMaximumPoint()
{
    return &m_maxPt;
}

END_NAMESPACE_MDFMODEL
#endif // EXTENT3D_H_

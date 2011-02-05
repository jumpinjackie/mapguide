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

#ifndef MAPVIEW_H_
#define MAPVIEW_H_

#include "../MdfRootObject.h"
#include "../Common/Point3D.h"
#include "../Common/Vector3D.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The MapView class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API MapView : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    MapView();
    virtual ~MapView();

    // Operations
    Point3D* GetCenter();

    double GetHeight() const;
    void SetHeight(double height);

    double GetRotation() const;
    void SetRotation(double rotation);

    Vector3D* GetViewDirection();

    const MdfString& GetModelUnits() const;
    void SetModelUnits(const MdfString& modelUnits);

private:
    // Hidden copy constructor and assignment operator.
    MapView(const MapView&);
    MapView& operator=(const MapView&);

    // Data Members
    Point3D m_center;
    double m_height;
    double m_rotation;
    Vector3D m_viewDirection;
    MdfString m_modelUnits;
};

// Inline Methods
inline Point3D* MapView::GetCenter()
{
    return &m_center;
}

inline double MapView::GetHeight() const
{
    return m_height;
}

inline double MapView::GetRotation() const
{
    return m_rotation;
}

inline Vector3D* MapView::GetViewDirection()
{
    return &m_viewDirection;
}

inline const MdfString& MapView::GetModelUnits() const
{
    return m_modelUnits;
}

END_NAMESPACE_MDFMODEL
#endif // MAPVIEW_H_

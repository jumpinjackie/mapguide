//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef MAPVIEWPORT_H_
#define MAPVIEWPORT_H_

#include "PrintLayoutElement.h"
#include "Point3D.h"
#include "StringObject.h"
#include "MapView.h"

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The MapViewport class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API MapViewport : public PrintLayoutElement
{
public:
    // Constructor(s)/Destructor
    MapViewport();
    virtual ~MapViewport();

    // Operations
    Point3D& GetCenterPoint();

    double GetOrientation() const;
    void SetOrientation(double orientation);

    const MdfString& GetMapName() const;
    void SetMapName(const MdfString& mapName);

    StringObjectCollection& GetVisibleLayerNames();

    bool GetIsLocked() const;
    void SetIsLocked(bool isLocked);

    bool GetIsOn() const;
    void SetIsOn(bool isOn);

    MapView& GetMapView();

private:
    // Hidden copy constructor and assignment operator.
    MapViewport(const MapViewport&);
    MapViewport& operator=(const MapViewport&);

    // Data Members
    Point3D m_centerPt;
    double m_orientation;
    MdfString m_mapName;
    StringObjectCollection m_visibleLayerNames;
    bool m_isLocked;
    bool m_isOn;
    MapView m_mapView;
};

// Inline Methods
inline Point3D& MapViewport::GetCenterPoint()
{
    return m_centerPt;
}

inline double MapViewport::GetOrientation() const
{
    return m_orientation;
}

inline const MdfString& MapViewport::GetMapName() const
{
    return m_mapName;
}

inline StringObjectCollection& MapViewport::GetVisibleLayerNames()
{
    return m_visibleLayerNames;
}

inline bool MapViewport::GetIsLocked() const
{
    return m_isLocked;
}

inline bool MapViewport::GetIsOn() const
{
    return m_isOn;
}

inline MapView& MapViewport::GetMapView()
{
    return m_mapView;
}

END_NAMESPACE_MDFMODEL
#endif // MAPVIEWPORT_H_

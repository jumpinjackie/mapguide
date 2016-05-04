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

#ifndef MAPVIEWPORTDEFINITION_H_
#define MAPVIEWPORTDEFINITION_H_

#include "PrintLayoutElementDefinition.h"
#include "../Common/StringObject.h"
#include "../PrintLayout/MapView.h"

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The MapViewportDefinition class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API MapViewportDefinition : public PrintLayoutElementDefinition
{
public:
    // Constructor(s)/Destructor
    MapViewportDefinition();
    virtual ~MapViewportDefinition();

    // Operations
    virtual const MdfString& GetType();

    const MdfString& GetMapName() const;
    void SetMapName(const MdfString& mapName);

    StringObjectCollection* GetHiddenLayerNames();

    bool GetIsLocked() const;
    void SetIsLocked(bool isLocked);

    bool GetIsOn() const;
    void SetIsOn(bool isOn);

    MapView* GetMapView();

private:
    // Hidden copy constructor and assignment operator.
    MapViewportDefinition(const MapViewportDefinition&);
    MapViewportDefinition& operator=(const MapViewportDefinition&);

    // Data Members
    MdfString m_mapName;
    StringObjectCollection m_hiddenLayerNames;
    bool m_isLocked;
    bool m_isOn;
    MapView m_mapView;
};

// Inline Methods
inline const MdfString& MapViewportDefinition::GetMapName() const
{
    return m_mapName;
}

inline StringObjectCollection* MapViewportDefinition::GetHiddenLayerNames()
{
    return &m_hiddenLayerNames;
}

inline bool MapViewportDefinition::GetIsLocked() const
{
    return m_isLocked;
}

inline bool MapViewportDefinition::GetIsOn() const
{
    return m_isOn;
}

inline MapView* MapViewportDefinition::GetMapView()
{
    return &m_mapView;
}

END_NAMESPACE_MDFMODEL
#endif // MAPVIEWPORTDEFINITION_H_

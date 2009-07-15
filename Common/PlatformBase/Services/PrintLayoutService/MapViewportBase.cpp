//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "MapViewportBase.h"

MG_IMPL_DYNCREATE(MgMapViewportBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgMapViewportBase object.
///
MgMapViewportBase::MgMapViewportBase() : m_isOn(false), m_isLocked(false), m_orientation(0.0)
{
    m_visibleLayerNames = static_cast<MgStringCollection*>(MgStringCollection::CreateObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgMapViewportBase object.
///
MgMapViewportBase::~MgMapViewportBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgMapViewportBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgMapViewportBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgMapViewportBase::Serialize(MgStream* stream)
{
    // Write raw data members
    stream->WriteBoolean(m_isOn);
    stream->WriteBoolean(m_isLocked);
    stream->WriteDouble(m_orientation);
    stream->WriteString(m_mapName);

    // Write associated objects
    stream->WriteObject(m_view);
    stream->WriteObject(m_centerPoint);
    stream->WriteObject(m_visibleLayerNames);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgMapViewportBase::Deserialize(MgStream* stream)
{
    // Read raw data members
    stream->GetBoolean(m_isOn);
    stream->GetBoolean(m_isLocked);
    stream->GetDouble(m_orientation);
    stream->GetString(m_mapName);

    // Read associated objects
    m_view = static_cast<MgMapView*>(stream->GetObject());
    m_centerPoint = static_cast<MgPoint3D*>(stream->GetObject());
    m_visibleLayerNames = static_cast<MgStringCollection*>(stream->GetObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center point of the map viewport.
///
MgPoint3D* MgMapViewportBase::GetCenterPoint()
{
    return SAFE_ADDREF(m_centerPoint.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the orientation of the map viewport.
///
double MgMapViewportBase::GetOrientation()
{
    return m_orientation;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the map.
///
STRING MgMapViewportBase::GetMapName()
{
    return m_mapName;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the names of the visible layers.
///
MgStringCollection* MgMapViewportBase::GetVisibleLayerNames()
{
    return SAFE_ADDREF(m_visibleLayerNames.p);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the flag that specifies if the map viewport is locked.
///
bool MgMapViewportBase::GetIsLocked()
{
    return m_isLocked;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the flag that specifies if the map viewport is on.
///
bool MgMapViewportBase::GetIsOn()
{
    return m_isOn;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the map view of the map viewport.
///
MgMapView* MgMapViewportBase::GetMapView()
{
    return SAFE_ADDREF(m_view.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes this object from the information in the resource XML string
///
void MgMapViewportBase::PopulateFromResource(MdfModel::PrintLayoutElementDefinition *element)
{
    m_view = NULL;
    m_isOn = m_isLocked = false;
    m_orientation = 0.0;
    m_mapName.clear();
    m_visibleLayerNames->Clear();

    MdfModel::MapViewportDefinition* mapViewport = dynamic_cast<MdfModel::MapViewportDefinition*>(element);
    assert(mapViewport != NULL);
    if (mapViewport)
    {
        m_isOn = mapViewport->GetIsOn();
        m_isLocked = mapViewport->GetIsLocked();
        m_orientation = mapViewport->GetOrientation();
        m_mapName = mapViewport->GetMapName();

        MdfModel::MapView* view = mapViewport->GetMapView();
        if (view)
        {
            m_view = new MgMapView();
            m_view->PopulateFromResource(view);
        }

        MdfModel::StringObjectCollection* visibleLayerNames = mapViewport->GetVisibleLayerNames();
        if (visibleLayerNames)
        {
            for (int i=0; i< visibleLayerNames->GetCount(); i++)
                m_visibleLayerNames->Add(visibleLayerNames->GetAt(i)->GetString());
        }
    }
}

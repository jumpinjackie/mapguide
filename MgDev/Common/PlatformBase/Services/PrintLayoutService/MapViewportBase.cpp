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
MgMapViewportBase::MgMapViewportBase()
{
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
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgMapViewportBase::Deserialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center point of the map viewport.
///
MgPoint3D* MgMapViewportBase::GetCenterPoint()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the orientation of the map viewport.
///
double MgMapViewportBase::GetOrientation()
{
    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the map.
///
STRING MgMapViewportBase::GetMapName()
{
    return L"";
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the names of the visible layers.
///
MgStringCollection* MgMapViewportBase::GetVisibleLayerNames()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the flag that specifies if the map viewport is locked.
///
bool MgMapViewportBase::GetIsLocked()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the flag that specifies if the map viewport is on.
///
bool MgMapViewportBase::GetIsOn()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the map view of the map viewport.
///
MgMapView* MgMapViewportBase::GetMapView()
{
    return NULL;
}

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
#include "MapView.h"

MG_IMPL_DYNCREATE(MgMapView)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgMapView object.
///
MgMapView::MgMapView()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgMapView object.
///
MgMapView::~MgMapView()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgMapView::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgMapView::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgMapView::Serialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgMapView::Deserialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center of the map view.
///
MgPoint3D* MgMapView::GetCenter()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the height of the map view.
///
double MgMapView::GetHeight()
{
    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the twist angle of the map view.
///
double MgMapView::GetTwistAngle()
{
    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the map view direction.
///
MgVector3D* MgMapView::GetViewDirection()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the model units.
///
STRING MgMapView::GetModelUnits()
{
    return L"";
}

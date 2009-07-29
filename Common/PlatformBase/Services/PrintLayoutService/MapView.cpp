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
MgMapView::MgMapView() : m_height(0.0), m_rotation(0.0)
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
    // Write raw data members
    stream->WriteDouble(m_height);
    stream->WriteDouble(m_rotation);
    stream->WriteString(m_modelUnits);

    // Write associated objects
    stream->WriteObject(m_center);
    stream->WriteObject(m_viewDirection);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgMapView::Deserialize(MgStream* stream)
{
    // Read raw data members
    stream->GetDouble(m_height);
    stream->GetDouble(m_rotation);
    stream->GetString(m_modelUnits);

    // Read associated objects
    m_center = static_cast<MgPoint3D*>(stream->GetObject());
    m_viewDirection = static_cast<MgVector3D*>(stream->GetObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center of the map view.
///
MgPoint3D* MgMapView::GetCenter()
{
    return SAFE_ADDREF(m_center.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the height of the map view.
///
double MgMapView::GetHeight()
{
    return m_height;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the twist angle of the map view.
///
double MgMapView::GetRotation()
{
    return m_rotation;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the map view direction.
///
MgVector3D* MgMapView::GetViewDirection()
{
    return SAFE_ADDREF(m_viewDirection.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the model units.
///
STRING MgMapView::GetModelUnits()
{
    return  m_modelUnits;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the map view from the MDF representation.
///
void MgMapView::PopulateFromResource(MdfModel::MapView *view)
{
    m_height = m_rotation = 0.0;
    m_modelUnits.clear();
    m_center = NULL;
    m_viewDirection = NULL;

    assert(NULL != view);
    if (NULL != view)
    {
        m_height = view->GetHeight();
        m_rotation = view->GetRotation();
        m_modelUnits = view->GetModelUnits();

        MdfModel::Point3D* center = view->GetCenter();
        if (NULL != center)
        {
            m_center = new MgPoint3D(center->GetX(), center->GetY(), center->GetZ());
        }

        MdfModel::Vector3D* viewDirection = view->GetViewDirection();
        if (NULL != viewDirection)
        {
            m_viewDirection = new MgVector3D(viewDirection->GetX(), viewDirection->GetY(), viewDirection->GetZ());
        }
    }
}

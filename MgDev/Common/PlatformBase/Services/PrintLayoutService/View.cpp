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
#include "View.h"

MG_IMPL_DYNCREATE(MgView)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgView object.
///
MgView::MgView()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgView object.
///
MgView::~MgView()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgView::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgView::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgView::Serialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgView::Deserialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center of the view.
///
MgPoint3D* MgView::GetCenter()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the height of the view.
///
double MgView::GetHeight()
{
    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the twist angle of the view.
///
double MgView::GetTwistAngle()
{
    return 0.0;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the view direction.
///
MgVector3D* MgView::GetViewDirection()
{
    return NULL;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the model units.
///
STRING MgView::GetModelUnits()
{
    return L"";
}

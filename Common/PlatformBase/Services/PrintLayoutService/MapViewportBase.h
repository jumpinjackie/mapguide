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

#ifndef MGMAPVIEWPORTBASE_H_
#define MGMAPVIEWPORTBASE_H_

#include "PrintLayoutElementBase.h"

class MgView;

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgMapViewportBase object.
///
class MG_PLATFORMBASE_API MgMapViewportBase : public MgPrintLayoutElementBase
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMapViewportBase)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgMapViewportBase object.
    ///
    MgMapViewportBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgMapViewportBase object.
    ///
    ~MgMapViewportBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the center point of the map viewport.
    ///
    /// \return
    /// Returns an MgPoint3D object containing the center point of the map viewport.
    ///
    MgPoint3D* GetCenterPoint();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the orientation of the map viewport.
    ///
    /// \return
    /// Returns the orientation of the map viewport.
    ///
    double GetOrientation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the map.
    ///
    /// \return
    /// Returns the name of the map.
    ///
    virtual STRING GetMapName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the names of the visible layers.
    ///
    /// \return
    /// Returns an MgStringCollection object containing the names of the visible layers.
    ///
    virtual MgStringCollection* GetVisibleLayerNames();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the flag that specifies if the map viewport is locked.
    ///
    /// \return
    /// Returns the flag indicating if the map viewport is locked.
    ///
    virtual bool GetLocked();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the map viewport is on.
    ///
    /// \return
    /// Returns true if the map viewport is on; false otherwise.
    ///
    virtual bool IsOn();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the view of the map viewport.
    ///
    /// \return
    /// Returns the view of the map viewport.
    ///
    virtual MgView* GetView();

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class ID.
    ///
    /// \return
    /// The integer value.
    ///
    virtual INT32 GetClassId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Disposes this object.
    ///
    /// \return
    /// Nothing.
    ///
    virtual void Dispose();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_MapViewportBase;
};

#endif

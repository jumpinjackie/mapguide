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

#ifndef MGMAPVIEW_H_
#define MGMAPVIEW_H_

// Forward Declarations
BEGIN_NAMESPACE_MDFMODEL
class MapView;
END_NAMESPACE_MDFMODEL

class MgPoint3D;
class MgVector3D;

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgMapView object.
///
class MG_PLATFORMBASE_API MgMapView : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgMapView)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgMapView object.
    ///
    MgMapView();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgMapView object.
    ///
    ~MgMapView();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the center of the map view.
    ///
    /// \return
    /// Returns an MgPoint3D object containing the center of the map view.
    ///
    virtual MgPoint3D* GetCenter();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the height of the map view.
    ///
    /// \return
    /// Returns the height of the map view.
    ///
    virtual double GetHeight();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the twist angle of the map view.
    ///
    /// \return
    /// Returns the twist angle of the map view.
    ///
    virtual double GetRotation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the map view direction.
    ///
    /// \return
    /// Returns an MgVector3D object containing the map view direction.
    ///
    virtual MgVector3D* GetViewDirection();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the model units.
    ///
    /// \return
    /// Returns the model units.
    ///
    virtual STRING GetModelUnits();

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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates the map view from the MDF representation.
    ///
    void PopulateFromResource(MdfModel::MapView *view);

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

private:

    // Data Members
    double m_height;
    double m_rotation;
    STRING m_modelUnits;
    Ptr<MgPoint3D> m_center;
    Ptr<MgVector3D> m_viewDirection;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_MapView;
};

#endif

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

#ifndef MGVIEW_H_
#define MGVIEW_H_

class MgPoint3D;
class MgVector3D;

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgView object.
///
class MG_PLATFORMBASE_API MgView : public MgSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgView)

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgView object.
    ///
    MgView();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgView object.
    ///
    ~MgView();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the center of the view.
    ///
    /// \return
    /// Returns an MgPoint3D object containing the center of the view.
    ///
    virtual MgPoint3D* GetCenter();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the height of the view.
    ///
    /// \return
    /// Returns the height of the view.
    ///
    virtual double GetHeight();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the twist angle of the view.
    ///
    /// \return
    /// Returns the twist angle of the view.
    ///
    virtual double GetTwistAngle();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the view direction.
    ///
    /// \return
    /// Returns an MgVector3D object containing the view direction.
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
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_View;
};

#endif

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

#ifndef MGPRINTLAYOUTELEMENTDEFINITIONBASE_H_
#define MGPRINTLAYOUTELEMENTDEFINITIONBASE_H_

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgPrintLayoutElementDefinitionBase object.
///
class MG_PLATFORMBASE_API MgPrintLayoutElementDefinitionBase : public MgResource
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgPrintLayoutBase)

INTERNAL_API:
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name.
    ///
    virtual STRING GetName();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name.
    ///
    virtual void SetName(CREFSTRING name);


    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutElementDefinitionBase object.
    ///
    MgPrintLayoutElementDefinitionBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgPrintLayoutElementDefinitionBase object.
    ///
    ~MgPrintLayoutElementDefinitionBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Initializes this object from the resource XML string
    ///
    virtual void PopulateFromResource(CREFSTRING resourceXml);

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

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

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the name of the resource type.
    ///
    virtual const char* GetResourceTypeName();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PrintLayoutElementDefinitionBase;

protected:
    STRING m_name;
};

#endif

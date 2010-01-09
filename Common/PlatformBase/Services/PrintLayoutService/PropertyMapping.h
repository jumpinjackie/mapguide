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

#ifndef _MGPROPERTYMAPPING_H
#define _MGPROPERTYMAPPING_H

/// \defgroup MgPropertyMapping MgPropertyMapping
/// \{

// Forward Declarations
BEGIN_NAMESPACE_MDFMODEL
class PropertyMapping;
END_NAMESPACE_MDFMODEL

class MgPropertyMapping;
template class MG_PLATFORMBASE_API Ptr<MgPropertyMapping>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines a feature class belonging to a schema.
///
class MG_PLATFORMBASE_API MgPropertyMapping : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgPropertyMapping)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPropertyMapping object.
    ///
    MgPropertyMapping();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    ~MgPropertyMapping();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source property in this class.
    ///
    STRING GetSourceProperty();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the source units in this class.
    ///
    STRING GetSourceUnits();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the target property in this class.
    ///
    STRING GetTargetProperty();  

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
    /// Populates the property mapping from the MDF representation.
    ///
    void PopulateFromResource(MdfModel::PropertyMapping *propMapping);

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

    STRING m_sourceProperty;
    STRING m_sourceUnits;
    STRING m_targetProperty;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PropertyMapping;
};
/// \}

#endif

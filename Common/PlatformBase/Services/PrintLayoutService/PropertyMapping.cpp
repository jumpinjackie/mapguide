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

#include "PlatformBase.h"

MG_IMPL_DYNCREATE(MgPropertyMapping)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPropertyMapping object.
///
MgPropertyMapping::MgPropertyMapping()
{
    m_sourceProperty = L"";
    m_sourceUnits = L"";
    m_targetProperty = L"";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPropertyMapping object.
///
MgPropertyMapping::~MgPropertyMapping()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPropertyMapping::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPropertyMapping::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPropertyMapping::Serialize(MgStream* stream)
{
    stream->WriteString(m_sourceProperty);
    stream->WriteString(m_sourceUnits);
    stream->WriteString(m_targetProperty);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPropertyMapping::Deserialize(MgStream* stream)
{
    stream->GetString(m_sourceProperty);
    stream->GetString(m_sourceUnits);
    stream->GetString(m_targetProperty);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the source property in this class.
///
/// \return
/// The name of the source property.
///
STRING MgPropertyMapping::GetSourceProperty()
{
    return m_sourceProperty;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the target property in this class.
///
/// \return
/// The name of the target property.
///
STRING MgPropertyMapping::GetTargetProperty()
{
    return m_targetProperty;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the source property units in this class.
///
/// \return
/// The source property units.
///
STRING MgPropertyMapping::GetSourceUnits()
{
    return m_sourceUnits;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout from the MDF representation.
///
void MgPropertyMapping::PopulateFromResource(MdfModel::PropertyMapping *propMapping)
{
    m_sourceProperty.clear();
    m_targetProperty.clear();
    m_sourceUnits.clear();

    assert(NULL != propMapping);
    if (NULL != propMapping)
    {
        m_sourceProperty = propMapping->GetSourceProperty();
        m_targetProperty = propMapping->GetTargetProperty();
        m_sourceUnits    = propMapping->GetSourceUnits();
    }
}


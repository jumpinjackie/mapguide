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
#include "PrintLayoutElementBase.h"

MG_IMPL_DYNCREATE(MgPrintLayoutElementBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::MgPrintLayoutElementBase()
{
    //TODO Default initializations
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::~MgPrintLayoutElementBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPrintLayoutElementBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPrintLayoutElementBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the resource type.
///
const char* MgPrintLayoutElementBase::GetResourceTypeName()
{
    return "PrintLayoutElement";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutElementBase::CanSetName()
{
    // via Create
    return true;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Serializes data to a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgPrintLayoutElementBase::Serialize(MgStream* stream)
{
    //TODO
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserializes data from a TCP/IP stream.
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgPrintLayoutElementBase::Deserialize(MgStream* stream)
{
    //TODO
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name.
///
STRING MgPrintLayoutElementBase::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the name.
///
void MgPrintLayoutElementBase::SetName(CREFSTRING name)
{
    m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the visibility.
///
bool MgPrintLayoutElementBase::GetVisibility()
{
    return m_visible;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the visibility.
///
void MgPrintLayoutElementBase::SetVisibility(bool visible)
{
    m_visible = visible;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the references.
///
MgStringCollection* MgPrintLayoutElementBase::GetReferences()
{
    return SAFE_ADDREF((MgStringCollection*)m_references);
}
    
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the description.
///
STRING MgPrintLayoutElementBase::GetDescription()
{
    return m_description;
}
    
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the units.
///
STRING MgPrintLayoutElementBase::GetUnits()
{
    return m_units;
}
    
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID of the associated print layout element definition.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetDefinition()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_elementDefinition);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the extents.
///
MgEnvelope* MgPrintLayoutElementBase::GetExtents()
{
    return SAFE_ADDREF((MgEnvelope*)m_extents);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID of the layer definition used for stylization.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetStylization()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_stylization);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID for the feature source associated with this print layout element.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetFeatureSource()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_datasource);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the fully qualified feature class from which to retrieve data.
///
STRING MgPrintLayoutElementBase::GetFeatureClass()
{
    return m_featureClass;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the name of the geometry property in the feature class.
///
STRING MgPrintLayoutElementBase::GetGeometryProperty()
{
    return m_geometryName;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the FDO filter to use in retrieving data.
///
STRING MgPrintLayoutElementBase::GetFilter()
{
    return m_filter;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the property mappings between feature class properties and print layout 
/// element properties.
///
MgPropertyMappingCollection* MgPrintLayoutElementBase::GetPropertyMappings()
{
    return SAFE_ADDREF((MgPropertyMappingCollection*)m_propertyMappings);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Forces this object to refresh itself from the associated resource
///
void MgPrintLayoutElementBase::ForceRefresh()
{
    //TODO
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the type.
///
STRING MgPrintLayoutElementBase::GetType()
{
    //TODO
    return L"";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes this object from the information in the resource XML string
///
void MgPrintLayoutElementBase::PopulateFromResource(CREFSTRING resourceXml)
{
    //TODO
}

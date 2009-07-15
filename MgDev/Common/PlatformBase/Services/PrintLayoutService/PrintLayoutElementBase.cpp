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

// Includes
#include "PlatformBase.h"
#include "PrintLayout/PrintLayoutElementDefinition.h"

MG_IMPL_DYNCREATE(MgPrintLayoutElementBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::MgPrintLayoutElementBase() :
    m_visible(true)
{
    m_propertyMappings = static_cast<MgPropertyMappingCollection*>(MgPropertyMappingCollection::CreateObject());
    m_references = static_cast<MgStringCollection*>(MgStringCollection::CreateObject());
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
    return "PrintLayoutElementDefinition";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutElementBase::CanSetName()
{
    return true; // via Create
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
    // Write raw data members
    stream->WriteBoolean(m_visible);
    stream->WriteString(m_name);
    stream->WriteString(m_type);
    stream->WriteString(m_description);
    stream->WriteString(m_units);
    stream->WriteString(m_featureClass);
    stream->WriteString(m_geometryName);
    stream->WriteString(m_filter);

    // Write associated objects
    stream->WriteObject(m_extent);
    stream->WriteObject(m_datasource);
    stream->WriteObject(m_stylization);
    stream->WriteObject(m_propertyMappings);
    stream->WriteObject(m_references);
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
    // Read raw data members
    stream->GetBoolean(m_visible);
    stream->GetString(m_name);
    stream->GetString(m_type);
    stream->GetString(m_description);
    stream->GetString(m_units);
    stream->GetString(m_featureClass);
    stream->GetString(m_geometryName);
    stream->GetString(m_filter);

    // Read associated objects
    m_extent = static_cast<MgEnvelope*>(stream->GetObject());
    m_datasource = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_stylization = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_propertyMappings = static_cast<MgPropertyMappingCollection*>(stream->GetObject());
    m_references = static_cast<MgStringCollection*>(stream->GetObject());
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
/// Gets the extent.
///
MgEnvelope* MgPrintLayoutElementBase::GetExtent()
{
    return SAFE_ADDREF((MgEnvelope*)m_extent);
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
void MgPrintLayoutElementBase::ForceRefresh(MgResourceService* resourceService)
{
    if (m_elementDefinition) 
    {
        this->Open(resourceService, m_elementDefinition);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the type.
///
STRING MgPrintLayoutElementBase::GetType()
{
    return m_type;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes this object from the information in the resource XML string
///
void MgPrintLayoutElementBase::PopulateFromResource(CREFSTRING resourceXml)
{
    Ptr<MgByteReader> reader = new MgByteReader(resourceXml, MgMimeType::Xml);
    Ptr<MgByteSink> sink = new MgByteSink(reader);
    Ptr<MgByte> bytes = sink->ToBuffer();
    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());
    if (!parser.GetSucceeded()) 
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidResourceTypeException(
            L"MgPrintLayoutElementBase.PopulateFromResource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    std::auto_ptr<MdfModel::PrintLayoutElementDefinition> spledef(parser.DetachPrintLayoutElementDefinition());
    this->PopulateFromResource(spledef.get());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes this object from the information in the resource XML string
///
void MgPrintLayoutElementBase::PopulateFromResource(MdfModel::PrintLayoutElementDefinition *element)
{
    assert(element != NULL);
    if (element)
        m_name = element->GetName();
}

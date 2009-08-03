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

MG_IMPL_DYNCREATE(MgPrintLayoutElementBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::MgPrintLayoutElementBase() :
    m_width(0.0),
    m_height(0.0),
    m_rotation(0.0),
    m_visible(true),
    m_opacity(1.0)
{
    m_propertyMappings = new MgPropertyMappingCollection();
    m_references = static_cast<MgStringCollection*>(MgStringCollection::CreateObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::MgPrintLayoutElementBase(CREFSTRING type) :
    m_type(type),
    m_width(0.0),
    m_height(0.0),
    m_rotation(0.0),
    m_visible(true),
    m_opacity(1.0)
{
    m_propertyMappings = new MgPropertyMappingCollection();
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPrintLayoutElementBase::Serialize(MgStream* stream)
{
    // Write raw data members
    stream->WriteString(m_type);
    stream->WriteString(m_name);
    stream->WriteDouble(m_width);
    stream->WriteDouble(m_height);
    stream->WriteDouble(m_rotation);
    stream->WriteString(m_units);
    stream->WriteBoolean(m_visible);
    stream->WriteDouble(m_opacity);
    stream->WriteString(m_description);
    stream->WriteString(m_featureClass);
    stream->WriteString(m_geometryName);
    stream->WriteString(m_filter);

    // Write associated objects
    stream->WriteObject(m_resourceId);
    stream->WriteObject(m_center);
    stream->WriteObject(m_references);
    stream->WriteObject(m_extent);
    stream->WriteObject(m_datasource);
    stream->WriteObject(m_stylization);
    stream->WriteObject(m_style);
    stream->WriteObject(m_propertyMappings);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutElementBase::Deserialize(MgStream* stream)
{
    // Read raw data members
    stream->GetString(m_type);
    stream->GetString(m_name);
    stream->GetDouble(m_width);
    stream->GetDouble(m_height);
    stream->GetDouble(m_rotation);
    stream->GetString(m_units);
    stream->GetBoolean(m_visible);
    stream->GetDouble(m_opacity);
    stream->GetString(m_description);
    stream->GetString(m_featureClass);
    stream->GetString(m_geometryName);
    stream->GetString(m_filter);

    // Read associated objects
    m_resourceId = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_center = static_cast<MgPoint3D*>(stream->GetObject());
    m_references = static_cast<MgStringCollection*>(stream->GetObject());
    m_extent = static_cast<MgEnvelope*>(stream->GetObject());
    m_datasource = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_stylization = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_style = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_propertyMappings = static_cast<MgPropertyMappingCollection*>(stream->GetObject());
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

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetResourceId()
{
    return SAFE_ADDREF(m_resourceId.p);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the resource ID.
///
void MgPrintLayoutElementBase::SetResourceId(MgResourceIdentifier* resourceId)
{
    m_resourceId = SAFE_ADDREF(resourceId);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the center.
///
MgPoint3D* MgPrintLayoutElementBase::GetCenter()
{
    return SAFE_ADDREF(m_center.p);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the center.
///
void MgPrintLayoutElementBase::SetCenter(MgPoint3D* center)
{
    m_center = SAFE_ADDREF(center);
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the width.
///
double MgPrintLayoutElementBase::GetWidth()
{
    return m_width;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the width.
///
void MgPrintLayoutElementBase::SetWidth(double width)
{
    m_width = width;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the height.
///
double MgPrintLayoutElementBase::GetHeight()
{
    return m_height;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the height.
///
void MgPrintLayoutElementBase::SetHeight(double height)
{
    m_height = height;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the rotation.
///
double MgPrintLayoutElementBase::GetRotation()
{
    return m_rotation;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the rotation.
///
void MgPrintLayoutElementBase::SetRotation(double rotation)
{
    m_rotation = rotation;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the units.
///
STRING MgPrintLayoutElementBase::GetUnits()
{
    return m_units;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the units.
///
void MgPrintLayoutElementBase::SetUnits(CREFSTRING units)
{
    m_units = units;
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

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the opacity.
///
double MgPrintLayoutElementBase::GetOpacity()
{
    return m_opacity;
}

///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the opacity.
///
void MgPrintLayoutElementBase::SetOpacity(double opacity)
{
    m_opacity = opacity;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the references.
///
MgStringCollection* MgPrintLayoutElementBase::GetReferences()
{
    return SAFE_ADDREF(m_references.p);
}
    
///////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the references.
///
void MgPrintLayoutElementBase::SetReferences(MgStringCollection* references)
{
    m_references = SAFE_ADDREF(references);
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
/// Gets the extent.
///
MgEnvelope* MgPrintLayoutElementBase::GetExtent()
{
    return SAFE_ADDREF(m_extent.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID for the feature source associated with this print layout element.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetFeatureSource()
{
    return SAFE_ADDREF(m_datasource.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID of the layer definition used for stylization.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetStylization()
{
    return SAFE_ADDREF(m_stylization.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the resource ID of the associated print layout element style.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetStyle()
{
    return SAFE_ADDREF(m_style.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the property mappings between feature class properties and print layout 
/// element properties.
///
MgPropertyMappingCollection* MgPrintLayoutElementBase::GetPropertyMappings()
{
    return SAFE_ADDREF(m_propertyMappings.p);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Forces this object to refresh itself from the associated resource
///
void MgPrintLayoutElementBase::ForceRefresh(MgResourceService* resourceService)
{
    if (NULL != m_resourceId.p) 
    {
        this->Open(resourceService, m_resourceId);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout element from the resource XML string.
///
void MgPrintLayoutElementBase::PopulateFromResource(CREFSTRING elementXml)
{
    assert(!elementXml.empty());

    // Parse the resource XML.
    string strXml = MgUtil::WideCharToMultiByte(elementXml);
    MdfParser::SAX2Parser parser;
    parser.ParseString(strXml.c_str(), strXml.length());

    if (!parser.GetSucceeded()) 
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);

        throw new MgXmlParserException(L"MgPrintLayoutElementBase.PopulateFromResource",
            __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }

    // Populate the data.
    std::auto_ptr<MdfModel::PrintLayoutElementDefinition> elementDef(parser.DetachPrintLayoutElementDefinition());
    PopulateFromResource(elementDef.get());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout element from the MDF representation.
///
void MgPrintLayoutElementBase::PopulateFromResource(MdfModel::PrintLayoutElementDefinition *elementDef)
{
    // Reset the data.
    m_name.clear();
    m_width = 0.0;
    m_height = 0.0;
    m_rotation = 0.0;
    m_units.clear();
    m_visible = true;
    m_opacity = 1.0;
    m_description.clear();
    m_featureClass.clear();
    m_geometryName.clear();
    m_filter.clear();

    m_resourceId = NULL;
    m_center = NULL;
    m_references->Clear();
    m_extent = NULL;
    m_datasource = NULL;
    m_stylization = NULL;
    m_style = NULL;
    m_propertyMappings->Clear();

    // Populate the data.
    assert(NULL != elementDef);
    if (NULL != elementDef)
    {
        m_type = elementDef->GetType();
        m_description = elementDef->GetDescription();

        DataConfiguration* dataConf = elementDef->GetDataConfiguration();
        if (NULL != dataConf)
        {
            m_featureClass = dataConf->GetFeatureClass();
            m_geometryName = dataConf->GetGeometry();
            m_filter = dataConf->GetFilter();
            m_datasource = new MgResourceIdentifier(dataConf->GetResourceId());

            MdfModel::PropertyMappingCollection* propMappings = dataConf->GetPropertyMappings();
            if (NULL != propMappings)
            {
                for (int i = 0; i < propMappings->GetCount(); ++i)
                {
                    PropertyMapping* thisMapping = propMappings->GetAt(i);
                    assert(NULL != thisMapping);
                    if (NULL != thisMapping)
                    {
                        Ptr<MgPropertyMapping> newMapping = new MgPropertyMapping();
                        newMapping->PopulateFromResource(thisMapping);
                        m_propertyMappings->Add(newMapping);
                    }
                }
            }
        }        

        StylizationConfiguration* stylizationConf = elementDef->GetStylizationConfiguration();
        if (NULL != stylizationConf)
        {
            m_stylization = new MgResourceIdentifier(stylizationConf->GetResourceId());
        }        

        m_style = new MgResourceIdentifier(elementDef->GetResourceId());
    }
}

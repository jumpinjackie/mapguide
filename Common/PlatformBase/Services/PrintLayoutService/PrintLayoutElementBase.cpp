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
    m_visible(true)
{
    m_propertyMappings = new MgPropertyMappingCollection();
    m_references = static_cast<MgStringCollection*>(MgStringCollection::CreateObject());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementBase object.
///
MgPrintLayoutElementBase::MgPrintLayoutElementBase(CREFSTRING type) :
    m_visible(true),
    m_type(type)
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
    stream->WriteBoolean(m_visible);
    stream->WriteString(m_type);
    stream->WriteString(m_name);
    stream->WriteString(m_description);
    stream->WriteString(m_units);
    stream->WriteString(m_featureClass);
    stream->WriteString(m_geometryName);
    stream->WriteString(m_filter);

    // Write associated objects
    stream->WriteObject(m_extent);
    stream->WriteObject(m_datasource);
    stream->WriteObject(m_stylization);
    stream->WriteObject(m_elementDefinition);
    stream->WriteObject(m_propertyMappings);
    stream->WriteObject(m_references);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutElementBase::Deserialize(MgStream* stream)
{
    // Read raw data members
    stream->GetBoolean(m_visible);
    stream->GetString(m_type);
    stream->GetString(m_name);
    stream->GetString(m_description);
    stream->GetString(m_units);
    stream->GetString(m_featureClass);
    stream->GetString(m_geometryName);
    stream->GetString(m_filter);

    // Read associated objects
    m_extent = static_cast<MgEnvelope*>(stream->GetObject());
    m_datasource = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_stylization = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_elementDefinition = static_cast<MgResourceIdentifier*>(stream->GetObject());
    m_propertyMappings = static_cast<MgPropertyMappingCollection*>(stream->GetObject());
    m_references = static_cast<MgStringCollection*>(stream->GetObject());
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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the references.
///
MgStringCollection* MgPrintLayoutElementBase::GetReferences()
{
    return SAFE_ADDREF(m_references.p);
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
    return SAFE_ADDREF(m_elementDefinition.p);
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
/// Gets the resource ID of the layer definition used for stylization.
///
MgResourceIdentifier* MgPrintLayoutElementBase::GetStylization()
{
    return SAFE_ADDREF(m_stylization.p);
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
    return SAFE_ADDREF(m_propertyMappings.p);
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
/// Populates the print layout element from the resource XML string.
///
void MgPrintLayoutElementBase::PopulateFromResource(CREFSTRING resourceXml)
{
    assert(!resourceXml.empty());

    // Parse the resource XML.
    string strXml = MgUtil::WideCharToMultiByte(resourceXml);
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
    std::auto_ptr<MdfModel::PrintLayoutElementDefinition> element(parser.DetachPrintLayoutElementDefinition());
    this->PopulateFromResource(element.get());
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Populates the print layout element from the MDF representation.
///
void MgPrintLayoutElementBase::PopulateFromResource(MdfModel::PrintLayoutElementDefinition *element)
{
    // Reset the data.
    m_visible = true;
    m_name.clear();
    m_description.clear();
    m_units.clear();
    m_featureClass.clear();
    m_geometryName.clear();
    m_filter.clear();
    m_extent = NULL;
    m_datasource = NULL;
    m_stylization = NULL;
    m_elementDefinition = NULL;
    m_propertyMappings->Clear();
    m_references->Clear();

    // Populate the data.
    assert(NULL != element);
    if (NULL != element)
    {
        m_name = element->GetName();
        m_description = element->GetDescription();
        m_units = element->GetUnits();

        Extent3D* extent = element->GetExtent();
        if (NULL != extent)
        {
            Point3D* minPt = extent->GetMinimumPoint();
            Point3D* maxPt = extent->GetMinimumPoint();
            assert(NULL != minPt && NULL != maxPt);

            Ptr<MgCoordinateXYZ> lowerLeft  = new MgCoordinateXYZ(minPt->GetX(), minPt->GetY(), minPt->GetZ());
            Ptr<MgCoordinateXYZ> upperRight = new MgCoordinateXYZ(maxPt->GetX(), maxPt->GetY(), maxPt->GetZ());
            
            m_extent = new MgEnvelope(lowerLeft, upperRight);
        }

        DataConfiguration* dataConf = element->GetDataConfiguration();
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

        StylizationConfiguration* stylizationConf = element->GetStylizationConfiguration();
        if (NULL != stylizationConf)
        {
            m_stylization = new MgResourceIdentifier(stylizationConf->GetResourceId());
        }        

        m_elementDefinition = new MgResourceIdentifier(element->GetResourceId());

        MdfModel::StringObjectCollection* references = element->GetReferences();
        if (NULL != references)
        {
            for (int i = 0; i < references->GetCount(); ++i)
            {
                m_references->Add(references->GetAt(i)->GetString());
            }
        }
    }
}

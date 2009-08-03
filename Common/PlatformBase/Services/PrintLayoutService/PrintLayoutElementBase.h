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

#ifndef MGPRINTLAYOUTELEMENTBASE_H_
#define MGPRINTLAYOUTELEMENTBASE_H_

// Forward Declarations
BEGIN_NAMESPACE_MDFMODEL
class PrintLayoutElementDefinition;
END_NAMESPACE_MDFMODEL

class MgPropertyMappingCollection;

////////////////////////////////////////////////////////////
/// \brief
/// Defines the MgPrintLayoutElementBase object.
///
class MG_PLATFORMBASE_API MgPrintLayoutElementBase : public MgResource
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgPrintLayoutElementBase)

INTERNAL_API:
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutElementBase object.
    ///
    MgPrintLayoutElementBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPrintLayoutElementBase object.
    ///
    MgPrintLayoutElementBase(CREFSTRING type);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroys an MgPrintLayoutElementBase object.
    ///
    ~MgPrintLayoutElementBase();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this element.
    ///
    virtual STRING GetType();

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
    /// Gets the resource ID.
    ///
    virtual MgResourceIdentifier* GetResourceId();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the resource ID.
    ///
    virtual void SetResourceId(MgResourceIdentifier* resourceId);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the center.
    ///
    virtual MgPoint3D* GetCenter();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the center.
    ///
    virtual void SetCenter(MgPoint3D* center);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the width.
    ///
    virtual double GetWidth();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the width.
    ///
    virtual void SetWidth(double width);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the height.
    ///
    virtual double GetHeight();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the height.
    ///
    virtual void SetHeight(double height);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the rotation.
    ///
    virtual double GetRotation();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the rotation.
    ///
    virtual void SetRotation(double rotation);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the units.
    ///
    virtual STRING GetUnits();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the units.
    ///
    virtual void SetUnits(CREFSTRING units);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the visibility.
    ///
    virtual bool GetVisibility();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the visibility.
    ///
    virtual void SetVisibility(bool visible);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the opacity.
    ///
    virtual double GetOpacity();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the opacity.
    ///
    virtual void SetOpacity(double opacity);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the references.
    ///
    virtual MgStringCollection* GetReferences();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the references.
    ///
    virtual void SetReferences(MgStringCollection* references);
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of this element
    ///
    virtual STRING GetDescription();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the resource ID of this element's element style resource
    ///
    virtual MgResourceIdentifier* GetStyle();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the extent of this element
    ///
    virtual MgEnvelope* GetExtent();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the resource ID of the layer definition that provides stylization
    /// for this element
    ///
    virtual MgResourceIdentifier* GetStylization();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the resource ID of the feature source used by this element to store
    /// feature data
    ///
    virtual MgResourceIdentifier* GetFeatureSource();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the fully qualified feature class name used to retrieve feature
    /// data for this element
    ///
    virtual STRING GetFeatureClass();
    
    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry property to use to retrieve features from the feature
    /// class
    ///
    virtual STRING GetGeometryProperty();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the FDO filter to use to constrain the features returned from the
    /// feature source
    ///
    virtual STRING GetFilter();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the property mappings between the element properties and the feature
    /// class properties
    ///
    virtual MgPropertyMappingCollection* GetPropertyMappings();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Forces this element object to refresh itself from its resource
    ///
    virtual void ForceRefresh(MgResourceService* resourceService);

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
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates the print layout element from the resource XML string.
    ///
    virtual void PopulateFromResource(CREFSTRING elementXml);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Populates the print layout element from the MDF representation.
    ///
    virtual void PopulateFromResource(MdfModel::PrintLayoutElementDefinition *elementDef);

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

protected:

    // Data Members
    STRING m_type;
    STRING m_name;
    Ptr<MgResourceIdentifier> m_resourceId;
    Ptr<MgPoint3D> m_center;
    double m_width;
    double m_height;
    double m_rotation;
    STRING m_units;
    bool m_visible;
    double m_opacity;
    Ptr<MgStringCollection> m_references;
    STRING m_description;
    STRING m_featureClass;
    STRING m_geometryName;
    STRING m_filter;
    Ptr<MgEnvelope> m_extent;
    Ptr<MgResourceIdentifier> m_datasource;
    Ptr<MgResourceIdentifier> m_stylization;
    Ptr<MgResourceIdentifier> m_style;
    Ptr<MgPropertyMappingCollection> m_propertyMappings;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_PrintLayoutService_PrintLayoutElementBase;
};

#endif

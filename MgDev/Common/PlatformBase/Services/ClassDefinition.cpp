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
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgClassDefinition)

//////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgClassDefinition::MgClassDefinition()
{
    m_classProperties = new MgPropertyDefinitionCollection();
    m_identityProperties = new MgPropertyDefinitionCollection();
    m_className = L"";
    m_description = L"";
    m_defaultGeometryPropertyName = L"";
    m_serializedXml = L"";
    m_hasRasterProperty = false;
    m_rasterPropName = L"";
    m_isComputed = false;
    m_isAbstract = false;
    m_isDeleted = false;
    m_baseClassDefinition = (MgClassDefinition*)NULL;
    m_totalProperties = (MgPropertyDefinitionCollection*)NULL;
    m_classList = (MgStringCollection*)NULL;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgClassDefinition::~MgClassDefinition()
{
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns a collection containing all the properties for this class
/// </summary>
/// <returns>
/// Returns a collection of PropertyDefinition classes.
/// </returns>
MgPropertyDefinitionCollection* MgClassDefinition::GetProperties()
{
    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_classProperties);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns a collection containing references to the
/// data properties that can be used to uniquely identify
/// instances of the class.
/// </summary>
/// <returns>
/// Returns a collection of PropertyDefinition classes.
/// </returns>
/// TODO:
/// MgPropertyDefinitionCollection is owned by ClassDefinition.  This should
/// be the standard mechanism for our API.
MgPropertyDefinitionCollection* MgClassDefinition::GetIdentityProperties()
{
    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_identityProperties);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the default geometry property in this class.
/// </summary>
/// <returns>Returns the name of the default geometry property</returns>
STRING MgClassDefinition::GetDefaultGeometryPropertyName()
{
    return m_defaultGeometryPropertyName;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the name of the default geometry property in this class.
/// </summary>
/// <param name="name">name of the default geometry property</param>
void MgClassDefinition::SetDefaultGeometryPropertyName(CREFSTRING name)
{
    m_defaultGeometryPropertyName = name;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the name of the feature class
/// </summary>
/// <param name="name">
/// Feature class name
/// </param>
void MgClassDefinition::SetName(CREFSTRING name)
{
    m_className = name;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the feature class
/// </summary>
/// <returns>
/// Returns the feature class name
/// </returns>
STRING MgClassDefinition::GetName()
{
    return m_className;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Sets the feature class description
/// </summary>
/// <param name="description">
/// Feature class description
/// </param>
void MgClassDefinition::SetDescription(CREFSTRING description)
{
    m_description = description;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the feature class description
/// </summary>
/// <returns>
/// Returns the feature class description
/// </returns>
STRING MgClassDefinition::GetDescription()
{
    return m_description;
}

//////////////////////////////////////////////////////////////
bool MgClassDefinition::HasRasterProperty()
{
    CHECKNULL((MgPropertyDefinitionCollection*)m_classProperties, L"MgClassDefinition.HasRasterProperty");

    bool hasRasterProp = false;

    if (!m_rasterPropName.empty())
    {
        hasRasterProp = true;
    }

    if (!hasRasterProp)
    {
        Ptr<MgPropertyDefinitionCollection> pdc = GetPropertiesIncludingBase();

        INT32 cnt = pdc->GetCount();
        for (INT32 i=0; i < cnt; i++)
        {
            Ptr<MgPropertyDefinition> propDef = pdc->GetItem(i);
            INT16 type = propDef->GetPropertyType();
            if (type == MgFeaturePropertyType::RasterProperty)
            {
                hasRasterProp = true;
                m_rasterPropName = ((MgRasterPropertyDefinition*)((MgPropertyDefinition*)propDef))->GetName();
            }
        }
    }

    return hasRasterProp;
}

//////////////////////////////////////////////////////////////
STRING MgClassDefinition::GetRasterPropertyName()
{
    bool check = HasRasterProperty();
    return m_rasterPropName;
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::Serialize(MgStream* stream)
{
    stream->WriteObject((MgPropertyDefinitionCollection*)m_classProperties);
    stream->WriteObject((MgPropertyDefinitionCollection*)m_identityProperties);
    stream->WriteString(m_schemaName);
    stream->WriteString(m_className);
    stream->WriteString(m_description);
    stream->WriteString(m_defaultGeometryPropertyName);
    stream->WriteString(m_serializedXml);
    stream->WriteBoolean(m_isComputed);
    stream->WriteBoolean(m_isAbstract);
    stream->WriteObject(m_baseClassDefinition);
    stream->WriteBoolean(m_isDeleted);
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::Deserialize(MgStream* stream)
{
    m_classProperties = (MgPropertyDefinitionCollection*)stream->GetObject();
    m_identityProperties  = (MgPropertyDefinitionCollection*)stream->GetObject();
    stream->GetString(m_schemaName);
    stream->GetString(m_className);
    stream->GetString(m_description);
    stream->GetString(m_defaultGeometryPropertyName);
    stream->GetString(m_serializedXml);
    stream->GetBoolean(m_isComputed);
    stream->GetBoolean(m_isAbstract);
    m_baseClassDefinition = (MgClassDefinition*)stream->GetObject();
    stream->GetBoolean(m_isDeleted);
}

//////////////////////////////////////////////////////////////
bool MgClassDefinition::CanSetName()
{
    return true;
}

//////////////////////////////////////////////////////////////
INT32 MgClassDefinition::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::SetSerializedXml(CREFSTRING strXml)
{
    m_serializedXml = strXml;
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::ToSimpleXml(string & str, bool includeProlog)
{
    STRING xml;
    if (includeProlog) 
    {
        xml = L"<?xml version=\"1.0\" encoding=\"utf-8\"?>";
        xml.append(L"<ClassDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"ClassDefinition-3.3.0.xsd\">");
    }
    else 
    {
        xml.append(L"<ClassDefinition>");
    }

    xml.append(L"<Name>");
    xml.append(GetName());
    xml.append(L"</Name>");

    xml.append(L"<Description>");
    xml.append(GetDescription());
    xml.append(L"</Description>");

    xml.append(L"<IsAbstract>");
    xml.append(IsAbstract() ? L"true" : L"false");
    xml.append(L"</IsAbstract>");

    xml.append(L"<IsComputed>");
    xml.append(IsComputed() ? L"true" : L"false");
    xml.append(L"</IsComputed>");

    xml.append(L"<DefaultGeometryPropertyName>");
    xml.append(GetDefaultGeometryPropertyName());
    xml.append(L"</DefaultGeometryPropertyName>");

    xml.append(L"<Properties>");
    Ptr<MgPropertyDefinitionCollection> props = GetProperties();
    Ptr<MgPropertyDefinitionCollection> idProps = GetIdentityProperties();
    INT32 pcount = props->GetCount();
    for (INT32 i = 0; i < pcount; i++) 
    {
        Ptr<MgPropertyDefinition> pdef = props->GetItem(i);
        INT32 ptype = pdef->GetPropertyType();
        if (ptype == MgFeaturePropertyType::DataProperty ||
            ptype == MgFeaturePropertyType::RasterProperty ||
            ptype == MgFeaturePropertyType::GeometricProperty)
        {
            xml.append(L"<Property>");

            xml.append(L"<Name>");
            xml.append(pdef->GetName());
            xml.append(L"</Name>");

            xml.append(L"<Description>");
            xml.append(pdef->GetDescription());
            xml.append(L"</Description>");
            
            xml.append(L"<PropertyType>");
            STRING wpType;
            MgUtil::Int32ToString(ptype, wpType);
            xml.append(wpType);
            xml.append(L"</PropertyType>");

            MgDataPropertyDefinition* dpd = dynamic_cast<MgDataPropertyDefinition*>(pdef.p);
            MgGeometricPropertyDefinition* gpd = dynamic_cast<MgGeometricPropertyDefinition*>(pdef.p);
            MgRasterPropertyDefinition* rpd = dynamic_cast<MgRasterPropertyDefinition*>(pdef.p);

            xml.append(L"<IsIdentity>");
            xml.append((idProps->IndexOf(pdef->GetName()) >= 0) ? L"true" : L"false");
            xml.append(L"</IsIdentity>");

            if (NULL != dpd)
            {
                xml.append(L"<DataType>");
                STRING wDataType;
                MgUtil::Int32ToString(dpd->GetDataType(), wDataType);
                xml.append(wDataType);
                xml.append(L"</DataType>");

                xml.append(L"<DefaultValue>");
                xml.append(dpd->GetDefaultValue());
                xml.append(L"</DefaultValue>");

                xml.append(L"<Length>");
                STRING wLength;
                MgUtil::Int32ToString(dpd->GetLength(), wLength);
                xml.append(wLength);
                xml.append(L"</Length>");

                xml.append(L"<Nullable>");
                xml.append(dpd->GetNullable() ? L"true" : L"false");
                xml.append(L"</Nullable>");

                xml.append(L"<ReadOnly>");
                xml.append(dpd->GetReadOnly() ? L"true" : L"false");
                xml.append(L"</ReadOnly>");

                xml.append(L"<IsAutoGenerated>");
                xml.append(dpd->IsAutoGenerated() ? L"true" : L"false");
                xml.append(L"</IsAutoGenerated>");

                xml.append(L"<Precision>");
                STRING wPrecision;
                MgUtil::Int32ToString(dpd->GetPrecision(), wPrecision);
                xml.append(wPrecision);
                xml.append(L"</Precision>");

                xml.append(L"<Scale>");
                STRING wScale;
                MgUtil::Int32ToString(dpd->GetScale(), wScale);
                xml.append(wScale);
                xml.append(L"</Scale>");
            }
            else if (NULL != gpd)
            {
                xml.append(L"<ReadOnly>");
                xml.append(gpd->GetReadOnly() ? L"true" : L"false");
                xml.append(L"</ReadOnly>");

                xml.append(L"<SpatialContextAssociation>");
                xml.append(gpd->GetSpatialContextAssociation());
                xml.append(L"</SpatialContextAssociation>");

                xml.append(L"<GeometryTypes>");
                STRING wGeomTypes;
                MgUtil::Int32ToString(gpd->GetGeometryTypes(), wGeomTypes);
                xml.append(wGeomTypes);
                xml.append(L"</GeometryTypes>");

                xml.append(L"<SpecificGeometryTypes>");
                Ptr<MgGeometryTypeInfo> sgt = gpd->GetSpecificGeometryTypes();
                INT32 scount = sgt->GetCount();
                for (INT32 g = 0; g < scount; g++)
                {
                    INT32 gt = sgt->GetType(g);
                    STRING wSpecGeomType;
                    MgUtil::Int32ToString(gt, wSpecGeomType);
                    xml.append(L"<Type>");
                    xml.append(wSpecGeomType);
                    xml.append(L"</Type>");
                }
                xml.append(L"</SpecificGeometryTypes>");

                xml.append(L"<HasElevation>");
                xml.append(gpd->GetHasElevation() ? L"true" : L"false");
                xml.append(L"</HasElevation>");

                xml.append(L"<HasMeasure>");
                xml.append(gpd->GetHasMeasure() ? L"true" : L"false");
                xml.append(L"</HasMeasure>");
            }
            else if (NULL != rpd)
            {
                xml.append(L"<Nullable>");
                xml.append(rpd->GetNullable() ? L"true" : L"false");
                xml.append(L"</Nullable>");

                xml.append(L"<ReadOnly>");
                xml.append(rpd->GetReadOnly() ? L"true" : L"false");
                xml.append(L"</ReadOnly>");

                xml.append(L"<SpatialContextAssociation>");
                xml.append(rpd->GetSpatialContextAssociation());
                xml.append(L"</SpatialContextAssociation>");

                xml.append(L"<DefaultImageXSize>");
                STRING wImageXSize;
                MgUtil::Int32ToString(rpd->GetDefaultImageXSize(), wImageXSize);
                xml.append(wImageXSize);
                xml.append(L"</DefaultImageXSize>");

                xml.append(L"<DefaultImageYSize>");
                STRING wImageYSize;
                MgUtil::Int32ToString(rpd->GetDefaultImageYSize(), wImageYSize);
                xml.append(wImageYSize);
                xml.append(L"</DefaultImageYSize>");
            }

            xml.append(L"</Property>");
        }
    }
    xml.append(L"</Properties>");

    xml.append(L"</ClassDefinition>");

    MgUtil::WideCharToMultiByte(xml, str);
}

//////////////////////////////////////////////////////////////
void MgClassDefinition::ToXml(string &str)
{
    assert(!m_serializedXml.empty());

    Ptr<MgClassDefinition> classDef = SAFE_ADDREF(this);
    while (classDef != NULL)
    {
        if (!(classDef->m_serializedXml).empty())
        {
            str += MgUtil::WideCharToMultiByte(classDef->m_serializedXml);
        }
        classDef = classDef->GetBaseClassDefinition();
    }
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the base class definition
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
MgClassDefinition* MgClassDefinition::GetBaseClassDefinition()
{
    return SAFE_ADDREF((MgClassDefinition*)m_baseClassDefinition);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests whether class definition is computed. Computed class is
/// transient and does not persist in database
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
bool MgClassDefinition::IsComputed()
{
    return m_isComputed;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests whether a class definition is abstract or not. If a class definition
/// is abstract it can not be used for insertions, deletion
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
bool MgClassDefinition::IsAbstract()
{
    return m_isAbstract;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Gets the base class definition
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
void MgClassDefinition::SetBaseClassDefinition(MgClassDefinition* baseClassDef)
{
    m_baseClassDefinition = SAFE_ADDREF((MgClassDefinition*)baseClassDef);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests whether class definition is computed. Computed class is
/// transient and does not persist in database
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
void MgClassDefinition::MakeClassComputed(bool isComputed)
{
    m_isComputed = isComputed;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Tests whether a class definition is abstract or not. If a class definition
/// is abstract it can not be used for insertions, deletion
/// </summary>
/// <returns>
/// Returns the feature class definition or NULL if no base class is defined.
/// </returns>
void MgClassDefinition::MakeClassAbstract(bool isAbstract)
{
    m_isAbstract = isAbstract;
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns a collection containing all the properties for this class
/// and its base classes
/// </summary>
/// <returns>
/// Returns a collection of PropertyDefinition classes.
/// </returns>
MgPropertyDefinitionCollection* MgClassDefinition::GetPropertiesIncludingBase()
{
    if (m_totalProperties == NULL)
    {
        m_totalProperties = new MgPropertyDefinitionCollection();
        Ptr<MgClassDefinition> classDef = SAFE_ADDREF(this);
        while (classDef != NULL)
        {
            Ptr<MgPropertyDefinitionCollection> properties = classDef->GetProperties();
            INT32 cnt = properties->GetCount();

            for (int i = 0; i < cnt; i++)
            {
                Ptr<MgPropertyDefinition> prop = properties->GetItem(i);
                m_totalProperties->Add(prop);
            }

            classDef = classDef->GetBaseClassDefinition();
        }
    }

    return SAFE_ADDREF((MgPropertyDefinitionCollection*)m_totalProperties);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Returns a collection containing all the properties for this class
/// and its base classes
/// </summary>
/// <returns>
/// Returns a collection of PropertyDefinition classes.
/// </returns>
MgStringCollection* MgClassDefinition::GetClassesIncludingBase()
{
    if (m_classList == NULL)
    {
        m_classList = new MgStringCollection();
        Ptr<MgClassDefinition> classDef = SAFE_ADDREF(this);
        while (classDef != NULL)
        {
            STRING name = classDef->GetName();
            m_classList->Add(name);
            classDef = classDef->GetBaseClassDefinition();
        }
    }

    return SAFE_ADDREF((MgStringCollection*)m_classList);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Marks the class definition for deletion.
/// </summary>
/// <returns>
/// Returns nothing.
/// </returns>
void MgClassDefinition::Delete()
{
    m_isDeleted = true;
}

bool MgClassDefinition::IsDeleted()
{
    return m_isDeleted;
}

STRING MgClassDefinition::GetSchemaName()
{
    return m_schemaName;
}


void MgClassDefinition::SetSchemaName(CREFSTRING schemaName)
{
    m_schemaName = schemaName;
}

STRING MgClassDefinition::GetQualifiedName()
{
    STRING qualifiedName;
    if (!m_schemaName.empty())
    {
        qualifiedName = m_schemaName;
        qualifiedName.append(L":");
    }
    qualifiedName.append(m_className);

    return qualifiedName;
}

bool MgClassDefinition::HasSerializedXml()
{
    return !(m_serializedXml.empty());
}

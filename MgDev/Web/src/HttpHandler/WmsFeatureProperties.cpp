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

#include "OgcFramework.h"
#include "WmsFeatureProperties.h"


MgWmsFeatureProperties::MgWmsFeatureProperties(MgPropertyCollection* propertyCollection)
:   m_index(-1),
    m_format(MgMimeType::Xml)
{
    m_propertyCollection = SAFE_ADDREF(propertyCollection);
}

MgWmsFeatureProperties::~MgWmsFeatureProperties()
{
}

void MgWmsFeatureProperties::SetFormat(CREFSTRING format)
{
    m_format = format;
}

STRING MgWmsFeatureProperties::GetLayerName()
{
    STRING ret;
    if (m_propertyCollection != NULL)
    {
        INT32 gidx = m_propertyCollection->IndexOf(_("_MgLayerName"));
        if (gidx >= 0)
        {
            Ptr<MgProperty> prop = m_propertyCollection->GetItem(gidx);
            if (prop->GetPropertyType() == MgPropertyType::String)
            {
                MgStringProperty* sp = static_cast<MgStringProperty*>(prop.p);
                if (!sp->IsNull())
                {
                    ret = sp->GetValue();
                }
            }
        }
    }
    return ret;
}

MgWmsFeatureGeometry* MgWmsFeatureProperties::GetGeometry()
{
    Ptr<MgWmsFeatureGeometry> ret;
    Ptr<MgGeometry> geom = NULL;
    if (m_propertyCollection != NULL)
    {
        INT32 gidx = m_propertyCollection->IndexOf(_("_MgGeometry"));
        if (gidx >= 0)
        {
            Ptr<MgProperty> prop = m_propertyCollection->GetItem(gidx);
            if (prop->GetPropertyType() == MgPropertyType::Geometry)
            {
                MgGeometryProperty* gp = static_cast<MgGeometryProperty*>(prop.p);
                if (!gp->IsNull())
                {
                    Ptr<MgByteReader> agf = gp->GetValue();
                    MgAgfReaderWriter agfRw;
                    geom = agfRw.Read(agf);
                }
            }
        }
    }

    ret = new MgWmsFeatureGeometry(geom);
    ret->SetFormat(m_format);

    return ret.Detach();
}

bool MgWmsFeatureProperties::IsSpecialProperty(MgProperty* prop)
{
    return (szcmp(prop->GetName().c_str(), _("_MgLayerName")) == 0)
        || (szcmp(prop->GetName().c_str(), _("_MgGeometry")) == 0)
        || (szcmp(prop->GetName().c_str(), _("_MgFeatureBoundingBox")) == 0);
}

bool MgWmsFeatureProperties::Next()
{
    if(m_propertyCollection != NULL && m_index < m_propertyCollection->GetCount() - 1)
    {
        m_index++;

        // Skip the special properties
        Ptr<MgProperty> prop = m_propertyCollection->GetItem(m_index);
        if (IsSpecialProperty(prop))
        {
            return Next();
        }
        return true;
    }
    else
    {
        return false;
    }
}

void MgWmsFeatureProperties::GenerateDefinitions(MgUtilDictionary& dictionary)
{
    if(m_propertyCollection != NULL && m_index >= 0 && m_index < m_propertyCollection->GetCount())
    {
        Ptr<MgProperty> prop = m_propertyCollection->GetItem(m_index);
        STRING name = MgUtil::ReplaceEscapeCharInXml(prop->GetName());
        // Skip the special properties
        if (!IsSpecialProperty(prop))
        {
            STRING sIndex;
            MgUtil::Int32ToString(m_index, sIndex);
            if (prop->GetPropertyType() == MgPropertyType::String)
            {
                MgStringProperty* stringProp = static_cast<MgStringProperty*>(prop.p);
                STRING value = MgUtil::ReplaceEscapeCharInXml(stringProp->GetValue());

                dictionary.AddDefinition(_("FeatureProperty.Name"), name.c_str());
                dictionary.AddDefinition(_("FeatureProperty.Value"), value.c_str());
                //A bit hack-ish but we need this for formats like GeoJSON so we know whether to insert a "," or not
                //The server implementation will ensure that all the non-special properties are at the end of the collection
                //ie. _MgLayerName, _MgGeometry, etc will never be the last property
                dictionary.AddDefinition(_("FeatureProperty.IsLast"), (m_index == m_propertyCollection->GetCount() - 1) ? _("1") : _("0"));
            }
        }
    }
}



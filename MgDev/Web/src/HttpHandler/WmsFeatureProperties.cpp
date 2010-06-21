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

#include "OgcFramework.h"
#include "WmsFeatureProperties.h"


MgWmsFeatureProperties::MgWmsFeatureProperties(MgPropertyCollection* propertyCollection)
:   m_index(-1)
{
    m_propertyCollection = SAFE_ADDREF(propertyCollection);
}

MgWmsFeatureProperties::~MgWmsFeatureProperties()
{
}

bool MgWmsFeatureProperties::Next()
{
    if(m_propertyCollection != NULL && m_index < m_propertyCollection->GetCount() - 1)
    {
        m_index++;

        // Skip the special layer name property
        Ptr<MgProperty> prop = m_propertyCollection->GetItem(m_index);
        if(szcmp(prop->GetName().c_str(), _("_MgLayerName")) == 0)
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
        Ptr<MgStringProperty> stringProp = (MgStringProperty*)m_propertyCollection->GetItem(m_index);
        if(stringProp != NULL)
        {
            STRING name = MgUtil::ReplaceEscapeCharInXml(stringProp->GetName());

            // Skip the special layer name property
            if(szcmp(name.c_str(), _("_MgLayerName")) != 0)
            {
                STRING value = MgUtil::ReplaceEscapeCharInXml(stringProp->GetValue());

                dictionary.AddDefinition(_("FeatureProperty.Name"), name.c_str());
                dictionary.AddDefinition(_("FeatureProperty.Value"), value.c_str());
            }
        }
    }
}



//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
#include "WmsFeatureInfo.h"
#include "WmsFeatureProperties.h"

CPSZ kpszDefinitionFeatureInfoLayerName = _("FeatureInfo.LayerName");
CPSZ kpszLayerNameProperty = _("_MgLayerName");

MgWmsFeatureInfo::MgWmsFeatureInfo(MgBatchPropertyCollection* propertyCollection)
:   m_index(-1)
{
    m_propertyCollection = SAFE_ADDREF(propertyCollection);
}

MgWmsFeatureInfo::~MgWmsFeatureInfo()
{
}

bool MgWmsFeatureInfo::Next()
{
    if(m_propertyCollection != NULL && m_index < m_propertyCollection->GetCount() - 1)
    {
        m_index++;
        return true;
    }
    else
    {
        return false;
    }
}

void MgWmsFeatureInfo::GenerateDefinitions(MgUtilDictionary& dictionary)
{
    if(m_propertyCollection != NULL && m_index >= 0 && m_index < m_propertyCollection->GetCount())
    {
        MgPropertyCollection* props = m_propertyCollection->GetItem(m_index);
        if(props->Contains(kpszLayerNameProperty))
        {
            MgStringProperty* layerNameProp = (MgStringProperty*)props->GetItem(kpszLayerNameProperty);
            STRING value = MgUtil::ReplaceEscapeCharInXml(layerNameProp->GetValue());
            if(!value.empty())
            {
                dictionary.AddDefinition(kpszDefinitionFeatureInfoLayerName, value);
            }
        }
    }
}

MgWmsFeatureProperties* MgWmsFeatureInfo::GetCurrentProperties()
{
    MgWmsFeatureProperties* wmsProps = NULL;
    if(m_propertyCollection != NULL && m_index >= 0 && m_index < m_propertyCollection->GetCount())
    {
        MgPropertyCollection* props = m_propertyCollection->GetItem(m_index);
        if(props != NULL)
        {
            wmsProps = new MgWmsFeatureProperties(props);
        }
    }
    return wmsProps;
}




//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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
#include "WmsFeatureGeometry.h"

MgWmsFeatureGeometry::MgWmsFeatureGeometry(MgGeometry * geom)
    : m_bRead(false), m_format(MgMimeType::Text)
{
    m_geom = SAFE_ADDREF(geom);
}

MgWmsFeatureGeometry::~MgWmsFeatureGeometry()
{
    m_geom = NULL;
}

void MgWmsFeatureGeometry::SetFormat(CREFSTRING format)
{
    m_format = format;
}

bool MgWmsFeatureGeometry::Next()
{
    // Per the IOgcResourceEnumerator contract, we have to iterate, but we only want to iterate once
    if (!m_bRead) 
    {
        m_bRead = true;
        return true;
    }
    return false;
}

void MgWmsFeatureGeometry::GenerateDefinitions(MgUtilDictionary& dictionary)
{
    if (NULL != m_geom.p)
    {
        if (MgMimeType::Json == m_format) //Give GeoJSON geometry to json
        {
            MgGeoJsonWriter gjw;
            STRING value;
            gjw.ToGeoJson(m_geom, value, false);
            dictionary.AddDefinition(_("FeatureGeometry.Value"), value);
        }
        else //Give everything else WKT
        {
            MgWktReaderWriter wktRw;
            STRING value = wktRw.Write(m_geom);
            dictionary.AddDefinition(_("FeatureGeometry.Value"), value);
        }
    }
    else
    {
        if (MgMimeType::Json == m_format) 
        {
            dictionary.AddDefinition(_("FeatureGeometry.Value"), _("null"));
        }
        else 
        {
            dictionary.AddDefinition(_("FeatureGeometry.Value"), _(""));
        }
    }
}

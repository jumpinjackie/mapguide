//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgFileFeatureSourceParams);

MgFileFeatureSourceParams::MgFileFeatureSourceParams()
:
m_xyTolerance(0.0),
m_zTolerance(0.0)
{
}

MgFileFeatureSourceParams::MgFileFeatureSourceParams(CREFSTRING providerName)
:
m_xyTolerance(0.0),
m_zTolerance(0.0)
{
    m_providerName = providerName;
}

MgFileFeatureSourceParams::MgFileFeatureSourceParams(CREFSTRING providerName, CREFSTRING spatialContextName, CREFSTRING srsWkt, MgFeatureSchema* featureSchema)
:
m_xyTolerance(0.0),
m_zTolerance(0.0)
{
    m_providerName = providerName;
    m_spatialContextName = spatialContextName;
    m_srsWkt = srsWkt;
    m_featureSchema = SAFE_ADDREF(featureSchema);
}

STRING MgFileFeatureSourceParams::GetProviderName()
{
    return m_providerName;
}

void MgFileFeatureSourceParams::SetProviderName(CREFSTRING name)
{
    m_providerName = name;
}

STRING MgFileFeatureSourceParams::GetFileName()
{
    return m_fileName;
}

void MgFileFeatureSourceParams::SetFileName(CREFSTRING name)
{
    m_fileName = name;
}

STRING MgFileFeatureSourceParams::GetSpatialContextName()
{
    return m_spatialContextName;
}

void MgFileFeatureSourceParams::SetSpatialContextName(CREFSTRING name)
{
    m_spatialContextName = name;
}

STRING MgFileFeatureSourceParams::GetSpatialContextDescription()
{
    return m_spatialContextDescription;
}

void MgFileFeatureSourceParams::SetSpatialContextDescription(CREFSTRING description)
{
    m_spatialContextDescription = description;
}

STRING MgFileFeatureSourceParams::GetCoordinateSystemWkt()
{
    return m_srsWkt;
}

void MgFileFeatureSourceParams::SetCoordinateSystemWkt(CREFSTRING srsWkt)
{
    m_srsWkt = srsWkt;
}

double MgFileFeatureSourceParams::GetXYTolerance()
{
    return m_xyTolerance;
}

void MgFileFeatureSourceParams::SetXYTolerance(double tolerance)
{
    m_xyTolerance = tolerance;
}

double MgFileFeatureSourceParams::GetZTolerance()
{
    return m_zTolerance;
}

void MgFileFeatureSourceParams::SetZTolerance(double tolerance)
{
    m_zTolerance = tolerance;
}

MgFeatureSchema* MgFileFeatureSourceParams::GetFeatureSchema()
{
    return SAFE_ADDREF((MgFeatureSchema*)m_featureSchema);
}

void MgFileFeatureSourceParams::SetFeatureSchema(MgFeatureSchema* featureSchema)
{
    m_featureSchema = SAFE_ADDREF(featureSchema);
}

MgFileFeatureSourceParams::~MgFileFeatureSourceParams()
{
}


void MgFileFeatureSourceParams::Serialize(MgStream* stream)
{
    stream->WriteString(m_providerName);
    stream->WriteString(m_spatialContextName);
    stream->WriteString(m_spatialContextDescription);
    stream->WriteString(m_srsWkt);
    stream->WriteDouble(m_xyTolerance);
    stream->WriteDouble(m_zTolerance);
    stream->WriteString(m_fileName);
    stream->WriteObject(m_featureSchema);
}


void MgFileFeatureSourceParams::Deserialize(MgStream* stream)
{
    stream->GetString(m_providerName);
    stream->GetString(m_spatialContextName);
    stream->GetString(m_spatialContextDescription);
    stream->GetString(m_srsWkt);
    stream->GetDouble(m_xyTolerance);
    stream->GetDouble(m_zTolerance);
    stream->GetString(m_fileName);
    m_featureSchema = dynamic_cast<MgFeatureSchema*>(stream->GetObject());
}


INT32 MgFileFeatureSourceParams::GetClassId()
{
    return m_cls_id;
}

void MgFileFeatureSourceParams::Dispose()
{
    delete this;
}

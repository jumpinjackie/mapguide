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

MG_IMPL_DYNCREATE(MgCreateSdfParams);


MgCreateSdfParams::MgCreateSdfParams()
:
m_xyTolerance(0.0),
m_zTolerance(0.0)
{
}

MgCreateSdfParams::MgCreateSdfParams(CREFSTRING spatialContextName, CREFSTRING srsWkt, MgFeatureSchema* featureSchema)
:
m_xyTolerance(0.0),
m_zTolerance(0.0)
{
    m_spatialContextName = spatialContextName;
    m_srsWkt = srsWkt;
    m_featureSchema = SAFE_ADDREF(featureSchema);
}

STRING MgCreateSdfParams::GetSpatialContextName()
{
    return m_spatialContextName;
}

void MgCreateSdfParams::SetSpatialContextName(CREFSTRING name)
{
    m_spatialContextName = name;
}

STRING MgCreateSdfParams::GetSpatialContextDescription()
{
    return m_spatialContextDescription;
}

void MgCreateSdfParams::SetSpatialContextDescription(CREFSTRING description)
{
    m_spatialContextDescription = description;
}

STRING MgCreateSdfParams::GetCoordinateSystemWkt()
{
    return m_srsWkt;
}

void MgCreateSdfParams::SetCoordinateSystemWkt(CREFSTRING srsWkt)
{
    m_srsWkt = srsWkt;
}

double MgCreateSdfParams::GetXYTolerance()
{
    return m_xyTolerance;
}

void MgCreateSdfParams::SetXYTolerance(double tolerance)
{
    m_xyTolerance = tolerance;
}

double MgCreateSdfParams::GetZTolerance()
{
    return m_zTolerance;
}

void MgCreateSdfParams::SetZTolerance(double tolerance)
{
    m_zTolerance = tolerance;
}

MgFeatureSchema* MgCreateSdfParams::GetFeatureSchema()
{
    return SAFE_ADDREF((MgFeatureSchema*)m_featureSchema);
}

void MgCreateSdfParams::SetFeatureSchema(MgFeatureSchema* featureSchema)
{
    m_featureSchema = SAFE_ADDREF(featureSchema);
}

MgCreateSdfParams::~MgCreateSdfParams()
{
}


void MgCreateSdfParams::Serialize(MgStream* stream)
{
    stream->WriteString(m_spatialContextName);
    stream->WriteString(m_spatialContextDescription);
    stream->WriteString(m_srsWkt);
    stream->WriteDouble(m_xyTolerance);
    stream->WriteDouble(m_zTolerance);
    stream->WriteObject(m_featureSchema);
}


void MgCreateSdfParams::Deserialize(MgStream* stream)
{
    stream->GetString(m_spatialContextName);
    stream->GetString(m_spatialContextDescription);
    stream->GetString(m_srsWkt);
    stream->GetDouble(m_xyTolerance);
    stream->GetDouble(m_zTolerance);
    m_featureSchema = dynamic_cast<MgFeatureSchema*>(stream->GetObject());
}


INT32 MgCreateSdfParams::GetClassId()
{
    return m_cls_id;
}

void MgCreateSdfParams::Dispose()
{
    delete this;
}

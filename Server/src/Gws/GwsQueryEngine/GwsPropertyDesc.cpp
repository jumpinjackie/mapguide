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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"


CGwsPropertyDesc::CGwsPropertyDesc (
    const WSTR &    name,
    FdoPropertyType ptype,
    FdoDataType     dataprop,
    bool            nullable,
    int             length,
    int             precision,
    int             scale,
    bool            haselevation,
    bool            hasmeasure,
    int             geometrytypes,
    bool            bReadOnly,
    bool            bIsComputedIdentifier,
    const WSTR  &   spatialcontext
)
{
    m_name     =  name;
    m_ptype    =  ptype;
    m_dataprop =  dataprop;
    m_nullable =  nullable;
    m_length   =  length;
    m_precision=  precision;
    m_scale    =  scale;
    m_haselevation  = haselevation;
    m_hasmeasure    = hasmeasure;
    m_geometrytypes = geometrytypes;
    m_bReadOnly = bReadOnly;
    m_bIsComputedIdentifier = bIsComputedIdentifier;
    m_spatialcontext = spatialcontext;
}


CGwsPropertyDesc::CGwsPropertyDesc (FdoPropertyDefinition * propdef)
{
    FdoPropertyType ptype = propdef->GetPropertyType ();
    m_ptype = ptype;
    if (ptype == FdoPropertyType_DataProperty) {
        FdoDataPropertyDefinition * pDataProp = static_cast<FdoDataPropertyDefinition*>(propdef);
        m_dataprop = pDataProp->GetDataType ();
        m_nullable = pDataProp->GetNullable ();
        m_length = pDataProp->GetLength ();
        m_precision = pDataProp->GetPrecision ();
        m_scale  = pDataProp->GetScale ();
        m_bReadOnly = pDataProp->GetReadOnly ();
        m_haselevation = false;
        m_hasmeasure = false;
        m_geometrytypes = 0;
    } else if (ptype == FdoPropertyType_GeometricProperty) {
        FdoGeometricPropertyDefinition * pGeomDef = (FdoGeometricPropertyDefinition*)propdef;
        m_dataprop = FdoDataType_String;
        m_nullable = false;
        m_length = 0;
        m_precision = 0;
        m_scale  = 0;
        m_bReadOnly = 0;
        m_haselevation = pGeomDef->GetHasElevation ();
        m_hasmeasure = pGeomDef->GetHasMeasure ();
        m_geometrytypes = pGeomDef->GetGeometryTypes ();
        m_spatialcontext = pGeomDef->GetSpatialContextAssociation () != NULL ?
                                pGeomDef->GetSpatialContextAssociation () : L"";
    }
    m_name = propdef->GetName ();
    m_bIsComputedIdentifier = false;
}


CGwsPropertyDesc::CGwsPropertyDesc (const CGwsPropertyDesc & other)
{
    * this = other;
}


void CGwsPropertyDesc::operator= (const CGwsPropertyDesc & other)
{
    m_name     =  other.m_name;
    m_ptype    =  other.m_ptype;
    m_dataprop =  other.m_dataprop;
    m_nullable =  other.m_nullable;
    m_length   =  other.m_length;
    m_precision=  other.m_precision;
    m_scale    =  other.m_scale;
    m_haselevation  = other.m_haselevation;
    m_hasmeasure    = other.m_hasmeasure;
    m_geometrytypes = other.m_geometrytypes;
    m_bReadOnly = other.m_bReadOnly;
    m_bIsComputedIdentifier = other.m_bIsComputedIdentifier;
    m_spatialcontext = other.m_spatialcontext;
}

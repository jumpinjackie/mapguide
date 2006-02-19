//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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
#include <stdafx.h>

#include "GwsQueryEngineImp.h"

static CGwsPropertyDesc s_pdesc (L"", (FdoPropertyType) 0, (FdoDataType) 0, false, 0, 0, 0, false, false, 0, false, L"");


/////////////////////////////////////////////////////////////////////
//
// class CGwsQueryResultDescriptors
//
/////////////////////////////////////////////////////////////////////
CGwsQueryResultDescriptors::CGwsQueryResultDescriptors ()
{
}

CGwsQueryResultDescriptors::CGwsQueryResultDescriptors (
    const CGwsQueryResultDescriptors & other
)
{
    m_classDef = other.m_classDef;
    m_classname = other.m_classname;
    m_propertynames = GisStringCollection::Create (other.m_propertynames);
    m_propdsc = other.m_propdsc;
    m_suffix = other.m_suffix;
    CGwsQueryResultDescriptors & src = (CGwsQueryResultDescriptors &) other;
    for (int i = 0; i < src.GetCount (); i ++) {
        GisPtr<IGWSExtendedFeatureDescription> fdsc = src.GetItem (i);
        Add (fdsc);
    }
}


CGwsQueryResultDescriptors::CGwsQueryResultDescriptors (
    FdoClassDefinition     * classDef,
    const GWSQualifiedName & classname,
    GisStringCollection    * propnames
)
{
    m_classDef = classDef;
    if (classDef != NULL)
        classDef->AddRef ();
    m_classname = classname;
    m_propertynames = GisStringCollection::Create ();
    appendPropertyNames (propnames, classDef, m_propertynames, m_propdsc);
}

CGwsQueryResultDescriptors::~CGwsQueryResultDescriptors () throw()
{
    for (size_t i = 0; i < m_descriptors.size (); i ++) {
        CGwsQueryResultDescriptors * pDesc = (CGwsQueryResultDescriptors *) m_descriptors[i];
        pDesc->SetOwner (NULL);
        pDesc->Release ();
    }
}

void CGwsQueryResultDescriptors::appendPropertyNames (
    GisStringCollection       * propnamestoadd,
    FdoClassDefinition        * classDef,
    GisStringCollection       * propnames,
    std::vector<CGwsPropertyDesc> & propdsc
)
{
    GisPtr<FdoPropertyDefinitionCollection> properties;

    GisPtr<FdoClassDefinition> baseClass = classDef->GetBaseClass ();
    if (baseClass != NULL) {
        appendPropertyNames (propnamestoadd, baseClass, propnames, propdsc);
    }

    properties = classDef->GetProperties ();
    if (properties == NULL)
        return;
    for (int i = 0; i < properties->GetCount (); i ++) {
        GisPtr<FdoPropertyDefinition>   pFdoProperty = properties->GetItem (i);
        if (propnamestoadd == NULL ||
            propnamestoadd->IndexOf (pFdoProperty->GetName ()) >= 0)
            pushPropDefinition (pFdoProperty, propnames,propdsc);
    }
}

void CGwsQueryResultDescriptors::pushPropDefinition (
    FdoPropertyDefinition     * propdef,
    GisStringCollection       * propnames,
    std::vector<CGwsPropertyDesc> & propdsc
)
{
    propnames->Add (propdef->GetName());

    FdoPropertyType ptype = propdef->GetPropertyType ();
    FdoDataType     dtype = FdoDataType_String;
    bool            nullable = true;
    int             length = 0;
    int             precision = 0;
    int             scale  = 0;
    bool            haselevation = false;
    bool            hasmeasure = false;
    int             geometrytypes = 0;
    bool            bReadOnly = false;
    WSTR            spatialcontext;
    if (ptype == FdoPropertyType_DataProperty) {
        FdoDataPropertyDefinition * pDataProp = static_cast<FdoDataPropertyDefinition*>(propdef);
        dtype = pDataProp->GetDataType ();
        nullable = pDataProp->GetNullable ();
        length = pDataProp->GetLength ();
        precision = pDataProp->GetPrecision ();
        scale  = pDataProp->GetScale ();
        bReadOnly = pDataProp->GetReadOnly ();
    } else if (ptype == FdoPropertyType_GeometricProperty) {
        FdoGeometricPropertyDefinition * pGeomDef = (FdoGeometricPropertyDefinition*)propdef;
        haselevation = pGeomDef->GetHasElevation ();
        hasmeasure = pGeomDef->GetHasMeasure ();
        geometrytypes = pGeomDef->GetGeometryTypes ();
        GisString * sc = pGeomDef->GetSpatialContextAssociation ();
        spatialcontext = sc != NULL ? sc : L"";
    }

    propdsc.push_back (CGwsPropertyDesc (propdef->GetName (),
                                     ptype, dtype,
                                     nullable, length, precision, scale,
                                     haselevation, hasmeasure, geometrytypes,
                                     bReadOnly,
                                     spatialcontext));
}

const GWSQualifiedName  & CGwsQueryResultDescriptors::ClassName ()
{
    return m_classname;
}
FdoClassDefinition  * CGwsQueryResultDescriptors::ClassDefinition ()
{
    if (m_classDef != NULL)
        m_classDef.p->AddRef ();
    return m_classDef;
}
void CGwsQueryResultDescriptors::SetCSName (const GWSCoordinateSystem & csname)
{
    m_csname = csname;
}

const GWSCoordinateSystem & CGwsQueryResultDescriptors::GetCoordinateSystem ()
{
    return m_csname;
}

int CGwsQueryResultDescriptors::GetCount  ()
{
    return (int )m_descriptors.size ();
}

IGWSExtendedFeatureDescription * CGwsQueryResultDescriptors::GetItem   (int i)
{
    if (i < 0 || i >= GetCount ())
        GWS_THROW (eGwsIndexOutOfBounds);

    IGWSExtendedFeatureDescription * pdesc = m_descriptors[i];
    if (pdesc != NULL)
        pdesc->AddRef ();
    return pdesc;
}

IGWSExtendedFeatureDescription * CGwsQueryResultDescriptors::GetItem (
    const GWSQualifiedName & name
)
{
    IGWSExtendedFeatureDescription * res = NULL;
    if (m_classname == name) {
        AddRef ();
        res = this;
    }

    for (int i = 0; res == NULL && i < GetCount (); i ++) {
        GisPtr<IGWSExtendedFeatureDescription> fdesc = GetItem (i);
        res = fdesc->GetItem (name);
    }
    if (res == NULL)
        GWS_THROW (eGwsFailedToDesribeClass);
    return res;
}


GisStringCollection  * CGwsQueryResultDescriptors::PropertyNames ()
{
    if (m_propertynames != NULL)
        m_propertynames.p->AddRef ();
    return m_propertynames;
}

void CGwsQueryResultDescriptors::Add (IGWSExtendedFeatureDescription * pIDesc)
{
    if (pIDesc == NULL)
        return;
    CGwsQueryResultDescriptors * pDesc = (CGwsQueryResultDescriptors *) pIDesc;
    CGwsQueryResultDescriptors * pClone = new CGwsQueryResultDescriptors (*pDesc);
    pClone->AddRef ();

    pClone->SetOwner (this);
    m_descriptors.push_back (pClone);
}


const CGwsPropertyDesc & CGwsQueryResultDescriptors::GetPropertyDescriptor (int idx)
{

    if (0 <= idx && idx < (int) m_propdsc.size ())
        return m_propdsc [idx];
    return s_pdesc;
}

const CGwsPropertyDesc & CGwsQueryResultDescriptors::GetPropertyDescriptor (
    GisString * name
)
{

    for (size_t i = 0; i < m_propdsc.size (); i ++) {
        if (wcscmp (m_propdsc[i].m_name.c_str (), name) == 0)
            return m_propdsc[i];
    }
    return s_pdesc;
}

const CGwsPropertyDesc & CGwsQueryResultDescriptors::GetGeometryPropertyDescriptor ()
{
    for (int i = 0; i < (int) m_propdsc.size (); i ++) {
        if (m_propdsc [i].m_ptype == FdoPropertyType_GeometricProperty)
            return  m_propdsc [i];
    }
    return s_pdesc;
}


const std::vector<CGwsPropertyDesc> & CGwsQueryResultDescriptors::GetPropertyDescriptors ()
{
    return m_propdsc;
}

int CGwsQueryResultDescriptors::Contains (GisString* propertyName)
{
    GisPtr<GisStringCollection> pPropNames = PropertyNames();
    for(int i=0;i<pPropNames->GetCount();i++)
    {
        GisString* pComp = pPropNames->GetString(i);
        if( _wcsicmp( propertyName, pComp ) == 0)
            return i;
    }
    return -1;
}

FdoDataPropertyDefinitionCollection *
    CGwsQueryResultDescriptors::GetIdentityProperties ()
{
    if (m_identprops == NULL) {
        GisPtr<FdoClassDefinition> classdef = ClassDefinition ();
        GwsCommonFdoUtils::GetFdoClassIdentityProperties (classdef, m_identprops.p);
    }
    if (m_identprops != NULL)
        m_identprops.p->AddRef ();
    return m_identprops;
}


FdoPropertyDefinition * CGwsQueryResultDescriptors::FindPropertyDefinition (
    GisString* propertyName
)
{
    for (size_t i = 0; i < m_propdsc.size (); i ++) {
        if (wcscmp (m_propdsc[i].m_name.c_str (), propertyName) == 0) {
            return GwsCommonFdoUtils::GetPropertyDefinition (m_classDef, m_propdsc[i].m_name.c_str ());
        }
    }
    return NULL;
}

const WSTR & CGwsQueryResultDescriptors::GetSuffix () const
{
    return m_suffix;
}

void CGwsQueryResultDescriptors::SetSuffix (const WSTR & suffix)
{
    m_suffix = suffix;
}

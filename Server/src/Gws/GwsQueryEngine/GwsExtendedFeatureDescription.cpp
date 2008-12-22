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

/////////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GwsQueryEngineImp.h"
#include "FdoExpressionEngine.h"


static CGwsPropertyDesc s_pdesc (L"", (FdoPropertyType) 0, (FdoDataType) 0, false, 0, 0, 0, false, false, 0, false, false, L"");


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
    m_propertynames = FdoStringCollection::Create (other.m_propertynames);
    m_propdsc = other.m_propdsc;
    m_suffix = other.m_suffix;
    m_joinName = other.m_joinName;
    m_joinDelimiter = other.m_joinDelimiter;
    m_forceOneToOne = other.m_forceOneToOne;
    CGwsQueryResultDescriptors & src = (CGwsQueryResultDescriptors &) other;
    for (int i = 0; i < src.GetCount (); i ++) {
        FdoPtr<IGWSExtendedFeatureDescription> fdsc = src.GetItem (i);
        Add (fdsc);
    }
}


CGwsQueryResultDescriptors::CGwsQueryResultDescriptors (
    FdoClassDefinition     * classDef,
    const GWSQualifiedName & classname,
    const FdoString        * joinName,
    const FdoString        * joinDelimiter,
    bool                     forceOneToOne,
    FdoIdentifierCollection    * propnames
)
{
    m_classDef = classDef;
    if (classDef != NULL)
        classDef->AddRef ();
    m_classname = classname;
    if(NULL != joinName)
        m_joinName = joinName;
    if(NULL != joinDelimiter)
        m_joinDelimiter = joinDelimiter;
    m_forceOneToOne = forceOneToOne;
    m_propertynames = FdoStringCollection::Create ();
    appendPropertyNames (propnames, classDef, m_propertynames, m_propdsc);
    if( propnames != NULL )
    {
        // We need to apend the computed identifier;
        for (int i = 0; i < propnames->GetCount (); i ++) {
            FdoPtr<FdoIdentifier>prop = propnames->GetItem(i);
            FdoComputedIdentifier* c_prop = dynamic_cast<FdoComputedIdentifier*>(prop.p);
            if( c_prop != NULL )
            {
                FdoPropertyType propType = FdoPropertyType_DataProperty;
                FdoDataType dataType = FdoDataType_Double;
                FdoExpressionEngine::GetExpressionType( classDef,  FdoPtr<FdoExpression>(c_prop->GetExpression()) , propType, dataType);
                m_propertynames->Add (c_prop->GetName());

                m_propdsc.push_back (CGwsPropertyDesc (c_prop->GetName (),
                                     propType, dataType,
                                     false, 0, 0, 0,
                                     false, false, 0,
                                     true,
                                     true,
                                     L""));
            }
        }
    }
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
    FdoIdentifierCollection       * propnamestoadd,
    FdoClassDefinition        * classDef,
    FdoStringCollection       * propnames,
    std::vector<CGwsPropertyDesc> & propdsc
)
{
    FdoPtr<FdoPropertyDefinitionCollection> properties;

    FdoPtr<FdoClassDefinition> baseClass = classDef->GetBaseClass ();
    if (baseClass != NULL) {
        appendPropertyNames (propnamestoadd, baseClass, propnames, propdsc);

    }

    properties = classDef->GetProperties ();
    if (properties == NULL)
        return;
    for (int i = 0; i < properties->GetCount (); i ++) {
        FdoPtr<FdoPropertyDefinition>   pFdoProperty = properties->GetItem (i);
        if (propnamestoadd == NULL ||
            propnamestoadd->IndexOf (pFdoProperty->GetName ()) >= 0)
            pushPropDefinition (pFdoProperty, propnames,propdsc);
    }
}


void CGwsQueryResultDescriptors::pushPropDefinition (
    FdoPropertyDefinition     * propdef,
    FdoStringCollection       * propnames,
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
        FdoString * sc = pGeomDef->GetSpatialContextAssociation ();
        spatialcontext = sc != NULL ? sc : L"";
    }

    propdsc.push_back (CGwsPropertyDesc (propdef->GetName (),
                                     ptype, dtype,
                                     nullable, length, precision, scale,
                                     haselevation, hasmeasure, geometrytypes,
                                     bReadOnly,
                                     false,
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
        FdoPtr<IGWSExtendedFeatureDescription> fdesc = GetItem (i);
        try {
            res = fdesc->GetItem (name);
        } catch (IGWSException* pex) {
            // eat failed to describe. this is expected in join case
            if(eGwsFailedToDesribeClass != pex->GetStatus())
                throw;
        }
    }
    if (res == NULL)
        GWS_THROW (eGwsFailedToDesribeClass);
    return res;
}


FdoStringCollection  * CGwsQueryResultDescriptors::PropertyNames ()
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
    FdoString * name
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


int CGwsQueryResultDescriptors::Contains (FdoString* propertyName)
{
    FdoPtr<FdoStringCollection> pPropNames = PropertyNames();
    for(int i=0;i<pPropNames->GetCount();i++)
    {
        FdoString* pComp = pPropNames->GetString(i);
        if( _wcsicmp( propertyName, pComp ) == 0)
            return i;
    }
    return -1;
}


FdoDataPropertyDefinitionCollection *
    CGwsQueryResultDescriptors::GetIdentityProperties ()
{
    if (m_identprops == NULL) {
        FdoPtr<FdoClassDefinition> classdef = ClassDefinition ();
        GwsCommonFdoUtils::GetFdoClassIdentityProperties (classdef, m_identprops.p);
    }
    if (m_identprops != NULL)
        m_identprops.p->AddRef ();
    return m_identprops;
}


FdoPropertyDefinition * CGwsQueryResultDescriptors::FindPropertyDefinition (
    FdoString* propertyName
)
{
    FdoPropertyDefinition* retProp = NULL;
    for (size_t i = 0; i < m_propdsc.size (); i ++) {
        if (wcscmp (m_propdsc[i].m_name.c_str (), propertyName) == 0) {
            retProp = GwsCommonFdoUtils::GetPropertyDefinition (m_classDef, m_propdsc[i].m_name.c_str ());
            if( retProp == NULL && m_propdsc[i].m_bIsComputedIdentifier )
            {
                // It must be a computed property
                CGwsPropertyDesc propDesc = m_propdsc[i];
                FdoDataPropertyDefinition* prop = FdoDataPropertyDefinition::Create();
                prop->SetName( propertyName );
                prop->SetDataType( propDesc.m_dataprop );
                prop->SetNullable( propDesc.m_nullable );
                prop->SetLength( propDesc.m_length );
                prop->SetPrecision( propDesc.m_precision );
                prop->SetScale( propDesc.m_scale );
                prop->SetReadOnly( propDesc.m_bReadOnly );
                retProp = prop;
            }
            break;
        }
    }
    return retProp;
}


const WSTR & CGwsQueryResultDescriptors::GetSuffix () const
{
    return m_suffix;
}


void CGwsQueryResultDescriptors::SetSuffix (const WSTR & suffix)
{
    m_suffix = suffix;
}

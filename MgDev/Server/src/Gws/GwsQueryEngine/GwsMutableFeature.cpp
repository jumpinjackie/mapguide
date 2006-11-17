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
#include "GwsBinaryFeatureWriter.h"

//TODO: Resourceify this
#define INVALID_PROPERTY_NAME  L"Invalid qualified property name"
#define PROPERTY_VALUE_NOT_SET  L"Property value has not been set on the feature."
#define PROPERTY_WRONG_PROPERTY_TYPE  L"Wrong property type."

EGwsStatus IGWSMutableFeature::Create(
    IGWSFeature         * pFrom,
    IGWSMutableFeature ** pNewFeature
)
{
    int i;
    if (pNewFeature == NULL)
        GWS_THROW (eGwsNullPointer);

    FdoPtr<IGWSExtendedFeatureDescription> iResdscs;
    pFrom->DescribeFeature (& iResdscs);
    int nFailed = 0;
    int nProps = 0;

    try {

        FdoPtr<CGwsMutableFeature>  mfeature;

        IGWSMutableFeature::Create (iResdscs, NULL, (IGWSMutableFeature **)& mfeature.p);

        mfeature->SetFeatureId (pFrom->GetFeatureId ());
        mfeature->SetRevisionNumber (pFrom->GetRevisionNumber ());
        mfeature->SetCacheId (pFrom->GetCacheId ());
        mfeature->SetLayerSource (pFrom->GetLayerSource());
        mfeature->SetCacheLockType (pFrom->GetCacheLockType ());
        long stat = 0;
        if (pFrom->IsNew ())
            stat |= eGwsNewFeature;
        if (pFrom->IsModified ())
            stat |= eGwsModifiedFeature;
        if (pFrom->IsDeleted ())
            stat |= eGwsDeletedFeature;
        mfeature->SetCacheStatus (stat);

        CGwsQueryResultDescriptors * resdscs = dynamic_cast<CGwsQueryResultDescriptors *> (iResdscs.p);
        const std::vector<CGwsPropertyDesc> & props =
                                                resdscs->GetPropertyDescriptors ();
        nProps = (int)props.size ();

        for (i = 0; i < nProps; i ++) {
            try {
                FdoString * pname = props[i].m_name.c_str ();
                if (props[i].m_ptype == FdoPropertyType_DataProperty) {
                    if (pFrom->IsNull (pname)) {  // continue for now
                        mfeature->SetNull (props[i].m_name.c_str ());
                        continue;
                    }

                    switch (props[i].m_dataprop) {
                    case FdoDataType_Boolean:
                        mfeature->SetBoolean (pname, pFrom->GetBoolean (pname));
                        break;

                    case FdoDataType_Byte:
                        mfeature->SetByte (pname, pFrom->GetByte (pname));
                        break;

                    case FdoDataType_Decimal:
                    case FdoDataType_Double:
                        mfeature->SetDouble (pname, pFrom->GetDouble (pname));
                        break;

                    case FdoDataType_Int16:
                        mfeature->SetInt16 (pname, pFrom->GetInt16 (pname));
                        break;

                    case FdoDataType_Int32:
                        mfeature->SetInt32 (pname, pFrom->GetInt32 (pname));
                        break;

                    case FdoDataType_Int64:
                        mfeature->SetInt64 (pname, pFrom->GetInt64 (pname));
                        break;

                    case FdoDataType_Single:
                        mfeature->SetSingle (pname, pFrom->GetSingle (pname));
                        break;

                    case FdoDataType_String:
                        mfeature->SetString (pname, pFrom->GetString (pname));
                        break;

                    case FdoDataType_BLOB:
                    case FdoDataType_CLOB:
                        {
                            FdoPtr<FdoLOBValue> lob = pFrom->GetLOB (pname);
                            mfeature->SetValue (pname, lob);
                        }
                        break;

                    case FdoDataType_DateTime:
                        mfeature->SetDateTime (pname, pFrom->GetDateTime (pname));
                        break;

                    default:
                        break;
                    }

                } else if (props[i].m_ptype == FdoPropertyType_GeometricProperty) {
                    FdoByteArray * geom = pFrom->GetGeometry (pname);
                    if (geom != NULL) {
                        mfeature->SetGeometry (pname, geom);
                        geom->Release ();
                    }

                } else {
                    continue;
                }
            } catch (FdoException * ex) {
                ex->Release ();
                nFailed  ++;
            }
        }
        mfeature->SetCSName (pFrom->GetCSName ());
        * pNewFeature = mfeature;
        (* pNewFeature)->AddRef ();

    } catch (FdoException * gis) {
        GWS_RETHROW (gis, eGwsFailedToCreateMutableFeature);
    }
    if (nFailed == 0)
        return eGwsOk;
    if (nFailed == nProps) // failed to copy some features. return warinig status
        GWS_THROW (eGwsFailedToCreateMutableFeature);

    return eGwsWarningSomePropertyValuesAreUninitialized;
}

EGwsStatus IGWSMutableFeature::Create(
    IGWSExtendedFeatureDescription * pefdsc,
    IGWSObject                     * owner,
    IGWSMutableFeature            ** pNewFeature
)
{
    if (pNewFeature == NULL)
        GWS_THROW (eGwsNullPointer);

    CGwsMutableFeature * feature  =
         (CGwsMutableFeature *)
            CGwsMutableFeature::CreateInstance<CGwsMutableFeature>(owner);
    feature->Initialize (pefdsc);
    * pNewFeature = feature;
    return eGwsOk;
}

EGwsStatus IGWSMutableFeature::Create(IGWSExtendedFeatureDescription * pefdsc,
                                      unsigned char      * pBuf,
                                      int                  len,
                                      IGWSMutableFeature** pNewFeature)
{
    if (pNewFeature == NULL)
        GWS_THROW (eGwsNullPointer);

    CGwsBinaryFeature * feature  = new CGwsBinaryFeature(pefdsc);
    feature->AddRef();
    feature->Set (pBuf, len);
    * pNewFeature = feature;
    return eGwsOk;
}

CGwsMutableFeature::CGwsMutableFeature()
{
    m_cacheId = 0;
    m_revisionnumber = INVALID_SEQUENCE_VALUE;
    m_cacheLockType = eGwsLockNolock;
    m_cacheStatus = 0;
}


void CGwsMutableFeature::Initialize (IGWSExtendedFeatureDescription * pFeatDesc)

{
    assert (pFeatDesc != NULL);
    m_pFeatDesc = pFeatDesc;
    if (pFeatDesc != NULL)
        pFeatDesc->AddRef ();
    m_pProperties = FdoPropertyValueCollection::Create();
    m_cacheId = 0;
    m_revisionnumber = INVALID_SEQUENCE_VALUE;
    m_cacheLockType = eGwsLockNolock;
    m_cacheStatus = 0;
    m_csname = pFeatDesc->GetCoordinateSystem ();
}

void CGwsMutableFeature::DescribeFeature(IGWSExtendedFeatureDescription ** ppResDesc)
{
    *ppResDesc = m_pFeatDesc;
    (*ppResDesc)->AddRef();
}

void CGwsMutableFeature::InitializePropertyValues ()
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    if (m_pProperties == NULL) {
        m_pProperties = FdoPropertyValueCollection::Create ();
    } else {
        m_pProperties->Clear ();
    }

    FdoPtr<FdoStringCollection> names =  featDsc->PropertyNames ();


    CGwsQueryResultDescriptors           *  fdesc
                    = (CGwsQueryResultDescriptors *) featDsc.p;

    for (int i = 0; i < names->GetCount (); i ++) {
        const CGwsPropertyDesc & desc = fdesc->GetPropertyDescriptor (i);
        FdoPtr<FdoPropertyValue> propval = ConstructPropertyValue (desc);
        m_pProperties->Add (propval);
    }
}


FdoPropertyValue * CGwsMutableFeature::ConstructPropertyValue (
    const CGwsPropertyDesc & desc
)
{
    FdoPtr<FdoValueExpression> valexpr = ConstructValueExpression (desc);

    if (valexpr != NULL)
        return FdoPropertyValue::Create (desc.m_name.c_str (), valexpr);
    // TODO: unsupported property value to be implemented
    assert (false);
    return NULL;
}

FdoValueExpression * CGwsMutableFeature::ConstructValueExpression (
    const CGwsPropertyDesc & desc
)
{
    FdoValueExpression * valexpr = NULL;

    if (desc.m_ptype == FdoPropertyType_DataProperty)
        valexpr = FdoDataValue::Create (desc.m_dataprop);

    else if (desc.m_ptype == FdoPropertyType_GeometricProperty) {
        valexpr = FdoGeometryValue::Create();

    } else if (desc.m_ptype == FdoPropertyType_AssociationProperty ||
               desc.m_ptype == FdoPropertyType_ObjectProperty ||
               desc.m_ptype == FdoPropertyType_RasterProperty)
    {
        // just something
        valexpr = FdoInt64Value::Create ();

    } else {
        // Property type yet unknown
        assert(false);
    }
    return valexpr;
}


void CGwsMutableFeature::SetPropertyValues (IGWSFeature * feature)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    CGwsQueryResultDescriptors * resdscs = dynamic_cast<CGwsQueryResultDescriptors *> (featDsc.p);
    const std::vector<CGwsPropertyDesc> & props =
                                           resdscs->GetPropertyDescriptors ();

    for (int i = 0; i < m_pProperties->GetCount (); i ++) {
        FdoPtr<FdoPropertyValue> propval = m_pProperties->GetItem (i);
        if (propval == NULL)
            continue;
        FdoPtr<FdoValueExpression> valexpr = propval->GetValue ();
        FdoPtr<FdoIdentifier>      ident   = propval->GetName ();
        FdoString               *  pName   = props[i].m_name.c_str ();

        if (props[i].m_ptype == FdoPropertyType_DataProperty) {
            FdoDataValue * dval = dynamic_cast<FdoDataValue *> (valexpr.p);
            if (feature->IsNull (pName)) {
                dval->SetNull ();
                continue;
            }

            switch (props[i].m_dataprop) {
            case FdoDataType_Boolean:
                {
                FdoBooleanValue* pBoolVal = (FdoBooleanValue *) dval;
                pBoolVal->SetBoolean(feature->GetBoolean(pName));
                }
                break;

            case FdoDataType_Byte:
                {
                FdoByteValue* pByteVal = (FdoByteValue*)dval;
                pByteVal->SetByte(feature->GetByte (pName));
                }
                break;

            case FdoDataType_Decimal:
                {
                FdoByteValue* pByteVal = (FdoByteValue*)dval;
                pByteVal->SetByte(feature->GetByte (pName));
                }
                break;
            case FdoDataType_Double:
                {
                FdoDoubleValue* pVal = (FdoDoubleValue*)dval;
                pVal->SetDouble(feature->GetDouble(pName));
                }
                break;

            case FdoDataType_Int16:
                {
                FdoInt16Value* pVal = (FdoInt16Value*)dval;
                pVal->SetInt16(feature->GetInt16(pName));
                }
                break;

            case FdoDataType_Int32:
                {
                FdoInt32Value* pVal = (FdoInt32Value*)dval;
                pVal->SetInt32(feature->GetInt32(pName));
                }
                break;

            case FdoDataType_Int64:
                {
                FdoInt64Value* pVal = (FdoInt64Value*)dval;
                pVal->SetInt64(feature->GetInt64(pName));
                }
                break;

            case FdoDataType_Single:
                {
                FdoSingleValue* pVal = (FdoSingleValue*)dval;
                pVal->SetSingle(feature->GetSingle(pName));
                }
                break;

            case FdoDataType_String:
                {
                FdoStringValue* pVal = (FdoStringValue*)dval;
                pVal->SetString(feature->GetString(pName));
                }
                break;

            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
                {
                    FdoLOBValue * pVal = (FdoLOBValue *) dval;
                    FdoPtr<FdoLOBValue> lob = feature->GetLOB (pName);
                    FdoPtr<FdoByteArray> barray = lob->GetData ();
                    pVal->SetData (barray);
                }
                break;

            case FdoDataType_DateTime:
                {
                FdoDateTimeValue* pVal = (FdoDateTimeValue*)dval;
                pVal->SetDateTime (feature->GetDateTime (pName));
                }
                break;

            default:
                break;
            }

        } else if (props[i].m_ptype == FdoPropertyType_GeometricProperty) {
            FdoByteArray * geom = feature->GetGeometry (pName);
            FdoGeometryValue * geomval = dynamic_cast<FdoGeometryValue *> (valexpr.p);

            if (geom != NULL) {
                geomval->SetGeometry (geom);
                geom->Release ();
            } else
                geomval->SetNullValue ();

        } else {
            continue;
        }
    }
}


void CGwsMutableFeature::ValidatePropertyName(
    FdoString                * propName,
    const CGwsPropertyDesc  ** propdesc
)
{
    const CGwsPropertyDesc & pdesc =
         ((CGwsQueryResultDescriptors *) m_pFeatDesc.p)->GetPropertyDescriptor (propName);
    if (pdesc.m_name.empty())
        throw FdoException::Create(INVALID_PROPERTY_NAME);
    if (propdesc != NULL)
        * propdesc = & pdesc;
}

void CGwsMutableFeature::ValidatePropertyName(
    FdoString                * propName,
    FdoPropertyType            ptype,
    FdoDataType                datatype,
    const CGwsPropertyDesc  ** propdesc
)
{
    const CGwsPropertyDesc & pdesc =
         ((CGwsQueryResultDescriptors *) m_pFeatDesc.p)->GetPropertyDescriptor (propName);
    if (pdesc.m_name.empty())
        throw FdoException::Create(INVALID_PROPERTY_NAME);

    if (pdesc.m_ptype != ptype ||
        pdesc.m_dataprop != datatype)
        throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);

    if (propdesc != NULL)
        * propdesc = & pdesc;
}

void CGwsMutableFeature::ValidatePropertyName(
    FdoString                * propName,
    FdoPropertyType            ptype,
    const CGwsPropertyDesc  ** propdesc
)
{
    const CGwsPropertyDesc & pdesc =
         ((CGwsQueryResultDescriptors *) m_pFeatDesc.p)->GetPropertyDescriptor (propName);
    if (pdesc.m_name.empty())
        throw FdoException::Create(INVALID_PROPERTY_NAME);

    if (pdesc.m_ptype != ptype)
        throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);

    if (propdesc != NULL)
        * propdesc = & pdesc;
}

FdoInt32 CGwsMutableFeature::GetCacheId ()
{
    return m_cacheId;
}

FdoInt32 CGwsMutableFeature::GetRevisionNumber ()
{
    return m_revisionnumber;
}

GWSFeatureId CGwsMutableFeature::GetFeatureId ()
{
    return m_featureId;
}

void CGwsMutableFeature::SetRevisionNumber (FdoInt32 revnum)
{
    m_revisionnumber = revnum;
}
void CGwsMutableFeature::SetFeatureId (const GWSFeatureId & fid)
{
    m_featureId = fid;
}

void CGwsMutableFeature::SetCacheId (FdoInt32 cacheId)
{
    m_cacheId = cacheId;
}

void CGwsMutableFeature::SetCacheLockType (EGwsLockType locktype)
{
    m_cacheLockType = locktype;
}

void CGwsMutableFeature::SetCacheStatus (long status)
{
    m_cacheStatus = status;
}


bool CGwsMutableFeature::IsNew ()
{
  return (m_cacheStatus == eGwsNewFeature) != 0;
}

bool CGwsMutableFeature::IsModified ()
{
    return (m_cacheStatus == eGwsModifiedFeature) != 0;
}
bool CGwsMutableFeature::IsDeleted ()
{
    return (m_cacheStatus == eGwsDeletedFeature) != 0;
}
EGwsLockType CGwsMutableFeature::GetCacheLockType ()
{
    return m_cacheLockType;
}


bool CGwsMutableFeature::IsPropertyValueSet (FdoString* propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem (propertyName);
    if (pValue == NULL)
        return false;
    return true;
}


bool CGwsMutableFeature::IsNull(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);

    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty) {
        return ((FdoDataValue *) pVal.p)->IsNull ();
    } else if (desc.m_ptype == FdoPropertyType_GeometricProperty) {
        return ((FdoGeometryValue *) pVal.p)->IsNull ();
    } else {
        return pVal != NULL;
    }
}

FdoString* CGwsMutableFeature::GetString(FdoString * propertyName)
{

    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);

    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_String)
    {
        return ((FdoStringValue *) pVal.p)->GetString ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

bool  CGwsMutableFeature::GetBoolean(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Boolean)
    {
        return ((FdoBooleanValue *) pVal.p)->GetBoolean ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);

}

FdoByte CGwsMutableFeature::GetByte(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Byte)
    {
        return ((FdoByteValue *) pVal.p)->GetByte ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoDateTime CGwsMutableFeature::GetDateTime(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_DateTime)
    {
        return ((FdoDateTimeValue *) pVal.p)->GetDateTime ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

double CGwsMutableFeature::GetDouble(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Double)
    {
        return ((FdoDoubleValue *) pVal.p)->GetDouble ();
    } else if (desc.m_ptype == FdoPropertyType_DataProperty &&
               desc.m_dataprop == FdoDataType_Decimal)
    {
        return ((FdoDecimalValue *) pVal.p)->GetDecimal ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoInt16 CGwsMutableFeature::GetInt16(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Int16)
    {
        return ((FdoInt16Value *) pVal.p)->GetInt16 ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoInt32 CGwsMutableFeature::GetInt32(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Int32)
    {
        return ((FdoInt32Value *) pVal.p)->GetInt32 ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoInt64 CGwsMutableFeature::GetInt64(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Int64)
    {
        return ((FdoInt64Value *) pVal.p)->GetInt64 ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

float CGwsMutableFeature::GetSingle(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_DataProperty &&
        desc.m_dataprop == FdoDataType_Single)
    {
        return ((FdoSingleValue *) pVal.p)->GetSingle ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoLOBValue* CGwsMutableFeature::GetLOB(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    FdoLOBValue * lob = dynamic_cast<FdoLOBValue*>(pVal.p);
    if (lob != NULL)
        lob->AddRef ();
    return lob;
}

const FdoByte * CGwsMutableFeature::GetGeometry(
    FdoString * propertyName,
    FdoInt32  * count
)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_GeometricProperty) {
        FdoGeometryValue * pGeom = (FdoGeometryValue *) pVal.p;
        FdoPtr<FdoByteArray> pVal = pGeom->GetGeometry ();
        *count = pVal->GetCount();
        return pVal->GetData();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}

FdoByteArray* CGwsMutableFeature::GetGeometry(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);

    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    FdoPtr<FdoValueExpression> pVal = pValue->GetValue();
    if (desc.m_ptype == FdoPropertyType_GeometricProperty) {
        FdoGeometryValue * pGeom = (FdoGeometryValue *) pVal.p;
        return pGeom->GetGeometry ();
    }
    throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
}


//TODO: How to get these if we don't support setting them
FdoIStreamReader* CGwsMutableFeature::GetLOBStreamReader(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);

    return NULL;
}

FdoIRaster* CGwsMutableFeature::GetRaster(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    GWS_THROW (eGwsCannotGetPropertyValueOffline);
}

FdoIFeatureReader* CGwsMutableFeature::GetFeatureObject(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    GWS_THROW (eGwsCannotGetPropertyValueOffline);
}

FdoDataValue * CGwsMutableFeature::GetDataValue (FdoString* propertyName)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    if (desc.m_name.empty ())
        throw FdoException::Create(INVALID_PROPERTY_NAME);
    if (desc.m_ptype != FdoPropertyType_DataProperty)
        throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    if(pValue == NULL)
        throw FdoException::Create(PROPERTY_VALUE_NOT_SET);
    return (FdoDataValue *) pValue->GetValue();
}

FdoDataValueCollection * CGwsMutableFeature::GetDataValues (
    FdoStringCollection* propertyNames
)
{
    CGwsDataValueCollection * datavals = NULL;
    for (int i = 0; i < propertyNames->GetCount (); i ++) {
        FdoPtr<FdoDataValue> val = GetDataValue (propertyNames->GetString (i));
        assert (val != NULL);
        if (datavals == NULL)
            datavals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        datavals->Add (val);
    }
    return datavals;
}

/*
/// <summary>
/// CGwsMutableFeature::Getters by property index
/// </summary>
bool CGwsMutableFeature::IsNull(FdoInt32 iProp)
{
    return false;
}

FdoString* CGwsMutableFeature::GetString(FdoInt32 iProp)
{
    return NULL;
}

bool CGwsMutableFeature::GetBoolean(FdoInt32 iProp)
{
    return false;
}

FdoByte CGwsMutableFeature::GetByte(FdoInt32 iProp)
{
    return FdoByte();
}

FdoDateTime CGwsMutableFeature::GetDateTime(FdoInt32 iProp)
{
    return FdoDateTime();
}

double CGwsMutableFeature::GetDouble(FdoInt32 iProp)
{
    return 0.0;
}

FdoInt16 CGwsMutableFeature::GetInt16(FdoInt32 iProp)
{
    return 0;
}

FdoInt32 CGwsMutableFeature::GetInt32(FdoInt32 iProp)
{
    return 0;
}

FdoInt64 CGwsMutableFeature::GetInt64(FdoInt32 iProp)
{
    return 0;
}

float CGwsMutableFeature::GetSingle(FdoInt32 iProp)
{
    return 0.0;
}

FdoLOBValue* CGwsMutableFeature::GetLOB(FdoInt32 iProp)
{
    return NULL;
}

FdoIStreamReader* CGwsMutableFeature::GetLOBStreamReader(FdoInt32 iProp)
{
    return NULL;
}

FdoIRaster* CGwsMutableFeature::GetRaster(FdoInt32 iProp)
{
    return NULL;
}

const FdoByte * CGwsMutableFeature::GetGeometry(FdoInt32 iProp, FdoInt32 * count)
{
    return NULL;
}

FdoByteArray* CGwsMutableFeature::GetGeometry(FdoInt32 iProp)
{
    return NULL;
}

FdoIFeatureReader* CGwsMutableFeature::GetFeatureObject (FdoInt32 iProp)
{
    return NULL;
}


void CGwsMutableFeature::ToString(FdoInt32 iProp, wchar_t * buff, int len)
{
    try {
        const CGwsPropertyDesc & desc = GetPropertyDescriptor (iProp);
        GwsQueryUtils::ToString (this, desc, buff, len);
    } catch (FdoException * gis) {
        wcsncpy (buff, L"*ERROR*", len);
        gis->Release ();
    }
}
*/
void CGwsMutableFeature::ToString(
    FdoString*  propertyName,
    wchar_t *   buff,
    int         len
)
{
    try {
        const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
        GwsQueryUtils::ToString (this, desc, buff, len);
    } catch (FdoException * gis) {
        wcsncpy (buff, L"*ERROR*", len);
        gis->Release ();
    }
}


void CGwsMutableFeature::SetValue(FdoString* propertyName, FdoValueExpression* pVal)
{
    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);

    if (pPropertyValue != NULL) {
        pPropertyValue->SetValue(pVal);
    } else {
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);
    }
}

void CGwsMutableFeature::SetNull(FdoString* propertyName)
{
    const CGwsPropertyDesc * desc = NULL;

    ValidatePropertyName(propertyName, & desc);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    if (pPropertyValue == NULL) { // not yet set
        pPropertyValue = ConstructPropertyValue (* desc);
        m_pProperties->Add(pPropertyValue);
    } else {
        FdoPtr<FdoValueExpression> pVal = pPropertyValue->GetValue ();
        if (desc->m_ptype == FdoPropertyType_DataProperty) {
            ((FdoDataValue *) pVal.p)->SetNull ();
        } else if (desc->m_ptype == FdoPropertyType_GeometricProperty) {
            ((FdoGeometryValue *) pVal.p)->SetNullValue ();
        }
    }
}

void CGwsMutableFeature::SetString(FdoString* propertyName, FdoString* value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_String,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_String)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoStringValue *) pVal.p)->SetString (value);
    }
}

void CGwsMutableFeature::SetBoolean(FdoString* propertyName, bool value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Boolean,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Boolean)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoBooleanValue *) pVal.p)->SetBoolean (value);
    }
}

void CGwsMutableFeature::SetByte(FdoString* propertyName, FdoByte value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Byte,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Byte)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoByteValue *) pVal.p)->SetByte (value);
    }
}

void CGwsMutableFeature::SetDateTime(FdoString* propertyName, FdoDateTime value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_DateTime,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_DateTime)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoDateTimeValue *) pVal.p)->SetDateTime (value);
    }
}

void CGwsMutableFeature::SetDouble(FdoString* propertyName, double value)
{
    const CGwsPropertyDesc * pdesc = NULL;
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         & pdesc);

    if (pdesc->m_dataprop != FdoDataType_Double &&
        pdesc->m_dataprop != FdoDataType_Decimal)
        throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create (value, pdesc->m_dataprop);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () == FdoDataType_Double) {
            ((FdoDoubleValue *) pVal.p)->SetDouble (value);

        } else if (pVal->GetDataType () == FdoDataType_Decimal) {
            ((FdoDecimalValue *) pVal.p)->SetDecimal (value);

        } else
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
    }
}

void CGwsMutableFeature::SetInt16(FdoString* propertyName, FdoInt16 value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Int16,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Int16)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoInt16Value *) pVal.p)->SetInt16 (value);
    }
}

void CGwsMutableFeature::SetInt32(FdoString* propertyName, FdoInt32 value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Int32,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Int32)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoInt32Value *) pVal.p)->SetInt32 (value);
    }
}

void CGwsMutableFeature::SetInt64(FdoString* propertyName, FdoInt64 value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Int64,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Int64)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoInt64Value *) pVal.p)->SetInt64 (value);
    }
}

void CGwsMutableFeature::SetSingle(FdoString* propertyName, float value)
{
    ValidatePropertyName(propertyName,
                         FdoPropertyType_DataProperty,
                         FdoDataType_Single,
                         NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);
    FdoPtr<FdoDataValue> pVal;

    if (pPropertyValue == NULL) {
        pVal = FdoDataValue::Create(value);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pVal);
        m_pProperties->Add(pPropertyValue);

    } else {
        pVal = (FdoDataValue *) pPropertyValue->GetValue ();
        if (pVal->GetDataType () != FdoDataType_Single)
            throw FdoException::Create(PROPERTY_WRONG_PROPERTY_TYPE);
        ((FdoSingleValue *) pVal.p)->SetSingle (value);
    }
}

void CGwsMutableFeature::SetLOB(FdoString* propertyName, FdoLOBValue* pValue)
{
    ValidatePropertyName(propertyName, NULL);
    SetValue(propertyName, pValue);
}

void CGwsMutableFeature::SetGeometry(FdoString * propertyName, FdoByteArray * geom)
{
    ValidatePropertyName(propertyName, FdoPropertyType_GeometricProperty, NULL);

    FdoPtr<FdoPropertyValue> pPropertyValue =
                                m_pProperties->FindItem (propertyName);

    FdoPtr<FdoGeometryValue> pGeometryValue;

    if (pPropertyValue == NULL) {
        FdoPtr<FdoGeometryValue>   pGeometryValue = FdoGeometryValue::Create(geom);
        pPropertyValue = FdoPropertyValue::Create(propertyName, pGeometryValue);
        m_pProperties->Add(pPropertyValue);

    } else {
        pGeometryValue = (FdoGeometryValue *) pPropertyValue->GetValue ();
        pGeometryValue->SetGeometry (geom);
    }
}

int CGwsMutableFeature::GetCount ()
{
    if (m_pProperties)
        return m_pProperties->GetCount ();

    return 0;
}

FdoPropertyValue * CGwsMutableFeature::GetPropertyValue (int idx)
{
    if (0 <= idx && idx < GetCount ()) {
        return m_pProperties->GetItem (idx);
    }
    return 0;
}

IGWSMutableFeature * CGwsMutableFeature::GetSimpleFeature ()
{
    AddRef ();
    return this;
}


const GWSQualifiedName & CGwsMutableFeature::ClassName () const
{
    return m_pFeatDesc->ClassName ();
}

const CGwsPropertyDesc & CGwsMutableFeature::GetPropertyDescriptor (
    int iProp
)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (iProp);
}

const CGwsPropertyDesc & CGwsMutableFeature::GetPropertyDescriptor (
    FdoString * propname
)
{
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    return ((CGwsQueryResultDescriptors *) featDsc.p)->GetPropertyDescriptor (propname);
}

FdoGeometryType CGwsMutableFeature::GetGeometryType(FdoByteArray* pArray)
{
    FdoPtr<FdoFgfGeometryFactory> pAgfFactory = FdoFgfGeometryFactory::GetInstance();
    FdoPtr<FdoIGeometry> pAgfGeometry = pAgfFactory->CreateGeometryFromFgf(pArray);
    return pAgfGeometry->GetDerivedType();
}

FdoString* CGwsMutableFeature::GetPrimaryGeometryName()
{
    static std::wstring name;
    assert(m_pFeatDesc != NULL);
    FdoPtr<FdoClassDefinition> pClassDef(m_pFeatDesc->ClassDefinition());
    GwsCommonFdoUtils::GetGeometryName(pClassDef, name);
    return name.c_str();
}

void CGwsMutableFeature::SetCSName (
    const GWSCoordinateSystem & csname
)
{
    m_csname = csname;
}

const GWSCoordinateSystem & CGwsMutableFeature::GetCSName ()
{
    return m_csname;
}

unsigned char* CGwsMutableFeature::ToBuffer(int& bufLen)
{
    GwsBinaryFeatureWriter wrtr;
    FdoPtr<FdoClassDefinition> pClassDef(m_pFeatDesc->ClassDefinition());
    wchar_t buf[256];
    int len = m_pFeatDesc->ClassName().ToFullyQualifedString(buf, 256);
    assert(len < 256);
    len;

    wrtr.WriteFeature(pClassDef, buf, m_pProperties);
    return wrtr.ToBuffer(bufLen);
}

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
#include "GwsBinaryFeatureWriter.h"
#include "GwsFlatFdoReader.h"

//TODO: Resourceify this
#define INVALID_PROPERTY_NAME  L"Invalid qualified property name"
#define PROPERTY_VALUE_NOT_SET  L"Property value has not been set on the feature."
#define PROPERTY_WRONG_PROPERTY_TYPE  L"Wrong property type."


EGwsStatus IGWSMutableFeature::CreatePrimary(
    IGWSFeature         * pFrom,
    IGWSMutableFeature ** pNewFeature) {

    CGwsFlatGwsIterator* flatIter = dynamic_cast<CGwsFlatGwsIterator*>(pFrom);
    if(NULL == flatIter)
        return IGWSMutableFeature::Create(pFrom,pNewFeature);
    else
        return IGWSMutableFeature::Create(flatIter->GetPrimaryIterator(),pNewFeature);
}


EGwsStatus IGWSMutableFeature::Create(
    IGWSFeature         * pFrom,
    IGWSMutableFeature ** pNewFeature
)
{
    return IGWSMutableFeature::Create(pFrom, NULL, pNewFeature);
}


EGwsStatus IGWSMutableFeature::Create(
    IGWSFeature         * pFrom,
    IGWSObject         * owner,
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

        IGWSMutableFeature::Create (iResdscs, owner, (IGWSMutableFeature **)& mfeature.p);

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
                if (pFrom->IsNull (pname)) {  // continue for now
                    mfeature->SetNull (props[i].m_name.c_str ());
                    continue;
                }
                if (props[i].m_ptype == FdoPropertyType_DataProperty) {

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
                        FdoPtr<FdoByteArray> pNewGeom = FdoByteArray::Create(geom->GetData(), geom->GetCount());
                        mfeature->SetGeometry (pname, pNewGeom);
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

    } catch (FdoException * fdoEx) {
        GWS_RETHROW (fdoEx, eGwsFailedToCreateMutableFeature);
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


void CGwsMutableFeature::SetSourceCSName (const GWSCoordinateSystem & csname)
{
    m_srccsname = csname;
}


const GWSCoordinateSystem & CGwsMutableFeature::GetSourceCSName ()
{
    return m_srccsname;
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
    //  unsupported property value to be implemented
    assert (false);
    return NULL;
}


FdoValueExpression * CGwsMutableFeature::ConstructValueExpression (
    const CGwsPropertyDesc & desc
)
{
    FdoValueExpression * valexpr = NULL;

    if (desc.m_ptype == FdoPropertyType_DataProperty)
    {
        valexpr = FdoDataValue::Create (desc.m_dataprop);

        if ( desc.m_nullable && desc.m_dataprop == FdoDataType_String )
        {
            // sometimes a string type field may be nullable, but does not accept a zero length string.
            // e.g. the cities table in Defect 770036 attachment
            ((FdoDataValue *)valexpr)->SetNull();
        }
        else if ( !desc.m_nullable )
        {
            // this is a data property that cannot be null.  Give it some value.
            switch (desc.m_dataprop) {
            case FdoDataType_String:
                ((FdoStringValue *)valexpr)->SetString( L"" );
                break;
            case FdoDataType_Boolean:
                ((FdoBooleanValue *)valexpr)->SetBoolean( false );
                break;
            case FdoDataType_Byte:
                ((FdoByteValue *)valexpr)->SetByte( 0 );
                break;
            case FdoDataType_DateTime:
                {
                    FdoDateTime fdoDt;
                    fdoDt.year = 0;
                    fdoDt.month = 0;
                    fdoDt.day = 0;
                    fdoDt.hour = 0;
                    fdoDt.minute = 0;
                    fdoDt.seconds = 0.0;
                    ((FdoDateTimeValue *)valexpr)->SetDateTime( fdoDt );
                }
                break;
            case FdoDataType_Double:
                ((FdoDoubleValue *)valexpr)->SetDouble( 0.0 );
                break;
            case FdoDataType_Decimal:
                ((FdoDecimalValue *)valexpr)->SetDecimal( 0.0 );
                break;
            case FdoDataType_Single:
                ((FdoSingleValue *)valexpr)->SetSingle( 0.0 );
                break;
            case FdoDataType_Int16:
                ((FdoInt16Value *)valexpr)->SetInt16( 0 );
                break;
            case FdoDataType_Int32:
                ((FdoInt32Value *)valexpr)->SetInt32( 0 );
                break;
            case FdoDataType_Int64:
                ((FdoInt64Value *)valexpr)->SetInt64( 0 );
                break;
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            default:
                // Leave null value.
                break;
            }
        }
    }

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
            FdoPtr<FdoDataValue> srcVal;
            try {
                if(feature)
                {
                    srcVal = feature->GetDataValue (pName);
                }
            } catch (FdoException * e) {
                // the value is not set.
                e->Release ();
            }
            SetValue (pName, srcVal);

        } else if (props[i].m_ptype == FdoPropertyType_GeometricProperty) {
            FdoByteArray * geom = NULL;
            FdoGeometryValue * geomval = NULL;

            if(feature)
            {
                if (! feature->IsNull (pName)) {
                    geom = feature->GetGeometry (pName);
                    geomval = dynamic_cast<FdoGeometryValue *> (valexpr.p);
                }
            }

            if (geom != NULL) {
                FdoPtr<FdoByteArray> pNewGeom = FdoByteArray::Create(geom->GetData(), geom->GetCount());
                geomval->SetGeometry (pNewGeom);
                geom->Release ();
            } else if(geomval)
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
    return NULL;
}


FdoIFeatureReader* CGwsMutableFeature::GetFeatureObject(FdoString * propertyName)
{
    FdoPtr<FdoPropertyValue> pValue = m_pProperties->FindItem(propertyName);
    GWS_THROW (eGwsCannotGetPropertyValueOffline);
    return NULL;
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


void CGwsMutableFeature::ToString(
    FdoString*  propertyName,
    wchar_t *   buff,
    int         len
)
{
    try {
        const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
        GwsQueryUtils::ToString (this, desc, buff, len);
    } catch (FdoException * fdoEx) {
        wcsncpy (buff, L"*ERROR*", len);
        fdoEx->Release ();
    }
}


void CGwsMutableFeature::SetValue(FdoString* propertyName, FdoValueExpression* pVal)
{
    FdoPtr<FdoPropertyValue>     pPropertyValue;

    pPropertyValue = m_pProperties->FindItem (propertyName);
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
        FdoPtr<FdoPropertyValue>  pPropertyValue = ConstructPropertyValue (* desc);
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
    FdoPtr<FdoFgfGeometryFactory> pFgfFactory = FdoFgfGeometryFactory::GetInstance();
    FdoPtr<FdoIGeometry> pFgfGeometry = pFgfFactory->CreateGeometryFromFgf(pArray);
    return pFgfGeometry->GetDerivedType();
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
    wrtr.WriteFeature(pClassDef, buf, m_pProperties);
    return wrtr.ToBuffer(bufLen);
}

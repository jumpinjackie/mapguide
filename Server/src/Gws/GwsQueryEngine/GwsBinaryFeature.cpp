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
#include "GwsBinaryFeatureReader.h"


/////////////////////////////////////////////////////////////////////
//
// class CGwsBinaryFeature
//
/////////////////////////////////////////////////////////////////////

CGwsBinaryFeature::CGwsBinaryFeature (IGWSExtendedFeatureDescription * iResdscs)
{
    Initialize (iResdscs);
    InitializePropertyValues ();
    m_pBinaryReader = NULL;
}


CGwsBinaryFeature::~CGwsBinaryFeature () throw()
{
    if(m_pBinaryReader)
        delete m_pBinaryReader;
}


//copies the binary feature data into the base class's FdoPropertyValueCollection
void CGwsBinaryFeature::Set (unsigned char* pBuf, int len)
{
    int numProps = m_pProperties->GetCount();
    m_pBinaryReader = new GwsBinaryFeatureReader(pBuf, len, numProps);

    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    CGwsQueryResultDescriptors * resdscs = dynamic_cast<CGwsQueryResultDescriptors *> (featDsc.p);
    const std::vector<CGwsPropertyDesc> & props =
                                           resdscs->GetPropertyDescriptors ();

    for (int i = 0; i < numProps; i ++) {
        FdoPtr<FdoPropertyValue> propval = m_pProperties->GetItem (i);
        if (propval == NULL)
            continue;
        FdoPtr<FdoValueExpression> valexpr = propval->GetValue ();
        FdoPtr<FdoIdentifier>      ident   = propval->GetName ();
        FdoString               *  pName   = props[i].m_name.c_str ();

        if (props[i].m_ptype == FdoPropertyType_DataProperty) {
            FdoDataValue * dval = dynamic_cast<FdoDataValue *> (valexpr.p);
            if (m_pBinaryReader->IsNull (i)) {
                dval->SetNull ();
                continue;
            }

            switch (props[i].m_dataprop) {
            case FdoDataType_Boolean:
                {
                FdoBooleanValue* pBoolVal = (FdoBooleanValue *) dval;
                pBoolVal->SetBoolean(m_pBinaryReader->GetBoolean(i));
                }
                break;

            case FdoDataType_Byte:
                {
                FdoByteValue* pByteVal = (FdoByteValue*)dval;
                pByteVal->SetByte(m_pBinaryReader->GetByte (i));
                }
                break;

            case FdoDataType_Decimal:
                {
                FdoDecimalValue* pDecimalVal = (FdoDecimalValue*)dval;
                pDecimalVal->SetDecimal(m_pBinaryReader->GetDouble (i));
                }
                break;
            case FdoDataType_Double:
                {
                FdoDoubleValue* pVal = (FdoDoubleValue*)dval;
                pVal->SetDouble(m_pBinaryReader->GetDouble(i));
                }
                break;

            case FdoDataType_Int16:
                {
                FdoInt16Value* pVal = (FdoInt16Value*)dval;
                pVal->SetInt16(m_pBinaryReader->GetInt16(i));
                }
                break;

            case FdoDataType_Int32:
                {
                FdoInt32Value* pVal = (FdoInt32Value*)dval;
                pVal->SetInt32(m_pBinaryReader->GetInt32(i));
                }
                break;

            case FdoDataType_Int64:
                {
                FdoInt64Value* pVal = (FdoInt64Value*)dval;
                pVal->SetInt64(m_pBinaryReader->GetInt64(i));
                }
                break;

            case FdoDataType_Single:
                {
                FdoSingleValue* pVal = (FdoSingleValue*)dval;
                pVal->SetSingle(m_pBinaryReader->GetSingle(i));
                }
                break;

            case FdoDataType_String:
                {
                FdoStringValue* pVal = (FdoStringValue*)dval;
                pVal->SetString(m_pBinaryReader->GetString(i));
                }
                break;

            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
                GWS_THROW(eGwsNotSupported);
                break;

            case FdoDataType_DateTime:
                {
                FdoDateTimeValue* pVal = (FdoDateTimeValue*)dval;
                pVal->SetDateTime (m_pBinaryReader->GetDateTime (i));
                }
                break;

            default:
                break;
            }

        } else if (props[i].m_ptype == FdoPropertyType_GeometricProperty) {
            FdoByteArray * geom = NULL;
            FdoGeometryValue * geomval = NULL;

            if (! m_pBinaryReader->IsNull (i)) {
                geom = m_pBinaryReader->GetGeometry (i);
                geomval = dynamic_cast<FdoGeometryValue *> (valexpr.p);
            }

            if (geom != NULL) {
                geomval->SetGeometry (geom);
                geom->Release ();
            } else
                geomval->SetNullValue ();

        } else {
            assert(false); //unsupported property type
            continue;
        }

    }
    //set up the feature id
    FdoPtr<CGwsDataValueCollection> keyvals;
    FdoPtr<FdoDataPropertyDefinitionCollection> iddefs =
                                resdscs->GetIdentityProperties ();

    for (int i = 0; iddefs != NULL && i < iddefs->GetCount (); i ++) {
        FdoPtr<FdoDataPropertyDefinition> idprop = iddefs->GetItem (i);
        FdoPtr<FdoPropertyValue> pVal = m_pProperties->FindItem(idprop->GetName ());
        FdoPtr<FdoValueExpression> valexpr = pVal->GetValue();
        FdoDataValue* val = dynamic_cast<FdoDataValue *> (valexpr.p);
        if (val == NULL)
             continue;
        if (keyvals == NULL)
            keyvals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        keyvals->Add (val);
    }
    m_featureId = keyvals;
}

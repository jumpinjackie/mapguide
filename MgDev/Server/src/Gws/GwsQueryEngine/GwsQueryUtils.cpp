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


/////////////////////////////////////////////////////////////////////
//
// namespace GwsQueryFdoUtils
//
/////////////////////////////////////////////////////////////////////
FdoDataValue * GwsQueryUtils::GetDataPropertyValue (
    FdoIReader  *   reader,
    FdoDataType     dataprop,
    FdoString   *   name
)
{
    if (reader->IsNull (name))
        return FdoDataValue::Create (dataprop);

    switch (dataprop) {
    case FdoDataType_Boolean:
        return FdoDataValue::Create (reader->GetBoolean (name));

    case FdoDataType_Byte:
        return FdoDataValue::Create (reader->GetByte (name));

    case FdoDataType_Decimal:
    case FdoDataType_Double:
        return FdoDataValue::Create (reader->GetDouble (name),
                                     dataprop);

    case FdoDataType_Int16:
        return FdoDataValue::Create (reader->GetInt16 (name));

    case FdoDataType_Int32:
        return FdoDataValue::Create (reader->GetInt32 (name));

    case FdoDataType_Int64:
        return FdoDataValue::Create (reader->GetInt64 (name));

    case FdoDataType_Single:
        return FdoDataValue::Create (reader->GetSingle (name));

    case FdoDataType_String:
        return FdoDataValue::Create (reader->GetString (name));

    case FdoDataType_BLOB:
    case FdoDataType_CLOB:
        {
            FdoPtr<FdoLOBValue> bval = (FdoBLOBValue *) reader->GetLOB (name);
            FdoPtr<FdoByteArray> bdata = bval->GetData ();
            return FdoDataValue::Create (bdata, dataprop);
        }
    case FdoDataType_DateTime:
        return FdoDataValue::Create (reader->GetDateTime (name));

    default:
        // Unknown data type
        assert(false);
    }
    return NULL;
}

void GwsQueryUtils::ToString (
    IGWSFeature            * feature,
    const CGwsPropertyDesc & desc,
    wchar_t                * buff,
    int                      len
)
{
    if (desc.m_name.empty ()) {
        wcsncpy (buff,  L"*ERROR*", len);
        return;
    }

    if (feature->IsNull (desc.m_name.c_str ())) {
        wcsncpy (buff,  L"NULL", len);
        return;
    }

    wchar_t tbuff [256];

    switch (desc.m_ptype) {
    case FdoPropertyType_ObjectProperty:
        wcsncpy (buff,  L"ObjectProperty", len);
        return;

    case FdoPropertyType_GeometricProperty:
        {
            FdoPtr<FdoByteArray> barr = feature->GetGeometry(desc.m_name.c_str ());
            wcsncpy (buff,  L"GeometricProperty", len);
        }
        return;

    case FdoPropertyType_AssociationProperty:
        wcsncpy (buff,  L"AssociationProperty", len);
        return;

    case FdoPropertyType_RasterProperty:
        wcsncpy (buff,  L"RasterProperty", len);
        return;

    case FdoPropertyType_DataProperty:
        switch (desc.m_dataprop) {
        case FdoDataType_Boolean: {
                bool val = feature->GetBoolean (desc.m_name.c_str ());
                wcsncpy (buff,  val ? L"TRUE" : L"FALSE", len);
            }
            break;

        case FdoDataType_Byte: {
                FdoByte val = feature->GetByte (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%d", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Decimal:
        case FdoDataType_Double: {
                double val = feature->GetDouble (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%f", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int16: {
                FdoInt16 val = feature->GetInt16 (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%d", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int32: {
                FdoInt32 val = feature->GetInt32 (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%ld", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Int64: {
                FdoInt64 val = feature->GetInt64 (desc.m_name.c_str ());
                #ifdef WIN32
                _i64tow (val, tbuff, 10);
                #else
                swprintf(tbuff, 256, L"%lli", val);
                #endif
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_Single:{
                float val = feature->GetSingle (desc.m_name.c_str ());
                swprintf (tbuff, 256, L"%f", val);
                wcsncpy (buff,  tbuff, len);
            }
            break;

        case FdoDataType_String:
            wcsncpy (buff,  feature->GetString (desc.m_name.c_str ()), len);
            break;

        case FdoDataType_CLOB:
            {
                FdoPtr<FdoCLOBValue> lob = (FdoCLOBValue *) feature->GetLOB (desc.m_name.c_str ());
                FdoPtr<FdoByteArray> bdata = lob->GetData ();
                WSTR                 wbuff;
                FdoByte            * bytes = bdata->GetData ();

                // what is clob representation?
                // unicode? codepage if not unicode?

                // assuming single byte character set

                for (int i = 0; i < bdata->GetCount (); i ++) {
                    wchar_t symbol [16];
                    if (i != 0)
                        wbuff.append (L",");
                    swprintf (symbol, 16, L"%c", bytes[i]);
                    wbuff.append (symbol);

                }
                wcsncpy (buff,  wbuff.c_str (), len);
            }
            break;

        case FdoDataType_BLOB:
            {
                FdoPtr<FdoLOBValue>  lob = feature->GetLOB (desc.m_name.c_str ());
                FdoPtr<FdoByteArray> bdata = lob->GetData ();
                WSTR                 wbuff;
                FdoByte            * bytes = bdata->GetData ();
                for (int i = 0; i < bdata->GetCount (); i ++) {
                    wchar_t symbol [16];
                    if (i != 0)
                        wbuff.append (L",");
                    swprintf (symbol, 16, L"%2hx", bytes[i]);
                    wbuff.append (symbol);

                }
                wcsncpy (buff,  wbuff.c_str (), len);
            }
            break;

        case FdoDataType_DateTime: {
            FdoDateTime dt = feature->GetDateTime (desc.m_name.c_str ());
            swprintf (tbuff, 256, L"%d-%d-%d %2d:%2d:%2.4f",
                             dt.year, dt.month, dt.day, dt.hour, dt.minute, dt.seconds);
            wcsncpy (buff,  tbuff, len);
            break;
        }

        default:
            wcsncpy (buff,  L"Unknown data", len);

        }
        return;
    }
    assert (false);
    return;
}

static bool CompareStringCollections (
    FdoStringCollection * slist1,
    FdoStringCollection * slist2
)
{

    int n1 = slist1 != NULL ? slist1->GetCount () : 0;
    int n2 = slist2 != NULL ? slist2->GetCount () : 0;

    if (n1 != n2)
        return false;

    for (int i = 0; i < n1; i ++) {
        if (wcscmp (slist1->GetString (i), slist2->GetString (i)) != 0)
            return false;
    }
    return true;
}

bool GwsQueryUtils::QueryDefinitionsEqual (
    IGWSQueryDefinition * qdef1,
    IGWSQueryDefinition * qdef2
)
{
    if (qdef1 == qdef2)
        return true;

    if (qdef1 == NULL || qdef2 == NULL)
        return false;

    if (qdef1->Type () != qdef2->Type ())
        return false;

    FdoPtr<FdoStringCollection> slist1;
    FdoPtr<FdoStringCollection> slist2;

    if (qdef1->Type () == eGwsQueryFeature) {
        IGWSFeatureQueryDefinition * fqdef1 = dynamic_cast<IGWSFeatureQueryDefinition *> (qdef1);
        IGWSFeatureQueryDefinition * fqdef2 = dynamic_cast<IGWSFeatureQueryDefinition *> (qdef2);

        if (! (fqdef1->ClassName () == fqdef2->ClassName ()))
            return false;

        FdoPtr<FdoFilter> flt1 = fqdef1->Filter ();
        FdoPtr<FdoFilter> flt2 = fqdef2->Filter ();

        FdoString * fstr1 = flt1 != NULL ? flt1->ToString () : L"f";
        FdoString * fstr2 = flt2 != NULL ? flt2->ToString () : L"f";
        if (wcscmp (fstr1, fstr2) != 0)
            return false;

        slist1 =  fqdef1->SelectList ();
        slist2 =  fqdef2->SelectList ();

        if (! CompareStringCollections (slist1, slist2))
            return false;

    } else {

        IGWSJoinQueryDefinition * jqdef1 = dynamic_cast<IGWSJoinQueryDefinition *> (qdef1);
        IGWSJoinQueryDefinition * jqdef2 = dynamic_cast<IGWSJoinQueryDefinition *> (qdef2);

        slist1 =  jqdef1->LeftJoinAttributes ();
        slist2 =  jqdef2->LeftJoinAttributes ();
        if (! CompareStringCollections (slist1, slist2))
            return false;

        slist1 =  jqdef1->RightJoinAttributes ();
        slist2 =  jqdef2->RightJoinAttributes ();
        if (! CompareStringCollections (slist1, slist2))
            return false;

        FdoPtr<IGWSQueryDefinition> jqdefp1;
        FdoPtr<IGWSQueryDefinition> jqdefp2;

        jqdefp1 = jqdef1->LeftQueryDefinition ();
        jqdefp2 = jqdef2->LeftQueryDefinition ();

        if (! QueryDefinitionsEqual (jqdefp1, jqdefp2))
            return false;

        jqdefp1 = jqdef1->RightQueryDefinition ();
        jqdefp2 = jqdef2->RightQueryDefinition ();
        if (! QueryDefinitionsEqual (jqdefp1, jqdefp2))
            return false;
    }
    return true;
}

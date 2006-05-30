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
#include "AceCommon.h"
#include "stdafx.h"

#include "GwsCommonImp.h"

// useful template
template <typename T> int Compare (T val1, T val2)
{
    if (val1 < val2)
        return -1;
    if (val1 > val2)
        return 1;
    return 0;
}

/////////////////////////////////////////////////////////////////////
//
// namespace GwsCommonFdoUtils
//
/////////////////////////////////////////////////////////////////////
void GwsCommonFdoUtils::GetClassDefinition (
    FdoIConnection         * pConn,
    const GWSQualifiedName & classname,
    FdoFeatureSchema     * & schema,
    FdoClassDefinition   * & classDef
)
{
    GisPtr<FdoIDescribeSchema>  descSchema =
            (FdoIDescribeSchema *) pConn->CreateCommand (FdoCommandType_DescribeSchema);

    GisPtr <FdoFeatureSchemaCollection> schemas =
            (FdoFeatureSchemaCollection *) descSchema->Execute ();

    schema = (FdoFeatureSchema *)schemas->GetItem (classname.Schema());
    GisPtr<FdoClassCollection> pClasses = schema->GetClasses();
    classDef = pClasses->GetItem(classname.Name());
}

bool GwsCommonFdoUtils::GetFdoClassIdentityProperties (
    FdoClassDefinition                  *   pClassDef,
    FdoDataPropertyDefinitionCollection * & pIdentityProps
)
{
    // identity properties are acquired from the principal base class

    pClassDef->AddRef ();
    GisPtr<FdoClassDefinition> childClass = pClassDef;
    GisPtr<FdoClassDefinition> baseClass;
    pIdentityProps = NULL;

    while ((baseClass = childClass->GetBaseClass ()) != NULL) {
        childClass = baseClass;
    }

    pIdentityProps = childClass->GetIdentityProperties ();
    if (pIdentityProps == NULL)
        return false;

    if(pIdentityProps->GetCount() == 0)
    {
        pIdentityProps->Release ();
        pIdentityProps = NULL;
        return false;
    }
    return true;
}


///////////////////////////////////////////////////////////////////////////////
FdoPropertyDefinition * GwsCommonFdoUtils::GetPropertyDefinition (
    FdoClassDefinition  * pClassDef,
    GisString           * PropertyName
)
{
    pClassDef->AddRef ();
    GisPtr<FdoClassDefinition> classDef = pClassDef;
    FdoPropertyDefinition    * propDef  = NULL;

    for (; propDef == NULL && classDef != NULL ; classDef = classDef->GetBaseClass ())
    {
        GisPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties ();
        propDef = propdsc->FindItem (PropertyName);
    }
    return propDef;
}

int GWSFdoUtilities::CompareDataValues (
    FdoDataValue * val1,
    FdoDataValue * val2
)
{
    // assuming NULL's are equal, which is not really true.
    // result of will NULL comparison is unknown, but we don't support it

    if ((val1 == NULL || val1->IsNull ()) && (val2 == NULL || val2->IsNull ()))
        return 0;

    if (val1 == NULL || val1->IsNull ())
        return -2;

    if (val2 == NULL || val2->IsNull ())
        return 2;

    bool        bVal2;
    GisByte     byteVal2;


    double      doubleVal2;
    float       floatVal2;
    GisInt16    int16Val2;
    GisInt32    int32Val2;
    GisInt64    int64Val2;
    GisString * strVal2;
    GisDateTime dtVal2;

    switch (val1->GetDataType ()) {
    case FdoDataType_Boolean:
        {
            bool val = (bool) (* (FdoBooleanValue *)  val1);
            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<bool> (val, bVal2);

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<GisByte> ((GisByte)val, byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double> ((double)val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<GisInt16> ((GisInt16)val, int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<GisInt32> ((GisInt32)val, int32Val2);

            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<GisInt64> ((GisInt64)val, int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float> ((float)val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // boolean is less than other tipes
                break;
            }
            break;
        }
    case FdoDataType_Byte:
        {
            GisByte  val = (GisByte) (* (FdoByteValue *)  val1);;
            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<GisByte> (val, (GisByte)bVal2);

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<GisByte> (val, byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double> ((double)val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<GisInt16> ((GisInt16)val, int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<GisInt32> ((GisInt32)val, int32Val2);

            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<GisInt64> ((GisInt64)val, int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float> ((float)val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }
    case FdoDataType_Decimal:
    case FdoDataType_Double:
        {
            double val;
            if (val1->GetDataType () == FdoDataType_Decimal)
                val = (double) (* (FdoDoubleValue *)  val1);
            else
                val = (double) (* (FdoDecimalValue *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<double>(val, (double) bVal2);
                break;

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<double>(val, (double) byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double>(val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<double>(val, (double) int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<double>(val, (double) int32Val2);


            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<double>(val, (double) int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<double>(val, (double) floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }
    case FdoDataType_Int16:
        {
            GisInt16 val = (GisInt16) (* (FdoInt16Value *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<GisInt16>(val, (GisInt16) bVal2);
                break;

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<GisInt16>(val, (GisInt16) byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double>((double) val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<double>(val, int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<GisInt32>((GisInt32)val, int32Val2);


            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<GisInt64>((GisInt64)val, int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float>((float)val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }
        break;

    case FdoDataType_Int32:
        {
            GisInt32 val = (GisInt32) (* (FdoInt32Value *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<GisInt32>(val, (GisInt32) bVal2);
                break;

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<GisInt32>(val, (GisInt32) byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double>((double) val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<GisInt32>(val, (GisInt32)int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<GisInt32>(val, int32Val2);

            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<GisInt64>((GisInt64)val, int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float>((float)val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }
        break;

    case FdoDataType_Int64:
        {
            GisInt64 val = (GisInt64) (* (FdoInt64Value *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<GisInt64>(val, (GisInt64) bVal2);
                break;

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<GisInt64>(val, (GisInt64) byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double>((double) val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<GisInt64>(val, (GisInt64)int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<GisInt64>(val, (GisInt64)int32Val2);

            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<GisInt64>(val, int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float>((float)val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }
        break;

    case FdoDataType_Single:
        {
            float val = (float) (* (FdoSingleValue *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
                bVal2 = (bool) (* (FdoBooleanValue *)  val2);
                return Compare<float>(val, (float) bVal2);
                break;

            case FdoDataType_Byte:
                byteVal2 = (GisByte) (* (FdoByteValue *)  val2);
                return Compare<float>(val, (float) byteVal2);

            case FdoDataType_Decimal:
            case FdoDataType_Double:
                if (val2->GetDataType () == FdoDataType_Decimal)
                    doubleVal2 = (double) (* (FdoDoubleValue *)  val2);
                else
                    doubleVal2 = (double) (* (FdoDecimalValue *)  val2);
                return Compare<double>((double)val, doubleVal2);

            case FdoDataType_Int16:
                int16Val2 = (GisInt16) (* (FdoInt16Value *)  val2);
                return Compare<float>(val, (float) int16Val2);

            case FdoDataType_Int32:
                int32Val2 = (GisInt32) (* (FdoInt32Value *)  val2);
                return Compare<float>(val, (float) int32Val2);


            case FdoDataType_Int64:
                int64Val2 = (GisInt64) (* (FdoInt64Value *)  val2);
                return Compare<float>(val, (float) int64Val2);

            case FdoDataType_Single:
                floatVal2 = (float) (* (FdoSingleValue *)  val2);
                return Compare<float>(val, floatVal2);

            case FdoDataType_String:
            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            case FdoDataType_DateTime:
            default:
                return -1; // byte is less than other tipes
                break;
            }
            break;
        }

    case FdoDataType_String:
        {
            GisString*  val = ((FdoStringValue *) val1)->GetString ();
            switch (val2->GetDataType ()) {
            case FdoDataType_String:
                {
                    strVal2 = ((FdoStringValue *) val2)->GetString ();
                    int res = wcscmp (val, strVal2);
                    if (res < 0)
                        return -1;
                    if (res > 0)
                        return 1;
                    return 0;
                }
                break;

            case FdoDataType_Boolean:
            case FdoDataType_Byte:
            case FdoDataType_Decimal:
            case FdoDataType_Double:
            case FdoDataType_Int16:
            case FdoDataType_Int32:
            case FdoDataType_Int64:
            case FdoDataType_Single:
            case FdoDataType_DateTime:
                return 1;   // greater than these types

            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
                break;
            }
        }
        break;

    case FdoDataType_BLOB:
    case FdoDataType_CLOB:
        switch (val2->GetDataType ()) {
        case FdoDataType_Boolean:
        case FdoDataType_Byte:
        case FdoDataType_Decimal:
        case FdoDataType_Double:
        case FdoDataType_Int16:
        case FdoDataType_Int32:
        case FdoDataType_Int64:
        case FdoDataType_Single:
        case FdoDataType_String:
        case FdoDataType_DateTime:
            return 1;

        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
            return 0;
        }
        break;

    case FdoDataType_DateTime:
        {
            GisDateTime val = (GisDateTime) (* (FdoDateTimeValue *)  val1);

            switch (val2->GetDataType ()) {
            case FdoDataType_Boolean:
            case FdoDataType_Byte:
            case FdoDataType_Decimal:
            case FdoDataType_Double:
            case FdoDataType_Int16:
            case FdoDataType_Int32:
            case FdoDataType_Int64:
            case FdoDataType_Single:
            case FdoDataType_String:
                return 1;

            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
                return -1;

            case FdoDataType_DateTime:
                {
                    GisDateTime dtVal2 = (GisDateTime) (* (FdoDateTimeValue *)  val2);
                    int         res = 0;
                    res = Compare<int>(val.year, dtVal2.year);
                    if (res != 0)
                        return res;
                    res = Compare<int>(val.month, dtVal2.month);
                    if (res != 0)
                        return res;
                    res = Compare<int>(val.day, dtVal2.day);
                    if (res != 0)
                        return res;
                    res = Compare<int>(val.hour, dtVal2.hour);
                    if (res != 0)
                        return res;
                    res = Compare<int>(val.minute, dtVal2.minute);
                    if (res != 0)
                        return res;
                    res = Compare<float>(val.seconds, dtVal2.seconds);
                    return res;
                }
            }
            break;
        }

    }
    return 0;
}


bool GWSFdoUtilities::IsComparableDataTypes (
    FdoDataPropertyDefinition * val1,
    FdoDataPropertyDefinition * val2
)
{
    switch (val1->GetDataType ()) {
    case FdoDataType_Boolean:
        switch (val2->GetDataType ()) {
        case FdoDataType_Boolean:
            return true;

        case FdoDataType_Byte:
        case FdoDataType_Decimal:
        case FdoDataType_Double:
        case FdoDataType_Int16:
        case FdoDataType_Int32:
        case FdoDataType_Int64:
        case FdoDataType_Single:
        case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        case FdoDataType_DateTime:
        default:
            return false;
        }
        break;

    case FdoDataType_Byte:
    case FdoDataType_Int16:
    case FdoDataType_Int32:
    case FdoDataType_Int64:
    case FdoDataType_Decimal:
    case FdoDataType_Single:
    case FdoDataType_Double:
        switch (val2->GetDataType ()) {
        case FdoDataType_Byte:
        case FdoDataType_Decimal:
        case FdoDataType_Double:
        case FdoDataType_Int16:
        case FdoDataType_Int32:
        case FdoDataType_Int64:
        case FdoDataType_Single:
        case FdoDataType_String:  // Allow numeric type to compare to string here.  When doing comparison, caller will be responsible
                                  // for making sure the comparsion can succeed.
            return true;
        case FdoDataType_Boolean:
        //case FdoDataType_String:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        case FdoDataType_DateTime:
        default:
            return false;
        }
        break;

    case FdoDataType_String:
        switch (val2->GetDataType ()) {
            // Allow string type to compare to numeric type here.  When doing comparison, caller will be responsible
            // for making sure the comparison can succeed.
        case FdoDataType_Byte:
        case FdoDataType_Decimal:
        case FdoDataType_Double:
        case FdoDataType_Int16:
        case FdoDataType_Int32:
        case FdoDataType_Int64:
        case FdoDataType_Single:
        case FdoDataType_String:
            return true;

        //case FdoDataType_Byte:
        //case FdoDataType_Decimal:
        //case FdoDataType_Double:
        //case FdoDataType_Int16:
        //case FdoDataType_Int32:
        //case FdoDataType_Int64:
        //case FdoDataType_Single:
        case FdoDataType_Boolean:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        case FdoDataType_DateTime:
        default:
            return false;
        }
        break;

    case FdoDataType_BLOB:
    case FdoDataType_CLOB:
        return false;

    case FdoDataType_DateTime:
        switch (val2->GetDataType ()) {
        case FdoDataType_DateTime:
            return true;

        case FdoDataType_Byte:
        case FdoDataType_Decimal:
        case FdoDataType_Double:
        case FdoDataType_Int16:
        case FdoDataType_Int32:
        case FdoDataType_Int64:
        case FdoDataType_Single:
        case FdoDataType_Boolean:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        case FdoDataType_String:
        default:
            return false;
        }
        break;
    }
    return false;
}

WSTR GwsCommonFdoUtils::MakeFdoQualifiedName(const GWSQualifiedName & classname)
{
    WSTR strRet = classname.Schema();
    assert(!strRet.empty());

    strRet += L":";
    strRet += classname.Name ();
    return strRet;
}

WSTR GwsCommonFdoUtils::GetRevisionProperty (FdoClassDefinition * classdef)
{
    // discover revision property. Revision property is defined
    // for a given class
    GisPtr<FdoReadOnlyPropertyDefinitionCollection> pBaseProperties = classdef->GetBaseProperties();

    // See if property with this name already exists:
    for (int idx=0; pBaseProperties != NULL && idx < pBaseProperties->GetCount(); idx++) {
        GisPtr<FdoPropertyDefinition> pBaseProperty;
        pBaseProperty = (FdoPropertyDefinition*)pBaseProperties->GetItem(idx);
        if (! _wcsicmp (pBaseProperty->GetName(), REVISIONNUMBER_PROPNAME)) {
            return pBaseProperty->GetName();
        }
    }
    return L"";
}


///////////////////////////////////////////////////////////////////////////////
GWSExtendedFeatureId GwsCommonFdoUtils::MakeFeatureId (
    const GWSQualifiedName            & classname,
    GisPtr<FdoPropertyValueCollection>  ident,
    const wchar_t                     * ltname
)
{
    ltname; // For "unreferenced formal parameter" warning

    static GWSExtendedFeatureId s_fid;

    try {
        CGwsDataValueCollection   *  keyvals = NULL;
        keyvals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();

        int size = ident->GetCount ();

        for (int i = 0; i < size ; i ++) {
            GisPtr<FdoPropertyValue>    propval;
            GisPtr<FdoDataValue>        value;
            propval = ident->GetItem (i);
            value = (FdoDataValue *) propval->GetValue ();
            keyvals->Add (value);

        }
        return GWSExtendedFeatureId (classname, keyvals);

    } catch (GisException * e) {
        assert (false);
        e->Release ();
    }
    return s_fid;
}

FdoFilter * GwsCommonFdoUtils::CombineFilters (
    FdoFilter                   * flt1,
    FdoFilter                   * flt2,
    FdoBinaryLogicalOperations    op
)
{
    FdoFilter * pFilter = NULL;
    if (flt1 != NULL) {
        if (flt2 != NULL) {
            return FdoFilter::Combine(flt1, op, flt2);
        }
        pFilter = flt1;

    } else {
        pFilter = flt2;
    }
    if (pFilter != NULL)
        pFilter->AddRef ();
    return pFilter;
}

bool GwsCommonFdoUtils::GetGeometryName (FdoClassDefinition *pClassDef, std::wstring &name)
{
    if(pClassDef == NULL)
        return false;

    if(pClassDef->GetClassType() == FdoClassType_FeatureClass)
    {
        FdoFeatureClass* pFeatClass = dynamic_cast<FdoFeatureClass*>(pClassDef);
        if(!pFeatClass)
        {
            assert(false);
            return false;
        }

        GisPtr<FdoGeometricPropertyDefinition> pgDef = pFeatClass->GetGeometryProperty();
        if (pgDef != NULL) {
            name = pgDef->GetName();
            return true;
        }
    }
    GisInt32 idx;

    GisPtr<FdoClassDefinition>  classDef = pClassDef;
    GIS_SAFE_ADDREF(pClassDef);
    for (; classDef != NULL ; classDef = classDef->GetBaseClass ())
    {
        GisPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties ();
        // discover geometric property name. Use the first one if there are many.
        for (idx = 0; idx < propdsc->GetCount(); idx ++) {
            GisPtr<FdoPropertyDefinition>   prop;
            prop = propdsc->GetItem(idx);
            if (prop->GetPropertyType () == FdoPropertyType_GeometricProperty)
            {
                name = prop->GetName ();
                return true;
            }
        }
    }
    return false;
}

IGWSException* GwsCommonFdoUtils::Create (
    EGwsStatus   stat,
    CGwsObject * diaginfo
)
{
    CGwsException *pExc = new CGwsException (stat);

    for (int i = 0; i < diaginfo->NumStatuses (); i ++) {
        const CGwsStatus & stat = diaginfo->Status (i);
        pExc->PushStatus (stat);
    }
    return pExc;
}

EGwsStatus GwsCommonFdoUtils::DescribeClassSC (
    FdoIConnection          * conn,
    const GWSQualifiedName &  classname,
    GwsSpatialContextDescription & desc
)
{
    GisPtr<FdoFeatureSchema>   schema;
    GisPtr<FdoClassDefinition> classDef;

    GwsCommonFdoUtils::GetClassDefinition (conn, classname, schema.p, classDef.p);

    for (; classDef != NULL ; classDef = classDef->GetBaseClass ()) {
        GisPtr<FdoPropertyDefinitionCollection> propdsc = classDef->GetProperties ();
        // discover geometric property name. Use the first one if there are many.
        for (int idx = 0; idx < propdsc->GetCount(); idx ++) {
            GisPtr<FdoPropertyDefinition>   prop;
            prop = propdsc->GetItem (idx);

            if (prop->GetPropertyType () == FdoPropertyType_GeometricProperty) {
                FdoGeometricPropertyDefinition* geomProp = static_cast<FdoGeometricPropertyDefinition*>(prop.p);
                GisString* pSC = geomProp->GetSpatialContextAssociation();
                if(pSC != NULL) {
                    return GwsCommonFdoUtils::DescribeSC (conn, pSC, desc);
                }
            }
        }
    }
    return eGwsSCNotFound;
}


EGwsStatus GwsCommonFdoUtils::DescribeSC (
    FdoIConnection               * conn,
    GisString                    * scname,
    GwsSpatialContextDescription & scdesc
)
{
    GisPtr<FdoIGetSpatialContexts>   cmd;
    GisPtr<FdoISpatialContextReader> reader;
    scdesc.SetClassName (GWSQualifiedName ());
    scdesc.SetPropertyName (NULL);
    scdesc.SetSpatialContextName (NULL);
    scdesc.SetSpatialContextDesc (NULL);
    scdesc.SetCsNameWkt (NULL);

    if (scname == NULL || * scname == 0)
        return eGwsOk;

    try {
        cmd = (FdoIGetSpatialContexts *) conn->CreateCommand (FdoCommandType_GetSpatialContexts);
        reader = cmd->Execute ();
        while (reader->ReadNext ()) {
            if (wcscmp (reader->GetName (), scname) == 0) {
                GisString * cswkt = reader->GetCoordinateSystemWkt ();
                STRING srcwkt = cswkt;

                // If the WKT is not defined, attempt to resolve it from the name.
                // This is a work around for MG298: WKT not set for WMS and 
                // WFS spatial contexts.
                if (srcwkt.empty())
                {
                    try
                    {
                        Ptr<MgCoordinateSystem> csPtr = new MgCoordinateSystem();
                        srcwkt = csPtr->ConvertCoordinateSystemCodeToWkt(reader->GetName());
                    }
                    catch (MgException* e)
                    {
                        SAFE_RELEASE(e);
                    }
                    catch(...)
                    {
                        // Just use the empty WKT. 
                    }
                }

                GisString * desc  = reader->GetDescription ();
                scdesc.SetCsNameWkt (srcwkt.c_str());
                scdesc.SetSpatialContextDesc (desc);
                scdesc.SetSpatialContextName (scname);

                GisPtr<GisByteArray> pByteArray = reader->GetExtent();
                if (pByteArray) {
                    GisPtr<GisAgfGeometryFactory> pAwkbFactory = GisAgfGeometryFactory::GetInstance();
                    GisPtr<GisIGeometry> pGeometry = pAwkbFactory->CreateGeometryFromAgf(pByteArray);
                    GisPtr<GisIEnvelope> pEnvelope = pGeometry->GetEnvelope();
                    scdesc.SetExtents (pEnvelope);
                }

                return eGwsOk;
            }
        }
        return eGwsSCNotFound;

    } catch (GisException * gis) {
        gis->Release ();
    }
    // in case when exception thrown, assume that sc are not
    // supported.
    return eGwsNotSupported;
}

/*
 * Copyright (C) 2007 by Autodesk, Inc. All Rights Reserved.
 *
 * By using this code, you are agreeing to the terms and conditions of
 * the License Agreement included in the documentation for this code.
 *
 * AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
 * DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
 *
 * Use, duplication, or disclosure by the U.S. Government is subject
 * to restrictions set forth in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
 * (Rights in Technical Data and Computer Software), as applicable.
 */

#include "stdafx.h"
#include "ExpressionHelper.h"
#include "ExpressionFunctionArgb.h"
#include "ExpressionFunctionDecap.h"
#include "ExpressionFunctionFeatureClass.h"
#include "ExpressionFunctionFeatureId.h"
#include "ExpressionFunctionLayerId.h"
#include "ExpressionFunctionMapName.h"
#include "ExpressionFunctionSession.h"
#include "ExpressionFunctionUrlEncode.h"

const RS_String s_Empty(L"");


// Returns a collection of the custom expression engine functions
// defined by stylization.
FdoExpressionEngineFunctionCollection* ExpressionHelper::GetCustomFunctions(Renderer* renderer, RS_FeatureReader* reader)
{
    RS_MapUIInfo* mapInfo = renderer->GetMapInfo();
    RS_LayerUIInfo* layerInfo = renderer->GetLayerInfo();
    RS_FeatureClassInfo* featInfo = renderer->GetFeatureClassInfo();

    const RS_String& session = (  mapInfo != NULL)?   mapInfo->session() : s_Empty;
    const RS_String& mapName = (  mapInfo != NULL)?   mapInfo->name()    : s_Empty;
    const RS_String& layerId = (layerInfo != NULL)? layerInfo->guid()    : s_Empty;
    const RS_String& featCls = ( featInfo != NULL)?  featInfo->name()    : s_Empty;

    FdoPtr<ExpressionFunctionArgb> funcArgb = ExpressionFunctionArgb::Create();
    FdoPtr<ExpressionFunctionDecap> funcDecap = ExpressionFunctionDecap::Create();
    FdoPtr<ExpressionFunctionFeatureId> funcFeatureId = ExpressionFunctionFeatureId::Create(reader);
    FdoPtr<ExpressionFunctionFeatureClass> funcFeatureClass = ExpressionFunctionFeatureClass::Create(featCls.c_str());
    FdoPtr<ExpressionFunctionLayerId> funcLayerId = ExpressionFunctionLayerId::Create(layerId.c_str());
    FdoPtr<ExpressionFunctionMapName> funcMapName = ExpressionFunctionMapName::Create(mapName.c_str());
    FdoPtr<ExpressionFunctionSession> funcSession = ExpressionFunctionSession::Create(session.c_str());
    FdoPtr<ExpressionFunctionUrlEncode> funcUrlEncode = ExpressionFunctionUrlEncode::Create();

    FdoExpressionEngineFunctionCollection* userDefinedFunctions = FdoExpressionEngineFunctionCollection::Create();
    userDefinedFunctions->Add(funcArgb);
    userDefinedFunctions->Add(funcDecap);
    userDefinedFunctions->Add(funcFeatureId);
    userDefinedFunctions->Add(funcFeatureClass);
    userDefinedFunctions->Add(funcLayerId);
    userDefinedFunctions->Add(funcMapName);
    userDefinedFunctions->Add(funcSession);
    userDefinedFunctions->Add(funcUrlEncode);

    return userDefinedFunctions;
}


// Evaluates an FdoLiteralValue as a boolean
bool ExpressionHelper::GetAsBoolean(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsBoolean((FdoDataValue*)literalValue);

    _ASSERT(false);
    return false;
}


// Evaluates an FdoLiteralValue as an integer
int ExpressionHelper::GetAsInt32(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsInt32((FdoDataValue*)literalValue);

    _ASSERT(false);
    return 0;
}


// Evaluates an FdoLiteralValue as a double
double ExpressionHelper::GetAsDouble(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsDouble((FdoDataValue*)literalValue);

    _ASSERT(false);
    return 0.0;
}


// Evaluates an FdoLiteralValue as a string
const wchar_t* ExpressionHelper::GetAsString(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsString((FdoDataValue*)literalValue);

    _ASSERT(false);
    return NULL;
}


// Evaluates an FdoDataValue as a boolean
bool ExpressionHelper::GetAsBoolean(FdoDataValue* dataValue)
{
    switch (dataValue->GetDataType())
    {
        case FdoDataType_Boolean:
            return ((FdoBooleanValue*)dataValue)->GetBoolean();

        case FdoDataType_Byte:
            return ((FdoByteValue*)dataValue)->GetByte() != 0;

        case FdoDataType_Int16:
            return ((FdoInt16Value*)dataValue)->GetInt16() != 0;

        case FdoDataType_Int32:
            return ((FdoInt32Value*)dataValue)->GetInt32() != 0;

        case FdoDataType_Int64:
            return ((FdoInt64Value*)dataValue)->GetInt64() != 0;

        case FdoDataType_Single:
            return ((FdoSingleValue*)dataValue)->GetSingle() != 0.0f;

        case FdoDataType_Double:
            return ((FdoDoubleValue*)dataValue)->GetDouble() != 0.0;

        case FdoDataType_Decimal:
            return ((FdoDecimalValue*)dataValue)->GetDecimal() != 0.0;

        case FdoDataType_String:
        {
            FdoString* value = ((FdoStringValue*)dataValue)->GetString();
            return _wcsnicmp(value, L"true", 4) == 0;
        }

        case FdoDataType_DateTime:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            // unsupported conversion to boolean data type
            _ASSERT(false);
    }

    return false;
}


// Evaluates an FdoDataValue as an integer
int ExpressionHelper::GetAsInt32(FdoDataValue* dataValue)
{
    switch (dataValue->GetDataType())
    {
        case FdoDataType_Byte:
            return (int)((FdoByteValue*)dataValue)->GetByte();

        case FdoDataType_Int16:
            return (int)((FdoInt16Value*)dataValue)->GetInt16();

        case FdoDataType_Int32:
            return ((FdoInt32Value*)dataValue)->GetInt32();

        case FdoDataType_Int64:
            return (int)((FdoInt64Value*)dataValue)->GetInt64();

        case FdoDataType_Single:
            return (int)((FdoSingleValue*)dataValue)->GetSingle();

        case FdoDataType_Double:
            return (int)((FdoDoubleValue*)dataValue)->GetDouble();

        case FdoDataType_Decimal:
            return (int)((FdoDecimalValue*)dataValue)->GetDecimal();

        case FdoDataType_String:
        {
            FdoString* value = ((FdoStringValue*)dataValue)->GetString();

            int n = 0;
            int ret = swscanf(value, L"%ld", &n);
            _ASSERT(ret == 1);

            return n;
        }

        case FdoDataType_Boolean:
        case FdoDataType_DateTime:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            // unsupported conversion to int32 data type
            _ASSERT(false);
    }

    return 0;
}


// Evaluates an FdoDataValue as a double
double ExpressionHelper::GetAsDouble(FdoDataValue* dataValue)
{
    switch (dataValue->GetDataType())
    {
        case FdoDataType_Byte:
            return (double)((FdoByteValue*)dataValue)->GetByte();

        case FdoDataType_Int16:
            return (double)((FdoInt16Value*)dataValue)->GetInt16();

        case FdoDataType_Int32:
            return (double)((FdoInt32Value*)dataValue)->GetInt32();

        case FdoDataType_Int64:
            return (double)((FdoInt64Value*)dataValue)->GetInt64();

        case FdoDataType_Single:
            return (double)((FdoSingleValue*)dataValue)->GetSingle();

        case FdoDataType_Double:
            return ((FdoDoubleValue*)dataValue)->GetDouble();

        case FdoDataType_Decimal:
            return ((FdoDecimalValue*)dataValue)->GetDecimal();

        case FdoDataType_String:
        {
            FdoString* value = ((FdoStringValue*)dataValue)->GetString();

            double d = 0.0;

            // check for a number format that uses comma instead of point
            // for decimal place separator -- it is pretty basic way to do
            // this check, and may not work in all cases...? //TODO
            lconv* lconv_info = localeconv();
            char sep = *lconv_info->decimal_point;

            const wchar_t* hasInvalidSeparator = NULL;

            if (sep == '.')
                hasInvalidSeparator = wcschr(value, L',');
            else
                hasInvalidSeparator = wcschr(value, L'.');

            if (!hasInvalidSeparator)
            {
                int ret = swscanf(value, L"%lf", &d);
                _ASSERT(ret == 1);
            }
            else
            {
                // case where we will need to switch locale.
                // remember the old one
                char* oldloc = setlocale(LC_NUMERIC, NULL);
                char memoldloc[64];
                _ASSERT(strlen(oldloc) < 63);
                strcpy(memoldloc, oldloc);

                // switch to a locale that will likely do a good job
                if (sep == '.')
                {
                    // case where string contains a comma but separator
                    // in current locale is .
                    // switch to a known comma using locale
                    setlocale(LC_NUMERIC, "fr_FR");
                }
                else
                {
                    // case where string contains a . but separator in current
                    // locale is ,
                    setlocale(LC_NUMERIC, "C");
                }

                int ret = swscanf(value, L"%lf", &d);
                _ASSERT(ret == 1);

                // restore the original locale
                setlocale(LC_NUMERIC, memoldloc);
            }

            return d;
        }

        case FdoDataType_Boolean:
        case FdoDataType_DateTime:
        case FdoDataType_BLOB:
        case FdoDataType_CLOB:
        default:
            // unsupported conversion to double data type
            _ASSERT(false);
    }

    return 0.0;
}


// Evaluates an FdoDataValue as a string
const wchar_t* ExpressionHelper::GetAsString(FdoDataValue* dataValue)
{
    if (dataValue->GetDataType() == FdoDataType_String)
    {
        FdoStringValue* strval = (FdoStringValue*)dataValue;
        return strval->GetString();
    }

    return dataValue->ToString();
}

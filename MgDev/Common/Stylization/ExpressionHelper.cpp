//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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

#include "stdafx.h"
#include "ExpressionHelper.h"
#include "ExpressionFunctionArgb.h"
#include "ExpressionFunctionDecap.h"
#include "ExpressionFunctionFeatureClass.h"
#include "ExpressionFunctionFeatureId.h"
#include "ExpressionFunctionFeatureSource.h"
#include "ExpressionFunctionHtmlColor.h"
#include "ExpressionFunctionLayerId.h"
#include "ExpressionFunctionLayerDefinition.h"
#include "ExpressionFunctionMapCenterX.h"
#include "ExpressionFunctionMapCenterY.h"
#include "ExpressionFunctionMapName.h"
#include "ExpressionFunctionMapScale.h"
#include "ExpressionFunctionSession.h"
#include "ExpressionFunctionUrlEncode.h"
#include "ExpressionFunctionIf.h"
#include "ExpressionFunctionLookup.h"
#include "ExpressionFunctionRange.h"



#include "RS_FeatureReader.h"


const RS_String s_Empty(L"");


// Returns a collection of the custom functions defined by stylization.
// The IF function - the first one in the collection - needs to be hooked
// to the engine in order to evaluate correctly.
FdoExpressionEngineFunctionCollection* ExpressionHelper::GetExpressionEngineFunctions(Renderer* renderer, RS_FeatureReader* reader)
{
    FdoExpressionEngineFunctionCollection* userDefinedFunctions = FdoExpressionEngineFunctionCollection::Create();

    // common functions
    RS_FeatureClassInfo* featInfo = renderer? renderer->GetFeatureClassInfo() : NULL;
    const RS_String& featCls = (featInfo != NULL)? featInfo->name() : s_Empty;
    const RS_String& featSource = (featInfo != NULL) ? featInfo->source() : s_Empty;

    RS_MapUIInfo* mapInfo = renderer? renderer->GetMapInfo() : NULL;
    RS_LayerUIInfo* layerInfo = renderer? renderer->GetLayerInfo() : NULL;
    const double mapScale = (mapInfo != NULL) ? mapInfo->scale() : 0.0;
    const double mapX = (mapInfo != NULL) ? mapInfo->viewx() : 0.0;
    const double mapY = (mapInfo != NULL) ? mapInfo->viewy() : 0.0;
    const RS_String& layerDef = (layerInfo != NULL) ? layerInfo->layerdef() : s_Empty;

    FdoPtr<ExpressionFunctionArgb> funcArgb = ExpressionFunctionArgb::Create();
    FdoPtr<ExpressionFunctionDecap> funcDecap = ExpressionFunctionDecap::Create();
    FdoPtr<ExpressionFunctionFeatureClass> funcFeatureClass = ExpressionFunctionFeatureClass::Create(featCls.c_str());
    FdoPtr<ExpressionFunctionUrlEncode> funcUrlEncode = ExpressionFunctionUrlEncode::Create();
    FdoPtr<ExpressionFunctionIf> funcIf = ExpressionFunctionIf::Create();
    FdoPtr<ExpressionFunctionLookup> funcLookup = ExpressionFunctionLookup::Create();
    FdoPtr<ExpressionFunctionRange> funcRange = ExpressionFunctionRange::Create();
    FdoPtr<ExpressionFunctionFeatureSource> funcFeatureSource = ExpressionFunctionFeatureSource::Create(featSource.c_str());
    FdoPtr<ExpressionFunctionLayerDefinition> funcLayerDefinition = ExpressionFunctionLayerDefinition::Create(layerDef.c_str());
    FdoPtr<ExpressionFunctionMapCenterX> funcMapCenterX = ExpressionFunctionMapCenterX::Create(mapX);
    FdoPtr<ExpressionFunctionMapCenterY> funcMapCenterY = ExpressionFunctionMapCenterY::Create(mapY);
    FdoPtr<ExpressionFunctionMapScale> funcMapScale = ExpressionFunctionMapScale::Create(mapScale);

    userDefinedFunctions->Add(funcIf);      // make IF the first one to optimize search in GetExpressionEngine
    userDefinedFunctions->Add(funcArgb);
    userDefinedFunctions->Add(funcDecap);
    userDefinedFunctions->Add(funcFeatureClass);
    userDefinedFunctions->Add(funcUrlEncode);
    userDefinedFunctions->Add(funcLookup);
    userDefinedFunctions->Add(funcRange);
    userDefinedFunctions->Add(funcFeatureSource);
    userDefinedFunctions->Add(funcLayerDefinition);
    userDefinedFunctions->Add(funcMapCenterX);
    userDefinedFunctions->Add(funcMapCenterY);
    userDefinedFunctions->Add(funcMapScale);

    // MapGuide-specific functions
#ifndef MG_EXCLUDE_EXPRESSION_FUNCTIONS
    const RS_String& session = (  mapInfo != NULL)?   mapInfo->session() : s_Empty;
    const RS_String& mapName = (  mapInfo != NULL)?   mapInfo->name()    : s_Empty;
    const RS_String& layerId = (layerInfo != NULL)? layerInfo->guid()    : s_Empty;

    FdoPtr<ExpressionFunctionHtmlColor> funcHtmlColor = ExpressionFunctionHtmlColor::Create();
    FdoPtr<ExpressionFunctionFeatureId> funcFeatureId = ExpressionFunctionFeatureId::Create(reader);
    FdoPtr<ExpressionFunctionLayerId> funcLayerId = ExpressionFunctionLayerId::Create(layerId.c_str());
    FdoPtr<ExpressionFunctionMapName> funcMapName = ExpressionFunctionMapName::Create(mapName.c_str());
    FdoPtr<ExpressionFunctionSession> funcSession = ExpressionFunctionSession::Create(session.c_str());

    userDefinedFunctions->Add(funcHtmlColor);
    userDefinedFunctions->Add(funcFeatureId);
    userDefinedFunctions->Add(funcLayerId);
    userDefinedFunctions->Add(funcMapName);
    userDefinedFunctions->Add(funcSession);
#endif

    return userDefinedFunctions;
}


// Returns an FDO expression engine configured with the custom functions
// defined by stylization.
FdoExpressionEngine* ExpressionHelper::GetExpressionEngine(Renderer* renderer, RS_FeatureReader* reader)
{
    // get the user-defined functions
    FdoPtr<FdoExpressionEngineFunctionCollection> userDefinedFunctions = ExpressionHelper::GetExpressionEngineFunctions(renderer, reader);

    // create the engine
    FdoPtr<FdoIFeatureReader> fdoReader = reader? reader->GetInternalReader() : NULL;
    FdoPtr<FdoClassDefinition> classDef = fdoReader? fdoReader->GetClassDefinition() : FdoClass::Create();
    FdoExpressionEngine* exec = FdoExpressionEngine::Create(fdoReader, classDef, userDefinedFunctions);

    // now that we have the engine, set it on the functions that need it - for now
    // this is only the IF function
    for (int i=0; i<userDefinedFunctions->GetCount(); ++i)
    {
        FdoPtr<FdoExpressionEngineIFunction> func = userDefinedFunctions->GetItem(i);
        ExpressionFunctionIf* funcIf = dynamic_cast<ExpressionFunctionIf*>(func.p);
        if (funcIf)
        {
            funcIf->SetExpressionEngine(exec);
            break;
        }
    }

    return exec;
}


// Evaluates an FdoLiteralValue as a boolean
bool ExpressionHelper::GetAsBoolean(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsBoolean(static_cast<FdoDataValue*>(literalValue));

    _ASSERT(false);
    return false;
}


// Evaluates an FdoLiteralValue as an integer
int ExpressionHelper::GetAsInt32(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsInt32(static_cast<FdoDataValue*>(literalValue));

    _ASSERT(false);
    return 0;
}


// Evaluates an FdoLiteralValue as a double
double ExpressionHelper::GetAsDouble(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsDouble(static_cast<FdoDataValue*>(literalValue));

    _ASSERT(false);
    return 0.0;
}


// Evaluates an FdoLiteralValue as a string
const wchar_t* ExpressionHelper::GetAsString(FdoLiteralValue* literalValue)
{
    if (literalValue && literalValue->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::GetAsString(static_cast<FdoDataValue*>(literalValue));

    _ASSERT(false);
    return NULL;
}


// Evaluates an FdoDataValue as a boolean
bool ExpressionHelper::GetAsBoolean(FdoDataValue* dataValue)
{
    switch (dataValue->GetDataType())
    {
        case FdoDataType_Boolean:
            return static_cast<FdoBooleanValue*>(dataValue)->GetBoolean();

        case FdoDataType_Byte:
            return static_cast<FdoByteValue*>(dataValue)->GetByte() != 0;

        case FdoDataType_Int16:
            return static_cast<FdoInt16Value*>(dataValue)->GetInt16() != 0;

        case FdoDataType_Int32:
            return static_cast<FdoInt32Value*>(dataValue)->GetInt32() != 0;

        case FdoDataType_Int64:
            return static_cast<FdoInt64Value*>(dataValue)->GetInt64() != 0;

        case FdoDataType_Single:
            return static_cast<FdoSingleValue*>(dataValue)->GetSingle() != 0.0f;

        case FdoDataType_Double:
            return static_cast<FdoDoubleValue*>(dataValue)->GetDouble() != 0.0;

        case FdoDataType_Decimal:
            return static_cast<FdoDecimalValue*>(dataValue)->GetDecimal() != 0.0;

        case FdoDataType_String:
        {
            FdoString* value = static_cast<FdoStringValue*>(dataValue)->GetString();
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
    FdoDataType dt = dataValue->GetDataType();
    switch (dt)
    {
        case FdoDataType_Byte:
            return (int)static_cast<FdoByteValue*>(dataValue)->GetByte();

        case FdoDataType_Int16:
            return (int)static_cast<FdoInt16Value*>(dataValue)->GetInt16();

        case FdoDataType_Int32:
            return static_cast<FdoInt32Value*>(dataValue)->GetInt32();

        case FdoDataType_Int64:
            return (int)static_cast<FdoInt64Value*>(dataValue)->GetInt64();

        case FdoDataType_Single:
            return (int)static_cast<FdoSingleValue*>(dataValue)->GetSingle();

        case FdoDataType_Double:
            return (int)static_cast<FdoDoubleValue*>(dataValue)->GetDouble();

        case FdoDataType_Decimal:
            return (int)static_cast<FdoDecimalValue*>(dataValue)->GetDecimal();

        case FdoDataType_String:
        {
            FdoString* value = static_cast<FdoStringValue*>(dataValue)->GetString();

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
            return (double)static_cast<FdoByteValue*>(dataValue)->GetByte();

        case FdoDataType_Int16:
            return (double)static_cast<FdoInt16Value*>(dataValue)->GetInt16();

        case FdoDataType_Int32:
            return (double)static_cast<FdoInt32Value*>(dataValue)->GetInt32();

        case FdoDataType_Int64:
            return (double)static_cast<FdoInt64Value*>(dataValue)->GetInt64();

        case FdoDataType_Single:
            return (double)static_cast<FdoSingleValue*>(dataValue)->GetSingle();

        case FdoDataType_Double:
            return static_cast<FdoDoubleValue*>(dataValue)->GetDouble();

        case FdoDataType_Decimal:
            return static_cast<FdoDecimalValue*>(dataValue)->GetDecimal();

        case FdoDataType_String:
        {
            FdoString* value = static_cast<FdoStringValue*>(dataValue)->GetString();

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
                _ASSERT(strlen(oldloc) < 63);
                char memoldloc[64];
                strncpy(memoldloc, oldloc, 64);
                memoldloc[63] = 0;

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
    if (dataValue->IsNull())
        return NULL;

    if (dataValue->GetDataType() == FdoDataType_String)
        return static_cast<FdoStringValue*>(dataValue)->GetString();

    return dataValue->ToString();
}


// Compares two FdoLiteralValue objects.
// * returns -1 if the first value is less than the second value
// * returns  0 if the first value equals the second value
// * returns +1 if the first value is greater than the second value
// * returns -2 if there was an error comparing the types
int ExpressionHelper::Compare(FdoLiteralValue* literalValue1, FdoLiteralValue* literalValue2)
{
    if (literalValue1 && literalValue1->GetLiteralValueType() == FdoLiteralValueType_Data &&
        literalValue2 && literalValue2->GetLiteralValueType() == FdoLiteralValueType_Data)
        return ExpressionHelper::Compare(static_cast<FdoDataValue*>(literalValue1), static_cast<FdoDataValue*>(literalValue2));

    // not yet handled
    _ASSERT(false);
    return -2;
}


// Compares two FdoDataValue objects.
// * returns -1 if the first value is less than the second value
// * returns  0 if the first value equals the second value
// * returns +1 if the first value is greater than the second value
// * returns -2 if there was an error comparing the types
int ExpressionHelper::Compare(FdoDataValue* dataValue1, FdoDataValue* dataValue2)
{
    FdoDataType dataType1 = dataValue1->GetDataType();
    FdoDataType dataType2 = dataValue2->GetDataType();

    if (dataType1 == dataType2)
    {
        switch (dataType1)
        {
            case FdoDataType_Boolean:
            {
                bool val1 = static_cast<FdoBooleanValue*>(dataValue1)->GetBoolean();
                bool val2 = static_cast<FdoBooleanValue*>(dataValue1)->GetBoolean();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Byte:
            {
                FdoByte val1 = static_cast<FdoByteValue*>(dataValue1)->GetByte();
                FdoByte val2 = static_cast<FdoByteValue*>(dataValue2)->GetByte();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Int16:
            {
                FdoInt16 val1 = static_cast<FdoInt16Value*>(dataValue1)->GetInt16();
                FdoInt16 val2 = static_cast<FdoInt16Value*>(dataValue2)->GetInt16();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Int32:
            {
                FdoInt32 val1 = static_cast<FdoInt32Value*>(dataValue1)->GetInt32();
                FdoInt32 val2 = static_cast<FdoInt32Value*>(dataValue2)->GetInt32();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Int64:
            {
                FdoInt64 val1 = static_cast<FdoInt64Value*>(dataValue1)->GetInt64();
                FdoInt64 val2 = static_cast<FdoInt64Value*>(dataValue2)->GetInt64();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Single:
            {
                float val1 = static_cast<FdoSingleValue*>(dataValue1)->GetSingle();
                float val2 = static_cast<FdoSingleValue*>(dataValue2)->GetSingle();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Double:
            {
                double val1 = static_cast<FdoDoubleValue*>(dataValue1)->GetDouble();
                double val2 = static_cast<FdoDoubleValue*>(dataValue2)->GetDouble();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_Decimal:
            {
                double val1 = static_cast<FdoDecimalValue*>(dataValue1)->GetDecimal();
                double val2 = static_cast<FdoDecimalValue*>(dataValue2)->GetDecimal();
                if (val1 < val2)
                    return -1;
                else if (val1 > val2)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_String:
            {
                FdoString* val1 = static_cast<FdoStringValue*>(dataValue1)->GetString();
                FdoString* val2 = static_cast<FdoStringValue*>(dataValue2)->GetString();
                int ret = wcscmp(val1, val2);
                if (ret < 0)
                    return -1;
                else if (ret > 0)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_DateTime:
            {
                FdoDateTime val1 = static_cast<FdoDateTimeValue*>(dataValue1)->GetDateTime();
                FdoDateTime val2 = static_cast<FdoDateTimeValue*>(dataValue2)->GetDateTime();

                // check the years
                if (val1.year < val2.year)
                    return -1;
                else if (val1.year > val2.year)
                    return 1;

                // years are the same - now check the months
                if (val1.month < val2.month)
                    return -1;
                else if (val1.month > val2.month)
                    return 1;

                // years/months are the same - now check the days
                if (val1.day < val2.day)
                    return -1;
                else if (val1.day > val2.day)
                    return 1;

                // years/months/days are the same - now check the hours
                if (val1.hour < val2.hour)
                    return -1;
                else if (val1.hour > val2.hour)
                    return 1;

                // years/months/days/hours are the same - now check the minutes
                if (val1.minute < val2.minute)
                    return -1;
                else if (val1.minute > val2.minute)
                    return 1;

                // years/months/days/hours/minutes are the same - now check the seconds
                if (val1.seconds < val2.seconds)
                    return -1;
                else if (val1.seconds > val2.seconds)
                    return 1;
                else
                    return 0;
            }

            case FdoDataType_BLOB:
            case FdoDataType_CLOB:
            default:
                // unsupported data type
                _ASSERT(false);
        }

        return -2;
    }

    // data types are unequal - convert to string and compare them
    const wchar_t* val1 = ExpressionHelper::GetAsString(dataValue1);
    const wchar_t* val2 = ExpressionHelper::GetAsString(dataValue2);
    if (val1 == NULL || val2 == NULL)
        return -2;

    int ret = wcscmp(val1, val2);
    if (ret < 0)
        return -1;
    else if (ret > 0)
        return 1;
    else
        return 0;
}

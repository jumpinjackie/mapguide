//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "FilterExecutor.h"

#include "DataValuePool.h"
#include "Int64Value.h"
#include "DoubleValue.h"
#include "StringValue.h"
#include "BooleanValue.h"
#include "DateTimeValue.h"
#include "KeyEncode.h"
#include "UnicodeString.h"

#include <wctype.h>


RS_FilterExecutor::RS_FilterExecutor(RS_FeatureReader* featureReader)
{
    m_reader = featureReader;

    m_pPool = new DataValuePool();
    m_keyEncode = new KeyEncode();
}

RS_FilterExecutor::~RS_FilterExecutor()
{
    while (!m_retvals.empty())
        delete m_retvals.pop();

    Reset(); //clean out feature property cache

    delete m_pPool;
    delete m_keyEncode;
}

RS_FilterExecutor* RS_FilterExecutor::Create(RS_FeatureReader* featureReader)
{
    return new RS_FilterExecutor(featureReader);
}


void RS_FilterExecutor::Dispose()
{
    delete this;
}


void RS_FilterExecutor::SetMapLayerInfo(const RS_String& session,
                                        const RS_String& mapName,
                                        const RS_String& layerID,
                                        const RS_String& featureClass)
{
    m_session = session;
    m_mapName = mapName;
    m_layerID = layerID;
    m_featCls = featureClass;
}


//cleans up any cached feature attributes
void RS_FilterExecutor::Reset()
{
    for (std::vector<std::pair<void*, DataValue*> >::iterator iter = m_hPropCache.begin();
           iter != m_hPropCache.end(); iter++)
    {
        if (iter->second)
        {
            m_pPool->RelinquishDataValue(iter->second);
            iter->second = (DataValue*)NULL;
        }
    }

    //also clean out the eval stack
    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());
}


//returns the result of a query execution
//A filter must be processed with this RS_FilterExecutor in
//order to have a valid result value
bool RS_FilterExecutor::GetResult()
{
    //only one DataValue should be left on the evaluation stack after
    //executing a query
    _ASSERT(m_retvals.size() == 1);

    bool ret = m_retvals.top()->GetAsBoolean();

    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());

    return ret;
}

bool RS_FilterExecutor::GetBooleanResult()
{
    //only one DataValue should be left on the evaluation stack after
    //executing a query
    _ASSERT(m_retvals.size() == 1);

    bool ret = m_retvals.top()->GetAsBoolean();

    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());

    return ret;
}

double RS_FilterExecutor::GetDoubleResult()
{
    //only one DataValue should be left on the evaluation stack after
    //executing a query
    _ASSERT(m_retvals.size() == 1);

    double ret = m_retvals.top()->GetAsDouble();

    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());

    return ret;
}

FdoInt64 RS_FilterExecutor::GetInt64Result()
{
    //only one DataValue should be left on the evaluation stack after
    //executing a query
    _ASSERT(m_retvals.size() == 1);

    FdoInt64 ret = m_retvals.top()->GetAsInt64();

    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());

    return ret;
}


wchar_t* RS_FilterExecutor::GetStringResult()
{
    //only one DataValue should be left on the evaluation stack after
    //executing a query
    _ASSERT(m_retvals.size() == 1);

    wchar_t* ret = m_retvals.top()->GetAsString();

    if (ret == NULL)
        return NULL;

    while (!m_retvals.empty())
        m_pPool->RelinquishDataValue(m_retvals.pop());

    wchar_t* retcpy = new wchar_t[wcslen(ret)+1];
    wcscpy(retcpy, ret);

    return retcpy;
}



void RS_FilterExecutor::ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> left = filter.GetLeftOperand();
    FdoPtr<FdoFilter> right = filter.GetRightOperand();

    //evaluate left hand side
    left->Process(this);

    //get Process() results from return value stack
    DataValue* argLeft = m_retvals.pop();

    //Optimize binary logic by looking at the value of the left operand and see
    //if we have to evaluate the right one also
    if (argLeft->GetAsBoolean() && filter.GetOperation() == FdoBinaryLogicalOperations_Or)
    {
        m_retvals.push(m_pPool->ObtainBooleanValue(true));
        m_pPool->RelinquishDataValue(argLeft);
        return;
    }
    else if (!argLeft->GetAsBoolean() && filter.GetOperation() == FdoBinaryLogicalOperations_And)
    {
        m_retvals.push(m_pPool->ObtainBooleanValue(false));
        m_pPool->RelinquishDataValue(argLeft);
        return;
    }

    //evaluate right hand side
    right->Process(this);
    DataValue* argRight = m_retvals.pop();

    switch (filter.GetOperation())
    {
    case FdoBinaryLogicalOperations_And:
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->GetAsBoolean() && argRight->GetAsBoolean()));
        break;
    case FdoBinaryLogicalOperations_Or :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->GetAsBoolean() || argRight->GetAsBoolean()));
        break;
    default:
        throw FdoException::Create(L"Invalid logical operation type");break;
    }

    m_pPool->RelinquishDataValue(argLeft);
    m_pPool->RelinquishDataValue(argRight);
}

void RS_FilterExecutor::ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter)
{
    FdoPtr<FdoFilter> right = filter.GetOperand();

    //evaluate right hand side
    right->Process(this);

    //get Process() results from return value stack
    DataValue* argRight = m_retvals.pop();

    switch (filter.GetOperation())
    {
    case FdoUnaryLogicalOperations_Not:
        m_retvals.push(m_pPool->ObtainBooleanValue(!argRight->GetAsBoolean()));
        break;
    default:
        throw FdoException::Create(L"Invalid logical operation type");break;
    }

    m_pPool->RelinquishDataValue(argRight);

}

void RS_FilterExecutor::ProcessComparisonCondition(FdoComparisonCondition& filter)
{
    FdoPtr<FdoExpression> left = filter.GetLeftExpression();
    FdoPtr<FdoExpression> right = filter.GetRightExpression();

    right->Process(this);
    left->Process(this);

    //get Process() results from return value stack
    DataValue* argLeft = m_retvals.pop();
    DataValue* argRight = m_retvals.pop();

    switch (filter.GetOperation())
    {
    case FdoComparisonOperations_EqualTo :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsEqualTo(*argRight)));
        break;
    case FdoComparisonOperations_NotEqualTo :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsNotEqualTo(*argRight)));
        break;
    case FdoComparisonOperations_GreaterThan :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsGreaterThan(*argRight)));
        break;
    case FdoComparisonOperations_GreaterThanOrEqualTo :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsGreaterThanOrEqualTo(*argRight)));
        break;
    case FdoComparisonOperations_LessThan :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsLessThan(*argRight)));
        break;
    case FdoComparisonOperations_LessThanOrEqualTo :
        m_retvals.push(m_pPool->ObtainBooleanValue(argLeft->IsLessThanOrEqualTo(*argRight)));
        break;
    case FdoComparisonOperations_Like : m_retvals.push(m_pPool->ObtainBooleanValue(MatchesHere(argRight->GetAsString(), argLeft->GetAsString())));
        break;
    default: throw FdoException::Create(L"Invalid comparison operation type");
    }

    m_pPool->RelinquishDataValue(argRight);
    m_pPool->RelinquishDataValue(argLeft);
}

void RS_FilterExecutor::ProcessInCondition(FdoInCondition& filter)
{
    //first get the value of the property we are checking
    FdoPtr<FdoIdentifier> prop = filter.GetPropertyName();
    ProcessIdentifier(*(prop.p));

    //get the property value off the stack
    DataValue* argLeft = m_retvals.pop();

    FdoPtr<FdoValueExpressionCollection> vals = filter.GetValues();

    bool result = false;

    //see if the value collection contains the property value
    for (int i=0; i<vals->GetCount(); i++)
    {
        FdoPtr<FdoValueExpression> expr = vals->GetItem(i);
        expr->Process(this);

        DataValue* argRight = m_retvals.pop();

        if (argLeft->IsEqualTo(*argRight))
            result = true;

        delete argRight;

        if (result)
            break;
    }

    //if prop val is not IN the val collection, push false on the stack
    m_retvals.push(m_pPool->ObtainBooleanValue(result));

    delete argLeft;
}

void RS_FilterExecutor::ProcessNullCondition(FdoNullCondition& filter)
{
    //first get the value of the property we are checking
    FdoPtr<FdoIdentifier> prop = filter.GetPropertyName();

    bool isnull = m_reader->IsNull(prop->GetName());

    m_retvals.push(m_pPool->ObtainBooleanValue(isnull));
}

void RS_FilterExecutor::ProcessSpatialCondition(FdoSpatialCondition& /*filter*/)
{
    throw FdoException::Create(L"Spatial conditions not supported");
}

void RS_FilterExecutor::ProcessDistanceCondition(FdoDistanceCondition& /*filter*/)
{
    throw FdoException::Create(L"DISTANCE condition not supported");
}

void RS_FilterExecutor::ProcessBinaryExpression(FdoBinaryExpression& expr)
{
    FdoPtr<FdoExpression> left = expr.GetLeftExpression();
    FdoPtr<FdoExpression> right = expr.GetRightExpression();

    right->Process(this);
    left->Process(this);

    //get Process() results from return value stack
    DataValue* argLeft = m_retvals.pop();
    DataValue* argRight = m_retvals.pop();

    switch (expr.GetOperation())
    {
    case FdoBinaryOperations_Add:
        m_retvals.push(argLeft->Add(*argRight, m_pPool));
        break;
    case FdoBinaryOperations_Multiply:
        m_retvals.push(argLeft->Multiply(*argRight, m_pPool));
        break;
    case FdoBinaryOperations_Subtract:
        m_retvals.push(argLeft->Subtract(*argRight, m_pPool));
        break;
    case FdoBinaryOperations_Divide:
        m_retvals.push(argLeft->Divide(*argRight, m_pPool));
        break;
    default:
        throw FdoException::Create(L"Unknown binary operation");
    }

    m_pPool->RelinquishDataValue(argLeft);
    m_pPool->RelinquishDataValue(argRight);
}

void RS_FilterExecutor::ProcessUnaryExpression(FdoUnaryExpression& expr)
{
    FdoPtr<FdoExpression> right = expr.GetExpression();

    right->Process(this);

    DataValue* argRight = m_retvals.pop();

    switch (expr.GetOperation())
    {
    case FdoUnaryOperations_Negate: m_retvals.push(argRight->Negate(m_pPool));
    default : throw FdoException::Create(L"Unknown unary operation");
    }

}

void RS_FilterExecutor::ProcessFunction(FdoFunction& expr)
{
    FdoString* name = expr.GetName();

    if (_wcsnicmp(name, L"ARGB", 4) == 0)
    {
        //color function
        ExecuteARGB(expr);
    }
    else if (_wcsnicmp(name, L"CONCAT", 6) == 0)
    {
        FdoPtr<FdoExpressionCollection> args = expr.GetArguments();

        if (args->GetCount() != 2)
            throw FdoException::Create(L"Invalid number of arguments.");

        FdoPtr<FdoExpression> left = args->GetItem(0);
        left->Process(this);

        FdoPtr<FdoExpression> right = args->GetItem(1);
        right->Process(this);

        DataValue* argRight = m_retvals.pop();

        DataValue* argLeft = m_retvals.pop();

        size_t len = wcslen(argLeft->GetAsString()) + wcslen(argRight->GetAsString()) + 1;

        wchar_t* res = new wchar_t[len];
        res[0] = 0;

        wcscat(res, argLeft->GetAsString());
        wcscat(res, argRight->GetAsString());

        m_pPool->RelinquishDataValue(argLeft);
        m_pPool->RelinquishDataValue(argRight);

        m_retvals.push(m_pPool->ObtainStringValue(res, true));
    }
    else if (wcscmp(name, L"Length") == 0)//From now on, functions need to be case sensitive for FDO compatibility
    {
        FdoPtr<FdoExpressionCollection> args = expr.GetArguments();

        if (args->GetCount() != 1)
            throw FdoException::Create(L"Invalid number of arguments.");

        FdoPtr<FdoExpression> arg = args->GetItem(0);
        arg->Process(this);

        DataValue* argVal = m_retvals.pop();

        size_t len = wcslen(argVal->GetAsString());

        m_pPool->RelinquishDataValue(argVal);

        m_retvals.push(m_pPool->ObtainInt64Value(len));
    }
    else if (_wcsnicmp(name, L"SESSION", 7) == 0)
    {
        ExecuteSession(expr);
    }
    else if (_wcsnicmp(name, L"MAPNAME", 7) == 0)
    {
        ExecuteMapName(expr);
    }
    else if (_wcsnicmp(name, L"LAYERID", 7) == 0)
    {
        ExecuteLayerID(expr);
    }
    else if (_wcsnicmp(name, L"FEATURECLASS", 12) == 0)
    {
        ExecuteFeatureClass(expr);
    }
    else if (_wcsnicmp(name, L"FEATUREID", 9) == 0)
    {
        ExecuteFeatureID(expr);
    }
    else if (_wcsnicmp(name, L"URLENCODE", 9) == 0)
    {
        ExecuteUrlEncode(expr);
    }
    else if (_wcsnicmp(name, L"DECAP", 5) == 0)
    {
        FdoPtr<FdoExpressionCollection> args = expr.GetArguments();

        if (args->GetCount() != 1)
            throw FdoException::Create(L"Incorrect number of arguments for function DECAP");

        FdoPtr<FdoExpression> val = args->GetItem(0);
        val->Process(this);

        DataValue* dvVal = m_retvals.pop();

        wchar_t* sval = dvVal->GetAsString();
        size_t len = wcslen(sval) + 1;

        if (len > 1)
        {
            wchar_t* nval = new wchar_t[len];

            wchar_t* dst = nval;
            for (wchar_t *src=sval; *src;)
            {
                if (src == sval || (*(src-1) == L' '))
                    *dst++ = towupper(*src++);
                else
                    *dst++ = towlower(*src++);
            }

            *dst = 0;

            m_pPool->RelinquishDataValue(dvVal);

            m_retvals.push(m_pPool->ObtainStringValue(nval, true));
        }
        else
        {
            m_retvals.push(dvVal);
        }
    }
    else
    {
        throw FdoException::Create(L"Function not supported");
    }
}

void RS_FilterExecutor::ProcessIdentifier(FdoIdentifier& expr)
{
    FdoString* name = expr.GetName();

    //did we cache the last attribute get?
    DataValue* cached = NULL;
    unsigned int index;
    for (index=0; index<m_hPropCache.size(); index++)
    {
        if (wcscmp((const wchar_t*)m_hPropCache[index].first, name) == 0)
        {
            cached = m_hPropCache[index].second;
            break;
        }
    }

    if (cached)
    {
         m_retvals.push(cached->Clone(m_pPool));
         return;
    }
    else
    {
        //otherwise we need to ask the feature reader for the value
        if (m_reader == NULL)
            throw FdoException::Create(L"No reader available!");

        FdoDataType dataType = (FdoDataType) m_reader->GetPropertyType(name);

        DataValue* dv = NULL;

        switch ((int)dataType)
        {
            case FdoDataType_Boolean :
                dv = m_pPool->ObtainBooleanValue(m_reader->GetBoolean(name));
                break;
            case FdoDataType_Byte :
                dv = m_pPool->ObtainInt64Value(m_reader->GetByte(name));
                break;
            case FdoDataType_DateTime :
                dv = m_pPool->ObtainDateTimeValue(m_reader->GetDateTime(name));
                break;
            case FdoDataType_Decimal :
                dv = m_pPool->ObtainDoubleValue(m_reader->GetDouble(name));
                break;
            case FdoDataType_Double :
                dv = m_pPool->ObtainDoubleValue(m_reader->GetDouble(name));
                break;
            case FdoDataType_Int16 :
                dv = m_pPool->ObtainInt64Value(m_reader->GetInt16(name));
                break;
            case FdoDataType_Int32 :
                dv = m_pPool->ObtainInt64Value(m_reader->GetInt32(name));
                break;
            case FdoDataType_Int64 :
                dv = m_pPool->ObtainInt64Value(m_reader->GetInt64(name));
                break;
            case FdoDataType_Single :
                dv = m_pPool->ObtainDoubleValue(m_reader->GetSingle(name));
                break;
            case FdoDataType_String :
                dv = m_pPool->ObtainStringValue((wchar_t*)m_reader->GetString(name), false);
                break;
            case FdoDataType_BLOB :
                throw FdoException::Create(L"BLOB property value encountered!");
                break;
            case FdoDataType_CLOB :
                throw FdoException::Create(L"CLOB property value encountered!");
                break;
            case -1 :
                throw FdoException::Create(L"Geometry property value encountered!");
                break;
                //TODO: GEOMETRY! yay!
            default: throw FdoException::Create(L"Unknown data type encountered!");
        }

        //now cache the last property get
        if (index < m_hPropCache.size())
            m_hPropCache[index].second = dv->Clone(m_pPool);
        else
            m_hPropCache.push_back(std::pair<void*, DataValue*>((void*)name, dv->Clone(m_pPool)));

        m_retvals.push(dv);
    }

}

void RS_FilterExecutor::ProcessParameter(FdoParameter& /*expr*/)
{
    throw FdoException::Create(L"Parameters not supported");
}

void RS_FilterExecutor::ProcessBooleanValue(FdoBooleanValue& expr)
{
    m_retvals.push(m_pPool->ObtainBooleanValue(expr.GetBoolean()));
}

void RS_FilterExecutor::ProcessByteValue(FdoByteValue& expr)
{
    m_retvals.push(m_pPool->ObtainInt64Value(expr.GetByte()));
}

void RS_FilterExecutor::ProcessDateTimeValue(FdoDateTimeValue& expr)
{
    m_retvals.push(m_pPool->ObtainDateTimeValue(expr.GetDateTime()));
}

void RS_FilterExecutor::ProcessDecimalValue(FdoDecimalValue& expr)
{
    m_retvals.push(m_pPool->ObtainDoubleValue(expr.GetDecimal()));
}

void RS_FilterExecutor::ProcessDoubleValue(FdoDoubleValue& expr)
{
    m_retvals.push(m_pPool->ObtainDoubleValue(expr.GetDouble()));
}

void RS_FilterExecutor::ProcessInt16Value(FdoInt16Value& expr)
{
    m_retvals.push(m_pPool->ObtainInt64Value(expr.GetInt16()));
}

void RS_FilterExecutor::ProcessInt32Value(FdoInt32Value& expr)
{
    m_retvals.push(m_pPool->ObtainInt64Value(expr.GetInt32()));
}

void RS_FilterExecutor::ProcessInt64Value(FdoInt64Value& expr)
{
    m_retvals.push(m_pPool->ObtainInt64Value(expr.GetInt64()));
}

void RS_FilterExecutor::ProcessSingleValue(FdoSingleValue& expr)
{
    m_retvals.push(m_pPool->ObtainDoubleValue(expr.GetSingle()));
}

void RS_FilterExecutor::ProcessStringValue(FdoStringValue& expr)
{
    m_retvals.push(m_pPool->ObtainStringValue((wchar_t*)expr.GetString()));
}

void RS_FilterExecutor::ProcessBLOBValue(FdoBLOBValue& /*expr*/)
{
    throw FdoException::Create(L"BLOBs not supported");
}

void RS_FilterExecutor::ProcessCLOBValue(FdoCLOBValue& /*expr*/)
{
    throw FdoException::Create(L"CLOBs not supported");
}

void RS_FilterExecutor::ProcessGeometryValue(FdoGeometryValue& /*expr*/)
{
    throw FdoException::Create(L"Geometry value not supported");
}

void RS_FilterExecutor::ProcessComputedIdentifier(FdoComputedIdentifier &)
{
    throw FdoException::Create(L"Computed identifier not supported");
}


//Matches strings according to the rules of the LIKE condition
// Wildcard characters supported in the pattern are:
//
// %    Any string of zero or more characters
// _    Any single character
// []   Any single character within the specified range ([a-f]) or set
// ([abcdef]).
// [^]  Any single character not within the specified range ([^a-f]) or
// set ([^abcdef]).
bool RS_FilterExecutor::MatchesHere(wchar_t* pattern, wchar_t* src)
{
    bool bMatches = false;

    if (*pattern == L'\0')
    {
        if (wcslen(src))
        {
            bMatches = false;
        }
        else
        {
            bMatches = true;
        }
    }
    else if (*pattern == L'%')
    {
        bMatches = MatchPercent(pattern+1, src);
    }
    else if (*pattern == L'[')
    {
        bMatches = MatchBracket(pattern+1, src);
    }
    else if (*src != L'\0' && ((*pattern == L'_') || _wcsnicmp(pattern, src, 1) == 0))
    {
        bMatches = MatchesHere(pattern+1, src+1);
    }

    return bMatches;
}

#ifdef _WIN32
#pragma warning(disable : 4127)
#endif

// %    Any string of zero or more characters
bool RS_FilterExecutor::MatchPercent(wchar_t* pattern, wchar_t* src)
{
    bool bMatches = false;

    while (true)
    {
        if (MatchesHere(pattern, src))
        {
            bMatches = true;
            break;
        }
        if (*src == L'\0')
        {
            break;
        }
        src++;
    }

    return bMatches;
}

#ifdef _WIN32
#pragma warning(default : 4127)
#endif

// []   Any single character within the specified range ([a-f]) or set
// ([abcdef]).
// [^]  Any single character not within the specified range ([^a-f]) or
// set ([^abcdef]).
bool RS_FilterExecutor::MatchBracket(wchar_t* pattern, wchar_t* src)
{
    bool bMatches = false;
    bool reverse = false;

    //invalid case, unclosed bracket
    if (*pattern == L'\0')
        bMatches = false;

    if (*pattern == L'^')
    {
        reverse = true;
        pattern++;
    }

    //case of [] -- not a useful case
    if (*pattern == L']')
    {
        if (*src == L'\0')
            bMatches = true;

        if (reverse)
            bMatches = MatchesHere(pattern+1, src+1);

        bMatches = false;
    }

    wchar_t start = *pattern++;

    //case of [a-b] or [^a-b]
    if (*pattern == L'-')
    {
        //go past - character and get range end character and go past range end character
        pattern++;
        wchar_t end = *pattern++;

        _ASSERT(end != NULL);

        if ((*src >= start && *src <= end) ^ reverse)
        {
            bMatches = MatchesHere(pattern+1, src+1);
        }
        else
            bMatches = false;
    }

    //case of [abcdef] ot [^abcdef]
    std::vector<wchar_t> set;
    set.push_back(start);

    do
    {
        set.push_back(*pattern++);
    }
    while (*pattern != L']');

    wchar_t srcchar = *src;
    bool isthere = false;

    //look for current character in the set
    for (unsigned i=0; i<set.size(); i++)
    {
        wchar_t current = set[i];
        if (_wcsnicmp(&current, &srcchar, 1) == 0)
        {
            isthere = true;
            break;
        }
    }

    if (isthere ^ reverse)
        bMatches = MatchesHere(pattern+1, src+1);
    else
        bMatches = false;


    return bMatches;
}

void RS_FilterExecutor::ExecuteARGB(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    int count = args->GetCount();

    //make sure we have 4 arguments. ARGB
    if (count != 4)
        throw FdoException::Create(L"Incorrect number of arguments for function ARGB");

    //evaluate each argument
    for (int i=count-1; i>=0; i--)
    {
        FdoPtr<FdoExpression> arg = args->GetItem(i);
        arg->Process(this);
    }

    //A
    DataValue* dvAlpha = m_retvals.pop();
    FdoInt64 alpha = (int)dvAlpha->GetAsInt64();
    m_pPool->RelinquishDataValue(dvAlpha);

    //R
    DataValue* dvRed = m_retvals.pop();
    FdoInt64 red = dvRed->GetAsInt64();
    m_pPool->RelinquishDataValue(dvRed);

    //G
    DataValue* dvGreen = m_retvals.pop();
    FdoInt64 green = dvGreen->GetAsInt64();
    m_pPool->RelinquishDataValue(dvGreen);

    //B
    DataValue* dvBlue = m_retvals.pop();
    FdoInt64 blue = dvBlue->GetAsInt64();
    m_pPool->RelinquishDataValue(dvBlue);

    //clean up
    red &= 0xFF;
    green &= 0xFF;
    blue &= 0xFF;
    alpha &= 0xFF;

    FdoInt64 color = (alpha << 24) | (red << 16) | (green << 8) | blue;

    m_retvals.push(m_pPool->ObtainInt64Value(color));
}


void RS_FilterExecutor::ExecuteSession(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have zero arguments
    if (args->GetCount() != 0)
        throw FdoException::Create(L"Incorrect number of arguments for function SESSION");

    wchar_t* res = new wchar_t[m_session.length()+1];
    wcscpy(res, m_session.c_str());

    m_retvals.push(m_pPool->ObtainStringValue(res, true));
}


void RS_FilterExecutor::ExecuteMapName(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have zero arguments
    if (args->GetCount() != 0)
        throw FdoException::Create(L"Incorrect number of arguments for function MAPNAME");

    wchar_t* res = new wchar_t[m_mapName.length()+1];
    wcscpy(res, m_mapName.c_str());

    m_retvals.push(m_pPool->ObtainStringValue(res, true));
}


void RS_FilterExecutor::ExecuteLayerID(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have zero arguments
    if (args->GetCount() != 0)
        throw FdoException::Create(L"Incorrect number of arguments for function LAYERID");

    wchar_t* res = new wchar_t[m_layerID.length()+1];
    wcscpy(res, m_layerID.c_str());

    m_retvals.push(m_pPool->ObtainStringValue(res, true));
}


void RS_FilterExecutor::ExecuteFeatureClass(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have zero arguments
    if (args->GetCount() != 0)
        throw FdoException::Create(L"Incorrect number of arguments for function FEATURECLASS");

    wchar_t* res = new wchar_t[m_featCls.length()+1];
    wcscpy(res, m_featCls.c_str());

    m_retvals.push(m_pPool->ObtainStringValue(res, true));
}


void RS_FilterExecutor::ExecuteFeatureID(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have zero arguments
    if (args->GetCount() != 0)
        throw FdoException::Create(L"Incorrect number of arguments for function FEATUREID");

    // generate base 64 id
    const char* base64 = m_keyEncode->EncodeKey(m_reader);
    size_t len = strlen(base64);

    // convert to a wide string
    wchar_t* res = new wchar_t[len+1];
    for (size_t k=0; k<len+1; k++)
        res[k] = (wchar_t)base64[k];

    m_retvals.push(m_pPool->ObtainStringValue(res, true));
}


// URL encodes a string.
//
// From the HTTP 1.1 specification:
//
//   Many HTTP/1.1 header field values consist of words separated by LWS
//   or special characters. These special characters MUST be in a quoted
//   string to be used within a parameter value (as defined in section 3.6).
//
//       token          = 1*<any CHAR except CTLs or separators>
//       separators     = "(" | ")" | "<" | ">" | "@"
//                      | "," | ";" | ":" | "\" | <">
//                      | "/" | "[" | "]" | "?" | "="
//                      | "{" | "}" | SP | HT
//
//
// 3.6 Transfer Codings
//
//   ...
//
//   Parameters are in  the form of attribute/value pairs.
//
//       parameter    = attribute "=" value
//       attribute    = token
//       value        = token | quoted-string
//
void RS_FilterExecutor::ExecuteUrlEncode(FdoFunction& function)
{
    FdoPtr<FdoExpressionCollection> args = function.GetArguments();

    // make sure we have one argument
    if (args->GetCount() != 1)
        throw FdoException::Create(L"Incorrect number of arguments for function URLENCODE");

    // evaluate the argument
    FdoPtr<FdoExpression> arg = args->GetItem(0);
    arg->Process(this);

    // get the string to encode
    DataValue* dvVal = m_retvals.pop();
    wchar_t* sval = dvVal->GetAsString();

    size_t len = wcslen(sval);
    if (len >= 0)
    {
        // must first UTF8 encode
        string sutf8;
        const XMLCh* srcX = W2X(sval);
        UnicodeString::UTF16toUTF8(srcX, sutf8);
        size_t utf8lenbytes = sutf8.length();

        // now URL encode the result
        size_t urllenbytes = 3*utf8lenbytes + 1;
        char* sUrl = (char*)alloca(urllenbytes * sizeof(char));
        memset(sUrl, 0, urllenbytes * sizeof(char));

        size_t i, j;
        for (i = j = 0; i < utf8lenbytes; ++i)
        {
            char chr = sutf8[i];
            if ((chr <= 0x20) || // a space or anything below it in value
                (chr == 0x21) || // a ! (exclamation)
                (chr == 0x22) || // a " (quotation mark)
                (chr == 0x23) || // a # (hash)
                (chr == 0x24) || // a $ (dollar)
                (chr == 0x25) || // a % (percentage)
                (chr == 0x26) || // a & (ampersand)
                (chr == 0x27) || // a ' (apostrophe)
                (chr == 0x28) || // a (
                (chr == 0x29) || // a )
//              (chr == 0x2A) || // a * (asterisk)
                (chr == 0x2B) || // a + (plus)
                (chr == 0x2C) || // a , (comma)
//              (chr == 0x2D) || // a - (hyphen)
//              (chr == 0x2E) || // a . (period)
                (chr == 0x2F) || // a / (forward slash)
                (chr == 0x3A) || // a : (colon)
                (chr == 0x3B) || // a ; (semi-colon)
                (chr == 0x3C) || // a < (less than)
                (chr == 0x3D) || // a = (equals)
                (chr == 0x3E) || // a > (greater than)
                (chr == 0x3F) || // a ? (question mark)
                (chr == 0x40) || // a @ (at)
                (chr == 0x5B) || // a [
                (chr == 0x5C) || // a \ (backslash)
                (chr == 0x5D) || // a ]
                (chr == 0x5E) || // a ^
//              (chr == 0x5F) || // a _ (underscore)
                (chr == 0x60) || // a `
                (chr == 0x7B) || // a { (opening bracket)
                (chr == 0x7C) || // a | (pipe)
                (chr == 0x7D) || // a } (closing bracket)
                (chr == 0x7E) || // a ~ (tilde)
                (chr == 0x7F) || // a control character
                (chr >= 0x80))   // 8-bit (encoded)
                j += sprintf(&sUrl[j], "%%%2X", (unsigned char)chr);
            else
                sUrl[j++] = chr;
        }

        // finally, convert to a wide string
        wchar_t* res = new wchar_t[j+1];
        for (size_t k=0; k<j+1; k++)
            res[k] = (wchar_t)sUrl[k];

        m_pPool->RelinquishDataValue(dvVal);
        m_retvals.push(m_pPool->ObtainStringValue(res, true));
    }
    else
    {
        m_retvals.push(dvVal);
    }
}

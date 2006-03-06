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

#ifndef FILTEREXECUTOR_H
#define FILTEREXECUTOR_H

#include "RS_FeatureReader.h"

#include <vector>

#include "DataValue.h"
#include "DataValueStack.h"

typedef DataValueStack<DataValue> retval_stack;

//forward declare
class DataValuePool;

///<summary>
///An implementation of FdoIFilterProcessor and FdoIExpressionProcessor which
///evaluates a filter in the context of a given feature, via an FdoIFeatureReader.
///</summary>
//beware: multiple non-virtual inheritance from GisIDisposable!!!
class RS_FilterExecutor : public FdoIFilterProcessor, public FdoIExpressionProcessor
{

public:

    RS_FilterExecutor(RS_FeatureReader* featureReader);
    virtual ~RS_FilterExecutor();

protected:

    void Dispose();

public:

    static RS_FilterExecutor* Create(RS_FeatureReader* featureReader);

public:

    //--------------------------------------------------------------------
    // FdoIFilterProcessor
    //--------------------------------------------------------------------

    virtual void ProcessBinaryLogicalOperator(FdoBinaryLogicalOperator& filter);
    virtual void ProcessUnaryLogicalOperator(FdoUnaryLogicalOperator& filter);
    virtual void ProcessComparisonCondition(FdoComparisonCondition& filter);
    virtual void ProcessInCondition(FdoInCondition& filter);
    virtual void ProcessNullCondition(FdoNullCondition& filter);
    virtual void ProcessSpatialCondition(FdoSpatialCondition& filter);
    virtual void ProcessDistanceCondition(FdoDistanceCondition& filter);

    //--------------------------------------------------------------------
    // FdoIExpressionProcessor
    //--------------------------------------------------------------------

    virtual void ProcessBinaryExpression(FdoBinaryExpression& expr);
    virtual void ProcessUnaryExpression(FdoUnaryExpression& expr);
    virtual void ProcessFunction(FdoFunction& expr);
    virtual void ProcessIdentifier(FdoIdentifier& expr);
    virtual void ProcessParameter(FdoParameter& expr);
    virtual void ProcessBooleanValue(FdoBooleanValue& expr);
    virtual void ProcessByteValue(FdoByteValue& expr);
    virtual void ProcessDateTimeValue(FdoDateTimeValue& expr);
    virtual void ProcessDecimalValue(FdoDecimalValue& expr);
    virtual void ProcessDoubleValue(FdoDoubleValue& expr);
    virtual void ProcessInt16Value(FdoInt16Value& expr);
    virtual void ProcessInt32Value(FdoInt32Value& expr);
    virtual void ProcessInt64Value(FdoInt64Value& expr);
    virtual void ProcessSingleValue(FdoSingleValue& expr);
    virtual void ProcessStringValue(FdoStringValue& expr);
    virtual void ProcessBLOBValue(FdoBLOBValue& expr);
    virtual void ProcessCLOBValue(FdoCLOBValue& expr);
    virtual void ProcessGeometryValue(FdoGeometryValue& expr);
    virtual void ProcessComputedIdentifier(FdoComputedIdentifier &);

public:

    //-----------------------------------------------------------------
    // RS_FilterExecutor
    //-----------------------------------------------------------------

    void Reset();
    bool GetResult();
    bool GetBooleanResult();
    double GetDoubleResult();
    GisInt64 GetInt64Result();
    wchar_t* GetStringResult();

private:

    void ExecuteARGB(FdoFunction& function);

    bool MatchesHere(wchar_t* pattern, wchar_t* src);
    bool MatchPercent(wchar_t* pattern, wchar_t* src);
    bool MatchBracket(wchar_t* pattern, wchar_t* src);

    retval_stack m_retvals;

    RS_FeatureReader* m_reader;

    DataValuePool* m_pPool;

    //cached identifier values
    std::vector<std::pair<void*, DataValue*> > m_hPropCache;
};

#endif

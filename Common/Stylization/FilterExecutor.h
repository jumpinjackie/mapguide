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

#ifndef FILTEREXECUTOR_H_
#define FILTEREXECUTOR_H_

#include "DataValue.h"
#include "DataValueStack.h"
#include "RS_FeatureReader.h"

#include <vector>

typedef DataValueStack<DataValue> retval_stack;

//forward declare
class DataValuePool;
class KeyEncode;

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
    STYLIZATION_API static RS_FilterExecutor* Create(RS_FeatureReader* featureReader);

    void SetMapLayerInfo(const RS_String& session,
                         const RS_String& mapName,
                         const RS_String& layerID,
                         const RS_String& featClass);

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
    virtual void ProcessComputedIdentifier(FdoComputedIdentifier& expr);
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

    //-----------------------------------------------------------------
    // RS_FilterExecutor
    //-----------------------------------------------------------------

    STYLIZATION_API void Reset();
    STYLIZATION_API bool GetResult();
    STYLIZATION_API bool GetBooleanResult();
    STYLIZATION_API double GetDoubleResult();
    STYLIZATION_API FdoInt64 GetInt64Result();
    STYLIZATION_API wchar_t* GetStringResult();

private:
    void ExecuteARGB(FdoFunction& function);
    void ExecuteSession(FdoFunction& function);
    void ExecuteMapName(FdoFunction& function);
    void ExecuteLayerID(FdoFunction& function);
    void ExecuteFeatureClass(FdoFunction& function);
    void ExecuteFeatureID(FdoFunction& function);
    void ExecuteUrlEncode(FdoFunction& function);

    bool MatchesHere(wchar_t* pattern, wchar_t* src);
    bool MatchPercent(wchar_t* pattern, wchar_t* src);
    bool MatchBracket(wchar_t* pattern, wchar_t* src);

    retval_stack m_retvals;

    RS_FeatureReader* m_reader;

    DataValuePool* m_pPool;

    RS_String m_session;
    RS_String m_mapName;
    RS_String m_layerID;
    RS_String m_featCls;

    KeyEncode* m_keyEncode;

    //cached identifier values
    std::vector< std::pair<void*, DataValue*> > m_hPropCache;
};

#endif

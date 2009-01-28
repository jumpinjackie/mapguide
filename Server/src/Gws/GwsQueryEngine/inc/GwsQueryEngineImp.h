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

#ifndef GwsQueryEngineImp_h
#define GwsQueryEngineImp_h

// Uncomment to debug the batch sorted block join
//#define _DEBUG_BATCHSORT_JOIN

#include "GwsQueryEngine.h"

#include "GwsCommonImp.h"
#include "GwsQueryDefinition.h"
#include "GwsMutableFeature.h"
#include "GwsFdoCommand.h"
#include "GwsQuery.h"
#include "GwsFlatFdoReader.h"


///////////////////////////////////////////////////////////////////////////////
struct GwsQueryXml
{
    static wchar_t * xmlGwsQuery;
    static wchar_t * xmlGwsQualifiedClassName;
    static wchar_t * xmlGwsClassFeatureSource;
    static wchar_t * xmlGwsClassSchema;
    static wchar_t * xmlGwsClassName;
    static wchar_t * xmlGwsFilter;
    static wchar_t * xmlGwsFilterStringValue;
    static wchar_t * xmlGwsQueryTypeAttribute;
    static wchar_t * xmlGwsQueryTypeFeature;
    static wchar_t * xmlGwsQueryTypeLeftOuterJoin;
    static wchar_t * xmlGwsQueryTypeEqualJoin;
    static wchar_t * xmlGwsLeftJoinAttributes;
    static wchar_t * xmlGwsRightJoinAttributes;
    static wchar_t * xmlGwsJoinAttibuteNames;
    static wchar_t * xmlGwsQuerySelectList;
    static wchar_t * xmlGwsQuerySelectExpressionList;
    static wchar_t * xmlGwsQuerySelectExpression;
    static wchar_t * xmlGwsQuerySelectExpressionName;
};


///////////////////////////////////////////////////////////////////////////////
namespace GwsQueryUtils
{
    // get tjhe data property value from the reader
    GWS_QUERYENGINE_API FdoDataValue * GetDataPropertyValue (
                                FdoIReader  *   reader,
                                FdoDataType     dataprop,
                                FdoString   *   name);

    // converts property specified by its descriptor into wchar string
    GWS_QUERYENGINE_API void ToString (IGWSFeature * feature, const CGwsPropertyDesc & desc, wchar_t * buff, int len);

    GWS_QUERYENGINE_API bool QueryDefinitionsEqual (IGWSQueryDefinition * qdef1, IGWSQueryDefinition * qdef2, bool excludeSelectList);

    GWS_QUERYENGINE_API
    bool CompareStringCollection(FdoStringCollection* firstString
                                         , FdoStringCollection* secondString);
    GWS_QUERYENGINE_API
    bool CompareIdentifierCollection(FdoIdentifierCollection* firstIdentifiers
                                         , FdoIdentifierCollection* secondIdentifiers);

    template<typename T>
    bool CompareToStringValues(T* firstVal, T* secondVal)
    {
        if(NULL == firstVal && NULL == secondVal) return true;

        if((((NULL == firstVal) && !(NULL == secondVal))
            || (!(NULL == firstVal) && (NULL == secondVal)))
            ||  wcscmp(firstVal->ToString() , secondVal->ToString()) != 0) return false;
        return true;
    }

    GWS_QUERYENGINE_API
    FdoIdentifierCollection* CreateIdentifiersFromStrings(FdoStringCollection* strings);

    GWS_QUERYENGINE_API bool FilterHasCalculatedProperties( FdoFilter* pFilter, FdoIdentifierCollection *pSelectList );
};

#endif  /* GwsQueryEngineImp_h */

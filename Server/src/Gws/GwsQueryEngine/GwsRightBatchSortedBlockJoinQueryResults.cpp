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

/////////////////////////////////////////////////////////////////////
//
// class CGwsRightBatchSortedBlockJoinQueryResults
//
/////////////////////////////////////////////////////////////////////
CGwsRightBatchSortedBlockJoinQueryResults::CGwsRightBatchSortedBlockJoinQueryResults ()
{
    m_pos = eBeforeFirstRow;
    m_joinKeyIndex = 0;
    m_bMoreData = true;
    m_bNullEntry = false;
    m_bPrimaryKeyNull = false;
}

CGwsRightBatchSortedBlockJoinQueryResults::~CGwsRightBatchSortedBlockJoinQueryResults () throw()
{
}

EGwsStatus CGwsRightBatchSortedBlockJoinQueryResults::InitializeReader  (
    IGWSQuery                  * query,
    CGwsPreparedQuery          * prepquery,
    FdoStringCollection        * joincols,
    bool                       bScrollable
)
{
    return CGwsRightNestedLoopJoinQueryResults::InitializeReader (query, prepquery, joincols, bScrollable);
}

bool CGwsRightBatchSortedBlockJoinQueryResults::ReadNext()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  ReadNext() - Start\n");
    printf("  ReadNext() - JoinColumns:%S\n", m_joincols->ToString());
    wchar_t buffer[2048];
    m_joinkeys.ToString(buffer, 2048);
    printf("  ReadNext() - JoinKeys:\n%S\n", buffer);
    #endif

    // Check if primary key is NULL
    if(m_bPrimaryKeyNull)
    {
        m_bNullEntry = true;
        return true;
    }

    FdoPtr<IGWSQueryDefinition> pQueryDef;
    m_query->GetQueryDefinition(&pQueryDef);

    bool bOneToOneJoin = false;

    FdoStringP propname = m_joincols->GetString (0);

    // Get the data type of the secondary property
    FdoDataType dtSecondary = FdoDataType_String;
    if (m_prepquery)
    {
        FdoPtr<CGwsQueryResultDescriptors> extFeatDsc;
        m_prepquery->DescribeResults((IGWSExtendedFeatureDescription**)&extFeatDsc);
        CGwsPropertyDesc propDesc = extFeatDsc->GetPropertyDescriptor(propname);
        dtSecondary = propDesc.m_dataprop;
        bOneToOneJoin = extFeatDsc->ForceOneToOneJoin();
    }

    // Advance the secondary
    bool bRet = false;
    bool bAddNull = false;
    m_bNullEntry = false;

    if(m_bMoreData)
    {
        bool bDone = false;
        while(!bDone)
        {
            if(eBeforeFirstRow == m_pos)
            {
                #ifdef _DEBUG_BATCHSORT_JOIN
                printf("  ReadNext() - Calling secondary ReadNext()\n");
                #endif
                bRet = CGwsRightJoinQueryResults::ReadNext();
                if(!bRet)
                {
                    // No more data
                    m_bMoreData = false;
                }
            }
            else
            {
                bRet = true;
            }

            if(bRet)
            {
                bool bIsNullResult = m_reader->IsNull(propname);
                if(!bIsNullResult)
                {
                    FdoPtr<FdoDataValue> dataValuePrimary = (FdoDataValue*)m_joinkeys.GetItem(m_joinKeyIndex);
                    FdoPtr<FdoDataValue> dataValueSecondary = GetSecondaryDataValue(dtSecondary, propname);
                    int compare = GWSFdoUtilities::CompareDataValues(dataValuePrimary, dataValueSecondary);

                    #ifdef _DEBUG_BATCHSORT_JOIN
                    printf("*** Comparing - KV:%S, S:%S = %d ***\n", dataValuePrimary->ToString(), dataValueSecondary->ToString(), compare);
                    #endif

                    if(compare == 0) // (keyValue == secondary)
                    {
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** MATCH - KV:%S = S:%S ***\n", dataValuePrimary->ToString(), dataValueSecondary->ToString());
                        #endif

                        bDone = true;

                        // What about one-to-one and one-to-many?
                        m_pos = eBeforeFirstRow;
                        bRet = true;
                    }
                    else if(compare == -1) // (keyValue < secondary)
                    {
                        // No Match
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** NO MATCH - KV:%S < S:%S ***\n", dataValuePrimary->ToString(), dataValueSecondary->ToString());
                        #endif

                        // Secondary key is after primary key and thus requires the primary to be advanced.
                        // ie: Primary Key = "CA"
                        //     Secondardy Key = "CD"
                        //     "CA" < "CD"
                        //
                        // However, we don't want to lose this data in the secondary so we want to not advance the secondary reader

                        // Check the primary query type to see if this is an Outer or Inner join
                        if (pQueryDef->Type() == eGwsQueryLeftOuterJoin)
                        {
                            // Outer Join
                            if(eAfterJoinRow == m_pos)
                            {
                                if(bOneToOneJoin)
                                {
                                    bAddNull = true;
                                    m_pos = eOnJoinRow;
                                    bDone = true;
                                    bRet = true;
                                }
                                else
                                {
                                    m_pos = eOnJoinRow;
                                    bDone = true;
                                    bRet = false;
                                }
                            }
                            else if(eBeforeFirstRow == m_pos)
                            {
                                if(bOneToOneJoin)
                                {
                                    bAddNull = true;
                                    m_pos = eOnJoinRow;
                                    bDone = true;
                                    bRet = true;
                                }
                                else
                                {
                                    // Secondary was just advanced, but doesn't match so we need to advance primary
                                    m_pos = eOnJoinRow;
                                    bDone = true;
                                    bRet = false;
                                }
                            }
                            else
                            {
                                // No match for secondary so add NULL secondary
                                bAddNull = true;
                                m_pos = eAfterJoinRow;
                                bDone = true;
                                bRet = true;
                            }
                        }
                        else
                        {
                            // Inner Join
                            // Skip primary as there is no secondary
                            m_pos = eOnJoinRow;
                            bDone = true;
                            bRet = false;
                        }
                    }
                    else
                    {
                        // No Match
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** NO MATCH - KV:%S != S:%S ***\n", dataValuePrimary->ToString(), dataValueSecondary->ToString());
                        #endif

                        // We want to continue reading from the secondary if we can
                        m_pos = eBeforeFirstRow;
                    }
                }
                else
                {
                    // NULL property
                    bAddNull = true;
                }
            }
            else
            {
                // No more records
                if (pQueryDef->Type() == eGwsQueryLeftOuterJoin)
                {
                    if(bOneToOneJoin)
                    {
                        bAddNull = true;
                        m_pos = eAfterJoinRow;
                        bDone = true;
                        bRet = true;
                    }
                    else
                    {
                        m_pos = eOnJoinRow;
                        bDone = true;
                        bRet = false;
                    }
                }
                else
                {
                    m_pos = eOnJoinRow;
                    bDone = true;
                    bRet = false;
                }
            }
        }
    }
    else
    {
        // If there is no more data from the secondary and it is an outer join add a NULL
        if (pQueryDef->Type() == eGwsQueryLeftOuterJoin)
        {
            if(bOneToOneJoin)
            {
                bAddNull = true;
                bRet = true;
            }
            else
            {
                if(eBeforeFirstRow == m_pos)
                {
                    m_pos = eOnJoinRow;
                    bRet = false;
                }
                else
                {
                    m_pos = eBeforeFirstRow;
                    bAddNull = true;
                }
            }
        }
    }

    if(bAddNull)
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("*** Adding NULL secondary ***\n");
        #endif
        m_bNullEntry = true;
        bRet = true;
    }

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  ReadNext() End - JKI:%d  POS:%d  Return:%d\n", m_joinKeyIndex, m_pos, bRet);
    #endif

    return bRet;
}

void CGwsRightBatchSortedBlockJoinQueryResults::Close ()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  Close()\n");
    #endif
    CGwsRightNestedLoopJoinQueryResults::Close ();
}

EGwsStatus CGwsRightBatchSortedBlockJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
)
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  SetRelatedValues()\n");
    #endif

    // The code below is similar to the base class, but we want to use an IN clause in order to
    // do a batch comparison instead of the typical = and LIKE operators.
    EGwsStatus stat = eGwsOk;

    try {
        Close ();

        FdoString* propname = m_joincols->GetString (0);
        WSTR filter = L"\"";
        filter += propname;
        filter += L"\"";
        filter += L" IN ("; // NOXLATE

        FdoPtr<FdoFilter> pFilter;

        for (int idx = 0; idx < vals.GetCount (); idx ++)
        {
            if(idx > 0)
            {
                filter += L","; // NOXLATE
            }

            FdoPtr<FdoDataValue> val = vals.GetItem (idx);
            FdoStringP valStr = val->ToString();
            filter += valStr;
        }

        filter += L")"; // NOXLATE
        pFilter = FdoFilter::Parse(filter.c_str());
        m_prepquery->SetFilter (pFilter);

#ifdef _DEBUG_BATCHSORT_JOIN
    long dwStartQuery = GetTickCount();
#endif
        IGWSFeatureIterator * fiter = NULL;
        // This query takes a considerable amount of time
        stat = m_prepquery->Execute (& fiter);
#ifdef _DEBUG_BATCHSORT_JOIN
    printf("***** SetRelatedValues() - IN clause query total time = %6.4f (s)\n", (GetTickCount()-dwStartQuery)/1000.0);
#endif
        if (IGWSException::IsError (stat)) {
            PushStatus (stat);
            return stat;
        }
        m_reader = dynamic_cast<FdoIFeatureReader*> (fiter);
        m_bClosed = false;

        stat = CGwsRightJoinQueryResults::SetRelatedValues (vals);

        // Reset flag
        m_bMoreData = true;
        m_pos = eBeforeFirstRow;

    } catch (FdoException *e) {
        PushFdoException (eGwsFailed, e);
        e->Release();
        stat = eGwsFailed;
    }
    return stat;
}

FdoDataValue* CGwsRightBatchSortedBlockJoinQueryResults::GetSecondaryDataValue(FdoDataType dtSecondary, FdoString* propname)
{
    FdoPtr<FdoDataValue> secondary;

    switch (dtSecondary)
    {
        case FdoDataType_Byte:
            {
                FdoByte result = m_reader->GetByte(propname);
                FdoPtr<FdoByteValue> val = FdoByteValue::Create();
                val->SetByte(result);
                secondary = val;
            }
            break;
        case FdoDataType_Int16:
            {
                FdoInt16 result = m_reader->GetInt16(propname);
                FdoPtr<FdoInt16Value> val = FdoInt16Value::Create();
                val->SetInt16(result);
                secondary = val;
            }
            break;
        case FdoDataType_Int32:
            {
                FdoInt32 result = m_reader->GetInt32(propname);
                FdoPtr<FdoInt32Value> val = FdoInt32Value::Create();
                val->SetInt32(result);
                secondary = val;
            }
            break;
        case FdoDataType_Int64:
            {
                FdoInt64 result = m_reader->GetInt64(propname);
                FdoPtr<FdoInt64Value> val = FdoInt64Value::Create();
                val->SetInt64(result);
                secondary = val;
            }
            break;
        case FdoDataType_Single:
            {
                float result = m_reader->GetSingle(propname);
                FdoPtr<FdoSingleValue> val = FdoSingleValue::Create();
                val->SetSingle(result);
                secondary = val;
            }
            break;
        case FdoDataType_Double:
            {
                double result = m_reader->GetDouble(propname);
                FdoPtr<FdoDoubleValue> val = FdoDoubleValue::Create();
                val->SetDouble(result);
                secondary = val;
            }
            break;
        case FdoDataType_Decimal:
            {
                double result = m_reader->GetDouble(propname);
                FdoPtr<FdoDecimalValue> val = FdoDecimalValue::Create();
                val->SetDecimal(result);
                secondary = val;
            }
            break;
        case FdoDataType_String:
            {
                FdoStringP result = m_reader->GetString(propname);
                FdoPtr<FdoStringValue> val = FdoStringValue::Create();
                val->SetString(result);
                secondary = val;
            }
            break;
        default:
            break;
    }

    return secondary.Detach();
}

bool CGwsRightBatchSortedBlockJoinQueryResults::IsNull (FdoString* propertyName)
{
    if(m_bNullEntry)
    {
        return true;
    }
    else
    {
        return CGwsFeatureIterator::IsNull (propertyName);
    }
}

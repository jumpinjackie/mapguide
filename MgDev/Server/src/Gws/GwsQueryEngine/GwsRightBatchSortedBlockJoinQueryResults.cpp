//
//  Copyright (C) 2004-2007  Autodesk, Inc.
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

    if (m_usepool) {
        // reading from the pool
        if (m_poolpos + 1 < m_pool->GetCount ()) {
            m_poolpos ++;
            return true;
        }
        return false;
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
                FdoPtr<FdoDataValue> dataValue = (FdoDataValue*)m_joinkeys.GetItem(m_joinKeyIndex);
                FdoStringP keyValue;
                if(dataValue->GetDataType() == FdoDataType_String )
                {
                    // Get the actual string value
                    keyValue = ((FdoStringValue*)dataValue.p)->GetString();
                }
                else
                {
                    // Convert it to a string
                    keyValue = dataValue->ToString();
                }

                bool bIsNullResult = m_reader->IsNull(propname);
                if(!bIsNullResult)
                {
                    FdoStringP secondary = L""; // NOXLATE
                    secondary = GetSecondaryAsString(dtSecondary, propname);

                    #ifdef _DEBUG_BATCHSORT_JOIN
                    printf("*** Comparing - KV:%S, S:%S = %d ***\n", (FdoString*)keyValue, (FdoString*)secondary, wcscmp(keyValue,secondary));
                    #endif

                    if(wcscmp(keyValue,L"UY") == 0)
                    {
                        int x = 0;
                    }

                    if(wcscmp(keyValue,secondary) < 0)
                    {
                        // No Match
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** NO MATCH - KV:%S < S:%S ***\n", (FdoString*)keyValue, (FdoString*)secondary);
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
                                m_pos = eOnJoinRow;
                                bDone = true;
                                bRet = false;
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
                    else if(wcscmp(keyValue,secondary) == 0)
                    {
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** MATCH - KV:%S = S:%S ***\n", (FdoString*)keyValue, (FdoString*)secondary);
                        #endif
                        m_pool->AddFeature (this);

                        bDone = true;

                        // What about one-to-one and one-to-many?
                        m_pos = eBeforeFirstRow;
                        bRet = true;
                    }
                    else
                    {
                        // No Match
                        #ifdef _DEBUG_BATCHSORT_JOIN
                        printf("*** NO MATCH - KV:%S != S:%S ***\n", (FdoString*)keyValue, (FdoString*)secondary);
                        #endif

                        // We want to continue reading from the secondary if we can
                        m_pos = eBeforeFirstRow;
                    }
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
        m_pool->AddFeature (NULL);
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
        if (m_joinkeys == vals) {
            if (! m_neverusepooling) {
                // this completes features pool
                if (! m_bClosed) {
                    while (ReadNext ())
                        ;
                    Close ();
                }
            }
            m_usepool = true;
            m_poolpos = -1;
            return eGwsOk;
        }
        m_pool->Reset ();
        m_usepool = false;
        m_poolpos = -1;

    // The code below is similar to the base class, but we want to use an IN clause in order to
    // do a batch comparison instead of the typical = and LIKE operators.
    EGwsStatus stat = eGwsOk;

    try {
        Close ();

        FdoString* propname = m_joincols->GetString (0);
        WSTR filter = propname;
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

        IGWSFeatureIterator * fiter = NULL;
        stat = m_prepquery->Execute (& fiter);
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

FdoStringP CGwsRightBatchSortedBlockJoinQueryResults::GetSecondaryAsString(FdoDataType dtSecondary, FdoString* propname)
{
    FdoStringP secondary = L""; // NOXLATE

    switch (dtSecondary)
    {
        case FdoDataType_Byte:
            {
                FdoByte result = m_reader->GetByte(propname);
                FdoPtr<FdoByteValue> val = FdoByteValue::Create();
                val->SetByte(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Int16:
            {
                FdoInt16 result = m_reader->GetInt16(propname);
                FdoPtr<FdoInt16Value> val = FdoInt16Value::Create();
                val->SetInt16(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Int32:
            {
                FdoInt32 result = m_reader->GetInt32(propname);
                FdoPtr<FdoInt32Value> val = FdoInt32Value::Create();
                val->SetInt32(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Int64:
            {
                FdoInt64 result = m_reader->GetInt64(propname);
                FdoPtr<FdoInt64Value> val = FdoInt64Value::Create();
                val->SetInt64(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Single:
            {
                float result = m_reader->GetSingle(propname);
                FdoPtr<FdoSingleValue> val = FdoSingleValue::Create();
                val->SetSingle(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Double:
            {
                double result = m_reader->GetDouble(propname);
                FdoPtr<FdoDoubleValue> val = FdoDoubleValue::Create();
                val->SetDouble(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_Decimal:
            {
                double result = m_reader->GetDouble(propname);
                FdoPtr<FdoDecimalValue> val = FdoDecimalValue::Create();
                val->SetDecimal(result);
                secondary = val->ToString();
            }
            break;
        case FdoDataType_String:
            {
                FdoStringP result = m_reader->GetString(propname);
                FdoPtr<FdoStringValue> val = FdoStringValue::Create();
                val->SetString(result);
                secondary = val->GetString();
            }
            break;
        default:
            break;
    }

    return secondary;
}

bool CGwsRightBatchSortedBlockJoinQueryResults::IsNull (FdoString* propertyName)
{
    if (m_usepool) {
        FdoPtr<IGWSFeature> f = GetPooledFeature ();
        return f->IsNull (propertyName);
    }

    if(m_bNullEntry)
    {
        return true;
    }
    else
    {
        return CGwsFeatureIterator::IsNull (propertyName);
    }
}

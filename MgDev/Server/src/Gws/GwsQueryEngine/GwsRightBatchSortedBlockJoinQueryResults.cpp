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
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext()\n");
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - JoinColumns:%S\n", m_joincols->ToString());
    wchar_t buffer[2048];
    m_joinkeys.ToString(buffer, 2048);
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - JoinKeys:%S\n", buffer);
    #endif

    if (m_usepool) {
        // reading from the pool
        if (m_poolpos + 1 < m_pool->GetCount ()) {
            m_poolpos ++;
            return true;
        }
        return false;
    }

    bool bAdvancePrimary = false;
    bool bRet = false;
    if(m_pos == eBeforeFirstRow)
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - eBeforeFirstRow\n");
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - Calling secondary ReadNext()\n");
        #endif

        bRet = CGwsRightJoinQueryResults::ReadNext ();
        if(!bRet)
        {
            m_pos = eBeforeFirstRow;
            m_joinKeyIndex = 0;
        }
    }
    else if(m_pos == eOnJoinRow)
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - eOnJoinRow\n");
        #endif
        // We need to skip over any matches for the join keys we have already done
        FdoStringP propname = m_joincols->GetString (0);
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

        bool bOneToOneJoin = true;

        bool bDone = false;
        while(!bDone)
        {
            #ifdef _DEBUG_BATCHSORT_JOIN
            printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - Calling secondary ReadNext()\n");
            #endif
            bRet = CGwsRightJoinQueryResults::ReadNext ();
            if(bRet)
            {
                bool bIsNullResult = m_reader->IsNull(propname);
                if(!bIsNullResult)
                {
                    FdoStringP secondary = L""; // NOXLATE

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

                    secondary = GetSecondaryAsString(dtSecondary, propname);

                    #ifdef _DEBUG_BATCHSORT_JOIN
                    printf("*** Comparing - KV:%S, S:%S = %d***\n", (FdoString*)keyValue, (FdoString*)secondary, wcscmp(keyValue,secondary));
                    #endif

                    if(wcscmp(keyValue,secondary) < 0)
                    {
                        // Stop here
                        bDone = true;

                        bAdvancePrimary = true;
                        m_pos = eAfterJoinRow;
                        m_joinKeyIndex++;
                        bRet = false;
                    }
                    else if(wcscmp(keyValue,secondary) == 0)
                    {
                        // Stop here
                        bDone = true;

                        if(bOneToOneJoin)
                        {
                            // NOP
                        }
                        else
                        {
                            bAdvancePrimary = true;
                            m_pos = eAfterJoinRow;
                            bRet = false;
                        }
                    }
                }
            }
            else
            {
                // No more records
                bDone = true;
            }
        }

        if((!bRet) && (!bAdvancePrimary)) 
        {
            m_pos = eBeforeFirstRow;
            m_joinKeyIndex = 0;
        }
    }
    else if(m_pos == eAfterJoinRow)
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - eAfterJoinRow\n");
        #endif
        bRet = true;
    }
    else
    {
        #ifdef _DEBUG_BATCHSORT_JOIN
        printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - ELSE\n");
        #endif
        // Next read will be for the next primary key so we don't want to advance the secondary
        m_pos = eBeforeFirstRow;
        m_joinKeyIndex = 0;
    }

    if (bRet)
    {
        // Do we have a key match?
        FdoString* propname = m_joincols->GetString (0);
        bool bIsNullResult = m_reader->IsNull(propname);
        if(!bIsNullResult)
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

            FdoStringP secondary = L""; // NOXLATE

            // Get the data type of the secondary property
            FdoDataType dtSecondary = FdoDataType_String;
            if (m_prepquery)
            {
                FdoPtr<CGwsQueryResultDescriptors> extFeatDsc;
                m_prepquery->DescribeResults((IGWSExtendedFeatureDescription**)&extFeatDsc);
                CGwsPropertyDesc propDesc = extFeatDsc->GetPropertyDescriptor(propname);
                dtSecondary = propDesc.m_dataprop;
            }

            secondary = GetSecondaryAsString(dtSecondary, propname);

            if(wcscmp(keyValue,secondary) == 0)
            {
                // Match
                #ifdef _DEBUG_BATCHSORT_JOIN
                printf("*** MATCH %d - KV:%S = S:%S ***\n", m_joinKeyIndex, (FdoString*)keyValue, (FdoString*)secondary);
                #endif
                m_pool->AddFeature (this);
                m_pos = eOnJoinRow;
            }
            else
            {
                // No Match
                #ifdef _DEBUG_BATCHSORT_JOIN
                printf("*** NO MATCH %d - KV:%S != S:%S ***\n", m_joinKeyIndex, (FdoString*)keyValue, (FdoString*)secondary);
                #endif
                m_pos = eAfterJoinRow;
                bRet = false;
            }
        }
        else
        {
            m_pos = eOnJoinRow;
            bRet = false;
        }

        m_joinKeyIndex++;
    }

    #ifdef _DEBUG_BATCHSORT_JOIN
    if(bRet)
    {
        FdoString* propname = m_joincols->GetString (0);
        FdoStringP secondary = L"";

        bool bIsNullResult = m_reader->IsNull(propname);
        if(bIsNullResult)
        {
            secondary = L"<Null>";
        }
        else
        {
            // Get the data type of the secondary property
            FdoDataType dtSecondary = FdoDataType_String;
            if (m_prepquery)
            {
                FdoPtr<CGwsQueryResultDescriptors> extFeatDsc;
                m_prepquery->DescribeResults((IGWSExtendedFeatureDescription**)&extFeatDsc);
                CGwsPropertyDesc propDesc = extFeatDsc->GetPropertyDescriptor(propname);
                dtSecondary = propDesc.m_dataprop;
            }

            secondary = GetSecondaryAsString(dtSecondary, propname);
        }
        printf("\n++++++++++ Secondary: %S\n\n", (FdoString*)secondary);
    }
    #endif

    // Check if index needs to be reset
    if(m_joinKeyIndex == m_joinkeys.GetCount())
    {
        m_pos = eBeforeFirstRow;
        m_joinKeyIndex = 0;
    }

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::ReadNext() - JKI:%d  POS:%d  AP:%d  Return:%d\n", m_joinKeyIndex, m_pos, bAdvancePrimary, bRet);
    #endif

    return bRet;
}

void CGwsRightBatchSortedBlockJoinQueryResults::Close ()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::Close()\n");
    #endif
    CGwsRightNestedLoopJoinQueryResults::Close ();
}

EGwsStatus CGwsRightBatchSortedBlockJoinQueryResults::SetRelatedValues (
    const GWSFeatureId & vals
)
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsRightBatchSortedBlockJoinQueryResults::SetRelatedValues()\n");
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

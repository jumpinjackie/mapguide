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
#include "GwsBinaryFeatureWriter.h"

// This setting limits the batch size used by the join algorithm
int CGwsBatchSortedBlockJoinQueryResults::sm_nBatchSize = 100; //Default

/////////////////////////////////////////////////////////////////////
//
// class CGwsBatchSortedBlockJoinQueryResults
//
/////////////////////////////////////////////////////////////////////
CGwsBatchSortedBlockJoinQueryResults::CGwsBatchSortedBlockJoinQueryResults (
)
{
    m_right = NULL;
    m_primaryFeatureIterator = NULL;
    m_pPrimaryCacheIterator = m_pPrimaryCache.begin();
}

CGwsBatchSortedBlockJoinQueryResults::~CGwsBatchSortedBlockJoinQueryResults (
) throw()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("CGwsBatchSortedBlockJoinQueryResults::~CGwsBatchSortedBlockJoinQueryResults()\n");
    #endif

    ClearIteratorCache();

    for ( size_t i=0;i<m_propertyDescriptionCollection.size();i++ )
    {
        CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
        delete propDesc;
    }

    m_propertyDescriptionCollection.clear();

    if (m_primaryFeatureIterator != NULL)
    {
        m_primaryFeatureIterator->Release ();
        m_primaryFeatureIterator = NULL;
    }
}

EGwsStatus CGwsBatchSortedBlockJoinQueryResults::InitializeReader (
    IGWSQuery                       * query,
    CGwsPreparedJoinQuery           * prepquery,
    bool                            bScrollable
)
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("CGwsBatchSortedBlockJoinQueryResults::InitializeReader() - Start\n");
    #endif
    EGwsStatus                  stat = eGwsOk;
    CGwsPreparedQuery         * leftquery = prepquery->LeftQuery ();
    CGwsPreparedQuery         * rightquery = prepquery->RightQuery ();
    FdoPtr<FdoStringCollection> leftcols = prepquery->LeftProperties ();
    FdoPtr<FdoStringCollection> rightcols = prepquery->RightProperties ();

    #ifdef _DEBUG_BATCHSORT_JOIN
    FdoPtr<FdoFilter> lqFilter = leftquery->GetFilter();
    FdoPtr<FdoFilter> rqFilter = rightquery->GetFilter();

    FdoStringP lqFilterStr = lqFilter ? lqFilter->ToString() : NULL;
    FdoStringP rqFilterStr = rqFilter ? rqFilter->ToString() : NULL;

    printf("  Left  Query Filter: %S\n", lqFilterStr);
    printf("  Right Query Filter: %S\n", rqFilterStr);
    printf("  Left  Query Properties: %S\n", leftcols ? leftcols->ToString() : L"<null>");
    printf("  Right Query Properties: %S\n", rightcols ? rightcols->ToString() : L"<null>");
    #endif

    stat = CGwsJoinQueryResults::InitializeReader (leftcols, query, leftquery, bScrollable);
    if (IGWSException::IsError (stat)) {
        PushStatus  (stat);
        return stat;
    }

    IGWSFeatureIterator * primaryFeatureIterator = NULL;
    stat = leftquery->Execute (& primaryFeatureIterator, bScrollable);
    if (IGWSException::IsError (stat)) {
        delete primaryFeatureIterator;
    }
    else
    {
        m_primaryFeatureIterator = primaryFeatureIterator;
    }

    m_prepquery = prepquery;

    CGwsRightJoinQueryResults * results =
            (CGwsRightJoinQueryResults *) rightquery->CreateFeatureIterator (eGwsRightBatchSortedBlockIterator);
    stat = results->InitializeReader (query, rightquery, rightcols, bScrollable);

    if (IGWSException::IsError (stat)) {
        delete results;
    } else {
        m_right = results;
        m_right->AddRef ();
    }

    // Get the feature description so that we can get the primary property names
    FdoPtr<IGWSExtendedFeatureDescription> featDsc;
    DescribeFeature (&featDsc);

    m_propertyNames = featDsc->PropertyNames();

    // Set the size of the property collection cache
    m_propertyDescriptionCollection.resize(m_propertyNames->GetCount());

    for(int j=0;j<m_propertyNames->GetCount();j++)
    {
        FdoString* propertyName = m_propertyNames->GetString(j);
        const CGwsPropertyDesc propertyDesc = GetPropertyDescriptor(propertyName);
        CGwsPropertyDesc* pPropertyDesc = new CGwsPropertyDesc(propertyDesc);
        if(pPropertyDesc)
        {
            m_propertyDescriptionCollection[j] = pPropertyDesc;
        }
    }

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("CGwsBatchSortedBlockJoinQueryResults::InitializeReader() - End\n");
    #endif
    return stat;
}

bool CGwsBatchSortedBlockJoinQueryResults::ReadNext ()
{

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("CGwsBatchSortedBlockJoinQueryResults::ReadNext()\n");
    #endif

    if(m_pPrimaryCache.size() > 0)
    {
        m_pPrimaryCacheIterator++;

        // Update the key index
        CGwsRightBatchSortedBlockJoinQueryResults* right = dynamic_cast<CGwsRightBatchSortedBlockJoinQueryResults*>(m_right);
        if(NULL != right)
        {
            right->IncrementKeyIndex();
        }
    }

    if(m_pPrimaryCacheIterator == m_pPrimaryCache.end())
    {
        m_bLeftJoinValuesSet = false;

        // Update the key index
        CGwsRightBatchSortedBlockJoinQueryResults* right = dynamic_cast<CGwsRightBatchSortedBlockJoinQueryResults*>(m_right);
        if(NULL != right)
        {
            right->ResetKeyIndex();
        }
    }

    // Read the left side
    bool bRes = CGwsFeatureIterator::ReadNext ();

    // Read the right side
    bRes = SetupBatchRightSide(bRes);

    return bRes;
}

IGWSFeatureIterator * CGwsBatchSortedBlockJoinQueryResults::GetJoinedFeatures (int iJoin)
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsBatchSortedBlockJoinQueryResults::GetJoinedFeatures(%d)\n", iJoin);
    #endif
    // check join feature index
    FdoPtr<IGWSExtendedFeatureDescription> fdsc;
    DescribeFeature (& fdsc);
    if (iJoin >= fdsc->GetCount ())
        GWS_THROW (eGwsIndexOutOfBounds);

    if (iJoin < m_prepquery->GetPathLength () - 1) {
        if (m_prepquery->QueryType () == eGwsQueryLeftOuterJoin ||
            m_prepquery->QueryType () == eGwsQueryEqualJoin)
        {
            CGwsJoinQueryResults * jqr = dynamic_cast<CGwsJoinQueryResults*> (m_reader.p);
            assert (jqr);
            return jqr->GetJoinedFeatures (iJoin);
        }
    }

    return GetJoinedFeatures ();
}

IGWSFeatureIterator * CGwsBatchSortedBlockJoinQueryResults::GetJoinedFeatures ()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsBatchSortedBlockJoinQueryResults::GetJoinedFeatures()\n");
    #endif

    // Only update the right side when needed
    EGwsStatus stat = eGwsOk;
    if(!m_bLeftJoinValuesSet)
    {
        GetJoinValues ();
        assert (m_bLeftJoinValuesSet);
        stat = m_right->SetRelatedValues (m_leftJoinVals);
    }

    if (IGWSException::IsError (stat)) {
        CopyStatus (* m_right); // TODO: candidate for exception throwing
        return NULL;
    }
    m_right->AddRef ();
    return m_right;

}

bool CGwsBatchSortedBlockJoinQueryResults::SetupBatchRightSide(bool bRes) {

    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsBatchSortedBlockJoinQueryResults::SetupBatchRightSide()\n");
    #endif

    return bRes;
}

FdoDataValueCollection * CGwsBatchSortedBlockJoinQueryResults::GetJoinValues ()
{
    #ifdef _DEBUG_BATCHSORT_JOIN
    printf("  CGwsBatchSortedBlockJoinQueryResults::GetJoinValues()\n");
    #endif
    if (! m_bLeftJoinValuesSet)
    {
        // Clear the cache
        ClearIteratorCache();

        // Populate the primary iterator cache
        for(int i=0;i<sm_nBatchSize;i++)
        {
            if(m_primaryFeatureIterator->ReadNext())
            {
                PrimaryCacheEntry* cacheEntry = new PrimaryCacheEntry();
                if(cacheEntry)
                {
                    // Need to cache all primary properties as we walk the primary iterator
                    cacheEntry->propertyCollection.resize(m_propertyNames->GetCount());

                    for(int j=0;j<m_propertyNames->GetCount();j++)
                    {
                        PropertyCacheEntry* propertyCacheEntry = new PropertyCacheEntry();
                        if(propertyCacheEntry)
                        {
                            FdoString* propertyName = m_propertyNames->GetString(j);
                            #ifdef _DEBUG_BATCHSORT_JOIN
                            printf("%S - ", propertyName);
                            #endif

                            CGwsPropertyDesc* propertyDesc = m_propertyDescriptionCollection[j];
                            if(FdoPropertyType_GeometricProperty == propertyDesc->m_ptype)
                            {
                                // This is a geometry property
                                FdoByteArray* geometry = m_primaryFeatureIterator->GetGeometry(propertyName);
                                #ifdef _DEBUG_BATCHSORT_JOIN
                                printf("<Geometry>\n");
                                #endif

                                propertyCacheEntry->geometry = geometry;
                            }
                            else
                            {
                                // This is a data property
                                FdoDataValue* dataValue =  m_primaryFeatureIterator->GetDataValue(propertyName);
                                #ifdef _DEBUG_BATCHSORT_JOIN
                                FdoStringP value = dataValue->ToString();
                                printf("%S\n", value);
                                #endif

                                // Is this the primary key?
                                if(wcscmp(propertyName, m_leftcols->GetString(0)) == 0)
                                {
                                    dataValue->AddRef();
                                    cacheEntry->primaryKey = dataValue;
                                }

                                propertyCacheEntry->dataValue = dataValue;
                            }

                            cacheEntry->propertyCollection[j] = propertyCacheEntry;
                        }
                    }

                    // Add the iterator cache entry
                    m_pPrimaryCache.push_back(cacheEntry);
                }
            }
            else
            {
                // No more keys
                break;
            }
        }

        // Sort the cache if needed
        if(m_pPrimaryCache.size() > 1)
        {
            QuickSort(m_pPrimaryCache, 0, (FdoInt32)(m_pPrimaryCache.size()-1));
        }

        // Set the 1st cache entry to read
        m_pPrimaryCacheIterator = m_pPrimaryCache.begin();

        FdoPtr<FdoDataValueCollection> dvcol;

        for ( size_t i=0;i<m_pPrimaryCache.size();i++ )
        {
            FdoPtr<FdoDataValue> val;

            PrimaryCacheEntry* cacheEntry = m_pPrimaryCache.at(i);
            if(cacheEntry)
            {
                FdoPtr<FdoDataValue> primary = cacheEntry->primaryKey;
                FdoDataType dtPrimary = primary->GetDataType();

                // We need to match primary data type to secondary join data type
                FdoDataType dtSecondary = dtPrimary;
                if (m_right)
                {
                    FdoPtr<FdoStringCollection> joinColumns = m_right->GetJoinColumns();
                    FdoString* propname = joinColumns->GetString (0);
                    CGwsPropertyDesc propDesc = m_right->GetPropertyDescriptor(propname);
                    dtSecondary = propDesc.m_dataprop;
                }

                switch (dtPrimary)
                {
                    case FdoDataType_Byte:
                    case FdoDataType_Int16:
                    case FdoDataType_Int32:
                    case FdoDataType_Int64:
                    case FdoDataType_Decimal:
                    case FdoDataType_Single:
                    case FdoDataType_Double:
                        switch (dtSecondary)
                        {
                            case FdoDataType_Byte:
                            case FdoDataType_Int16:
                            case FdoDataType_Int32:
                            case FdoDataType_Int64:
                            case FdoDataType_Decimal:
                            case FdoDataType_Single:
                            case FdoDataType_Double:
                                val = primary;
                                break;

                            case FdoDataType_String:
                                // Convert primary to string
                                {
                                    FdoStringP strPrimVal = primary->ToString();
                                    FdoPtr<FdoStringValue> strval = FdoStringValue::Create();
                                    strval->SetString(strPrimVal);
                                    val = strval;
                                }
                                break;
                        }
                        break;


                    case FdoDataType_String:
                        switch (dtSecondary)
                        {
                            case FdoDataType_String:
                                val = primary;
                                break;

                            case FdoDataType_Byte:
                            case FdoDataType_Int16:
                            case FdoDataType_Int32:
                            case FdoDataType_Int64:
                            case FdoDataType_Decimal:
                            case FdoDataType_Single:
                            case FdoDataType_Double:
                                // Convert primary string to number
                                {
                                    FdoStringP strPrimVal = primary->ToString();

                                    // Remove the single quote from the string
                                    FdoStringP quote = "'";           // NOXLATE
                                    FdoStringP emptyString = "";      // NOXLATE
                                    strPrimVal = strPrimVal.Replace(quote, emptyString);

                                    if (strPrimVal.IsNumber())
                                    {
                                        FdoPtr<FdoDoubleValue> dval = FdoDoubleValue::Create();
                                        dval->SetDouble(strPrimVal.ToDouble());
                                        val = dval;
                                    }
                                }
                                break;
                        }
                }

                if(val)
                {
                    if (dvcol == NULL)
                        dvcol = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
                    dvcol->Add (val);
                }
            }
        }

        m_leftJoinVals = dvcol;

        #ifdef _DEBUG_BATCHSORT_JOIN
        wchar_t* buffer = new wchar_t[2048];
        m_leftJoinVals.ToString(buffer, 2048);
        printf("Join Values = %S\n\n", buffer);
        delete [] buffer;
        #endif

        m_bLeftJoinValuesSet = true;
    }
    return NULL; // m_leftJoinVals;
}

FdoString * CGwsBatchSortedBlockJoinQueryResults::GetString (FdoString* propertyName)
{
    FdoString* result = NULL;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_String)
                        {
                            FdoStringValue* value = (FdoStringValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetString();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

bool CGwsBatchSortedBlockJoinQueryResults::GetBoolean(FdoString* propertyName)
{
    bool result = false;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Boolean)
                        {
                            FdoBooleanValue* value = (FdoBooleanValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetBoolean();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoByte CGwsBatchSortedBlockJoinQueryResults::GetByte(FdoString* propertyName)
{
    FdoByte result = 0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Byte)
                        {
                            FdoByteValue* value = (FdoByteValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetByte();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoDateTime CGwsBatchSortedBlockJoinQueryResults::GetDateTime(FdoString* propertyName)
{
    FdoDateTime result;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_DateTime)
                        {
                            FdoDateTimeValue* value = (FdoDateTimeValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetDateTime();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

double CGwsBatchSortedBlockJoinQueryResults::GetDouble(FdoString* propertyName)
{
    double result = 0.0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Double)
                        {
                            FdoDoubleValue* value = (FdoDoubleValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetDouble();
                            }
                        }
                        else if(propDesc->m_dataprop == FdoDataType_Decimal)
                        {
                            FdoDecimalValue* value = (FdoDecimalValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetDecimal();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoInt16 CGwsBatchSortedBlockJoinQueryResults::GetInt16(FdoString* propertyName)
{
    FdoInt16 result = 0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Int16)
                        {
                            FdoInt16Value* value = (FdoInt16Value*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetInt16();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoInt32 CGwsBatchSortedBlockJoinQueryResults::GetInt32(FdoString* propertyName)
{
    FdoInt32 result = 0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Int32)
                        {
                            FdoInt32Value* value = (FdoInt32Value*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetInt32();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoInt64 CGwsBatchSortedBlockJoinQueryResults::GetInt64(FdoString* propertyName)
{
    FdoInt64 result = 0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Int64)
                        {
                            FdoInt64Value* value = (FdoInt64Value*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetInt64();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

float CGwsBatchSortedBlockJoinQueryResults::GetSingle(FdoString* propertyName)
{
    float result = 0.0;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_Single)
                        {
                            FdoSingleValue* value = (FdoSingleValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value->GetSingle();
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoLOBValue * CGwsBatchSortedBlockJoinQueryResults::GetLOB(FdoString* propertyName)
{
    FdoLOBValue* result = NULL;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        if(propDesc->m_dataprop == FdoDataType_CLOB)
                        {
                            FdoLOBValue* value = (FdoLOBValue*)(propertyCacheEntry->dataValue.p);
                            if(value)
                            {
                                result = value;
                            }
                        }
                        else
                        {
                            // Wrong data type
                            // Exception!
                            GWS_THROW (eGwsFdoInvalidDataType);
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}

FdoIStreamReader* CGwsBatchSortedBlockJoinQueryResults::GetLOBStreamReader(const wchar_t* propertyName )
{
    // Not implemented
    assert (false);
    return NULL;
}

bool CGwsBatchSortedBlockJoinQueryResults::IsNull(FdoString* propertyName)
{
    bool result = true;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_DataProperty)
                    {
                        FdoDataValue* value = (FdoDataValue*)(propertyCacheEntry->dataValue.p);
                        if(value)
                        {
                            result = value->IsNull();
                        }
                    }
                    else if(propDesc->m_ptype == FdoPropertyType_GeometricProperty)
                    {
                        FdoByteArray* value = (FdoByteArray*)(propertyCacheEntry->geometry.p);
                        if(value)
                        {
                            result = false;
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    return result;
}
FdoIRaster*  CGwsBatchSortedBlockJoinQueryResults::GetRaster(FdoString* propertyName)
{
    // Not implemented
    assert (false);
    return NULL;
}
FdoDataValue * CGwsBatchSortedBlockJoinQueryResults::GetPropertyValue (
    const CGwsPropertyDesc & desc
)
{
    if (desc.m_ptype != FdoPropertyType_DataProperty)
        return NULL;

    FdoDataValue* result = NULL;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(propDesc->m_dataprop == desc.m_dataprop)
                {
                    if(wcscmp(propDesc->m_name.c_str(), desc.m_name.c_str()) == 0)
                    {
                        FdoDataValue* value = (FdoDataValue*)(propertyCacheEntry->dataValue.p);
                        if(value)
                        {
                            result = value;
                        }

                        break;
                    }
                }
            }
        }
    }

    return result;
}

FdoDataValue * CGwsBatchSortedBlockJoinQueryResults::GetDataValue (FdoString* propertyName)
{
    const CGwsPropertyDesc & desc = GetPropertyDescriptor (propertyName);
    if (desc.m_name.empty ())
        return NULL;
    return GetPropertyValue (desc);
}

FdoDataValueCollection * CGwsBatchSortedBlockJoinQueryResults::GetDataValues (
    FdoStringCollection* propertyNames
)
{
    CGwsDataValueCollection * vals = NULL;
    for (int i = 0; i < propertyNames->GetCount (); i ++) {
        FdoPtr<FdoDataValue> val = GetDataValue (propertyNames->GetString (i));
        assert (val != NULL);
        if (vals == NULL)
            vals = (CGwsDataValueCollection *) CGwsDataValueCollection::Create ();
        vals->Add (val);
    }
    return vals;
}

const FdoByte * CGwsBatchSortedBlockJoinQueryResults::GetGeometry(
    FdoString * propertyName,
    FdoInt32  * count
)
{
    FdoByte * gvalue = NULL;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_GeometricProperty)
                    {
                        FdoByteArray* value = (FdoByteArray*)(propertyCacheEntry->geometry.p);
                        if(value)
                        {
                            FdoByteArray* valueCopy = FdoByteArray::Create((unsigned char*)value->GetData(), value->GetCount());
                            gvalue = valueCopy->GetData();

                            if(count)
                            {
                                *count = value->GetCount();
                            }
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    if (m_converter != NULL && ! m_bGeometryConverted && gvalue) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue, * count);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;

}

FdoByteArray* CGwsBatchSortedBlockJoinQueryResults::GetGeometry(FdoString* propertyName)
{
    FdoByteArray * gvalue = NULL;

    // Read from the cache
    PrimaryCacheEntry* cacheEntry = *m_pPrimaryCacheIterator;
    for(size_t i=0;i<cacheEntry->propertyCollection.size();i++)
    {
        PropertyCacheEntry* propertyCacheEntry = cacheEntry->propertyCollection.at(i);
        if(propertyCacheEntry)
        {
            CGwsPropertyDesc* propDesc = m_propertyDescriptionCollection[i];
            if(propDesc)
            {
                if(wcscmp(propDesc->m_name.c_str(), propertyName) == 0)
                {
                    if(propDesc->m_ptype == FdoPropertyType_GeometricProperty)
                    {
                        FdoByteArray* value = (FdoByteArray*)(propertyCacheEntry->geometry.p);
                        if(value)
                        {
                            FdoByteArray* valueCopy = FdoByteArray::Create((unsigned char*)value->GetData(), value->GetCount());
                            gvalue = valueCopy;
                        }
                    }
                    else
                    {
                        // Invalid property type
                        // Exception!
                        GWS_THROW (eGwsFdoInvalidPropertyType);
                    }

                    break;
                }
            }
        }
    }

    if (m_converter != NULL && ! m_bGeometryConverted && gvalue) {
        EGwsStatus stat = m_converter->ConvertForward (gvalue);
        if (IGWSException::IsError (stat))
            GWS_THROW (stat);
        m_bGeometryConverted = true;
    }
    return gvalue;
}

FdoByteArray * CGwsBatchSortedBlockJoinQueryResults::GetOriginalGeometry (FdoString* propertyName)
{
    return GetGeometry (propertyName);
}

FdoIFeatureReader* CGwsBatchSortedBlockJoinQueryResults::GetFeatureObject(FdoString* propertyName)
{
    // Not implemented
    assert (false);
    return NULL;
}

void CGwsBatchSortedBlockJoinQueryResults::ClearIteratorCache()
{
    for ( size_t i=0;i<m_pPrimaryCache.size();i++ )
    {
        PrimaryCacheEntry* primaryCacheEntry = m_pPrimaryCache.at(i);
        if(primaryCacheEntry)
        {
            for(size_t j=0;j<primaryCacheEntry->propertyCollection.size();j++)
            {
                PropertyCacheEntry* propertyCacheEntry = primaryCacheEntry->propertyCollection.at(j);
                if(propertyCacheEntry)
                {
                    propertyCacheEntry->dataValue = NULL;
                    propertyCacheEntry->geometry = NULL;
                    delete propertyCacheEntry;
                }
            }

            primaryCacheEntry->propertyCollection.clear();

            primaryCacheEntry->primaryKey = NULL;
            delete primaryCacheEntry;
        }
    }

    m_pPrimaryCache.clear();
    m_pPrimaryCacheIterator = m_pPrimaryCache.begin();
}

void CGwsBatchSortedBlockJoinQueryResults::QuickSort(std::vector<PrimaryCacheEntry*>& cache, FdoInt32 left, FdoInt32 right)
{
    assert(left >= 0);
    assert(right >= 0);

    int i = left;
    int j = right;
    PrimaryCacheEntry* cacheEntry = cache[(left + right) / 2];

    do {

        while (QuickSortCompare(cache[i], cacheEntry))
            i++;

        while (QuickSortCompare(cacheEntry, cache[j]))
            j--;

        if (i <= j) {
            if (i < j) {
                PrimaryCacheEntry* tempCacheEntry = cache[i];
                cache[i] = cache[j];
                cache[j] = tempCacheEntry;
            }

            i++; j--;
        }
    } while (i <= j);

    if (left < j)
        QuickSort(cache, left, j);

    if (i < right)
        QuickSort(cache, i, right);
}

bool CGwsBatchSortedBlockJoinQueryResults::QuickSortCompare(PrimaryCacheEntry* compareA, PrimaryCacheEntry* compareB)
{
    bool bResult = false;

    if((compareA != NULL) && (compareB != NULL))
    {
        FdoPtr<FdoDataValue> compareADataValue;
        FdoPtr<FdoDataValue> compareBDataValue;

        compareADataValue = compareA->primaryKey;
        compareBDataValue = compareB->primaryKey;

        FdoDataType dataType = compareADataValue->GetDataType();
        switch (dataType)
        {
            case FdoDataType_Int16:
                {
                    FdoInt16 compareAValue = ((FdoInt16Value*)(compareADataValue.p))->GetInt16();
                    FdoInt16 compareBValue = ((FdoInt16Value*)(compareBDataValue.p))->GetInt16();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_Int32:
                {
                    FdoInt32 compareAValue = ((FdoInt32Value*)(compareADataValue.p))->GetInt32();
                    FdoInt32 compareBValue = ((FdoInt32Value*)(compareBDataValue.p))->GetInt32();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_Int64:
                {
                    FdoInt64 compareAValue = ((FdoInt64Value*)(compareADataValue.p))->GetInt64();
                    FdoInt64 compareBValue = ((FdoInt64Value*)(compareBDataValue.p))->GetInt64();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_Single:
                {
                    float compareAValue = ((FdoSingleValue*)(compareADataValue.p))->GetSingle();
                    float compareBValue = ((FdoSingleValue*)(compareBDataValue.p))->GetSingle();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_Double:
                {
                    double compareAValue = ((FdoDoubleValue*)(compareADataValue.p))->GetDouble();
                    double compareBValue = ((FdoDoubleValue*)(compareBDataValue.p))->GetDouble();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_Decimal:
                {
                    double compareAValue = ((FdoDecimalValue*)(compareADataValue.p))->GetDecimal();
                    double compareBValue = ((FdoDecimalValue*)(compareBDataValue.p))->GetDecimal();
                    if(compareAValue < compareBValue)
                    {
                        bResult = true;
                    }
                }
                break;
            case FdoDataType_String:
                {
                    FdoStringP compareAValue = ((FdoStringValue*)(compareADataValue.p))->GetString();
                    FdoStringP compareBValue = ((FdoStringValue*)(compareBDataValue.p))->GetString();
                    if(wcscmp(compareAValue, compareBValue) < 0)
                    {
                        bResult = true;
                    }
                }
                break;
            default:
                break;
        }
    }

    return bResult;
}

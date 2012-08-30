//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "FeatureDefs.h"
#include "FeatureUtil.h"
#include "FeatureDistribution.h"
#include "FeatureNumericFunctions.h"
#include "math.h"
#include "float.h"
#include <algorithm>
#include "Matrix.h"
#include "UniqueFunction.h"
#include "DataReaderCreator.h"
#include "DoubleDataReaderCreator.h"
#include "ByteDataReaderCreator.h"
#include "Int16DataReaderCreator.h"
#include "Int32DataReaderCreator.h"
#include "Int64DataReaderCreator.h"
#include "SingleDataReaderCreator.h"
#include "StringDataReaderCreator.h"
#include "DateTimeDataReaderCreator.h"
#include "BooleanDataReaderCreator.h"

MgdFeatureNumericFunctions::MgdFeatureNumericFunctions()
{
    m_type = MgPropertyType::Null;
    m_reader = NULL;
    m_customFunction = NULL;
    m_propertyAlias = L"";
}

MgdFeatureNumericFunctions::MgdFeatureNumericFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    Initialize(reader, customFunction, propertyAlias); // Initialize the instance
}

void MgdFeatureNumericFunctions::Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    CHECKNULL((MgReader*)reader, L"MgdFeatureNumericFunctions.Initialize");
    CHECKNULL((FdoFunction*)customFunction, L"MgdFeatureNumericFunctions.Initialize");

    if(1 == reader->GetPropertyCount())
    {
        m_type = MgdFeatureUtil::GetPropertyDefinition(reader, m_propertyName);
    }
    else
    {
        // Only get the property needed
        FdoPtr<FdoExpressionCollection> exprCol = customFunction->GetArguments();
        FdoInt32 cnt = exprCol->GetCount();
        FdoPtr<FdoExpression> expr;
        if(cnt == 1)
        {
            expr = exprCol->GetItem(0);
            FdoIdentifier* propName = dynamic_cast<FdoIdentifier*>(expr.p);
            CHECKNULL(propName, L"MgdFeatureNumericFunctions.Initialize");
            m_propertyName = propName->GetName();
            m_type = reader->GetPropertyType(m_propertyName);
        }
        else
        {
            // Throw original exception
            m_type = MgdFeatureUtil::GetPropertyDefinition(reader, m_propertyName);
        }
    }

    // TODO: Should we really check this, may be we can ignore ??
    // because we can only come to here if property type is numeric
    this->CheckSupportedPropertyType();

    // We must have an property alias
    // Though we can name a property with same name as function expression
    // But Fdo forces to have an alias. Therefore we implement this restriction.
    if (propertyAlias.empty())
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgMissingPropertyAlias");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureDistribution.Initialize", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    m_reader = SAFE_ADDREF(reader);
    m_customFunction = FDO_SAFE_ADDREF(customFunction);
    m_propertyAlias = propertyAlias;
}

MgdFeatureNumericFunctions::~MgdFeatureNumericFunctions()
{
}

MgReader* MgdFeatureNumericFunctions::Execute()
{
    CHECKNULL((MgReader*)m_reader, L"MgdFeatureNumericFunctions.Execute");
    CHECKNULL(m_customFunction, L"MgdFeatureNumericFunctions.Execute");

    Ptr<MgReader> reader;
    MG_LOG_TRACE_ENTRY(L"MgdFeatureNumericFunctions::Execute");
    // TODO: Can this be optimized to process them as they are read?
    // TODO: Should we put a limit on double buffer
    INT32 funcCode = -1;
    bool supported = MgdFeatureUtil::FindCustomFunction(m_customFunction, funcCode);
    if (supported)
    {
        // In case we have int64 but is a custom function use double to evaluate it.
        // Since we don't have a type which can make operations with big numbers we will fix only Unique/Min/Max functions
        // Even if we treat int64 different from double we don't solve the issue with truncation error.
        // If we emulate SUM adding two big numbers we will get overflow even we use int64, e.g.:
        // Int64 val1 = 9223372036854775806;
        // Int64 val2 = 9223372036854775807;
        // Int64 sum = val1 + val2; // the sum value will be -3 so is overflow error
        // In the future we will need to implement a type which can handle int64 numbers without getting overflow a sum/avg is calculated.
        // Since all other functions calculate a sum we will use double, at least to be able to get the right result for small numbers.
        if (!(m_type == MgPropertyType::Int64 && (funcCode == MINIMUM || funcCode == MAXIMUM || funcCode == UNIQUE)))
        {
            VECTOR values, distValues;
            while(m_reader->ReadNext())
            {
                // TODO: Add support for Geometry extents
                double val = GetValue();
                values.push_back(val);
            }
            // Calulate the distribution on the collected values
            CalculateDistribution(values, distValues);

            // Create FeatureReader from distribution values
            reader = GetReader(distValues);
        }
        else
        {
            VECTOR_INT64 values, distValues;
            while(m_reader->ReadNext())
            {
                INT64 int64Val = 0;
                if (!m_reader->IsNull(m_propertyName))
                    int64Val = m_reader->GetInt64(m_propertyName);
                values.push_back(int64Val);
            }

            // Calulate the distribution on the collected values
            CalculateDistribution(values, distValues);

            // Create FeatureReader from distribution values
            Ptr<MgdInt64DataReaderCreator> drCreator = new MgdInt64DataReaderCreator(m_propertyAlias);
            reader = drCreator->Execute(distValues);
        }
    }
    else
    {
        // just return an emty reader
        VECTOR distValues;
        reader = GetReader(distValues);
    }

    return reader.Detach();
}

// Check whether property type is a supported type
void MgdFeatureNumericFunctions::CheckSupportedPropertyType()
{
    bool supported = false;
    switch(m_type)
    {
        case MgPropertyType::DateTime:
        case MgPropertyType::Double:
        case MgPropertyType::Byte:
        case MgPropertyType::Int16:
        case MgPropertyType::Int32:
        case MgPropertyType::Int64:
        case MgPropertyType::Single:
        case MgPropertyType::Boolean:
        {
            supported = true;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgdFeatureNumericFunctions.CheckSupportedPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return;
}

// Get the value of property
double MgdFeatureNumericFunctions::GetValue()
{
    double val = 0;

    if (!m_reader->IsNull(m_propertyName))
    {
        switch(m_type)
        {
            case MgPropertyType::Double:
            {
                val = (double)m_reader->GetDouble(m_propertyName);
                break;
            }
            case MgPropertyType::Byte:
            {
                val = (double)m_reader->GetByte(m_propertyName);
                break;
            }
            case MgPropertyType::Int16:
            {
                val = (double)m_reader->GetInt16(m_propertyName);
                break;
            }
            case MgPropertyType::Int32:
            {
                val = (double)m_reader->GetInt32(m_propertyName);
                break;
            }
            case MgPropertyType::Int64:
            {
                val = (double)m_reader->GetInt64(m_propertyName);
                break;
            }
            case MgPropertyType::Single:
            {
                val = (double)m_reader->GetSingle(m_propertyName);
                break;
            }
            case MgPropertyType::DateTime:
            {
                Ptr<MgDateTime> dateTime = m_reader->GetDateTime(m_propertyName);
                val = dateTime->ToNumber();
                break;
            }
            case MgPropertyType::Boolean:
            {
                val = (double)m_reader->GetBoolean(m_propertyName);
                break;
            }
            default:
            {
                throw new MgInvalidPropertyTypeException(L"MgdFeatureNumericFunctions.CheckSupportedPropertyType",
                    __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }
    return val;
}

// Calculate equal categories
void MgdFeatureNumericFunctions::GetEqualCategories(VECTOR &values, int numCats, double dataMin, double dataMax, VECTOR &distValues)
{
    // Expected categories should be more than zero
    if (numCats <= 0)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgInvalidComputedProperty");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureNumericFunctions::GetEqualCategories", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // find the range of the data values
    double min = DoubleMaxValue;
    double max = -DoubleMaxValue;

    int cnt = (int)values.size();
    if (cnt <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

    for (int i=0; i < cnt; i++)
    {
        double val = values[i];

        if (val > max)
            max = val;
        if (val < min)
            min = val;
    }

    // expand the range a little to account for numerical instability
    double delta = 0.0001 * (max - min);
    min -= delta;
    max += delta;
    // but don't let the values extend beyond the data min/max
    if (min < dataMin)
        min = dataMin;
    if (max > dataMax)
        max = dataMax;

    // This method ignores dataMin and dataMax.  A different "Equal" distribution
    // might ignore the actual data values and create categories based on dataMin
    // and dataMax when those values are not +/- infinity.

    // fill in the categories
    distValues.push_back(min);
    delta = (max - min) / (double)numCats;
    for (int i=1; i<numCats; i++)
    {
        double nextval = distValues[i-1] + delta;
        distValues.push_back(nextval);
    }
    distValues.push_back(max);
}


void MgdFeatureNumericFunctions::GetMinimum(VECTOR &values, VECTOR &distValues)
{
    // TODO: Change this algorithm to take reader directly instead of vector

    // find the range of the data values
    distValues.push_back(MgdFeatureUtil::Minimum(values));
}


void MgdFeatureNumericFunctions::GetMinimum(VECTOR_INT64 &values, VECTOR_INT64 &distValues)
{
    // TODO: Change this algorithm to take reader directly instead of vector

    // find the range of the data values
    distValues.push_back(MgdFeatureUtil::Minimum(values));
}


void MgdFeatureNumericFunctions::GetMaximum(VECTOR &values, VECTOR &distValues)
{
    // TODO: Change this algorithm to take reader directly instead of vector

    // find the range of the data values
    distValues.push_back(MgdFeatureUtil::Maximum(values));
}


void MgdFeatureNumericFunctions::GetMaximum(VECTOR_INT64 &values, VECTOR_INT64 &distValues)
{
    // TODO: Change this algorithm to take reader directly instead of vector

    // find the range of the data values
    distValues.push_back(MgdFeatureUtil::Maximum(values));
}


void MgdFeatureNumericFunctions::CalculateDistribution(VECTOR& values, VECTOR& distValues)
{
    STRING funcName;
    int numCats;
    double dataMin, dataMax;
    INT32 funcCode = -1;

    // Get the arguments from the FdoFunction
    STRING propertyName;
    bool supported = MgdFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case EQUAL_CATEGORY: // Equal Category
            {
                MgdFeatureUtil::GetArguments(m_customFunction, propertyName, numCats, dataMin, dataMax, m_type);
                GetEqualCategories(values, numCats, dataMin, dataMax, distValues);
                break;
            }
            case STDEV_CATEGORY: // StdDev Category
            {
                MgdFeatureUtil::GetArguments(m_customFunction, propertyName, numCats, dataMin, dataMax, m_type);
                GetStandardDeviationCategories(values, numCats, dataMin, dataMax, distValues);
                break;
            }
            case QUANTILE_CATEGORY: // Quantile Category
            {
                MgdFeatureUtil::GetArguments(m_customFunction, propertyName, numCats, dataMin, dataMax, m_type);
                GetQuantileCategories(values, numCats, dataMin, dataMax, distValues);
                break;
            }
            case JENK_CATEGORY: // Jenk Category
            {
                MgdFeatureUtil::GetArguments(m_customFunction, propertyName, numCats, dataMin, dataMax, m_type);
                GetJenksCategories(values, numCats, dataMin, dataMax, distValues);
                break;
            }
            case MINIMUM:
            {
                GetMinimum(values,distValues);
                break;
            }
            case MAXIMUM:
            {
                GetMaximum(values,distValues);
                break;
            }
            case MEAN:
            {
                GetMeanValue(values,distValues);
                break;
            }
            case STANDARD_DEV:
            {
                GetStandardDeviation(values,distValues);
                break;
            }
            case UNIQUE:
            {
                MgdUniqueFunction<double>::Execute(values, distValues);
                break;
            }
        }
    }
}

void MgdFeatureNumericFunctions::CalculateDistribution(VECTOR_INT64& values, VECTOR_INT64& distValues)
{
    INT32 funcCode = -1;
    // Get the arguments from the FdoFunction
    bool supported = MgdFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case MINIMUM:
            {
                GetMinimum(values, distValues);
                break;
            }
            case MAXIMUM:
            {
                GetMaximum(values, distValues);
                break;
            }
            case UNIQUE:
            {
                MgdUniqueFunction<INT64>::Execute(values, distValues);
                break;
            }
        }
    }
}

MgReader* MgdFeatureNumericFunctions::GetReader(VECTOR& distValues)
{
    Ptr<MgDataReader> dataReader;

    switch(m_type)
    {
        case MgPropertyType::Double:
        {
            Ptr<MgdDoubleDataReaderCreator> drCreator = new MgdDoubleDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Byte:
        {
            Ptr<MgdByteDataReaderCreator> drCreator = new MgdByteDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Int16:
        {
            Ptr<MgdInt16DataReaderCreator> drCreator = new MgdInt16DataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Int32:
        {
            Ptr<MgdInt32DataReaderCreator> drCreator = new MgdInt32DataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Int64:
        {
            Ptr<MgdInt64DataReaderCreator> drCreator = new MgdInt64DataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Single:
        {
            Ptr<MgdSingleDataReaderCreator> drCreator = new MgdSingleDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::DateTime:
        {
            Ptr<MgdDateTimeDataReaderCreator> drCreator = new MgdDateTimeDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        case MgPropertyType::Boolean:
        {
            Ptr<MgdBooleanDataReaderCreator> drCreator = new MgdBooleanDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgdFeatureNumericFunctions.CheckSupportedPropertyType",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return dataReader.Detach();
}

//MgDataReader* MgdFeatureNumericFunctions::GetStringReader(std::vector<STRING>& distValues)
//{
//    MgdDoubleDataReaderCreator* drCreator = new MgdDoubleDataReaderCreator(m_propertyAlias);
//    Ptr<MgDataReader> dataReader = drCreator->Execute(distValues);
//    delete drCreator;
//
//    return dataReader.Detach();
//}

// Calculate Standard Deviation for the values
void MgdFeatureNumericFunctions::GetStandardDeviationCategories( VECTOR &values, int numCats,
                                                                double dataMin, double dataMax,
                                                                VECTOR &distValues)
{
    // Expected categories should be more than zero
    if (numCats <= 0)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgInvalidComputedProperty");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureNumericFunctions::GetStandardDeviationCategories", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // collect information about the data values
    double min = DoubleMaxValue;
    double max = -DoubleMaxValue;
    double mean = 0;

    int cnt = (int)values.size();
    if (cnt <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

    for (int i=0; i < cnt; i++)
    {
        double val = values[i];

        if (val > max)
            max = val;
        if (val < min)
            min = val;
        mean += val;
    }

    // expand min and max a little to account for numerical instability
    double delta = 0.0001 * (max - min);
    min -= delta;
    max += delta;

    // compute the mean, variance and standard deviation
    double count = (double)cnt;  // (guaranteed to be > 0)
    mean /= count;
    double variance = 0;

    for (int i=0; i < cnt; i++)
    {
        double val = values[i];
        variance += (val - mean) * (val - mean);
    }

    double deviation = sqrt(variance / count);

    // fill in the middle category/categories
    double* cats = new double[numCats+1];
    int midCat, highMidCat;
    if (numCats % 2 == 0)
    {
        midCat = numCats / 2;
        highMidCat = midCat;
        cats[midCat] = mean;
    }
    else
    {
        midCat = (numCats - 1) / 2;
        highMidCat = midCat + 1;
        cats[midCat] = mean - 0.5 * deviation;
        cats[highMidCat] = mean + 0.5 * deviation;
    }

    // fill in the other categories
    for (int i=midCat-1; i>=0; i--)
        cats[i] = cats[i+1] - deviation;

    for (int i=highMidCat; i<=numCats; i++)
        cats[i] = cats[i-1] + deviation;

    // if the data method specifies strict a strict min and/or max, use them
    if (!IsInf(dataMin) && !IsNan(dataMin) && (dataMin != -DoubleMaxValue))
        min = dataMin;
    if (!IsInf(dataMax) && !IsNan(dataMax) && (dataMax != DoubleMaxValue))
        max = dataMax;

    // flatten/clip any categories that extend beyond the min/max range
    for (int i=0; i<=numCats; i++)
    {
        if (cats[i] < min)
            cats[i] = min;
        else if (cats[i] > max)
            cats[i] = max;
    }

    for (int kk = 0; kk < numCats+1; kk++)
    {
        distValues.push_back(cats[kk]);
    }

    delete[] cats; // Delete the memory allocated before
}


// Calculate Quantile Distribution for the values
void MgdFeatureNumericFunctions::GetQuantileCategories(  VECTOR &values, int numCats,
                                                    double dataMin, double dataMax,
                                                    VECTOR &distValues )
{
    // Expected categories should be more than zero
    if (numCats <= 0)
    {
        STRING message = MgdFeatureUtil::GetMessage(L"MgInvalidComputedProperty");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgdFeatureNumericFunctions::GetQuantileCategories", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    int count = (int)values.size();
    if (count <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

    // Sort the data values in ascending order
    std::sort(values.begin(), values.end());

    // How many go into each full bucket?

    int perBucket = ROUND((double)count/(double)numCats);
    if (perBucket * numCats > count)
        perBucket--;

    // How many buckets are full, and how many are missing one?
    int nearlyFullBuckets = numCats - (count - perBucket * numCats);
    int fullBuckets = numCats - nearlyFullBuckets;

    // expand min and max a little to account for numerical instability
    double delta = 0.0001 * (values[count-1] - values[0]);
    double* categories = new double[numCats+1];

    // the first and last categories are limited by the data method limits
    categories[0] = values[0] - delta;
    if (categories[0] < dataMin)
        categories[0] = dataMin;

    categories[numCats] = values[count-1] + delta;
    if (categories[numCats] > dataMax)
        categories[numCats] = dataMax;

    // Mix full and nearly-full buckets to fill in the categories between the ends.
    int indexOfLast = -1;
    for ( int i = 1; i<numCats; i++)
    {
        bool doingSmallBucket = (nearlyFullBuckets > fullBuckets);
        // find the index of the last element we want in this bucket
        indexOfLast += (doingSmallBucket ? perBucket : perBucket + 1);
        // make category value be halfway between that element and the next
        categories[i] = 0.5 * (values[indexOfLast] + values[indexOfLast+1]);

        // Decrement count of correct bucket type.
        if (doingSmallBucket)
            nearlyFullBuckets--;
        else
            fullBuckets--;
    }

    for (int kk = 0; kk < numCats+1; kk++)
    {
        distValues.push_back(categories[kk]);
    }

    delete[] categories; // Delete the memory allocated before
}


bool MgdFeatureNumericFunctions::IsInf(double x)
{
    bool isInfinity = false;
#ifdef _WIN32
    int code = _fpclass(x);
    if ((code == _FPCLASS_NINF) || (code == _FPCLASS_PINF))
    {
        isInfinity = true;
    }
#else
    isInfinity = isinf(x);
#endif

    return isInfinity;
}

bool MgdFeatureNumericFunctions::IsNan(double x)
{
    bool isNan = false;
#ifdef _WIN32
    isNan = _isnan(x);
#else
    isNan = isnan(x);
#endif
    return isNan;
}

// Calculate Standard Deviation for the values
void MgdFeatureNumericFunctions::GetStandardDeviation(VECTOR &values, VECTOR &distValues)
{
    double mean = 0;

    int cnt = (int)values.size();
    if (cnt <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

    double min = DoubleMaxValue;
    double max = -DoubleMaxValue;

    for (int i=0; i < cnt; i++)
    {
        double val = values[i];

        if (val > max)
            max = val;

        if (val < min)
            min = val;

        mean += val;
    }

    // expand min and max a little to account for numerical instability
    double delta = 0.0001 * (max - min);
    min -= delta;
    max += delta;

    // compute the mean, variance and standard deviation
    double count = (double)cnt;  // (guaranteed to be > 0)
    mean /= count;
    double variance = 0;

    for (int i=0; i < cnt; i++)
    {
        variance += (values[i] - mean) * (values[i] - mean);
    }

    double deviation = sqrt((double)(variance / count));

    // Set the base date as min date
    if (m_type == MgPropertyType::DateTime)
    {
        deviation += min;
    }

    distValues.push_back(deviation);

    return;
}

// Calculate average
void MgdFeatureNumericFunctions::GetMeanValue(VECTOR &values, VECTOR &distValues)
{
    double mean = 0;

    int cnt = (int)values.size();
    if (cnt <= 0) { return; } // Nothing to do, we just send back Property Definition to clients from reader

    for (int i=0; i < cnt; i++)
    {
        double val = values[i];
        mean += val;
    }

    // compute the mean, variance and standard deviation
    double count = (double)cnt;  // (guaranteed to be > 0)
    mean /= count;

    distValues.push_back(mean);

    return;
}

// Calculate average
void MgdFeatureNumericFunctions::GetUniqueValue(VECTOR &values, VECTOR &distValues)
{
    MgdUniqueFunction<double>::Execute(values, distValues);
}

void MgdFeatureNumericFunctions::GetUniqueValue(VECTOR_INT64 &values, VECTOR_INT64 &distValues)
{
    MgdUniqueFunction<INT64>::Execute(values, distValues);
}

//-------------------------------------------------------------------------
// Jenks' Optimization Method
//
//-------------------------------------------------------------------------
void MgdFeatureNumericFunctions::GetJenksCategories(  VECTOR &inputData, int numPartsRequested,
                                                 double dataMin, double dataMax,
                                                 VECTOR &distValues )
{
    // numPartsRequested // 2 - 10; 5 is good
    int i = 0;  // index for numObservations (may be very large)
    int j = 0;  // index for numPartsRequested (about 4-8)
    int k = 0;

    // Sort the data values in ascending order
    std::sort(inputData.begin(), inputData.end());

    int numObservations = (int)inputData.size();  // may be very large
    // Possible improvement: Rework the code to use normal 0 based arrays.
    // Actually it doesn't matter much since we have to create two
    // matrices that themselves use more memory than the local copy
    // of inputData.
    //
    // In order to ease the use of original FORTRAN and the later BASIC
    // code, I will use 1 origin arrays and copy the inputData into
    // a local array;
    // I'll dimension the arrays one larger than necessary and use the
    // index values from the original code.
    //
    // The algorithm must calculate with floating point values.
    // If more optimization is attempted in the future, be aware of
    // problems with calculations using mixed numeric types.

    std::vector<double> data;
    data.push_back(0);  // dummy value at index 0
    std::copy(inputData.begin(), inputData.end(), std::back_inserter(data));
    // copy from parameter inputData so that data index starts from 1

    // Note that the Matrix constructors initialize all values to 0.
    // mat1 contains integer values used for indices into data
    // mat2 contains floating point values of data and bigNum
    MgdMatrix<int>     mat1(numObservations + 1, numPartsRequested + 1);
    MgdMatrix<double>  mat2(numObservations + 1, numPartsRequested + 1);

//  const double bigNum = 1e+14; // from original BASIC code;
//  const double bigNum = std::numeric_limits<double>::max();
    const double bigNum = DBL_MAX;   // compiler's float.h

    for (i = 1; i <= numPartsRequested; ++i)
    {
        mat1.Set(1, i, 1);
        for (j = 2; j <= numObservations; ++j)
        {
            mat2.Set(j, i, bigNum);
        }
    }

    std::vector<int> classBounds;
    classBounds.push_back(-2);  // dummy value
    for (i = 1; i <= numPartsRequested; ++i)
    {
        classBounds.push_back(-1);
    }

    for (int L = 2; L <= numObservations; ++L)
    {
        double s1 = 0;
        double s2 = 0;
        double v = 0;
        int w = 0;

        for (int m = 1; m <= L; ++m)
        {
            int i3 = L - m + 1;
            double val = data[i3];
            s2 += (double(val) * double(val));
            // if datatype of val is ever allowed to be same as template
            // parameter T, make sure multiplication is done in double.
            s1 += val;
            ++w;
            v = s2 - ((s1 * s1) / w);
            int i4 = i3 - 1;

            if (i4 > 0)
            {
                for (j = 2; j <= numPartsRequested; ++j)
                {
                    double tempnum = v + mat2.Get(i4, j - 1);
                    if (double(mat2.Get(L, j)) >= tempnum)
                    {
                        mat1.Set(L, j, i3);
                        mat2.Set(L, j, tempnum);
                    }
                }
            }
        }

        mat1.Set(L, 1, 1);
        mat2.Set(L, 1, v);
    }

    k = numObservations;
    for (j = numPartsRequested; j >= 1; --j)
    {
        if (k >= 0 && k <= numObservations)
        {
            classBounds[j] = mat1.Get(k, j);
            k = mat1.Get(k, j) - 1;
        }
    }

    std::vector<int> indices;

    indices.push_back(0);
    for (i = 2; i <= numPartsRequested; ++i)
    {
        int index = classBounds[i] - 1;
        if (index > indices.back())
        {
            indices.push_back(index);
        }
    }

    FixGroups(inputData, indices);

    double val = 0.0;
    int index = 0;
    int totIndex = (int)indices.size();

    for (int i = 1; i < totIndex; ++i)
    {
        index = indices[i] - 1;

        val = inputData[index];
        distValues.push_back(val);
    }

    index = numObservations - 1;
    val = inputData[index];
    distValues.push_back(val);

    int retCnt = (int)distValues.size();
    int inCnt = (int)inputData.size();

    if (retCnt > 0 && inCnt > 0)
    {
        if (!doubles_equal(distValues[0],inputData[0]))
        {
            distValues.insert(distValues.begin(), inputData[0]);
        }
    }

    return;
}

//-----------------------------------------------------------------------------
// FixGroups
// If the algorithm generated empty groups, then adjust the number of groups
// and the indices.
// Return true if adjustments we made;
// Return false if no changes were made.
bool MgdFeatureNumericFunctions::FixGroups(const std::vector<double>& data, std::vector<int>& indices)
{
    bool changed1 = FixDuplicateIndices(indices);
    bool changed2 = FixIndicesByValue(data, indices);

    return changed1 || changed2;
}


//-----------------------------------------------------------------------------
bool MgdFeatureNumericFunctions::FixDuplicateIndices(std::vector<int>& indices)
{
    if (indices.size() <= 1)
    {
        return false;
    }

    // We will create a new vector of indices that is possibly adjusted,
    // then replace the original vector with the new.  Since these vectors
    // have typically less than 10 entries, we won't deal with making the
    // adjustments in place.
    std::vector<int> newIndices;

    newIndices.push_back(indices[0]);

    for (unsigned int i = 1; i < indices.size(); ++i)
    {
        if (indices[i] != indices[i - 1])
            newIndices.push_back(indices[i]);
    }

    bool changed = (newIndices.size() != indices.size());
    indices.clear();
    indices = newIndices;

    return changed;
}


//-----------------------------------------------------------------------------
// Examine the values specified by the indices.  If any of the values
// are identical, then toss out the higher index.
bool MgdFeatureNumericFunctions::FixIndicesByValue(const std::vector<double>& data, std::vector<int>& indices)
{
    if (indices.size() <= 1)
    {
        return false;
    }

    // We will create a new vector of indices that is possibly adjusted,
    // then replace the original vector with the new.  Since these vectors
    // have typically less than 10 entries, we won't deal with making the
    // adjustments in place.
    std::vector<int> newIndices;

    newIndices.push_back(indices[0]);

    for (unsigned int i = 1; i < indices.size(); ++i)
    {
        if ( ! doubles_equal(data[indices[i]], data[indices[i - 1]]) )
            newIndices.push_back(indices[i]);
    }

    bool changed = (newIndices.size() != indices.size());
    indices.clear();
    indices = newIndices;

    return changed;
}

bool MgdFeatureNumericFunctions::doubles_equal(double d1, double d2)
{
    // We are doing our comparisons with a specific precision.
    const double epsilon = 1.0e-12;  // very small
    double d = fabs(d2 - d1);
    return (d < epsilon);  // absolute value of difference less than tolerance
}

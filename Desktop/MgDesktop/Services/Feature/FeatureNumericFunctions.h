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

#ifndef MG_FEATURE_NUMERIC_FUNCTIONS_H_
#define MG_FEATURE_NUMERIC_FUNCTIONS_H_

class MgDisposable;
class FdoFunction;
class MgReader;
class MgFeatureDistribution;

class MgFeatureNumericFunctions : public MgFeatureDistribution
{
    DECLARE_CLASSNAME(MgFeatureNumericFunctions)

public:

    MgFeatureNumericFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);
    virtual ~MgFeatureNumericFunctions();
    virtual MgReader* Execute();
    virtual void Dispose()
    {
        delete this;
    }

protected:

    MgFeatureNumericFunctions();

private:

    void Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);

    void CalculateDistribution(VECTOR& values, VECTOR& distValues);
    // do not call this method for other functions than MINIMUM, MAXIMUM, UNIQUE
    void CalculateDistribution(VECTOR_INT64& values, VECTOR_INT64& distValues);
    MgReader* GetReader(VECTOR& distValues);


    // Distribution methods
    void GetEqualCategories(VECTOR &values, int numCats, double dataMin, double dataMax, VECTOR &distValues);
    void GetStandardDeviationCategories(VECTOR &values, int numCats, double dataMin, double dataMax, VECTOR &distValues);
    void GetQuantileCategories(VECTOR &values, int numCats, double dataMin, double dataMax, VECTOR &distValues );

    // Jenk methods
    void GetJenksCategories(VECTOR &inputData, int numPartsRequested, double dataMin, double dataMax, VECTOR &distValues);
    bool FixGroups(const std::vector<double>& data, std::vector<int>& indices);
    bool FixDuplicateIndices(std::vector<int>& indices);
    bool FixIndicesByValue(const std::vector<double>& data, std::vector<int>& indices);
    bool doubles_equal(double d1, double d2);

    // Single row methods
    void GetMeanValue(VECTOR &values, VECTOR &distValues);
    void GetStandardDeviation(VECTOR &values, VECTOR &distValues);
    void GetUniqueValue(VECTOR &values, VECTOR &distValues);
    void GetUniqueValue(VECTOR_INT64 &values, VECTOR_INT64 &distValues);
    void GetMinimum(VECTOR &values, VECTOR &distValues);
    void GetMinimum(VECTOR_INT64 &values, VECTOR_INT64 &distValues);
    void GetMaximum(VECTOR &values, VECTOR &distValues);
    void GetMaximum(VECTOR_INT64 &values, VECTOR_INT64 &distValues);

    // Utility methods
    bool IsInf(double x);
    bool IsNan(double x);

    void CheckSupportedPropertyType();
    double GetValue();

private:

    STRING m_propertyName;
    INT16 m_type;
    Ptr<MgReader> m_reader;
    FdoPtr<FdoFunction> m_customFunction;
    STRING m_propertyAlias;
};

#endif

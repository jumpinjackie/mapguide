//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MG_FEATURE_STRING_FUNCTIONS_H_
#define MG_FEATURE_STRING_FUNCTIONS_H_

class MgDisposable;
class FdoFunction;
class MgReader;
class MgFeatureDistribution;

class MgFeatureStringFunctions : public MgFeatureDistribution
{
    DECLARE_CLASSNAME(MgFeatureStringFunctions)

public:

    MgFeatureStringFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);
    virtual MgReader* Execute();

protected:

    MgFeatureStringFunctions();
    virtual ~MgFeatureStringFunctions();
    virtual void Dispose() { delete this; }

private:

    void GetValue(REFSTRING val);
    void Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);

    void ExecuteOperation(std::map<STRING, char>& values, std::vector<STRING>& distValues);
    MgReader* GetReader(std::vector<STRING>& distValues);

    void CheckSupportedPropertyType();

private:

    STRING m_propertyName;
    INT16 m_type;
    Ptr<MgReader> m_reader;
    FdoPtr<FdoFunction> m_customFunction;
    STRING m_propertyAlias;
};

#endif

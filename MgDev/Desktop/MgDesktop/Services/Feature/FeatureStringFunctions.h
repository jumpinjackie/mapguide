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

#ifndef MG_FEATURE_STRING_FUNCTIONS_H_
#define MG_FEATURE_STRING_FUNCTIONS_H_

class MgDisposable;
class FdoFunction;
class MgReader;
class MgdFeatureDistribution;

class MgdFeatureStringFunctions : public MgdFeatureDistribution
{
    DECLARE_CLASSNAME(MgdFeatureStringFunctions)

public:

    MgdFeatureStringFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);
    virtual MgReader* Execute();

protected:

    MgdFeatureStringFunctions();
    virtual ~MgdFeatureStringFunctions();
    virtual void Dispose()
    {
        delete this;
    }

private:

    void GetValue(REFSTRING val);
    void Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias);

    void ExecuteOperation(std::map<STRING, char>& values, std::vector<STRING>& distValues);
    MgReader* GetReader(std::vector<STRING>& distValues);

	//WTF: The original method was void and threw MgInvalidPropertyTypeException if not supported
	//MSVC 2008 blows up on the linker phase with a combination of C1001 at this method and LNK1000
	//
	//Changing this method to return bool (supported = true) and throwing MgInvalidPropertyTypeException in
	//the method that calls this (Initialize()) makes the problem go away. Real WTF there!
    bool CheckSupportedPropertyType();

private:

    STRING m_propertyName;
    INT16 m_type;
    Ptr<MgReader> m_reader;
    FdoPtr<FdoFunction> m_customFunction;
    STRING m_propertyAlias;
};

#endif

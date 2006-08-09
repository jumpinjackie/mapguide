//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureUtil.h"
#include "FeatureDistribution.h"
#include "FeatureStringFunctions.h"
#include "ProxyFeatureReader.h"
#include "ProxyDataReader.h"
#include "DataReaderCreator.h"
#include "StringDataReaderCreator.h"
#include <algorithm>
#include "UniqueFunction.h"

MgFeatureStringFunctions::MgFeatureStringFunctions()
{
    m_type = MgPropertyType::Null;
    m_reader = NULL;
    m_customFunction = NULL;
    m_propertyAlias = L"";
}

MgFeatureStringFunctions::MgFeatureStringFunctions(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    Initialize(reader, customFunction, propertyAlias); // Initialize the instance
}

void MgFeatureStringFunctions::Initialize(MgReader* reader, FdoFunction* customFunction, CREFSTRING propertyAlias)
{
    CHECKNULL((MgReader*)reader, L"MgFeatureDistribution.Initialize");
    CHECKNULL((FdoFunction*)customFunction, L"MgFeatureDistribution.Initialize");

    m_type = MgServerFeatureUtil::GetPropertyDefinition(reader, m_propertyName);

    // TODO: Should we really check this, may be we can ignore ??
    // because we can only come to here if property type is numeric
    this->CheckSupportedPropertyType();

    // We must have an property alias
    // Though we can name a property with same name as function expression
    // But Fdo forces to have an alias. Therefore we implement this restriction.
    if (propertyAlias.empty())
    {
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingPropertyAlias");

        MgStringCollection arguments;
        arguments.Add(message);
        throw new MgFeatureServiceException(L"MgFeatureDistribution.Initialize", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    m_reader = SAFE_ADDREF(reader);
    m_customFunction = GIS_SAFE_ADDREF(customFunction);
    m_propertyAlias = propertyAlias;
}


MgFeatureStringFunctions::~MgFeatureStringFunctions()
{
    if (m_customFunction != NULL)
    {
        m_customFunction->Release();
    }
}

// Execute the function
MgReader* MgFeatureStringFunctions::Execute()
{
    CHECKNULL((MgReader*)m_reader, L"MgFeatureStringFunctions.Execute");
    CHECKNULL(m_customFunction, L"MgFeatureStringFunctions.Execute");

    std::vector<STRING> v2;

    MG_LOG_TRACE_ENTRY(L"MgFeatureStringFunctions::Execute");
    // TODO: Should we put a limit on double buffer

    std::map<STRING, char> mMap;
    typedef std::pair<STRING, char> String_Pair;

    while(m_reader->ReadNext())
    {
        STRING val = L"";

        // Get the value
        GetValue(val);

        // Insert the value into the std::map
        mMap.insert( String_Pair(val, '7') );  // 7 is just arbitrary placeholder
    }

    // Execute the operation on the collected values
    ExecuteOperation(mMap, v2);

    // Create FeatureReader from distribution values
    Ptr<MgReader> reader = GetReader(v2);

    return SAFE_ADDREF((MgReader*)reader);
}


// Check whether property type is a supported type
void MgFeatureStringFunctions::CheckSupportedPropertyType()
{
    bool supported = false;
    switch(m_type)
    {
        case MgPropertyType::String:
        {
            supported = true;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgFeatureStringFunctions.CheckSupportedPropertyType", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return;
}


// Get the value of property
void MgFeatureStringFunctions::GetValue(REFSTRING val)
{
    if (!m_reader->IsNull(m_propertyName))
    {
        switch(m_type)
        {
            case MgPropertyType::String:
            {
                val = m_reader->GetString(m_propertyName);
                break;
            }
            default:
            {
                throw new MgInvalidPropertyTypeException(
                    L"MgFeatureStringFunctions.GetValue", __LINE__, __WFILE__, NULL, L"", NULL);
            }
        }
    }
}


// Execute the function
void MgFeatureStringFunctions::ExecuteOperation(std::map<STRING, char>& values, std::vector<STRING>& distValues)
{
   INT32 funcCode = -1;

    // Get the arguments from the FdoFunction
    STRING propertyName;
    bool supported = MgServerFeatureUtil::FindCustomFunction(m_customFunction, funcCode);

    if (supported)
    {
        switch(funcCode)
        {
            case UNIQUE:
            {
                distValues.reserve(values.size());
                std::map<STRING, char>::iterator iter2;
                for (iter2 = values.begin(); iter2 != values.end(); iter2++)
                    distValues.push_back(iter2->first);
                break;
            }
            default:
            {
                STRING message = MgServerFeatureUtil::GetMessage(L"MgCustomFunctionNotSupported");

                MgStringCollection arguments;
                arguments.Add(message);
                throw new MgFeatureServiceException(
                    L"MgServerSelectFeatures.ExecuteOperation", __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }
}

// Create the reader for string properties
MgReader* MgFeatureStringFunctions::GetReader(std::vector<STRING>& distValues)
{
    Ptr<MgDataReader> dataReader = (MgDataReader*)NULL;

    switch(m_type)
    {
        case MgPropertyType::String:
        {
            Ptr<MgStringDataReaderCreator> drCreator = new MgStringDataReaderCreator(m_propertyAlias);
            dataReader = drCreator->Execute(distValues);
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(
                L"MgFeatureStringFunctions.GetReader", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }
    return SAFE_ADDREF((MgDataReader*)dataReader);
}

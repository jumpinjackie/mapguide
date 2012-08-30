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

#ifndef MG_DATA_READER_CREATOR_H
#define MG_DATA_READER_CREATOR_H

using namespace std;

#include <vector>
#include "MgDesktop.h"
#include "ProxyDataReader.h"

/// \cond INTERNAL
template <typename T>
class MgdDataReaderCreator : public MgDisposable
{
public:

    MgDataReader* Execute(const std::vector<double>& in)
    {
        // We convert to appropriate types here
        std::vector<T> values;
        ConvertVector(in, values);

        // Use the converted vector
        Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetPropertyDefinitions();
        Ptr<MgBatchPropertyCollection> bpCol = this->GetBatchCollection(values);
        return new MgdProxyDataReader(bpCol, propDefCol);
    }

    MgDataReader* Execute(const std::vector<INT64>& in)
    {
        // We convert to appropriate types here
        std::vector<T> values;
        ConvertVector(in, values);

        // Use the converted vector
        Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetPropertyDefinitions();
        Ptr<MgBatchPropertyCollection> bpCol = this->GetBatchCollection(values);
        return new MgdProxyDataReader(bpCol, propDefCol);
    }

    MgDataReader* Execute(const std::vector<STRING>& in)
    {
        Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetPropertyDefinitions();
        Ptr<MgBatchPropertyCollection> bpCol = this->GetBatchCollection(in);
        return new MgdProxyDataReader(bpCol, propDefCol);
    }

    MgDataReader* Execute(MgGeometryCollection* in)
    {
        Ptr<MgPropertyDefinitionCollection> propDefCol = this->GetPropertyDefinitions();
        Ptr<MgBatchPropertyCollection> bpCol = this->GetBatchCollection(in);
        return new MgdProxyDataReader(bpCol, propDefCol);
    }

    MgPropertyDefinitionCollection* GetPropertyDefinitions()
    {
        // Create property definition for the type specified
        Ptr<MgPropertyDefinition> propDef = new MgPropertyDefinition(m_propertyAlias, m_propType);

        // Add to property definition collection
        Ptr<MgPropertyDefinitionCollection> propDefCol = new MgPropertyDefinitionCollection();
        propDefCol->Add(propDef);

        return propDefCol.Detach();
    }

    MgBatchPropertyCollection* GetBatchCollection(MgGeometryCollection* in)
    {
        // Create collection of properties
        Ptr<MgBatchPropertyCollection> bpCol = new MgBatchPropertyCollection();

        if (in != NULL)
        {
            INT32 cnt = in->GetCount();
            for (INT32 i=0; i < cnt; i++)
            {
                // Add one value as one Feature (PropertyCollection)
                Ptr<MgGeometry> geom = in->GetItem(i);
                Ptr<MgProperty> prop = this->GetProperty(geom);
                Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
                propCol->Add(prop);     // Feature defined
                bpCol->Add(propCol);    //  Add to set of Batch Collection
            }
        }

        return bpCol.Detach();
    }

    MgBatchPropertyCollection* GetBatchCollection(const std::vector<T>& in)
    {
        int cnt = (int)in.size();

        // Create collection of properties
        Ptr<MgBatchPropertyCollection> bpCol = new MgBatchPropertyCollection();
        for (int i=0; i < cnt; i++)
        {
            // Add one value as one Feature (PropertyCollection)
            Ptr<MgProperty> prop = this->GetProperty((T)in[i]);
            Ptr<MgPropertyCollection> propCol = new MgPropertyCollection();
            propCol->Add(prop);     // Feature defined
            bpCol->Add(propCol);    //  Add to set of Batch Collection
        }

        return bpCol.Detach();
    }

    void ConvertVector(const std::vector<double>& in, std::vector<T>& values)
    {
        // There will always be only few values and therefore copy should be fine.
        int cnt = (int)in.size();
        for(int i = 0; i < cnt; i++)
        {
            values.push_back((T)in[i]);
        }
    }

    void ConvertVector(const std::vector<INT64>& in, std::vector<T>& values)
    {
        // There will always be only few values and therefore copy should be fine.
        int cnt = (int)in.size();
        for(int i = 0; i < cnt; i++)
        {
            values.push_back((T)in[i]);
        }
    }

    // The following GetProperty methods must be overridden by derived class
    virtual MgProperty* GetProperty(double val)     {return NULL;}
    virtual MgProperty* GetProperty(float val)      {return NULL;}
    virtual MgProperty* GetProperty(INT8 val)      {return NULL;}
    virtual MgProperty* GetProperty(INT16 val)      {return NULL;}
    virtual MgProperty* GetProperty(INT32 val)      {return NULL;}
    virtual MgProperty* GetProperty(INT64 val)      {return NULL;}
    virtual MgProperty* GetProperty(CREFSTRING val) {return NULL;}
    virtual MgProperty* GetProperty(MgGeometry* val) {return NULL;}

protected:
    virtual void Dispose()
    {
        delete this;
    }

    MgdDataReaderCreator()
    {
        m_propType = MgPropertyType::Null;
    }
    ~MgdDataReaderCreator()
    {
    }

    STRING m_propertyAlias;
    int m_propType;
};
/// \endcond
#endif

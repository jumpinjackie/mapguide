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

#ifndef DATACONFIGURATION_H_
#define DATACONFIGURATION_H_

#include "../MdfRootObject.h"
#include "PropertyMapping.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The DataConfiguration class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API DataConfiguration : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    DataConfiguration();
    virtual ~DataConfiguration();

    // Operations
    const MdfString& GetResourceId() const;
    void SetResourceId(const MdfString& resourceId);

    const MdfString& GetFeatureClass() const;
    void SetFeatureClass(const MdfString& featureClass);

    const MdfString& GetGeometry() const;
    void SetGeometry(const MdfString& geometry);

    const MdfString& GetFilter() const;
    void SetFilter(const MdfString& filter);

    PropertyMappingCollection* GetPropertyMappings();

private:
    // Hidden copy constructor and assignment operator.
    DataConfiguration(const DataConfiguration&);
    DataConfiguration& operator=(const DataConfiguration&);

    // Data Members
    MdfString m_resourceId;
    MdfString m_featureClass;
    MdfString m_geometry;
    MdfString m_filter;
    PropertyMappingCollection m_propertyMappings;
};

// Inline Methods
inline const MdfString& DataConfiguration::GetResourceId() const
{
    return m_resourceId;
}

inline const MdfString& DataConfiguration::GetFeatureClass() const
{
    return m_featureClass;
}

inline const MdfString& DataConfiguration::GetGeometry() const
{
    return m_geometry;
}

inline const MdfString& DataConfiguration::GetFilter() const
{
    return m_filter;
}

inline PropertyMappingCollection* DataConfiguration::GetPropertyMappings()
{
    return &m_propertyMappings;
}

END_NAMESPACE_MDFMODEL
#endif // DATACONFIGURATION_H_

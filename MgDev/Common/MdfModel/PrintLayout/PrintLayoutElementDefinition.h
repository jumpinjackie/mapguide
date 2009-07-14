//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef PRINTLAYOUTELEMENTDEFINITION_H_
#define PRINTLAYOUTELEMENTDEFINITION_H_

#include "MdfRootObject.h"
#include "StringObject.h"
#include "Extent3D.h"
#include "StylizationConfiguration.h"
#include "DataConfiguration.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The PrintLayoutElementDefinition class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API PrintLayoutElementDefinition : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    PrintLayoutElementDefinition();
    virtual ~PrintLayoutElementDefinition();

    // Operations
    const MdfString& GetName() const;
    void SetName(const MdfString& name);

    StringObjectCollection* GetReferences();

    const MdfString& GetDescription() const;
    void SetDescription(const MdfString& description);

    const MdfString& GetUnits() const;
    void SetUnits(const MdfString& units);

    const MdfString& GetResourceId() const;
    void SetResourceId(const MdfString& resourceId);

    Extent3D* GetExtent();

    StylizationConfiguration* GetStylizationConfiguration();

    DataConfiguration* GetDataConfiguration();

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutElementDefinition(const PrintLayoutElementDefinition&);
    PrintLayoutElementDefinition& operator=(const PrintLayoutElementDefinition&);

    // Data Members
    MdfString m_name;
    StringObjectCollection m_references;
    MdfString m_description;
    MdfString m_units;  
    MdfString m_resourceId;
    Extent3D m_extent;
    StylizationConfiguration m_stylizationConfiguration;
    DataConfiguration m_dataConfiguration;
};

// Inline Methods
inline const MdfString& PrintLayoutElementDefinition::GetName() const
{
    return m_name;
}

inline StringObjectCollection* PrintLayoutElementDefinition::GetReferences()
{
    return &m_references;
}

inline const MdfString& PrintLayoutElementDefinition::GetDescription() const
{
    return m_description;
}

inline const MdfString& PrintLayoutElementDefinition::GetUnits() const
{
    return m_units;
}

inline const MdfString& PrintLayoutElementDefinition::GetResourceId() const
{
    return m_resourceId;
}

inline Extent3D* PrintLayoutElementDefinition::GetExtent()
{
    return &m_extent;
}

inline StylizationConfiguration* PrintLayoutElementDefinition::GetStylizationConfiguration()
{
    return &m_stylizationConfiguration;
}

inline DataConfiguration* PrintLayoutElementDefinition::GetDataConfiguration()
{
    return &m_dataConfiguration;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTELEMENTDEFINITION_H_

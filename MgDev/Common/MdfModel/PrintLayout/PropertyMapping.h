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

#ifndef PROPERTYMAPPING_H_
#define PROPERTYMAPPING_H_

#include "../MdfRootObject.h"
#include "../MdfOwnerCollection.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The PropertyMapping class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API PropertyMapping : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    PropertyMapping();
    virtual ~PropertyMapping();

    // Operations
    const MdfString& GetTargetProperty() const;
    void SetTargetProperty(const MdfString& targetProperty);

    const MdfString& GetSourceProperty() const;
    void SetSourceProperty(const MdfString& sourceProperty);

    const MdfString& GetSourceUnits() const;
    void SetSourceUnits(const MdfString& sourceUnits);

private:
    // Hidden copy constructor and assignment operator.
    PropertyMapping(const PropertyMapping&);
    PropertyMapping& operator=(const PropertyMapping&);

    // Data Members
    MdfString m_targetProperty;
    MdfString m_sourceProperty;
    MdfString m_sourceUnits;

};

typedef MdfOwnerCollection<PropertyMapping> PropertyMappingCollection;
EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<PropertyMapping>;

// Inline Methods
inline const MdfString& PropertyMapping::GetTargetProperty() const
{
    return m_targetProperty;
}

inline const MdfString& PropertyMapping::GetSourceProperty() const
{
    return m_sourceProperty;
}

inline const MdfString& PropertyMapping::GetSourceUnits() const
{
    return m_sourceUnits;
}

END_NAMESPACE_MDFMODEL
#endif // PROPERTYMAPPING_H_

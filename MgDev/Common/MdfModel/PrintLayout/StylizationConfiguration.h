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

#ifndef STYLIZATIONCONFIGURATION_H_
#define STYLIZATIONCONFIGURATION_H_

#include "../MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The StylizationConfiguration class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API StylizationConfiguration : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    StylizationConfiguration();
    virtual ~StylizationConfiguration();

    // Operations
    const MdfString& GetResourceId() const;
    void SetResourceId(const MdfString& resourceId);

private:
    // Hidden copy constructor and assignment operator.
    StylizationConfiguration(const StylizationConfiguration&);
    StylizationConfiguration& operator=(const StylizationConfiguration&);

    // Data Members
    MdfString m_resourceId;
};

// Inline Methods
inline const MdfString& StylizationConfiguration::GetResourceId() const
{
    return m_resourceId;
}

END_NAMESPACE_MDFMODEL
#endif // STYLIZATIONCONFIGURATION_H_

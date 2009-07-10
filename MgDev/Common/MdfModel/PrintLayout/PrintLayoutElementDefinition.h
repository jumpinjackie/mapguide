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

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutElementDefinition(const PrintLayoutElementDefinition&);
    PrintLayoutElementDefinition& operator=(const PrintLayoutElementDefinition&);

protected:
    // Data Members
    MdfString m_name;
};

// Inline Methods
inline const MdfString& PrintLayoutElementDefinition::GetName() const
{
    return m_name;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTELEMENTDEFINITION_H_

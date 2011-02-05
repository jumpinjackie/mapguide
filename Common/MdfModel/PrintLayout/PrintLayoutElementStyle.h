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

#ifndef PRINTLAYOUTELEMENTSTYLE_H_
#define PRINTLAYOUTELEMENTSTYLE_H_

#include "../MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The PrintLayoutElementStyle class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API PrintLayoutElementStyle : public MdfRootObject
{
public:
    // Constructor(s)/Destructor
    PrintLayoutElementStyle();
    virtual ~PrintLayoutElementStyle();

    // Operations
    const MdfString& GetName() const;
    void SetName(const MdfString& name);

private:
    // Hidden copy constructor and assignment operator.
    PrintLayoutElementStyle(const PrintLayoutElementStyle&);
    PrintLayoutElementStyle& operator=(const PrintLayoutElementStyle&);

private:
    // Data Members
    MdfString m_name;
};

// Inline Methods
inline const MdfString& PrintLayoutElementStyle::GetName() const
{
    return m_name;
}

END_NAMESPACE_MDFMODEL
#endif // PRINTLAYOUTELEMENTSTYLE_H_

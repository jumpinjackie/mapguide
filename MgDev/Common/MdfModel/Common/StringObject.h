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

#ifndef STRINGOBJECT_H_
#define STRINGOBJECT_H_

#include "../MdfModel.h"
#include "../MdfOwnerCollection.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION: The StringObject class declaration.
//-------------------------------------------------------------------------
class MDFMODEL_API StringObject
{
public:
    // Constructor(s)/Destructor
    StringObject();
    explicit StringObject(const wchar_t* str);
    virtual ~StringObject();

    // Operations
    const MdfString& GetString() const;
    void SetString(const MdfString& str);

private:
    // Hidden copy constructor and assignment operator.
    StringObject(const StringObject&);
    StringObject& operator=(const StringObject&);

private:
    // Data Members
    MdfString m_str;
};

typedef MdfOwnerCollection<StringObject> StringObjectCollection;
EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<StringObject>;

// Inline Methods
inline const MdfString& StringObject::GetString() const
{
    return m_str;
}

END_NAMESPACE_MDFMODEL
#endif // STRINGOBJECT_H_

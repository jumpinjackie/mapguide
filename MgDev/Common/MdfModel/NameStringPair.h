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

#ifndef NAMESTRINGPAIR_H_
#define NAMESTRINGPAIR_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MdfRootObject.h"

// exporting of template classes for collections
// suppresses MS specific (non-ANSI) code warning
#pragma warning(disable: 4231)

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    //  Class NameStringPair represents a pair of strings.  The first is the
    //  name and the second is the associated text.
    // ------------------------------------------------------------------------
class MDFMODEL_API NameStringPair : public MdfRootObject
    {
    public:
        // Construction, initialization
        NameStringPair();
        NameStringPair(const MdfString& strName , const MdfString& strValue);

        //default copy constructor, destructor and assignment operator.

        // Operations
        // Property : Name
        const MdfString& GetName() const;
        void SetName(const MdfString& strName);

        // Property : Value
        const MdfString& GetValue() const;
        void SetValue(const MdfString& strValue);

    private:
        // The Name data member
        MdfString m_strName;

        // The Value associated with the name
        MdfString m_strValue;
    };

    typedef MdfOwnerCollection<NameStringPair> NameStringPairCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<NameStringPair>;

END_NAMESPACE_MDFMODEL
#endif//NAMESTRINGPAIR_H_

//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef PARAMETER_H_
#define PARAMETER_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"

BEGIN_NAMESPACE_MDFMODEL

    typedef MdfOwnerCollection<MdfString> ValueList;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<MdfString>;

    //-------------------------------------------------------------------------
    // DESCRIPTION: Definition of a user-configurable parameter in the symbol definition
    //-------------------------------------------------------------------------
    class MDFMODEL_API Parameter : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization
        Parameter();
        virtual ~Parameter();

        const MdfString& GetIdentifier() const;
        void SetIdentifier(const MdfString& identifier);

        const MdfString& GetDefaultValue() const;
        void SetDefaultValue(const MdfString& defaultValue);

        const MdfString& GetDisplayName() const;
        void SetDisplayName(const MdfString& displayName);

        const MdfString& GetDescription() const;
        void SetDescription(const MdfString& description);

        ValueList* GetValueList();

    private:
        // Hidden copy constructor and assignment operator.
        Parameter(const Parameter&);
        Parameter& operator=(const Parameter&);

        // Data members
        MdfString m_sIdentifier;
        MdfString m_sDefaultValue;
        MdfString m_sDisplayName;
        MdfString m_sDescription;
        ValueList m_collValueList;
    };

    typedef MdfOwnerCollection<Parameter> ParameterCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<Parameter>;

END_NAMESPACE_MDFMODEL
#endif // PARAMETER_H_

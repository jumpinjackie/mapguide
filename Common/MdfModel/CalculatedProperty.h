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

#ifndef CALCULATEDPROPERTY_H_
#define CALCULATEDPROPERTY_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

//-------------------------------------------------------------------------
// DESCRIPTION:
// CalculatedProperty class defines the calculated properties from attributes
// from the same feature class
//-------------------------------------------------------------------------
class MDFMODEL_API CalculatedProperty : public MdfRootObject
    {
    public:
         // Construction, destruction, initialization
        CalculatedProperty();
        CalculatedProperty(const MdfString &name, const MdfString &expression);
        virtual ~CalculatedProperty();

        // Operations
        // Property : Name
        const MdfString& GetName() const;
        void SetName(const MdfString &name);

        // Property : Expression
        const MdfString& GetExpression() const;
        void SetExpression(const MdfString& expression);

    private:
        // Hidden copy constructor and assignment operator.
        CalculatedProperty(const CalculatedProperty&);
        CalculatedProperty& operator=(const CalculatedProperty&);

        // Name
        MdfString m_strName;

        // Expression
        MdfString m_strExpression;
    };

    typedef MdfOwnerCollection<CalculatedProperty> CalculatedPropertyCollection;
    EXPIMP_TEMPLATE template
        class MDFMODEL_API MdfOwnerCollection<CalculatedProperty>;

END_NAMESPACE_MDFMODEL
#endif // CALCULATEDPROPERTY_H_

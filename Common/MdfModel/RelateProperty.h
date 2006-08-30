//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef RELATEPROPERTY_H_  
#define RELATEPROPERTY_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"

namespace MdfModel
{
    
//-------------------------------------------------------------------------
// DESCRIPTION:
// RelateProperty class defines the calculated properties from attributes
// from the same feature class
//-------------------------------------------------------------------------
    class MDFMODEL_API RelateProperty
    {
    public:
         // Construction, destruction, initialization
        RelateProperty();
        RelateProperty(const MdfString &featureClassProperty,
                       const MdfString &attributeClassProperty);
        virtual ~RelateProperty();

        // Operations
        // Property : FeatureClassProperty
        const MdfString& GetFeatureClassProperty() const;
        void SetFeatureClassProperty(const MdfString &name);

        // Property : AttributeClassProperty
        const MdfString& GetAttributeClassProperty()const;
        void SetAttributeClassProperty(const MdfString& expression);

    private:
        // Hidden copy constructor and assignment operator.
        RelateProperty(const RelateProperty&);
        RelateProperty& operator=(const RelateProperty&);

        // FeatureClassProperty
        MdfString m_strFeatureClassProperty;

        // AttributeClassProperty
        MdfString m_strAttributeClassProperty;
    };

    typedef MdfOwnerCollection<RelateProperty> RelatePropertyCollection;
    EXPIMP_TEMPLATE template
        class MDFMODEL_API MdfOwnerCollection<RelateProperty>;
}

#endif // RELATEPROPERTY_H_
// End of file.

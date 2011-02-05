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

#ifndef ATTRIBUTERELATE_H_
#define ATTRIBUTERELATE_H_

#include "MdfModel.h"
#include "RelateProperty.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // AttributeRelate class defines the attribute class in the target feature
    // source
    //-------------------------------------------------------------------------
    class MDFMODEL_API AttributeRelate : public MdfRootObject
    {
    public:
        // Defines the type of a join
        enum RelateType
        {
            LeftOuter,
            RightOuter,     // is not supported yet
            Inner,
            Association     // is not supported yet
        };

         // Construction, destruction, initialization
        AttributeRelate();
        AttributeRelate(const MdfString& joinName);

        virtual ~AttributeRelate();

        // Operations
        // Property : RelateProperty
        RelatePropertyCollection* GetRelateProperties();

        // Property : Name
        const MdfString& GetName() const;
        void SetName(const MdfString& joinName);

        // Property : ResourceId
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString &resourceId);

        // Property : AttributeClass
        const MdfString& GetAttributeClass() const;
        void SetAttributeClass(const MdfString& featureClass);

        // Property: AttributeNameDelimiter
        const MdfString& GetAttributeNameDelimiter() const;
        void SetAttributeNameDelimiter(const MdfString& delimiterStr);

        // Attribute : RelateType
        RelateType          GetRelateType() const;
        void                SetRelateType(RelateType type);

        // Attribute: ForceOneToOne
        bool                GetForceOneToOne() const;
        void                SetForceOneToOne(bool bVal);

    private:
        // Hidden copy constructor and assignment operator.
        AttributeRelate(const AttributeRelate&);
        AttributeRelate& operator=(const AttributeRelate&);

        // RelatePropertyCollection
        RelatePropertyCollection m_collRelateProperties;

        // Name
        MdfString m_strName;

        // ResourceId
        MdfString m_strResourceId;

        // AttributeClass
        MdfString m_strAttributeClass;

        MdfString m_strAttributeNameDelimiter;

        // Join type attribute
        RelateType m_eRelateType;

        // Force 1 : 1 attribute
        bool m_bForceOneToOne;
    };

    typedef MdfOwnerCollection<AttributeRelate> AttributeRelateCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<AttributeRelate>;

END_NAMESPACE_MDFMODEL
#endif // ATTRIBUTERELATE_H_

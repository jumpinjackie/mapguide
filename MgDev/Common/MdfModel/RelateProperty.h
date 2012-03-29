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

#pragma once 

#ifndef RELATEPROPERTY_H_
#define RELATEPROPERTY_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "MdfRootObject.h"


// exporting of template classes for collections
// suppresses MS specific (non-ANSI) code warning
#pragma warning(disable: 4231)

// ignore warning about needing to have dll-interface to be used by clients export
#pragma warning(disable: 4251)

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // RelateProperty class defines the calculated properties from attributes
    // from the same feature class
    //-------------------------------------------------------------------------
    class MDFMODEL_API RelateProperty : public MdfRootObject
    {
    public:
         // Construction, destruction, initialization
        RelateProperty();
        RelateProperty(const MdfString &featureClassProperty,
                       const MdfString &attributeClassProperty,
                       const MdfString &primaryAttributeRelateName = L"");
        virtual ~RelateProperty();

        // Operations
        // Property : FeatureClassProperty. It might be either class name (in
        // the case of the convential join) or primary AttributeRelate name and a class name
        // (in the case of a chain)
        const MdfString& GetFeatureClassProperty(bool bStripPrimaryAttributeRelateName = false) const;
        void SetFeatureClassProperty(const MdfString& PropertyName, const MdfString &primaryAttributeRelateName = L"");

        // Property: parces a Feature Class property and returns the primary
        // join name
        const MdfString& GetPrimaryAttributeRelateName() const;

        // Property : AttributeClassProperty
        const MdfString& GetAttributeClassProperty() const;
        void SetAttributeClassProperty(const MdfString& expression);

        // Operations: parses the string "PrimaryAttributeRelate.PropertyName"
        static void ParseDelimitedClassName(const MdfString& delimitedName,
                                            MdfString& attributeRelateName,
                                            MdfString& propertyName);
    private:
        // Hidden copy constructor and assignment operator.
        RelateProperty(const RelateProperty&);
        RelateProperty& operator=(const RelateProperty&);

        // AttributeClassProperty name
        MdfString m_strAttributeClassProperty;

        // complete FeatureClassProperty string that includes primary
        // AttributeRelate as a prefix with "." delimiter
        MdfString m_strPrefixedFeatureClassProperty;

        // FeatureClassProperty name
        MdfString m_strFeatureClassProperty;

        // primary AttributeRelate name
        MdfString m_strPrimaryAttributeRelate;
    };

    typedef MdfOwnerCollection<RelateProperty> RelatePropertyCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<RelateProperty>;

END_NAMESPACE_MDFMODEL
#endif //RELATEPROPERTY_H_

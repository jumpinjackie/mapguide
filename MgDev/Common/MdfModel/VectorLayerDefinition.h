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

#ifndef VECTORLAYERDEFINITION_H_
#define VECTORLAYERDEFINITION_H_

#include "LayerDefinition.h"
#include "VectorScaleRange.h"
#include "NameStringPair.h"
#include "URLData.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Layer that specifies a wholly encapsulated data source of stylizable
    // geometric features and their associated data attributes.
    //-------------------------------------------------------------------------
    class MDFMODEL_API VectorLayerDefinition : public LayerDefinition
    {
    public:

        // Describes the content of the FeatureName property
        enum FeatureNameType
        {
            FeatureClass,
            NamedExtension
        };

         // Construction, destruction, initialization
        VectorLayerDefinition(const MdfString& strDataResourceID, const MdfString &strFeatureClass);
        virtual ~VectorLayerDefinition();

        // Operations
        // Property : FeatureName
        const MdfString& GetFeatureName() const;
        void SetFeatureName(const MdfString&  strFeatureName);

        // Property : FeatureNameType
        FeatureNameType GetFeatureNameType() const;
        void SetFeatureNameType(FeatureNameType featureNameType);

        // Property : Filter
        const MdfString& GetFilter() const;
        void SetFilter(const MdfString& strFilter);

        // Property : Properties
        NameStringPairCollection* GetPropertyMappings();

        // Property : Geometry
        const MdfString& GetGeometry() const;
        void SetGeometry(const MdfString&  strGeometry);

        // Property : UrlData
        URLData* GetUrlData();
        void AdoptUrlData(URLData* urlData);

        // Property : ToolTip
        const MdfString& GetToolTip() const;
        void SetToolTip(const MdfString&  strToolTip);

        // Property : VectorScaleRanges
        VectorScaleRangeCollection* GetScaleRanges();

    private:
        // Hidden copy constructor and assignment operator.
        VectorLayerDefinition(const VectorLayerDefinition&);
        VectorLayerDefinition& operator=(const VectorLayerDefinition&);

        // Name of the feature class that is to be used from the feature source.
        MdfString m_strFeatureName;

        // The meaning of the string in the FeatureName property.
        FeatureNameType m_featureNameType;

        // Collection of attributes that are to be associated with each feature
        // returned to the client by this data setup.
        NameStringPairCollection m_collPropertyMappings;

        // Stores an SQL Where clause for returning the appropriate feature.
        MdfString m_strFilter;

        // Geometry
        MdfString m_strGeometry;

        // UrlData
        URLData* m_urlData;

        // ToolTip
        MdfString m_strToolTip;

        // Collection of VectorScaleRange objects.
        VectorScaleRangeCollection m_collScaleRanges;
    };

END_NAMESPACE_MDFMODEL
#endif //VECTORLAYERDEFINITION_H_

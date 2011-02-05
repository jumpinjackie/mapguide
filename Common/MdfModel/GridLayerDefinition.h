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

#ifndef GRIDLAYERDEFINITION_H_
#define GRIDLAYERDEFINITION_H_

#include "LayerDefinition.h"
#include "GridScaleRange.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // Layer that specifies a wholly encapsulated data source of stylizable
    // raster images and their associated data attributes.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridLayerDefinition : public LayerDefinition
    {
    public:
        GridLayerDefinition(const MdfString& strDataResourceID);
        virtual ~GridLayerDefinition();

        // Property : GridScaleRanges
        GridScaleRangeCollection* GetScaleRanges();

        // Property : FeatureName
        const MdfString& GetFeatureName() const;
        void SetFeatureName(const MdfString &strFeatureName);

        // Property : Geometry
        const MdfString& GetGeometry() const;
        void SetGeometry(const MdfString&  strGeometry);

        // Property : Filter
        const MdfString& GetFilter() const;
        void SetFilter(const MdfString& strFilter);

    private:
        // Hidden copy constructor and assignment operator.
        GridLayerDefinition(const GridLayerDefinition&);
        GridLayerDefinition& operator = (const GridLayerDefinition&);

        // Scale range collection.
        GridScaleRangeCollection m_collScaleRanges;

        // Feature Class
        MdfString                m_strFeatureName;

        // Geometry
        MdfString m_strGeometry;

        // Stores an SQL Where clause for returning the appropriate feature.
        MdfString m_strFilter;
    };

END_NAMESPACE_MDFMODEL
#endif //GRIDLAYERDEFINITION_H_

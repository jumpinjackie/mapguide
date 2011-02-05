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

#ifndef VECTORSCALERANGE_H_
#define VECTORSCALERANGE_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "FeatureTypeStyle.h"
#include "ElevationSettings.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The VectorScaleRange class is used by Styles to define a Stylization for
    // a given Map's scale range. If the Map's scale falls in the range specified
    // in this VectorScaleRange, the FeatureTypeStyles are applied to the Layers features.
    // The FeatureTypeStyles will be processed in the order they appear in the
    // collection. Each will result in the drawing of some of the layer's geometric
    // primitives and/or their labels.
    //-------------------------------------------------------------------------
class MDFMODEL_API VectorScaleRange : public MdfRootObject
    {
    public:
        // Represents the maximum value for the map scale.
        static const double MAX_MAP_SCALE;

    public:
        // Construction, destruction, initialization
        VectorScaleRange();
        virtual ~VectorScaleRange();

        // Operations
        // Property : MinScale
        double GetMinScale() const;
        void SetMinScale(const double& dMinScale);

        // Property : MaxScale
        double GetMaxScale() const;
        void SetMaxScale(const double& dMaxScale);

        // Property : FeatureTypeStyles
        FeatureTypeStyleCollection* GetFeatureTypeStyles();

        // Property : ElevationSettings
        ElevationSettings* GetElevationSettings();
        void AdoptElevationSettings(ElevationSettings* elevationSettings);

    private:
        // Hidden copy constructor and assignment operator.
        VectorScaleRange(const VectorScaleRange&);
        VectorScaleRange& operator=(const VectorScaleRange&);

        // Data members
        // The minimum value component of the Scale Range.
        double m_dMinScale;

        // The maximum value component of the Scale Range.
        double m_dMaxScale;

        // The collection of FeatureTypeStyle objects.
        FeatureTypeStyleCollection m_collFeatureTypeStyles;

        // Extended data
        ElevationSettings* m_elevationSettings;
    };

    typedef MdfOwnerCollection<VectorScaleRange> VectorScaleRangeCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<VectorScaleRange>;

END_NAMESPACE_MDFMODEL
#endif //VECTORSCALERANGE_H_

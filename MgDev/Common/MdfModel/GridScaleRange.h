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

#ifndef RASTERSCALERANGE_H_
#define RASTERSCALERANGE_H_

#include "MdfModel.h"
#include "MdfOwnerCollection.h"
#include "GridSurfaceStyle.h"
#include "GridColorStyle.h"
#include <vector>
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The GridScaleRange class is used by Styles to define a Stylization for
    // a given Map's scale range. If the Map's scale falls in the range specified
    // in this GridScaleRange, the GridStyles are applied to the Layers features.
    // The GridStyles will be processed in the order they appear in the
    // collection. Each will result in the drawing of some of the layer's geometric
    // primitives and/or their labels.
    //-------------------------------------------------------------------------
    class MDFMODEL_API GridScaleRange : public MdfRootObject
    {
    public:
        // Represents the maximum value for the map scale.
        static const double MAX_MAP_SCALE;

    public:
        // Construction, destruction, initialization
        GridScaleRange();
        virtual ~GridScaleRange();

        // Property : MinScale, optional.
        // Use 0 if not specified.
        double GetMinScale() const;
        void SetMinScale(const double& dMinScale);

        // Property : MaxScale, optional
        // Use MAX_MAP_SCALE if not specified.
        double GetMaxScale() const;
        void SetMaxScale(const double& dMaxScale);

        // Property : SurfaceStyle, optional.
        const MdfModel::GridSurfaceStyle* GetSurfaceStyle() const;
        MdfModel::GridSurfaceStyle* GetSurfaceStyle();
        void AdoptSurfaceStyle(MdfModel::GridSurfaceStyle *pSurfaceStyle);
        MdfModel::GridSurfaceStyle* OrphanSurfaceStyle();

        // Property : ColorStyle, optional.
        const MdfModel::GridColorStyle* GetColorStyle() const;
        MdfModel::GridColorStyle* GetColorStyle();
        void AdoptColorStyle(MdfModel::GridColorStyle *pColorStyle);
        MdfModel::GridColorStyle* OrphanColorStyle();

        // Property : RebuildFactor
        double GetRebuildFactor();
        void SetRebuildFactor(double dRebuildFactor);

    private:
        // Hidden copy constructor and assignment operator.
        GridScaleRange(const GridScaleRange&);
        GridScaleRange& operator=(const GridScaleRange&);

        // Data members
        // The minimum value component of the Scale Range.
        double m_dMinScale;

        // The maximum value component of the Scale Range.
        double m_dMaxScale;

        // Rebuild Factor
        double m_dRebuildFactor;

        // Grid Surface Style
        GridSurfaceStyle* m_spSurfaceStyle;

        // Grid Color Style
        GridColorStyle* m_spColorStyle;
    };

    typedef MdfOwnerCollection<GridScaleRange> GridScaleRangeCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<GridScaleRange>;

END_NAMESPACE_MDFMODEL
#endif //RASTERSCALERANGE_H_

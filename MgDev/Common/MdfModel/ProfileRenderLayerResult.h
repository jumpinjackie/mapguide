//
//  Copyright (C) 2011 by Autodesk, Inc.
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

#ifndef PROFILERENDERLAYERRESULT_H_
#define PROFILERENDERLAYERRESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "ScaleRange.h"
#include "MdfOwnerCollection.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderLayerResult is a Profiling result which records
    // information for a particular layer rendering process.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderLayerResult : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderLayerResult();
        virtual ~ProfileRenderLayerResult();

        // Operations

        // Property: ResourceID
        // The resource identifier of the layer.
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& pstrResourceId);

        // Property: LayerName
        // The name of the layer.
        const MdfString& GetLayerName() const;
        void SetLayerName(const MdfString& pstrLayerName);

        // Property: LayerType
        // The type (vector or raster) of the layer.
        const MdfString& GetLayerType() const;
        void SetLayerType(const MdfString& pstrLayerType);

        // Property: FeatureClassName
        // The name of the feature class referenced by the layer.
        const MdfString& GetFeatureClassName() const;
        void SetFeatureClassName(const MdfString& pstrFeatureClassName);

        // Property: CoordinateSystem
        // The coordinate system code used by the layer.
        const MdfString& GetCoordinateSystem() const;
        void SetCoordinateSystem(const MdfString& pstrCoordinateSystem);

        // Property: ScaleRange
        // The current scale range of the layer in the map.
        const ScaleRange* GetScaleRange() const;
        ScaleRange* GetScaleRange();
        void AdoptScaleRange(ScaleRange* pScaleRange);
        ScaleRange* OrphanScaleRange();

        // Property: Filter
        // A boolean FDO expression that specifies which features to return.  No filter means pass all features through.
        const MdfString& GetFilter() const;
        void SetFilter(const MdfString& pstrFilter);

        // Property: RenderTime
        // The time of stylizing a paricular layer in the map.
        const double GetRenderTime() const;
        void SetRenderTime(const double& dRenderTime);

        // Property: Error
        // Error message if render layer failed.
        const MdfString& GetError() const;
        void SetError(const MdfString& pstrError);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileRenderLayersResult copy constructor and assignment operator.
        ProfileRenderLayerResult(const ProfileRenderLayerResult&);
        ProfileRenderLayerResult& operator=(const ProfileRenderLayerResult&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strResourceId;
        MdfString m_strLayerName;
        MdfString m_strLayerType;
        MdfString m_strFeatureClassName;
        MdfString m_strCoordinateSystem;
        ScaleRange* m_scaleRange;
        MdfString m_strFilter;
        double m_dRenderTime;
        MdfString m_strError;
    };

    typedef MdfOwnerCollection<ProfileRenderLayerResult> ProfileRenderLayerResultCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<ProfileRenderLayerResult>;

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERLAYERRESULT_H_

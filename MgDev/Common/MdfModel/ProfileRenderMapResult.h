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

#ifndef PROFILERENDERMAPRESULT_H_
#define PROFILERENDERMAPRESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "ProfileResult.h"
#include "ProfileRenderLayersResult.h"
#include "ProfileRenderSelectionResult.h"
#include "ProfileRenderWatermarksResult.h"
#include "ProfileRenderLabelsResult.h"
#include "Box2D.h"


BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderMapResult class is one concrete implementation of ProfileResult.
    // It's a profiling result which records information during RenderMap or
    // RenderDynamicOverlay processes.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderMapResult : public ProfileResult
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderMapResult();
        virtual ~ProfileRenderMapResult();

        // Operations

        // Property: ResourceID
        // The resource identifier of the map.
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& pstrResourceId);

        // Property: CoordinateSystem
        // The coordinate system code used by the map.
        const MdfString& GetCoordinateSystem() const;
        void SetCoordinateSystem(const MdfString& pstrCoordinateSystem);

        // Property: Extents
        // A bounding box around the area of the map.
        const Box2D& GetExtents() const;
        void SetExtents(const Box2D& boxExtents);

        // Property: LayerCount
        // The number of layers in the map.
        const int GetLayerCount() const;
        void SetLayerCount(const int& iLayerCount);

        // Property: LayerCount
        // The initial display scale of the map.
        const double GetScale() const;
        void SetScale(const double& dScale);

        // Property: ImageFormat
        // The format of the map image.
        const MdfString& GetImageFormat() const;
        void SetImageFormat(const MdfString& pstrImageFormat);

        // Property: RendererType
        // The renderer used in the image rendering process.
        const MdfString& GetRendererType() const;
        void SetRendererType(const MdfString& pstrRendererType);

        // Property: RenderTime
        // The total render time to generate the map image.
        const double GetRenderTime() const;
        void SetRenderTime(const double& dRenderTime);

        // Property: ProfileRenderLayersResult
        // Profiling result which records information during layer rendering processes.
        const  ProfileRenderLayersResult* GetProfileRenderLayersResult() const;
        ProfileRenderLayersResult* GetProfileRenderLayersResult();
        void AdoptProfileRenderLayersResult(ProfileRenderLayersResult* pProfileRenderLayersResult);
        ProfileRenderLayersResult* OrphanProfileRenderLayersResult();

        // Property: ProfileRenderSelectionResult
        // Profiling result which records information during selection rendering processes.
        const  ProfileRenderSelectionResult* GetProfileRenderSelectionResult() const;
        ProfileRenderSelectionResult* GetProfileRenderSelectionResult();
        void AdoptProfileRenderSelectionResult(ProfileRenderSelectionResult* pProfileRenderSelectionResult);
        ProfileRenderSelectionResult* OrphanProfileRenderSelectionResult();

        // Property: ProfileRenderWatermarksResult
        // Profiling result which records information during watermark rendering processes.
        const  ProfileRenderWatermarksResult* GetProfileRenderWatermarksResult() const;
        ProfileRenderWatermarksResult* GetProfileRenderWatermarksResult();
        void AdoptProfileRenderWatermarksResult(ProfileRenderWatermarksResult* pProfileRenderWatermarksResult);
        ProfileRenderWatermarksResult* OrphanProfileRenderWatermarksResult();

        // Property: ProfileRenderLabelsResult
        // Profiling result which records information during label rendering processes.
        const  ProfileRenderLabelsResult* GetProfileRenderLabelsResult() const;
        ProfileRenderLabelsResult* GetProfileRenderLabelsResult();
        void AdoptProfileRenderLabelsResult(ProfileRenderLabelsResult* pProfileRenderLabelsResult);
        ProfileRenderLabelsResult* OrphanProfileRenderLabelsResult();

        // Property: CreateImageTime
        // The time of rendering stylized map to an image.
        const double GetCreateImageTime() const;
        void SetCreateImageTime(const double& dCreateImageTime);

        // Property: Error
        // Error message if rendering map failed.
        const MdfString& GetError() const;
        void SetError(const MdfString& pstrError);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileResult copy constructor and assignment operator.
        ProfileRenderMapResult(const ProfileRenderMapResult&);
        ProfileRenderMapResult& operator=(const ProfileRenderMapResult&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strResourceId;
        MdfString m_strCoordinateSystem;
        Box2D m_boxExtents;
        double m_dScale;
        int m_iLayerCount;
        MdfString m_strImageFormat;
        MdfString m_strRendererType;
        double m_dRenderTime;
        ProfileRenderLayersResult* m_profileRenderLayersResult;
        ProfileRenderSelectionResult* m_profileRenderSelectionResult;
        ProfileRenderWatermarksResult* m_profileRenderWatermarksResult;
        ProfileRenderLabelsResult* m_profileRenderLabelsResult;
        double m_dCreateImageTime;
        MdfString m_strError;
    };

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERMAPRESULT_H_

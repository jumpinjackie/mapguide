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

#ifndef PROFILERENDERWATERMARKRESULT_H_
#define PROFILERENDERWATERMARKRESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "MdfOwnerCollection.h"


BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderWatermarkResult is a Profiling result which records
    // information during for watermark rendering process.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderWatermarkResult : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderWatermarkResult();
        virtual ~ProfileRenderWatermarkResult();

        // Operations

        // Property: ResourceID
        // The resource identifier of the watermark.
        const MdfString& GetResourceId() const;
        void SetResourceId(const MdfString& pstrResourceId);

        // Property: PositionType
        // The position type (XY or Tile) of the watermark.
        const MdfString& GetPositionType() const;
        void SetPositionType(const MdfString& pstrPositionType);

        // Property: RenderTime
        // The time of stylizing a paricular watermark in the map.
        const double GetRenderTime() const;
        void SetRenderTime(const double& dRenderTime);

        // Property: Error
        // Error message if rendering watermark failed.
        const MdfString& GetError() const;
        void SetError(const MdfString& pstrError);

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileRenderWatermarkResult copy constructor and assignment operator.
        ProfileRenderWatermarkResult(const ProfileRenderWatermarkResult&);
        ProfileRenderWatermarkResult& operator=(const ProfileRenderWatermarkResult&);

        // Data members
        // See corresponding properties for descriptions
        MdfString m_strResourceId;
        MdfString m_strPositionType;
        double m_dRenderTime;
        MdfString m_strError;
    };

    typedef MdfOwnerCollection<ProfileRenderWatermarkResult> ProfileRenderWatermarkResultCollection;
    EXPIMP_TEMPLATE template class MDFMODEL_API MdfOwnerCollection<ProfileRenderWatermarkResult>;

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERWATERMARKRESULT_H_

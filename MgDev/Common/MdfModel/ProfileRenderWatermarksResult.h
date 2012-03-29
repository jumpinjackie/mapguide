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

#ifndef PROFILERENDERWATERMARKSRESULT_H_
#define PROFILERENDERWATERMARKSRESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "ProfileRenderWatermarkResult.h"


BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderWatermarksResult is a profiling result which records
    // information during watermark rendering processes.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderWatermarksResult : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderWatermarksResult();
        virtual ~ProfileRenderWatermarksResult();

        // Operations

        // Property: RenderTime
        // The time of stylizing all watermarks in the map.
        const double GetRenderTime() const;
        void SetRenderTime(const double& dRenderTime);

        // Property: ProfileRenderWatermarkResults
        // A collection of ProfileRenderWatermarkResult.
        ProfileRenderWatermarkResultCollection* GetProfileRenderWatermarkResults();

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileRenderSelectionResult copy constructor and assignment operator.
        ProfileRenderWatermarksResult(const ProfileRenderWatermarksResult&);
        ProfileRenderWatermarksResult& operator=(const ProfileRenderWatermarksResult&);

        // Data members
        // See corresponding properties for descriptions
        double m_dRenderTime;
        ProfileRenderWatermarkResultCollection m_profileRenderWatermarkResults;
    };

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERWATERMARKSRESULT_H_

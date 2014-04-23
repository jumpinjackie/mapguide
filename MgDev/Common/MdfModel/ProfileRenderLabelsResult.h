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

#ifndef PROFILERENDERLABELSRESULT_H_
#define PROFILERENDERLABELSRESULT_H_

#include "MdfModel.h"
#include "MdfRootObject.h"

BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderLabelsResult is the profiling result which records
    // information during label rendering processes.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderLabelsResult : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderLabelsResult();
        virtual ~ProfileRenderLabelsResult();

        // Operations

        // Property: RenderTime
        // The time of stylizing labels in the map.
        const double GetRenderTime() const;
        void SetRenderTime(const double& dRenderTime);

        // Property: Error
        // Error message if rendering labels failed.
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
        ProfileRenderLabelsResult(const ProfileRenderLabelsResult&);
        ProfileRenderLabelsResult& operator=(const ProfileRenderLabelsResult&);

        // Data members
        // See corresponding properties for descriptions
        double m_dRenderTime;
        MdfString m_strError;
    };

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERLABELSRESULT_H_

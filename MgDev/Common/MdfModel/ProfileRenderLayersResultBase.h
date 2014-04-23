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

#ifndef PROFILERENDERLAYERSRESULTBASE_H_
#define PROFILERENDERLAYERSRESULTBASE_H_

#include "MdfModel.h"
#include "MdfRootObject.h"
#include "ProfileRenderLayerResult.h"


BEGIN_NAMESPACE_MDFMODEL

    //-------------------------------------------------------------------------
    // DESCRIPTION:
    // The ProfileRenderLayersResultBase is a base class which records
    // information during layer rendering processes.
    //------------------------------------------------------------------------
    class MDFMODEL_API ProfileRenderLayersResultBase : public MdfRootObject
    {
    public:
        // Construction, destruction, initialization.
        ProfileRenderLayersResultBase();
        virtual ~ProfileRenderLayersResultBase();

        // Operations

        // Property: RenderTime
        // The time of stylizing all layers.
        virtual const double GetRenderTime() const;
        virtual void SetRenderTime(const double& dRenderTime);

        // Property: ProfileRenderLayerResults
        // A collection of ProfileRenderLayerResult.
        virtual ProfileRenderLayerResultCollection* GetProfileRenderLayerResults();

//#ifdef _WIN32
#if _DEBUG
        // for memory leak testing
        static int DumpMemoryLeaks();
#endif
//#endif

    private:
        // Hidden ProfileRenderLayersResult copy constructor and assignment operator.
        ProfileRenderLayersResultBase(const ProfileRenderLayersResultBase&);
        ProfileRenderLayersResultBase& operator=(const ProfileRenderLayersResultBase&);

        // Data members
        // See corresponding properties for descriptions
        double m_dRenderTime;
        ProfileRenderLayerResultCollection m_profileRenderLayerResults;
    };

END_NAMESPACE_MDFMODEL
#endif // PROFILERENDERLAYERSRESULTBASE_H_

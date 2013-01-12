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

//-------------------------------------------------------------------------
// DESCRIPTION:
// The ProfileRenderLayersResultBase class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ProfileRenderLayersResultBase.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the ProfileRenderLayersResultBase class.
//-------------------------------------------------------------------------
ProfileRenderLayersResultBase::ProfileRenderLayersResultBase()
{
    this->m_dRenderTime = 0.0;

}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
ProfileRenderLayersResultBase::~ProfileRenderLayersResultBase()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the RenderTime property.
// RETURNS: The time of stylizing all layers.
//-------------------------------------------------------------------------
const double ProfileRenderLayersResultBase::GetRenderTime() const
{
    return this->m_dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the RenderTime property.
// PARAMETERS:
//      Input:
//          dRenderTime - The time of stylizing all layers.
//-------------------------------------------------------------------------
void ProfileRenderLayersResultBase::SetRenderTime(const double& dRenderTime)
{
    this->m_dRenderTime = dRenderTime;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ProfileRenderLayerResults property.
// RETURNS: A collection of ProfileRenderLayerResult.
//-------------------------------------------------------------------------
ProfileRenderLayerResultCollection* ProfileRenderLayersResultBase::GetProfileRenderLayerResults()
{
    return &this->m_profileRenderLayerResults;
}

//-------------------------------------------------------------------------
// PURPOSE: For memory leak testing.
//-------------------------------------------------------------------------
#ifdef _WIN32
#ifdef _DEBUG
int ProfileRenderLayersResultBase::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif

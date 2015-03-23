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

#ifndef _MG_WATERMARK_H_
#define _MG_WATERMARK_H_

BEGIN_NAMESPACE_MDFMODEL
class WatermarkDefinition;
END_NAMESPACE_MDFMODEL

//Currently the only reason MgWatermark exists is that
//the rendering service can get the WatermarkDefinition.
//So it's only an internal API.

/// \cond INTERNAL
class MG_PLATFORMBASE_API MgWatermark
{
INTERNAL_API:

    static MdfModel::WatermarkDefinition* GetWatermarkDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);
    static MdfModel::WatermarkDefinition* GetWatermarkDefinition(CREFSTRING resourceContent);
};
/// \endcond

#endif

//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

/// \defgroup MgFeatureSourceParams MgFeatureSourceParams
/// \ingroup Feature_Service_classes
/// \{

#ifndef _MG_FEATURE_SOURCE_PARAMS_H
#define _MG_FEATURE_SOURCE_PARAMS_H

////////////////////////////////////////////////////////////////
/// \brief
/// Is a virtual base class for feature source parameters used
/// with MgFeatureSource::CreateFeatureSource. This class cannot
/// be instantiated directly.
/// \remarks
/// Currently there are two concrete classes: MgCreateSdfParams
/// and MgFileFeatureSourceParams.
///
class MG_PLATFORMBASE_API MgFeatureSourceParams : public MgSerializable
{
EXTERNAL_API:
    virtual ~MgFeatureSourceParams();
};
/// \}

#endif

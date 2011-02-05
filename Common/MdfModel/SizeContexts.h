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

#ifndef SIZECONTEXTS_H_
#define SIZECONTEXTS_H_

#include "MdfModel.h"

BEGIN_NAMESPACE_MDFMODEL

    //---------------------------------------------------------------------
    // DESCRIPTION:
    // The SizeContext enum is used to describe whether the numeric data
    // is in map space or device space while calculating expressions.
    // Used during the rendering of the Map and its Layers.
    //---------------------------------------------------------------------
    enum MDFMODEL_API SizeContext
    {
        MappingUnits, // map space
        DeviceUnits   // device space
    };

END_NAMESPACE_MDFMODEL
#endif // SIZECONTEXTS_H_

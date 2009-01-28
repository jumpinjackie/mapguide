//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _MGSERVERFEATUREPROCESSOR_H_
#define _MGSERVERFEATUREPROCESSOR_H_

#include "MapGuideCommon.h"
#include "Fdo.h"
#include "Services/FeatureService.h"

enum MgServerFeatureProcessorType
{
    msfptFeatureProcessor = 0,
    msfptFeatureJoinProcessor,
    msfptDataProcessor
};

class MgServerFeatureProcessor : public MgDisposable
{
public:
    virtual MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING propName) = 0;
    virtual MgServerFeatureProcessorType GetProcessorType() = 0;
};

#endif

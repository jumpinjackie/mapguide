//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#ifndef SEMG_SYMBOL_MANAGER_H
#define SEMG_SYMBOL_MANAGER_H

#include "Stylization.h"
#include "SE_SymbolManager.h"

class MgResourceService;

namespace MdfModel
{
    class SymbolDefinition;
};

using namespace MdfModel;

// MappingService specific implementation of the RS_SymbolManager interface.
// It retrieves DWF symbol data from the resource service and caches it
// for reuse by stylization.
class MG_SERVER_MAPPING_API SEMgSymbolManager : public SE_SymbolManager
{
public:
    SEMgSymbolManager(MgResourceService* svc);
    virtual ~SEMgSymbolManager();

    virtual SymbolDefinition* GetSymbolDefinition(const wchar_t* resource);
    virtual const unsigned char* GetImageData(const wchar_t* resource, int& length);

private:
    MgResourceService* m_svcResource;
    std::map<STRING, SymbolDefinition*> m_mSymbolCache;
    std::map<STRING, unsigned char*> m_mImageCache;
};

#endif

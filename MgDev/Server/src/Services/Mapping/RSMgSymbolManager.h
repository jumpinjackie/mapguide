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

#ifndef RSMG_SYMBOL_MANAGER_H
#define RSMG_SYMBOL_MANAGER_H

#include "RS_SymbolManager.h"

class MgResourceService;

// MappingService specific implementation of the RS_SymbolManager interface.
// It retrieves DWF symbol data from the resource service and caches it
// for reuse by stylization.
class MG_SERVER_MAPPING_API RSMgSymbolManager : public RS_SymbolManager
{
public:
    RSMgSymbolManager(MgResourceService* svc);
    virtual ~RSMgSymbolManager();

    virtual const RS_InputStream* GetSymbolData(const wchar_t* libraryName,
                                                const wchar_t* symbolName);

private:
    MgResourceService* m_svcResource;
    std::map<STRING, RS_InputStream*> m_mSymbolCache;
};

#endif

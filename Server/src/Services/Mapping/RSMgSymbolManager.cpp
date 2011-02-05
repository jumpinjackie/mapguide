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

#include "RSMgSymbolManager.h"
#include "RSMgInputStream.h"

#define ERROR_VAL (RS_InputStream*)1


RSMgSymbolManager::RSMgSymbolManager(MgResourceService* svc)
{
    m_svcResource = SAFE_ADDREF(svc);
}


RSMgSymbolManager::~RSMgSymbolManager()
{
    SAFE_RELEASE(m_svcResource);

    // free up cached symbols
    for (std::map<STRING, RS_InputStream*>::iterator iter = m_mSymbolCache.begin();
        iter != m_mSymbolCache.end(); iter++)
    {
        if (iter->second != ERROR_VAL)
            delete (RS_InputStream*)(iter->second);
    }
}


const RS_InputStream* RSMgSymbolManager::GetSymbolData(const wchar_t* libraryName,
                                                       const wchar_t* symbolName)
{
    // see if the named symbol already exists in the cache
    STRING uniqueName(libraryName);
    uniqueName.append(symbolName);
    RS_InputStream* ret = m_mSymbolCache[uniqueName];

    // check if we errored on this symbol before and don't try again
    if (ret == ERROR_VAL)
        return NULL;

    if (!ret)
    {
        try
        {
            MgResourceIdentifier resId(libraryName);
            Ptr<MgByteReader> rdr = m_svcResource->GetResourceData(&resId, symbolName);
            ret = new RSMgInputStream(rdr.p);
            m_mSymbolCache[uniqueName] = ret;
        }
        catch (MgException* e)
        {
            // either the symbol or symbol library was not found - set it to
            // something else that's invalid (like 1) in the cache so that we
            // know there was an an error and don't try to get it again
            e->Release();
            ret = NULL;
            m_mSymbolCache[uniqueName] = ERROR_VAL;
        }
    }
    else
    {
        // if the symbol exists, reset its stream to the beginning of the data
        ret->seek(SEEK_SET, 0);
    }

    return ret;
}

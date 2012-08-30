#include "RSMgSymbolManager.h"
#include "RSMgInputStream.h"

#define ERROR_VAL (RS_InputStream*)1


RSMgdSymbolManager::RSMgdSymbolManager(MgResourceService* svc)
{
    m_svcResource = SAFE_ADDREF(svc);
}


RSMgdSymbolManager::~RSMgdSymbolManager()
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


const RS_InputStream* RSMgdSymbolManager::GetSymbolData(const wchar_t* libraryName,
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
            ret = new RSMgdInputStream(rdr.p);
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

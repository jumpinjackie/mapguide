#ifndef DESKTOP_RS_SYMBOL_MANAGER_H
#define DESKTOP_RS_SYMBOL_MANAGER_H

#include "MgDesktop.h"
#include "RS_SymbolManager.h"

// MappingService specific implementation of the RS_SymbolManager interface.
// It retrieves DWF symbol data from the resource service and caches it
// for reuse by stylization.
class MG_DESKTOP_API RSMgdSymbolManager : public RS_SymbolManager
{
public:
    RSMgdSymbolManager(MgResourceService* svc);
    virtual ~RSMgdSymbolManager();

    virtual const RS_InputStream* GetSymbolData(const wchar_t* libraryName,
                                                const wchar_t* symbolName);

private:
    MgResourceService* m_svcResource;
    std::map<STRING, RS_InputStream*> m_mSymbolCache;
};

#endif
#ifndef DESKTOP_SE_SYMBOL_MANAGER_H
#define DESKTOP_SE_SYMBOL_MANAGER_H

#include "MgDesktop.h"
#include "SE_SymbolManager.h"

class MgResourceService;

class SEMgSymbolManager : public SE_SymbolManager
{
public:
    SEMgSymbolManager(MgResourceService* svc);
    virtual ~SEMgSymbolManager();

    virtual SymbolDefinition* GetSymbolDefinition(const wchar_t* resourceId);
    virtual bool GetImageData(const wchar_t* resourceId, const wchar_t* resourceName, ImageData& imageData);

private:
    MgResourceService* m_svcResource;
    std::map<STRING, SymbolDefinition*> m_mSymbolCache;
    std::map<STRING, ImageData> m_mImageCache;
};

#endif
#ifndef DESKTOP_SERVICE_FACTORY_H
#define DESKTOP_SERVICE_FACTORY_H

class MgService;
class MgTileService;
class MgdFeatureService;
class MgdResourceService;
class MgDrawingService;
class MgRenderingService;

class MG_DESKTOP_API MgServiceFactory : public MgGuardDisposable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgServiceFactory)

PUBLISHED_API:
    MgServiceFactory();
    MgService* CreateService(INT32 serviceType);

INTERNAL_API:
    static void Initialize();

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_ServiceFactory;

protected:
    ~MgServiceFactory();
    virtual void Dispose() { delete this; }
    virtual INT32 GetClassId() { return m_cls_id; }

private:
    static STRING sm_libContentPath;
    static STRING sm_libDataPath;
	static STRING sm_sesContentPath;
	static STRING sm_sesDataPath;
};

#endif
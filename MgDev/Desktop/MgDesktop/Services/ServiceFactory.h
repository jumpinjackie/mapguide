#ifndef DESKTOP_SERVICE_FACTORY_H
#define DESKTOP_SERVICE_FACTORY_H

class MgService;
class MgdTileService;
class MgdFeatureService;
class MgdMappingService;
class MgdResourceService;
class MgdDrawingService;
class MgdRenderingService;
/// \ingroup Desktop_Module
/// \{

///\brief
/// The MgdServiceFactory class allows you to create instances of service
/// classes
class MG_DESKTOP_API MgdServiceFactory : public MgGuardDisposable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgdServiceFactory)

PUBLISHED_API:
    MgdServiceFactory();
    ///\brief
    /// Creates an instances of the specified service
    ///
    ///\param serviceType (MgServiceType)
    /// The type of service to create
    ///
    ///\return
    /// An instance of the specified service. It must be cast to the appropriate
    /// subclass
    ///
    ///\exception MgServiceNotSupportedException
    ///
    ///\remarks
    /// Depending on the service type, this method will return instances of the following
    /// types:
    /// \li MgServiceType::FeatureService - MgdFeatureService
    /// \li MgServiceType::ResourceService - MgdResourceService
    /// \li MgServiceType::DrawingService - MgdDrawingService
    /// \li MgServiceType::MappingService - MgdMappingService
    /// \li MgServiceType::RenderingService - MgdRenderingService
    /// \li MgServiceType::ProfilingService - MgdProfilingService
    /// \li MgServiceType::TileService - MgdTileService
    ///
    MgService* CreateService(INT32 serviceType);

INTERNAL_API:
    static void Initialize();

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Desktop_ServiceFactory;

protected:
    ~MgdServiceFactory();
    virtual void Dispose() { delete this; }
    virtual INT32 GetClassId() { return m_cls_id; }

private:
    static STRING sm_libContentPath;
    static STRING sm_libDataPath;
	static STRING sm_sesContentPath;
	static STRING sm_sesDataPath;
    static STRING sm_schemaPath;
};
/// \}
#endif
#include "ServiceFactory.h"
#include "ResourceService.h"
#include "FeatureService.h"
#include "RenderingService.h"
#include "DrawingService.h"
#include "Exception/ServiceNotSupportedException.h"

STRING MgServiceFactory::sm_libContentPath = L"";
STRING MgServiceFactory::sm_libDataPath = L"";
STRING MgServiceFactory::sm_sesContentPath = L"";
STRING MgServiceFactory::sm_sesDataPath = L"";

MG_IMPL_DYNCREATE(MgServiceFactory);

MgServiceFactory::MgServiceFactory() { }

MgServiceFactory::~MgServiceFactory() { }

void MgServiceFactory::Initialize()
{
    MgConfiguration* conf = MgConfiguration::GetInstance();

    conf->GetStringValue(MgConfigProperties::ResourceServicePropertiesSection, 
                         MgConfigProperties::ResourceServicePropertyLibraryRepositoryPath,
                         sm_libContentPath,
                         MgConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath);

    conf->GetStringValue(MgConfigProperties::ResourceServicePropertiesSection, 
                         MgConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath,
                         sm_libDataPath,
                         MgConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath);

    conf->GetStringValue(MgConfigProperties::ResourceServicePropertiesSection, 
                         MgConfigProperties::ResourceServicePropertySessionRepositoryPath,
                         sm_sesContentPath,
                         MgConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    conf->GetStringValue(MgConfigProperties::ResourceServicePropertiesSection, 
                         MgConfigProperties::ResourceServicePropertySessionResourceDataFilePath,
                         sm_sesDataPath,
                         MgConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath);

    //Create these paths if they don't exist
    if (!MgFileUtil::IsDirectory(sm_libContentPath))
        MgFileUtil::CreateDirectory(sm_libContentPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_libDataPath))
        MgFileUtil::CreateDirectory(sm_libDataPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_sesContentPath))
        MgFileUtil::CreateDirectory(sm_sesContentPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_sesDataPath))
        MgFileUtil::CreateDirectory(sm_sesDataPath, false, true);
}

MgService* MgServiceFactory::CreateService(INT32 serviceType)
{
    switch(serviceType)
    {
    case MgServiceType::DrawingService:
        return new MgDrawingService();
    case MgServiceType::FeatureService:
        return new MgdFeatureService();
    case MgServiceType::RenderingService:
        return new MgRenderingService();
    case MgServiceType::ResourceService:
        return new MgdResourceService(sm_libContentPath,
                                      sm_libDataPath, 
                                      sm_sesContentPath, 
                                      sm_sesDataPath);
    case MgServiceType::TileService:
        return new MgTileService();
    }
    throw new MgServiceNotSupportedException(L"MgServiceFactory::CreateService", __LINE__, __WFILE__, NULL, L"", NULL);
}

/*
MgTileService* MgServiceFactory::CreateTileService()
{
    return new MgTileService();
}

MgdFeatureService* MgServiceFactory::CreateFeatureService()
{
	return new MgdFeatureService();
}

MgdResourceService* MgServiceFactory::CreateResourceService()
{
	return new MgdResourceService(sm_libContentPath,
                                  sm_libDataPath, 
                                  sm_sesContentPath, 
                                  sm_sesDataPath);
}

MgDrawingService* MgServiceFactory::CreateDrawingService()
{
    return new MgDrawingService();
}

MgRenderingService* MgServiceFactory::CreateRenderingService()
{
    return new MgRenderingService();
}
*/
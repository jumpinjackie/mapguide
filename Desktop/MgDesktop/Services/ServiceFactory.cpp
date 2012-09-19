#include "ServiceFactory.h"
#include "ResourceService.h"
#include "FeatureService.h"
#include "MappingService.h"
#include "ProfilingService.h"
#include "RenderingService.h"
#include "DrawingService.h"
#include "Exception/ServiceNotSupportedException.h"

STRING MgdServiceFactory::sm_libContentPath = L"";
STRING MgdServiceFactory::sm_libDataPath = L"";
STRING MgdServiceFactory::sm_sesContentPath = L"";
STRING MgdServiceFactory::sm_sesDataPath = L"";
STRING MgdServiceFactory::sm_schemaPath = L"";

MG_IMPL_DYNCREATE(MgdServiceFactory);

MgdServiceFactory::MgdServiceFactory() { }

MgdServiceFactory::~MgdServiceFactory() { }

void MgdServiceFactory::Initialize()
{
    MgConfiguration* conf = MgConfiguration::GetInstance();

    conf->GetStringValue(MgdConfigProperties::ResourceServicePropertiesSection, 
                         MgdConfigProperties::ResourceServicePropertyLibraryRepositoryPath,
                         sm_libContentPath,
                         MgdConfigProperties::DefaultResourceServicePropertyLibraryRepositoryPath);

    conf->GetStringValue(MgdConfigProperties::ResourceServicePropertiesSection, 
                         MgdConfigProperties::ResourceServicePropertyLibraryResourceDataFilePath,
                         sm_libDataPath,
                         MgdConfigProperties::DefaultResourceServicePropertyLibraryResourceDataFilePath);

    conf->GetStringValue(MgdConfigProperties::ResourceServicePropertiesSection, 
                         MgdConfigProperties::ResourceServicePropertySessionRepositoryPath,
                         sm_sesContentPath,
                         MgdConfigProperties::DefaultResourceServicePropertySessionRepositoryPath);

    conf->GetStringValue(MgdConfigProperties::ResourceServicePropertiesSection, 
                         MgdConfigProperties::ResourceServicePropertySessionResourceDataFilePath,
                         sm_sesDataPath,
                         MgdConfigProperties::DefaultResourceServicePropertySessionResourceDataFilePath);

    conf->GetStringValue(MgdConfigProperties::ResourceServicePropertiesSection,
                         MgdConfigProperties::ResourceServicePropertyResourceSchemaFilePath,
                         sm_schemaPath,
                         MgdConfigProperties::DefaultResourceServicePropertyResourceSchemaFilePath);

    //Create these paths if they don't exist
    if (!MgFileUtil::IsDirectory(sm_libContentPath))
        MgFileUtil::CreateDirectory(sm_libContentPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_libDataPath))
        MgFileUtil::CreateDirectory(sm_libDataPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_sesContentPath))
        MgFileUtil::CreateDirectory(sm_sesContentPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_sesDataPath))
        MgFileUtil::CreateDirectory(sm_sesDataPath, false, true);
    if (!MgFileUtil::IsDirectory(sm_schemaPath))
    {
        MgStringCollection args;
        args.Add(sm_schemaPath);
        throw new MgDirectoryNotFoundException(L"MgdServiceFactory::Initialize", __LINE__, __WFILE__, &args, L"", NULL);
    }
}

MgService* MgdServiceFactory::CreateService(INT32 serviceType)
{
    switch(serviceType)
    {
    case MgServiceType::DrawingService:
        return new MgdDrawingService();
    case MgServiceType::FeatureService:
        return new MgdFeatureService();
    case MgServiceType::MappingService:
        return new MgdMappingService();
	case MgServiceType::ProfilingService:
		return new MgdProfilingService();
    case MgServiceType::RenderingService:
        return new MgdRenderingService();
    case MgServiceType::ResourceService:
        return new MgdResourceService(sm_libContentPath,
                                      sm_libDataPath, 
                                      sm_sesContentPath, 
                                      sm_sesDataPath,
                                      sm_schemaPath);
    case MgServiceType::TileService:
        return new MgdTileService();
    }
    throw new MgServiceNotSupportedException(L"MgdServiceFactory::CreateService", __LINE__, __WFILE__, NULL, L"", NULL);
}
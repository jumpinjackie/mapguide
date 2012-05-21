#include "ProfilingService.h"
#include "ProfileResult.h"
#include "ProfileRenderMapResult.h"
#include "SAX2Parser.h" 

MgProfilingService::MgProfilingService()
{
	Ptr<MgServiceFactory> fact = new MgServiceFactory();
	m_svcRendering = static_cast<MgRenderingService*>(fact->CreateService(MgServiceType::RenderingService));
}

MgProfilingService::~MgProfilingService()
{
	SAFE_RELEASE(m_svcRendering);
}

MgByteReader* MgProfilingService::ProfileRenderDynamicOverlay(
        MgdMap* map,
        MgdSelection* selection,
        MgRenderingOptions* options)
{
	Ptr<MgByteReader> ret;
    
    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgProfilingService::ProfileRenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"", NULL);

    auto_ptr<ProfileRenderMapResult> pPRMResult; // a pointer points to Profile Render Map Result
    pPRMResult.reset(new ProfileRenderMapResult());

    // Start to profile the ProfileRenderDynamicOverlay process
    double renderMapStart = MgTimerUtil::GetTime(); 
    m_svcRendering->RenderDynamicOverlay(map, selection, options, pPRMResult.get());
    double renderMapEnd = MgTimerUtil::GetTime();

    pPRMResult->SetRenderTime(renderMapEnd - renderMapStart);
    pPRMResult->SetProfileResultType(ProfileResult::ProfileRenderDynamicOverlay);

    // Serialize the ProfileRenderMapResult to xml
    MdfParser::SAX2Parser parser;
    auto_ptr<Version> version;
    version.reset(new Version(2,4,0));
    string content = parser.SerializeToXML(pPRMResult.get(),version.get());
    ret = new MgByteReader(MgUtil::MultiByteToWideChar(content), MgMimeType::Xml);

    MG_CATCH_AND_THROW(L"MgProfilingService::ProfileRenderDynamicOverlay")

    return ret.Detach();
}


MgByteReader* MgProfilingService::ProfileRenderMap(
        MgdMap* map,
        MgdSelection* selection,
        MgCoordinate* center,
        double scale,
        INT32 width,
        INT32 height,
        MgColor* backgroundColor,
        CREFSTRING format,
        bool bKeepSelection)
{
	 Ptr<MgByteReader> ret;
    
    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgProfilingService::ProfileRenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    auto_ptr<ProfileRenderMapResult> pPRMResult; // a pointer points to Profile Render Map Result
    pPRMResult.reset(new ProfileRenderMapResult());

    // Start to profile the ProfileRenderMap process
    double renderMapStart = MgTimerUtil::GetTime(); 
    m_svcRendering->RenderMap(map, selection, center, scale, width, height, backgroundColor, format, bKeepSelection, pPRMResult.get());
    double renderMapEnd = MgTimerUtil::GetTime();

    pPRMResult->SetRenderTime(renderMapEnd - renderMapStart);
    pPRMResult->SetProfileResultType(ProfileResult::ProfileRenderMap);

    // Serialize the ProfileRenderMapResult to xml
    MdfParser::SAX2Parser parser;
    auto_ptr<Version> version;
    version.reset(new Version(2,4,0));
    string content = parser.SerializeToXML(pPRMResult.get(),version.get());
    ret = new MgByteReader(MgUtil::MultiByteToWideChar(content), MgMimeType::Xml);

    MG_CATCH_AND_THROW(L"MgProfilingService::ProfileRenderMap")

    return ret.Detach();
}
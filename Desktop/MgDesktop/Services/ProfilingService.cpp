#include "ProfilingService.h"
#include "ProfileResult.h"
#include "ProfileRenderMapResult.h"
#include "SAX2Parser.h" 

MgdProfilingService::MgdProfilingService()
{
	Ptr<MgdServiceFactory> fact = new MgdServiceFactory();
	m_svcRendering = static_cast<MgdRenderingService*>(fact->CreateService(MgServiceType::RenderingService));
}

MgdProfilingService::~MgdProfilingService()
{
	SAFE_RELEASE(m_svcRendering);
}

MgByteReader* MgdProfilingService::ProfileRenderDynamicOverlay(
        MgdMap* map,
        MgdSelection* selection,
        MgdRenderingOptions* options)
{
	Ptr<MgByteReader> ret;
    
    MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgdProfilingService::ProfileRenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"", NULL);

    auto_ptr<ProfileRenderMapResult> pPRMResult; // a pointer points to Profile Render Map Result
    pPRMResult.reset(new ProfileRenderMapResult());

    // Start to profile the ProfileRenderDynamicOverlay process
    double renderMapStart = MgdTimerUtil::GetTime(); 
    m_svcRendering->RenderDynamicOverlay(map, selection, options, pPRMResult.get());
    double renderMapEnd = MgdTimerUtil::GetTime();

    pPRMResult->SetRenderTime(renderMapEnd - renderMapStart);
    pPRMResult->SetProfileResultType(ProfileResult::ProfileRenderDynamicOverlay);

    // Serialize the ProfileRenderMapResult to xml
    MdfParser::SAX2Parser parser;
    auto_ptr<Version> version;
    version.reset(new Version(2,4,0));
    string content = parser.SerializeToXML(pPRMResult.get(),version.get());
    ret = new MgByteReader(MgUtil::MultiByteToWideChar(content), MgMimeType::Xml);

    MG_CATCH_AND_THROW(L"MgdProfilingService::ProfileRenderDynamicOverlay")

    return ret.Detach();
}


MgByteReader* MgdProfilingService::ProfileRenderMap(
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
        throw new MgNullArgumentException(L"MgdProfilingService::ProfileRenderMap", __LINE__, __WFILE__, NULL, L"", NULL);

    auto_ptr<ProfileRenderMapResult> pPRMResult; // a pointer points to Profile Render Map Result
    pPRMResult.reset(new ProfileRenderMapResult());

    // Start to profile the ProfileRenderMap process
    double renderMapStart = MgdTimerUtil::GetTime(); 
    m_svcRendering->RenderMap(map, selection, center, scale, width, height, backgroundColor, format, bKeepSelection, pPRMResult.get());
    double renderMapEnd = MgdTimerUtil::GetTime();

    pPRMResult->SetRenderTime(renderMapEnd - renderMapStart);
    pPRMResult->SetProfileResultType(ProfileResult::ProfileRenderMap);

    // Serialize the ProfileRenderMapResult to xml
    MdfParser::SAX2Parser parser;
    auto_ptr<Version> version;
    version.reset(new Version(2,4,0));
    string content = parser.SerializeToXML(pPRMResult.get(),version.get());
    ret = new MgByteReader(MgUtil::MultiByteToWideChar(content), MgMimeType::Xml);

    MG_CATCH_AND_THROW(L"MgdProfilingService::ProfileRenderMap")

    return ret.Detach();
}
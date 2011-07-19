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

#include "MapGuideCommon.h"
#include "ServerProfilingService.h"
//#include "MapProfileResult.h"
#include "SAX2Parser.h"


IMPLEMENT_CREATE_SERVICE(MgServerProfilingService)

///////////////////////////////////////////////////////////////////////////////
MgServerProfilingService::MgServerProfilingService() : MgProfilingService()
{
    // store references to the various services we use
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    m_svcResource = dynamic_cast<MgResourceService*>(serviceMan->RequestService(MgServiceType::ResourceService));
    assert(m_svcResource != NULL);

    m_svcFeature = dynamic_cast<MgFeatureService*>(serviceMan->RequestService(MgServiceType::FeatureService));
    assert(m_svcFeature != NULL);

    m_svcRendering = dynamic_cast<MgRenderingService*>(serviceMan->RequestService(MgServiceType::RenderingService));
    assert(m_svcRendering != NULL);
}


///////////////////////////////////////////////////////////////////////////////
MgServerProfilingService::~MgServerProfilingService()
{
}


///////////////////////////////////////////////////////////////////////////////

MgByteReader* MgServerProfilingService::ProfileRenderDynamicOverlay(MgMap* map,
                                                             MgSelection* selection,
                                                             MgRenderingOptions* options)
{
    Ptr<MgByteReader> ret;

    /*MG_TRY()

    if (NULL == map)
        throw new MgNullArgumentException(L"MgServerProfilingService.ProfileRenderDynamicOverlay", __LINE__, __WFILE__, NULL, L"", NULL);


    MG_CATCH_AND_THROW(L"MgServerProfilingService.ProfileRenderDynamicOverlay")*/

    return ret.Detach();
}


MgByteReader* MgServerProfilingService::ProfileRenderMap(MgMap* map,
                                                  MgSelection* selection,
                                                  MgCoordinate* center,
                                                  double scale,
                                                  INT32 width,
                                                  INT32 height,
                                                  MgColor* backgroundColor,
                                                  CREFSTRING format,
                                                  bool bKeepSelection)
{
    Ptr<MgByteReader> ret;

    //MG_TRY()
    //MapProfileResult* mapProfileResult = new MapProfileResult();
    //m_svcRendering->SetProfileResult(mapProfileResult);
    //ret = m_svcRendering->RenderMap(map,selection,center,scale,width,height,backgroundColor,format,bKeepSelection);
    //
    //Version mapProfileResultVersion23(2, 3, 0); // MapGuide 2012
    //MdfParser::SAX2Parser parser;
    //parser.WriteToFile("c:\\map.xml",mapProfileResult,&mapProfileResultVersion23);
    //MG_CATCH_AND_THROW(L"MgServerProfilingService.ProfileRenderMap")

    return ret.Detach();
}

///////////////////////////////////////////////////////////////////////////////
// Profile RenderMapLegend API
//MgByteReader* MgServerProfilingService::ProfileRenderMapLegend(MgMap* map,
//                                                        INT32 width,
//                                                        INT32 height,
//                                                        MgColor* backgroundColor,
//                                                        CREFSTRING format)
//{
//    Ptr<MgByteReader> ret;
//
//    MG_TRY()
//
//    
//    MG_CATCH_AND_THROW(L"MgServerRenderingService.RenderMapLegend")
//    return ret.Detach();
//}


///////////////////////////////////////////////////////////////////////////////
void MgServerProfilingService::SetConnectionProperties(MgConnectionProperties*)
{
    // Do nothing.  No connection properties are required for server-side service objects.
}


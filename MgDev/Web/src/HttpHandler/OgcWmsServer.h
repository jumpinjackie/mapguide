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

#ifndef _OgcWmsServer_h
#define _OgcWmsServer_h

#include "OgcServer.h"
#include "OgcWmsException.h"
#include "OgcDataAccessor.h"
#include "WmsLayerDefinitions.h"
#include "WmsFeatureInfo.h"
#include "WmsFeatureProperties.h"

class MgOgcWmsServer: public MgOgcServer
{
public:
    MgOgcWmsServer(MgHttpRequestParameters& Request,CStream& Response,MgWmsLayerDefinitions* pLayers);
    MgOgcWmsServer(MgHttpRequestParameters& Request,CStream& Response);

    void SetFeatureInfo(MgWmsFeatureInfo* pFeatureInfo);
    void SetLayerDefs(MgWmsLayerDefinitions* pLayerDefs);

protected:
    // Centralize initialization regardless of ctor used.
    bool InitWmsServer(CPSZ pszFilenameGlobalConfig);


    virtual void RespondToRequest();
    virtual bool ValidateRequest();

    void GetCapabilitiesResponse();
    void GetMapResponse();
    void GetFeatureInfoResponse();

    CPSZ GetServiceType();
    CPSZ GetTemplatePrefix();


    // Gets the template info for default exception, when no other exception type is at hand.
    void GetDefaultExceptionInfo(REFSTRING sTemplate,REFSTRING sMimeType);

    bool ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI);

    enum WmsRequestType
    {
        WmsUnknownType,
        WmsGetCapabilitiesType,
        WmsGetMapType,
        WmsGetFeatureInfoType
    };
    enum WmsRequestType GetRequestType();

private:

    // <?EnumLayers
    //   using="xml-stream-for-each-layer"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &Layer.Name; Additional metadata fields are defined
    //                    using the convention "Layer."+ metadata property name,
    //                    such as "Layer.Abstract"
    //   subset="list"
    //                 -- a comma-separated list of integers (note: no spaces around
    //                    commas) to be iterated.  For example, subset="1,3,5,7"
    //                    only iterates the first four odd items encountered (assuming
    //                    that there are that many.)
    //                    This attribute is optional, and if omitted, or is an empty
    //                    string, no subsetting is performed: the entire list of items
    //                    is enumerated.
    //   Creates the definition "Layer.iteration" that reflects the current iteration value,
    //   a one-based index (that is, it will equal 1, 2, 3, etc. or as dictated by the
    //   subset.)
    void ProcedureEnumLayers            (MgXmlProcessingInstruction& PIEnum);
    void ProcedureEnumFeatureProperties (MgXmlProcessingInstruction& PIEnum);
    void ProcedureEnumFeatureInfo       (MgXmlProcessingInstruction& PIEnum);

    bool ValidateGetCapabilitiesParameters();
    bool ValidateGetFeatureInfoParameters();
    bool ValidateMapParameters();
    bool ValidateMapParameters(MgStringCollection* queryableLayers);

    // Hetp method to get WMS request version
    STRING GetRequestVersion();

    CPSZ ServiceExceptionReportElement();

    Ptr<MgWmsLayerDefinitions> m_pLayers;
    Ptr<MgWmsFeatureInfo> m_pFeatureInfo;

    // The backing store for the default exception.
    static STRING ms_sExceptionTemplate;
    static STRING ms_sExceptionMimeType;

    // Everpresent global definitions.  This ends up being the end (deepest part) of
    // the dictionary stack, so subsequent frames have access to it, though their own
    // definitions may eclipse stuff found here.
    static MgUtilDictionary ms_GlobalDefinitions;
};

#endif//_OgcWmsServer_h

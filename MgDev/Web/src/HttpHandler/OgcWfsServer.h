//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _OgcWfsServer_h
#define _OgcWfsServer_h

#include "OgcServer.h"
#include "WfsFeatureDefinitions.h"
#include "WfsFeatures.h"

class MgOgcWfsServer: public MgOgcServer
{
public:
    MgOgcWfsServer(MgHttpRequestParameters& Request, CStream& Response, MgWfsFeatureDefinitions& Features);
    MgOgcWfsServer(MgHttpRequestParameters& Request, CStream& Response);

    void GenerateTypeNameException(CREFSTRING sTypeName);
    void SetFeatures(MgWfsFeatures* pFeatures);
    void SetGetFeatureRequestParams(WfsGetFeatureParams* pGetFeatureParams);
    void SetFeatureDefinitions(MgWfsFeatureDefinitions* pFeatureDefs);

protected:
    virtual void RespondToRequest();
    virtual bool ValidateRequest();

    void GetCapabilitiesResponse();
    void DescribeFeatureTypeResponse();
    void GetFeatureResponse();

    bool ValidateGetFeatureRequest();

    enum WfsRequestType
    {
        WfsUnknownType,
        WfsGetCapabilitiesType,
        WfsGetFeatureType,
        WfsDescribeFeatureTypeType
    };
    enum WfsRequestType GetRequestType();

    CPSZ GetServiceType();
    CPSZ GetTemplatePrefix();

    // Gets the template info for default exception, when no other exception type is at hand.
    void GetDefaultExceptionInfo(REFSTRING sTemplate,REFSTRING sMimeType);

    bool ProcessOtherInstruction(CREFSTRING sProc,MgXmlProcessingInstruction& PI);

private:

    // <?EnumFeatureTypes
    //   using="xml-stream-for-each-layer"
    //                 -- the contents to be used to format each item in
    //                    the list, typically an expansion of one or more
    //                    Define items that presumably at least reference
    //                    &Feature.Name; Additional metadata fields are defined
    //                    using the convention "Feature."+ metadata property name,
    //                    such as "Feature.Abstract"
    //   subset="list"
    //                 -- a comma-separated list of integers (note: no spaces around
    //                    commas) to be iterated.  For example, subset="1,3,5,7"
    //                    only iterates the first four odd items encountered (assuming
    //                    that there are that many.)
    //                    This attribute is optional, and if omitted, or is an empty
    //                    string, no subsetting is performed: the entire list of items
    //                    is enumerated.
    //   Creates the definition "Feature.iteration" that reflects the current iteration value,
    //   a one-based index (that is, it will equal 1, 2, 3, etc. or as dictated by the
    //   subset.)
    // ?>
    void ProcedureEnumFeatureTypes(MgXmlProcessingInstruction& PIEnum);

    // Soon to be deprecated.
    void ProcedureGetFeatureCollection(MgXmlProcessingInstruction& PIEnum);

    // <?EnumFeatures
    //
    void ProcedureEnumFeatures(MgXmlProcessingInstruction& PIEnum);


    CPSZ ServiceExceptionReportElement();

    MgWfsFeatureDefinitions* m_pFeatures;
    Ptr<MgWfsFeatures> m_pFeatureSet;
    Ptr<WfsGetFeatureParams> m_pGetFeatureParams;

    // The backing store for the default exception.

    static STRING ms_sExceptionTemplate;
    static STRING ms_sExceptionMimeType;

    // Everpresent global definitions.  This ends up being the end (deepest part) of
    // the dictionary stack, so subsequent frames have access to it, though their own
    // definitions may eclipse stuff found here.
    static MgUtilDictionary ms_GlobalDefinitions;

};

#endif//_OgcWmsServer_h

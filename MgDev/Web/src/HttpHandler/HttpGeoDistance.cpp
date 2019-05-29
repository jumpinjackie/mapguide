//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#include "HttpHandler.h"
#include "HttpGeoDistance.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGeoDistance)

MgHttpGeoDistance::MgHttpGeoDistance(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);
    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_geomWKT = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeometry);
    m_geomOther = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeometryOther);
    m_coordinateSystem = params->GetParameterValue(MgHttpResourceStrings::reqGeoCoordinateSystem);
}

void MgHttpGeoDistance::Execute(MgHttpResponse & hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

        // Check common parameters
        ValidateCommonParameters();

    if (m_geomWKT.empty())
    {
        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqFeatGeometry);
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpGeoDistance.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (m_geomOther.empty())
    {
        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqFeatGeometryOther);
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgHttpGeoDistance.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
    Ptr<MgGeometry> input = wktRw->Read(m_geomWKT);
    Ptr<MgGeometry> other = wktRw->Read(m_geomOther);

    Ptr<MgCoordinateSystem> cs;
    Ptr<MgMeasure> measure;

    if (!m_coordinateSystem.empty())
    {
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        cs = csFactory->CreateFromCode(m_coordinateSystem);
        measure = cs->GetMeasure();
    }

    double distance = input->Distance(other, measure);
    if (NULL != cs.p)
    {
        distance = cs->ConvertCoordinateSystemUnitsToMeters(distance);
    }

    std::string mbXml;
    mbXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    mbXml += "<UnitOfMeasure xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"UnitOfMeasure-4.0.0.xsd\">";
    mbXml += "<Value>";
    std::string sDistance;
    MgUtil::DoubleToString(distance, sDistance);
    mbXml += sDistance;
    mbXml += "</Value>";
    if (NULL != measure.p) //If a measure was given, this will be in meters
    {
        mbXml += "<Unit>meters</Unit>";
    }
    else //Otherwise this is a linear distance measure. We don't know what the units are here
    {
        mbXml += "<Unit>unknown</Unit>";
    }
    
    mbXml += "</UnitOfMeasure>";

    Ptr<MgByteSource> bs = new MgByteSource((BYTE_ARRAY_IN)mbXml.c_str(), (INT32)mbXml.length());
    bs->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> br = bs->GetReader();

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(br);

    hResult->SetResultObject(br, br->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGeoDistance.Execute")
}

void MgHttpGeoDistance::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(4, 0, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpGeoDistance.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGeoDistance.ValidateOperationVersion");
}

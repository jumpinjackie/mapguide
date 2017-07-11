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
#include "HttpGeoSimplify.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGeoSimplify)

MgHttpGeoSimplify::MgHttpGeoSimplify(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);
    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_geomWKT = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeometry);
    m_algorithm = MgUtil::StringToInt32(params->GetParameterValue(MgHttpResourceStrings::reqGeoAlgorithm));
    m_tolerance = MgUtil::StringToDouble(params->GetParameterValue(MgHttpResourceStrings::reqGeoTolerance));
    m_format = params->GetParameterValue(MgHttpResourceStrings::reqGeoFormat);
    m_coordinateSystem = params->GetParameterValue(MgHttpResourceStrings::reqGeoCoordinateSystem);
    m_transformTo = params->GetParameterValue(MgHttpResourceStrings::reqGeoTransformTo);
    m_precision = -1;
    m_bEnablePrecision = false;
    STRING sPrecision = params->GetParameterValue(MgHttpResourceStrings::reqGeoPrecision);
    if (!sPrecision.empty())
    {
        INT32 precision = MgUtil::StringToInt32(sPrecision);
        if (precision > 0)
        {
            m_precision = precision;
            m_bEnablePrecision = true;
        }
    }
}

void MgHttpGeoSimplify::Execute(MgHttpResponse & hResponse)
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

        throw new MgInvalidArgumentException(L"MgHttpGeoSimplify.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    if (m_algorithm != MgGeometrySimplificationAlgorithmType::DouglasPeucker &&
        m_algorithm != MgGeometrySimplificationAlgorithmType::TopologyPreserving)
    {
        STRING arg;
        MgUtil::Int32ToString(m_algorithm, arg);

        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqGeoAlgorithm);
        arguments.Add(arg);

        throw new MgInvalidArgumentException(L"MgHttpGeoSimplify.Execute",
            __LINE__, __WFILE__, &arguments, L"MgInvalidValueOutsideRange", NULL);
    }

    Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
    Ptr<MgGeometry> input = wktRw->Read(m_geomWKT);
    Ptr<MgGeometrySimplifier> simp = new MgGeometrySimplifier();
    Ptr<MgGeometry> result = simp->Simplify(input, m_tolerance, m_algorithm);

    if (!m_transformTo.empty())
    {
        Ptr<MgCoordinateSystemFactory> csFactory = new MgCoordinateSystemFactory();
        Ptr<MgCoordinateSystem> cs = csFactory->CreateFromCode(m_coordinateSystem);
        Ptr<MgCoordinateSystem> target = csFactory->CreateFromCode(m_transformTo);
        Ptr<MgCoordinateSystemTransform> xform = csFactory->GetTransform(cs, target);
        Ptr<MgGeometry> xGeom = (MgGeometry*)result->Transform(xform);

        result = xGeom;
    }

    if (m_format == L"WKT")
    {
        STRING wkt = wktRw->Write(result);
        Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(wkt);
        hResult->SetResultObject(value, MgMimeType::Text);
    }
    else if (m_format == L"GEOJSON")
    {
        MgGeoJsonWriter gw;
        gw.SetPrecisionEnabled(m_bEnablePrecision);
        gw.SetPrecision(m_precision);
        STRING geoJson = gw.GeometryToGeoJson(result);

        std::string mbGeoJson;
        MgUtil::WideCharToMultiByte(geoJson, mbGeoJson);

        Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)mbGeoJson.c_str(), (INT32)mbGeoJson.length());
        byteSource->SetMimeType(MgMimeType::Json);
        Ptr<MgByteReader> byteReader = byteSource->GetReader();

        hResult->SetResultObject(byteReader, byteReader->GetMimeType());
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGeoSimplify.Execute")
}

void MgHttpGeoSimplify::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(3, 3, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpGeoSimplify.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGeoSimplify.ValidateOperationVersion");
}

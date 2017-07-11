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
#include "HttpGeoBinaryOperation.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGeoBinaryOperation)

MgHttpGeoBinaryOperation::MgHttpGeoBinaryOperation(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_geomWKT_A = params->GetParameterValue(MgHttpResourceStrings::reqGeoA);
    m_geomWKT_B = params->GetParameterValue(MgHttpResourceStrings::reqGeoB);
    m_operator = params->GetParameterValue(MgHttpResourceStrings::reqGeoOperator);
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

void MgHttpGeoBinaryOperation::Execute(MgHttpResponse & hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
    Ptr<MgGeometry> geomA = wktRw->Read(m_geomWKT_A);
    Ptr<MgGeometry> geomB = wktRw->Read(m_geomWKT_B);
    Ptr<MgGeometry> result;

    if (m_operator == L"UNION")
        result = geomA->Union(geomB);
    else if (m_operator == L"DIFFERENCE")
        result = geomA->Difference(geomB);
    else if (m_operator == L"INTERSECTION")
        result = geomA->Intersection(geomB);
    else if (m_operator == L"SYMMETRICDIFFERENCE")
        result = geomA->SymetricDifference(geomB);
    else
    {
        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqGeoOperator);
        arguments.Add(m_operator);

        throw new MgInvalidArgumentException(L"MgHttpGeoBinaryOperation.Execute",
            __LINE__, __WFILE__, &arguments, L"MgInvalidFeatureSpatialOperation", NULL);
    }

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

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGeoBinaryOperation.Execute")
}

void MgHttpGeoBinaryOperation::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(3, 3, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpGeoBinaryOperation.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGeoBinaryOperation.ValidateOperationVersion");
}
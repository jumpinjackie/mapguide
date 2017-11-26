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
#include "HttpCsTransformCoordinates.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpCsTransformCoordinates)

/// <summary>
/// Initializes the common parameters and parameters specific to this request.
/// </summary>
/// <param name="name">Input
/// MgHttpRequest
/// This contains all the parameters of the request.
/// </param>
/// <returns>
/// nothing
/// </returns>
MgHttpCsTransformCoordinates::MgHttpCsTransformCoordinates(MgHttpRequest *hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_srcCs = params->GetParameterValue(MgHttpResourceStrings::reqCsSource);
    m_dstCs = params->GetParameterValue(MgHttpResourceStrings::reqCsTarget);
    m_coordinates = params->GetParameterValue(MgHttpResourceStrings::reqCsCoordinates);
}

/// <summary>
/// Executes the specific request.
/// </summary>
/// <returns>
/// MgHttpResponse
/// This contains the response (including MgHttpResult and StatusCode) from the server.
/// </returns>
void MgHttpCsTransformCoordinates::Execute(MgHttpResponse& hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgCoordinateSystemFactory> factory = new MgCoordinateSystemFactory();
    Ptr<MgCoordinateSystem> source = factory->CreateFromCode(m_srcCs);
    Ptr<MgCoordinateSystem> target = factory->CreateFromCode(m_dstCs);

    Ptr<MgTransform> xform = factory->GetTransform(source, target);
    
    std::string xml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    xml += "<TransformedCoordinateCollection xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"TransformedCoordinateCollection-3.3.0.xsd\">";

    // Output information about the target coordinate system

    xml += "<CoordinateSystem>";
    xml += "<MentorCode>";
    xml += MgUtil::WideCharToMultiByte(target->GetCsCode());
    xml += "</MentorCode>";
    xml += "<EpsgCode>";
    std::string mbEpsg;
    MgUtil::Int32ToString(target->GetEpsgCode(), mbEpsg);
    xml += mbEpsg;
    xml += "</EpsgCode>";
    xml += "<Wkt>";
    xml += MgUtil::WideCharToMultiByte(factory->ConvertCoordinateSystemCodeToWkt(m_dstCs));
    xml += "</Wkt>";
    xml += "</CoordinateSystem>";

    std::wstringstream wss(m_coordinates);
    STRING part;
    while (std::getline(wss, part, L','))
    {
        xml += "<TransformedCoordinate>";
        size_t found = part.find_first_of(L' ');
        bool bValid = false;
        if (found != STRING::npos)
        {
            STRING sX = part.substr(0, found);
            STRING sY = part.substr(found);

            double x = 0.0;
            double y = 0.0;

            if (MgUtil::TryParseDouble(sX, x) && MgUtil::TryParseDouble(sY, y))
            {
                Ptr<MgCoordinate> coord = new MgCoordinateXY(x, y);
                Ptr<MgCoordinate> tx = xform->Transform(coord);

                std::string mbX;
                std::string mbY;
                MgUtil::DoubleToString(tx->GetX(), mbX);
                MgUtil::DoubleToString(tx->GetY(), mbY);

                xml += "<X>";
                xml += mbX;
                xml += "</X>";

                xml += "<Y>";
                xml += mbY;
                xml += "</Y>";
                bValid = true;
            }
        }
        
        if (!bValid)
        {
            xml += "<Error>";
            std::string mbPart;
            MgUtil::WideCharToMultiByte(part, mbPart);
            xml += mbPart;
            xml += "</Error>";
        }
        xml += "</TransformedCoordinate>";
    }

    xml += "</TransformedCoordinateCollection>";

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xml.c_str(), (INT32)xml.length());
    byteSource->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> byteReader = byteSource->GetReader();

    ProcessFormatConversion(byteReader);

    hResult->SetResultObject(byteReader, byteReader->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpCsTransformCoordinates.Execute")
}

void MgHttpCsTransformCoordinates::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(3, 3, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpCsTransformCoordinates.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpCsTransformCoordinates.ValidateOperationVersion");
}
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
#include "HttpGeometryInfo.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGeometryInfo)

MgHttpGeometryInfo::MgHttpGeometryInfo(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);
    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_geomWKT = params->GetParameterValue(MgHttpResourceStrings::reqFeatGeometry);
}

void MgHttpGeometryInfo::Execute(MgHttpResponse & hResponse)
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

        throw new MgInvalidArgumentException(L"MgHttpGeometryInfo.Execute",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
    Ptr<MgGeometry> input = wktRw->Read(m_geomWKT);

    //Collect all geometry information
    double area = input->GetArea();
    INT32 dim = input->GetDimension();
    double length = input->GetLength();
    bool isClosed = input->IsClosed();
    bool isEmpty = input->IsEmpty();
    bool isSimple = input->IsSimple();
    bool isValid = input->IsValid();
    Ptr<MgEnvelope> env = input->Envelope();
    Ptr<MgPoint> centroid = input->GetCentroid();
    Ptr<MgCoordinate> cCentroid = centroid->GetCoordinate();
    Ptr<MgCoordinate> envLL = env->GetLowerLeftCoordinate();
    Ptr<MgCoordinate> envUR = env->GetUpperRightCoordinate();

    std::string mbXml;
    mbXml = "<?xml version=\"1.0\" encoding=\"utf-8\"?>";
    mbXml += "<GeometryInfo xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"GeometryInfo-4.0.0.xsd\">";
    mbXml += "<Area>";
    std::string sArea;
    MgUtil::DoubleToString(area, sArea);
    mbXml += sArea;
    mbXml += "</Area>";
    mbXml += "<Dimension>";
    std::string sDim;
    MgUtil::Int32ToString(dim, sDim);
    mbXml += sDim;
    mbXml += "</Dimension>";
    mbXml += "<Length>";
    std::string sLen;
    MgUtil::DoubleToString(length, sLen);
    mbXml += sLen;
    mbXml += "</Length>";
    mbXml += "<IsClosed>";
    mbXml += (isClosed ? "true" : "false");
    mbXml += "</IsClosed>";
    mbXml += "<IsEmpty>";
    mbXml += (isEmpty ? "true" : "false");
    mbXml += "</IsEmpty>";
    mbXml += "<IsSimple>";
    mbXml += (isSimple ? "true" : "false");
    mbXml += "</IsSimple>";
    mbXml += "<IsValid>";
    mbXml += (isValid ? "true" : "false");
    mbXml += "</IsValid>";
    mbXml += "<Envelope>";
    mbXml += "<LowerLeft>";
    mbXml += "<X>";
    std::string sLLX;
    MgUtil::DoubleToString(envLL->GetX(), sLLX);
    mbXml += sLLX;
    mbXml += "</X>";
    mbXml += "<Y>";
    std::string sLLY;
    MgUtil::DoubleToString(envLL->GetY(), sLLY);
    mbXml += sLLY;
    mbXml += "</Y>";
    mbXml += "</LowerLeft>";
    mbXml += "<UpperRight>";
    mbXml += "<X>";
    std::string sURX;
    MgUtil::DoubleToString(envUR->GetX(), sURX);
    mbXml += sURX;
    mbXml += "</X>";
    mbXml += "<Y>";
    std::string sURY;
    MgUtil::DoubleToString(envUR->GetY(), sURY);
    mbXml += sURY;
    mbXml += "</Y>";
    mbXml += "</UpperRight>";
    mbXml += "</Envelope>";
    mbXml += "<Centroid>";
    mbXml += "<X>";
    std::string sCentX;
    MgUtil::DoubleToString(cCentroid->GetX(), sCentX);
    mbXml += sCentX;
    mbXml += "</X>";
    mbXml += "<Y>";
    std::string sCentY;
    MgUtil::DoubleToString(cCentroid->GetY(), sCentY);
    mbXml += sCentY;
    mbXml += "</Y>";
    mbXml += "</Centroid>";
    mbXml += "</GeometryInfo>";

    Ptr<MgByteSource> bs = new MgByteSource((BYTE_ARRAY_IN)mbXml.c_str(), (INT32)mbXml.length());
    bs->SetMimeType(MgMimeType::Xml);
    Ptr<MgByteReader> br = bs->GetReader();

    //Convert to alternate response format, if necessary
    ProcessFormatConversion(br);

    hResult->SetResultObject(br, br->GetMimeType());

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGeometryInfo.Execute")
}

void MgHttpGeometryInfo::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(4, 0, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpGeometryInfo.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGeometryInfo.ValidateOperationVersion");
}

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
#include "HttpGeoSpatialPredicate.h"

HTTP_IMPLEMENT_CREATE_OBJECT(MgHttpGeoSpatialPredicate)

MgHttpGeoSpatialPredicate::MgHttpGeoSpatialPredicate(MgHttpRequest* hRequest)
{
    InitializeCommonParameters(hRequest);

    Ptr<MgHttpRequestParam> params = hRequest->GetRequestParam();
    m_geomWKT_A = params->GetParameterValue(MgHttpResourceStrings::reqGeoA);
    m_geomWKT_B = params->GetParameterValue(MgHttpResourceStrings::reqGeoB);
    m_operator = params->GetParameterValue(MgHttpResourceStrings::reqGeoOperator);
}


void MgHttpGeoSpatialPredicate::Execute(MgHttpResponse & hResponse)
{
    Ptr<MgHttpResult> hResult = hResponse.GetResult();

    MG_HTTP_HANDLER_TRY()

    // Check common parameters
    ValidateCommonParameters();

    Ptr<MgWktReaderWriter> wktRw = new MgWktReaderWriter();
    Ptr<MgGeometry> geomA = wktRw->Read(m_geomWKT_A);
    Ptr<MgGeometry> geomB = wktRw->Read(m_geomWKT_B);
    bool result = false;

    if (m_operator == L"CONTAINS")
        result = geomA->Contains(geomB);
    else if (m_operator == L"CROSSES")
        result = geomA->Crosses(geomB);
    else if (m_operator == L"DISJOINT")
        result = geomA->Disjoint(geomB);
    else if (m_operator == L"EQUALS")
        result = geomA->Equals(geomB);
    else if (m_operator == L"INTERSECTS")
        result = geomA->Intersects(geomB);
    else if (m_operator == L"OVERLAPS")
        result = geomA->Overlaps(geomB);
    else if (m_operator == L"TOUCHES")
        result = geomA->Touches(geomB);
    else if (m_operator == L"WITHIN")
        result = geomA->Within(geomB);
    else
    {
        MgStringCollection arguments;
        arguments.Add(MgHttpResourceStrings::reqGeoOperator);
        arguments.Add(m_operator);

        throw new MgInvalidArgumentException(L"MgHttpGeoSpatialPredicate.Execute",
            __LINE__, __WFILE__, &arguments, L"MgInvalidFeatureSpatialOperation", NULL);
    }

    Ptr<MgHttpPrimitiveValue> value = new MgHttpPrimitiveValue(result);
    hResult->SetResultObject(value, MgMimeType::Text);

    MG_HTTP_HANDLER_CATCH_AND_THROW_EX(L"MgHttpGeoSpatialPredicate.Execute")
}

void MgHttpGeoSpatialPredicate::ValidateOperationVersion()
{
    MG_HTTP_HANDLER_TRY()

    // There are multiple supported versions
    INT32 version = m_userInfo->GetApiVersion();
    if (version != MG_API_VERSION(3, 3, 0))
    {
        throw new MgInvalidOperationVersionException(
            L"MgHttpGeoSpatialPredicate.ValidateOperationVersion", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MG_HTTP_HANDLER_CATCH_AND_THROW(L"MgHttpGeoSpatialPredicate.ValidateOperationVersion");
}